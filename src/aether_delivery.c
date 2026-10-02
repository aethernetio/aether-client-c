
/*
 * Transport-agnostic message delivery.
 *
 * Design constraints:
 *   - no heap, caller-owned storage;
 *   - producer API (aether_delivery_send) only enqueues a logical message
 *     (recipient UID + copied payload); it never performs transport I/O,
 *     crypto or routing decisions;
 *   - the actual send happens from aether_delivery_work(), which is driven
 *     by aether_work() as a normal component;
 *   - exactly one provider decides for one pending message per attempt;
 *   - fallback to the next provider is allowed only when the previous
 *     provider contractually guarantees that no bytes were handed off;
 *   - ACCEPTED means "provider handed the message off to its transport",
 *     with the same guarantee aether_send_message() gives today.
 */

#include "aether_delivery.h"
#include "aether_client_app.h"

#include <string.h>


static bool delivery_is_initialized(
    const aether_delivery_t *delivery) {

    return
        delivery != NULL &&
        delivery->client != NULL &&
        delivery->slots != NULL &&
        delivery->slot_count != 0u &&
        delivery->payload_storage != NULL &&
        delivery->payload_capacity != 0u;
}


static uint8_t *delivery_payload_of(
    const aether_delivery_t *delivery,
    size_t slot_index) {

    return
        delivery->payload_storage +
        slot_index * delivery->payload_capacity;
}


void aether_delivery_init(
    aether_delivery_t *delivery,
    aether_t *client,
    aether_delivery_slot_t *slots,
    size_t slot_count,
    uint8_t *payload_storage,
    size_t payload_capacity) {

    if (delivery == NULL) {
        return;
    }

    memset(
        delivery,
        0,
        sizeof(*delivery));

    delivery->client =
        client;

    delivery->slots =
        slots;

    delivery->slot_count =
        slot_count;

    delivery->payload_storage =
        payload_storage;

    delivery->payload_capacity =
        payload_capacity;

    if (slots != NULL &&
        slot_count != 0u) {

        memset(
            slots,
            0,
            slot_count * sizeof(*slots));
    }
}


aether_status_t aether_delivery_set_providers(
    aether_delivery_t *delivery,
    aether_delivery_provider_t *providers,
    size_t capacity) {

    if (delivery == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (providers == NULL &&
        capacity != 0u) {

        return AETHER_ERR_ARGUMENT;
    }

    delivery->user_providers =
        providers;

    delivery->user_provider_capacity =
        capacity;

    delivery->user_provider_count =
        0u;

    return AETHER_OK;
}


aether_status_t aether_delivery_add_provider(
    aether_delivery_t *delivery,
    aether_delivery_provider_t provider) {

    if (delivery == NULL ||
        provider.try_deliver == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (delivery->user_providers == NULL ||
        delivery->user_provider_count >=
            delivery->user_provider_capacity) {

        return AETHER_ERR_OVERFLOW;
    }

    /*
     * Reject an exact duplicate (context, try_deliver) pair. The same
     * context with a different callback is a legal (if unusual) shape and
     * stays allowed, but the exact same pair twice would only cause the
     * same route to be attempted twice within one coordinator pass, which
     * directly undermines the "one provider decides per attempt" rule.
     */
    for (size_t i = 0u;
         i < delivery->user_provider_count;
         ++i) {

        if (delivery->user_providers[i].context ==
                provider.context &&
            delivery->user_providers[i].try_deliver ==
                provider.try_deliver) {

            return AETHER_ERR_STATE;
        }
    }

    delivery->user_providers[
        delivery->user_provider_count++] =
        provider;

    return AETHER_OK;
}


void aether_delivery_set_error_callback(
    aether_delivery_t *delivery,
    aether_delivery_error_fn callback,
    void *context) {

    if (delivery == NULL) {
        return;
    }

    delivery->on_error =
        callback;

    delivery->error_ctx =
        context;
}


aether_status_t aether_delivery_send(
    aether_delivery_t *delivery,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length) {

    if (delivery == NULL ||
        (payload == NULL &&
         length != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    if (!delivery_is_initialized(delivery)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (length > delivery->payload_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    /*
     * The slot stores payload_length as uint32_t. Reject any length that
     * would not fit, so silent truncation is impossible.
     */
    if (length > (size_t)UINT32_MAX) {
        return AETHER_ERR_OVERFLOW;
    }

    /*
     * Locate a free slot. The scan is bounded and does not modify any
     * occupied slot; a failed enqueue leaves the outbox untouched.
     */
    for (size_t i = 0u;
         i < delivery->slot_count;
         ++i) {

        aether_delivery_slot_t *slot =
            &delivery->slots[i];

        if (slot->state !=
            AETHER_DELIVERY_SLOT_EMPTY) {

            continue;
        }

        if (length != 0u) {

            memcpy(
                delivery_payload_of(delivery, i),
                payload,
                length);
        }

        slot->recipient =
            recipient;

        slot->payload_length =
            (uint32_t)length;

        slot->state =
            AETHER_DELIVERY_SLOT_READY;

        return AETHER_OK;
    }

    return AETHER_ERR_OVERFLOW;
}


/* ---------- providers ---------- */


/*
 * Cloud provider: the existing Aether cloud path.
 *
 * The transport codec is not visible here, but the work channel of a
 * normal deployment is UDP. On UDP, send() either delivers the whole
 * datagram or returns an error without any handoff. On TCP, a partial
 * write can leave bytes in the channel while send() still reports
 * failure. To avoid duplicate risk we therefore do NOT translate
 * AETHER_ERR_TRANSPORT into RETRY_LATER automatically; a future, more
 * precise transport-result contract will make safe UDP retry possible.
 *
 * Mapping:
 *   AETHER_OK              -> { ACCEPTED,   AETHER_OK             }
 *   AETHER_ERR_BUSY        -> { RETRY_LATER, AETHER_ERR_BUSY      }
 *   AETHER_ERR_STATE       -> { RETRY_LATER, AETHER_ERR_STATE     }
 *   AETHER_ERR_TRANSPORT   -> { ERROR,      AETHER_ERR_TRANSPORT }
 *   AETHER_ERR_OVERFLOW    -> { ERROR,      AETHER_ERR_OVERFLOW  }
 *   AETHER_ERR_CRYPTO      -> { ERROR,      AETHER_ERR_CRYPTO    }
 *   AETHER_ERR_ARGUMENT    -> { ERROR,      AETHER_ERR_ARGUMENT  }
 *   AETHER_ERR_UNSUPPORTED -> { ERROR,      AETHER_ERR_UNSUPPORTED}
 *   other                  -> { ERROR,      status               }
 *
 * "not ready right now" is a temporary condition, not inapplicability of
 * the route, so it maps to RETRY_LATER with AETHER_ERR_STATE.
 */
static aether_delivery_attempt_t delivery_cloud_try(
    void *context,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length) {

    aether_t *client =
        context;

    if (client == NULL) {
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_ERROR,
            AETHER_ERR_ARGUMENT
        };
    }

    if (!aether_is_ready(client)) {
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_RETRY_LATER,
            AETHER_ERR_STATE
        };
    }

    aether_status_t status =
        aether_client_app_send_with_id(
            &client->app,
            recipient,
            payload,
            length,
            NULL);

    switch (status) {

    case AETHER_OK:
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_ACCEPTED,
            AETHER_OK
        };

    case AETHER_ERR_BUSY:
    case AETHER_ERR_STATE:
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_RETRY_LATER,
            status
        };

    case AETHER_ERR_TRANSPORT:
        /*
         * Conservative: TCP partial write can be ambiguous. UDP would be
         * safe, but we cannot observe the codec from this layer without
         * coupling. Prefer correctness over retry availability.
         */
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_ERROR,
            status
        };

    default:
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_ERROR,
            status
        };
    }
}


/*
 * Run the ordered provider chain for one slot.
 *
 * Contract per provider result:
 *   ACCEPTED       -> terminal; no further provider is tried.
 *   NOT_APPLICABLE -> strict no handoff; next provider is allowed.
 *   RETRY_LATER    -> strict no handoff; next provider is allowed.
 *   ROUTE_LOST     -> strict no handoff; next provider is allowed.
 *   ERROR          -> terminal, no fallback. ERROR explicitly does NOT
 *                     imply a no-handoff guarantee, so the coordinator
 *                     must not try any further provider after ERROR.
 *   anything else  -> conservative: treated as ERROR (terminal, no
 *                     fallback). Unknown values may correspond to an
 *                     ambiguous handoff and must not trigger fallback.
 */
static aether_delivery_attempt_t delivery_attempt_slot(
    aether_delivery_t *delivery,
    size_t slot_index) {

    aether_delivery_slot_t *slot =
        &delivery->slots[slot_index];

    uint8_t *payload =
        delivery_payload_of(delivery, slot_index);

    size_t length =
        slot->payload_length;

    aether_uuid_t recipient =
        slot->recipient;

    for (size_t i = 0u;
         i < delivery->user_provider_count;
         ++i) {

        const aether_delivery_provider_t *provider =
            &delivery->user_providers[i];

        if (provider->try_deliver == NULL) {
            continue;
        }

        aether_delivery_attempt_t attempt =
            provider->try_deliver(
                provider->context,
                recipient,
                payload,
                length);

        switch (attempt.result) {

        case AETHER_DELIVERY_ACCEPTED:
            return attempt;

        case AETHER_DELIVERY_ERROR:
            return attempt;

        case AETHER_DELIVERY_NOT_APPLICABLE:
        case AETHER_DELIVERY_RETRY_LATER:
        case AETHER_DELIVERY_ROUTE_LOST:
            continue;

        default:
            /*
             * Unknown result value. Conservatively treat it as ERROR:
             * no fallback, error callback will fire with the attempt
             * status (which, for a custom provider, carries its own
             * diagnostic).
             */
            return (aether_delivery_attempt_t){
                AETHER_DELIVERY_ERROR,
                attempt.status
            };
        }
    }

    /*
     * Built-in cloud provider is always the last fallback.
     */
    return delivery_cloud_try(
        delivery->client,
        recipient,
        payload,
        length);
}


aether_status_t aether_delivery_work(
    aether_t *client,
    void *context) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    aether_delivery_t *delivery =
        context;

    if (delivery == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    /*
     * Protect the owner relationship: the delivery component may only be
     * registered on the same aether_t it was initialized with. This
     * mirrors the registration-component guard and prevents a delivery
     * object from driving another client's transport.
     */
    if (delivery->client != client) {
        return AETHER_ERR_ARGUMENT;
    }

    if (!delivery_is_initialized(delivery)) {
        return AETHER_OK;
    }

    /*
     * Bounded single pass. Every READY slot gets at most one attempt
     * sequence. A slot that cannot complete this tick remains READY and
     * will be tried again on the next aether_work() call. No slot is
     * skipped because another is blocked.
     */
    for (size_t i = 0u;
         i < delivery->slot_count;
         ++i) {

        aether_delivery_slot_t *slot =
            &delivery->slots[i];

        if (slot->state !=
            AETHER_DELIVERY_SLOT_READY) {

            continue;
        }

        aether_delivery_attempt_t attempt =
            delivery_attempt_slot(
                delivery,
                i);

        switch (attempt.result) {

        case AETHER_DELIVERY_ACCEPTED:
            memset(
                slot,
                0,
                sizeof(*slot));
            break;

        case AETHER_DELIVERY_ERROR:
            if (delivery->on_error != NULL) {

                delivery->on_error(
                    delivery->error_ctx,
                    slot->recipient,
                    attempt.status);
            }

            memset(
                slot,
                0,
                sizeof(*slot));
            break;

        case AETHER_DELIVERY_NOT_APPLICABLE:
        case AETHER_DELIVERY_RETRY_LATER:
        case AETHER_DELIVERY_ROUTE_LOST:
            /*
             * Ordinary waiting. Leave the slot READY; it will be retried
             * on the next tick. This is not a component error.
             */
            break;

        default:
            /*
             * Should not happen: delivery_attempt_slot already maps
             * unknown results to ERROR. Defensive: treat as terminal
             * and free the slot without a callback.
             */
            memset(
                slot,
                0,
                sizeof(*slot));
            break;
        }
    }

    return AETHER_OK;
}

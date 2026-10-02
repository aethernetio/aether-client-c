
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

    if (delivery == NULL ||
        delivery->client == NULL ||
        delivery->slots == NULL ||
        delivery->slot_count == 0u ||
        delivery->payload_storage == NULL ||
        delivery->payload_capacity == 0u) {

        return false;
    }

    /*
     * Slot metadata stores payload_length as uint32_t; a capacity that
     * cannot be represented is not a valid configuration.
     */
    if (delivery->payload_capacity > (size_t)UINT32_MAX) {
        return false;
    }

    /*
     * Slot address computation is payload_storage + slot_index *
     * payload_capacity. Reject configurations where that arithmetic
     * could overflow size_t.
     */
    if (delivery->slot_count >
        SIZE_MAX / delivery->payload_capacity) {

        return false;
    }

    return true;
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

    if (delivery->providers_locked) {
        return AETHER_ERR_STATE;
    }

    if (providers == NULL &&
        capacity != 0u) {

        return AETHER_ERR_ARGUMENT;
    }


    delivery->providers =
        providers;

    delivery->provider_capacity =
        capacity;

    delivery->provider_count =
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

    if (delivery->providers_locked) {
        return AETHER_ERR_STATE;
    }



    if (delivery->providers == NULL ||
        delivery->provider_count >=
            delivery->provider_capacity) {

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
         i < delivery->provider_count;
         ++i) {

        if (delivery->providers[i].context ==
                provider.context &&
            delivery->providers[i].try_deliver ==
                provider.try_deliver) {

            return AETHER_ERR_STATE;
        }
    }

    delivery->providers[
        delivery->provider_count++] =
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

        delivery->providers_locked =
            1;

        return AETHER_OK;

    }

    return AETHER_ERR_OVERFLOW;
}


/* ---------- coordinator ---------- */



/*
 * Normalize a provider attempt to a self-consistent (result, status) pair.
 *
 * The provider API is public, so a custom provider could return an
 * inconsistent pair:
 *
 *   ACCEPTED + non-OK status  -> contract violation; the coordinator
 *                                must not silently drop the message.
 *                                Normalize to ERROR with the offending
 *                                status preserved.
 *
 *   ERROR + AETHER_OK status  -> contract violation; there is no
 *                                diagnostic to surface. Normalize to
 *                                AETHER_ERR_STATE so the error callback
 *                                still carries an internal-contract error.
 *
 * Other combinations are preserved as-is.
 */
static aether_delivery_attempt_t delivery_normalize_attempt(
    aether_delivery_attempt_t attempt) {

    if (attempt.result == AETHER_DELIVERY_ACCEPTED &&
        attempt.status != AETHER_OK) {

        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_ERROR,
            attempt.status
        };
    }

    if (attempt.result == AETHER_DELIVERY_ERROR &&
        attempt.status == AETHER_OK) {

        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_ERROR,
            AETHER_ERR_STATE
        };
    }

    return attempt;
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
         i < delivery->provider_count;
         ++i) {

        const aether_delivery_provider_t *provider =
            &delivery->providers[i];

        if (provider->try_deliver == NULL) {
            continue;
        }

        aether_delivery_attempt_t attempt =
            delivery_normalize_attempt(
                provider->try_deliver(
                    provider->context,
                    recipient,
                    payload,
                    length));

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
     * No provider committed and no terminal result was produced: every
     * provider was either inapplicable or asked for a retry. Leave the
     * slot READY; the coordinator retries on the next tick.
     */
    return (aether_delivery_attempt_t){
        AETHER_DELIVERY_RETRY_LATER,
        AETHER_OK
    };

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
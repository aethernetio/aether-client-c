
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


void aether_delivery_set_providers(
    aether_delivery_t *delivery,
    aether_delivery_provider_t *providers,
    size_t capacity) {

    if (delivery == NULL) {
        return;
    }

    delivery->user_providers =
        providers;

    delivery->user_provider_capacity =
        capacity;

    delivery->user_provider_count =
        0u;
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
 * Current mapping:
 *   AETHER_OK              -> ACCEPTED
 *   AETHER_ERR_BUSY        -> RETRY_LATER  (no handoff: guard)
 *   AETHER_ERR_STATE       -> RETRY_LATER  (no handoff: guard)
 *   AETHER_ERR_TRANSPORT   -> ERROR        (conservative; TCP-ambiguous)
 *   AETHER_ERR_OVERFLOW    -> ERROR
 *   AETHER_ERR_CRYPTO      -> ERROR
 *   AETHER_ERR_ARGUMENT    -> ERROR
 *   AETHER_ERR_UNSUPPORTED -> ERROR
 *   other                  -> ERROR
 */
static aether_delivery_result_t delivery_cloud_try(
    void *context,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length) {


    aether_t *client =
        context;

    if (client == NULL) {
        return AETHER_DELIVERY_ERROR;
    }

    /*
     * Cloud is the universal default route for a normal Aether UID.
     * "Not ready right now" is a temporary condition, not an
     * inapplicability of the route.
     */
    if (!aether_is_ready(client)) {
        return AETHER_DELIVERY_RETRY_LATER;
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
        return AETHER_DELIVERY_ACCEPTED;

    case AETHER_ERR_BUSY:
    case AETHER_ERR_STATE:
        return AETHER_DELIVERY_RETRY_LATER;

    case AETHER_ERR_TRANSPORT:
        /*
         * Conservative: TCP partial write can be ambiguous. UDP would be
         * safe, but we cannot observe the codec from this layer without
         * coupling. Prefer correctness over retry availability.
         */
        return AETHER_DELIVERY_ERROR;

    default:
        return AETHER_DELIVERY_ERROR;
    }
}


/*
 * Run the ordered provider chain for one slot.
 *
 * Returns the terminal result. ACCEPTED frees the slot; ERROR fires the
 * error callback and frees the slot; RETRY_LATER / NOT_APPLICABLE /
 * ROUTE_LOST leave the slot READY for a later tick.
 */
static aether_delivery_result_t delivery_attempt_slot(
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

    /*
     * User providers first (LAN in the future). Each provider contract is
     * explicit: only ACCEPTED commits; every other result guarantees that
     * no bytes were handed off for that provider.
     */
    for (size_t i = 0u;
         i < delivery->user_provider_count;
         ++i) {

        const aether_delivery_provider_t *provider =
            &delivery->user_providers[i];

        if (provider->try_deliver == NULL) {
            continue;
        }

        aether_delivery_result_t result =
            provider->try_deliver(
                provider->context,
                recipient,
                payload,
                length);

        switch (result) {

        case AETHER_DELIVERY_ACCEPTED:
            return AETHER_DELIVERY_ACCEPTED;

        case AETHER_DELIVERY_ERROR:
            return AETHER_DELIVERY_ERROR;

        case AETHER_DELIVERY_NOT_APPLICABLE:
        case AETHER_DELIVERY_RETRY_LATER:
        case AETHER_DELIVERY_ROUTE_LOST:
            /*
             * v1 fallback policy: try the next provider. Every one of
             * these results contractually guarantees no handoff for this
             * provider, so trying the next provider cannot cause a
             * duplicate from this attempt.
             */
            continue;
        }
    }

    /*
     * Built-in cloud provider is always the last fallback.
     */
    aether_delivery_result_t cloud_result =
        delivery_cloud_try(
            delivery->client,
            recipient,
            payload,
            length);

    return cloud_result;
}


aether_status_t aether_delivery_work(
    aether_t *client,
    void *context) {

    (void)client;

    aether_delivery_t *delivery =
        context;

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

        aether_delivery_result_t result =
            delivery_attempt_slot(
                delivery,
                i);

        switch (result) {

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
                    AETHER_ERR_UNSUPPORTED);
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
        }
    }

    return AETHER_OK;
}
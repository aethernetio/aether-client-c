
/*
 * Shared internal helpers. See aether_internal.h.
 */

#include "aether_internal.h"


aether_status_t aether_dispatch_message_internal(
    aether_t *client,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    if (client == NULL ||
        (data == NULL && size != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    if (client->on_message == NULL) {
        return AETHER_OK;
    }


    /*
     * Reject nested dispatch. The callback_active guard is the same
     * observable flag that aether_send_message() checks, so a nested
     * dispatch would produce an inconsistent state.
     */
    if (client->app.core.callback_active) {
        return AETHER_ERR_BUSY;
    }


    /*
     * Set callback_active around the user callback so that any attempt
     * to call aether_send_message() from inside it observes the same
     * guard the cloud path produces via rx_active. Restore the previous
     * value afterwards, so a future nested dispatch (which we do not
     * currently support) would still be safe.
     *
     * The user callback returns void and cannot throw in C, so no early
     * exit path can leave callback_active stuck true.
     */
    bool was_active =
        client->app.core.callback_active;

    client->app.core.callback_active =
        true;

    client->on_message(
        client,
        from,
        data,
        size);

    client->app.core.callback_active =
        was_active;

    return AETHER_OK;
}
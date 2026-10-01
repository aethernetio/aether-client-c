
#include "aether_credentials.h"

#include "aether_client_internal.h"

#include <string.h>

static const aether_credentials_internal_ops_t
CREDENTIALS_INTERNAL_OPS = {
    aether_client_credentials_on_rx_internal,
    aether_client_credentials_on_transport_state_internal,
    aether_client_credentials_on_poll_internal
};

aether_status_t aether_set_credentials(
    aether_client_t *client,
    aether_uuid_t uid,
    const uint8_t master_key[AETHER_KEY_BYTES]) {

    if (client == NULL ||
        master_key == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    /*
     * Credentials are immutable through this API once an identity exists.
     *
     * In particular, a caller may invoke aether_set_credentials() on every
     * startup without accidentally replacing a future rotated master key
     * restored from persistent state.
     */
    if (client->registered) {
        return AETHER_OK;
    }

    bool has_uid =
        client->uid.msb != 0u ||
        client->uid.lsb != 0u;

    bool has_key =
        false;

    for (size_t i = 0u;
         i < AETHER_KEY_BYTES;
         ++i) {

        if (client->master_key[i] != 0u) {
            has_key =
                true;
            break;
        }
    }

    if (has_uid || has_key) {
        /*
         * Partial/existing credentials are not overwritten. This protects an
         * already provisioned key even when routing metadata is absent.
         */
        return AETHER_OK;
    }

    client->uid =
        uid;

    memcpy(
        client->master_key,
        master_key,
        AETHER_KEY_BYTES);

    return AETHER_OK;
}
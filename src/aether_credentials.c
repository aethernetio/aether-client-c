
#include "aether_credentials.h"


#include <string.h>



aether_status_t aether_set_credentials_typed(
    aether_client_t *client,
    aether_uuid_t uid,
    const uint8_t master_key[AETHER_KEY_BYTES],
    uint8_t master_key_type) {


    if (client == NULL ||
        master_key == NULL) {

        return AETHER_ERR_ARGUMENT;
    }


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
        return AETHER_OK;
    }

    client->uid =
        uid;

    memcpy(
        client->master_key,
        master_key,
        AETHER_KEY_BYTES);

    client->master_key_type =
        master_key_type;

    return AETHER_OK;
}



aether_status_t aether_set_credentials(
    aether_client_t *client,
    aether_uuid_t uid,
    const uint8_t master_key[AETHER_KEY_BYTES]) {

    if (client == NULL ||
        master_key == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    return
        aether_set_credentials_typed(
            client,
            uid,
            master_key,
            UINT8_C(0));
}

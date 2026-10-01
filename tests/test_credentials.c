
#include "aether_credentials.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <string.h>

int main(void) {
    aether_client_t client;

    memset(
        &client,
        0,
        sizeof(client));

    client.state =
        AETHER_STATE_STOPPED;

    aether_credentials_t credentials;

    aether_uuid_t uid = {
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };

    uint8_t master_key[AETHER_KEY_BYTES];

    for (size_t i = 0u;
         i < sizeof(master_key);
         ++i) {

        master_key[i] =
            (uint8_t)(i + 1u);
    }

    assert(
        aether_set_credentials(
            &credentials,
            &client,
            uid,
            master_key) ==
        AETHER_OK);

    assert(
        client.credentials ==
        &credentials);

    assert(
        client.uid.msb ==
        uid.msb);

    assert(
        client.uid.lsb ==
        uid.lsb);

    assert(
        memcmp(
            client.master_key,
            master_key,
            sizeof(master_key)) ==
        0);

    assert(!client.registered);
    assert(client.server_count == 0u);

    return 0;
}

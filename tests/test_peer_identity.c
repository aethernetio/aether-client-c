
#include "aether_peer_identity.h"

#include <assert.h>
#include <string.h>

static void test_hydrogen_key_registry(void) {
    hydrogen_sign_public_t public_key;
    memset(&public_key, 0, sizeof(public_key));

    for (size_t i = 0u; i < sizeof(public_key.data.data); ++i) {
        public_key.data.data[i] = (uint8_t)i;
    }

    key_sign_public_ref_t ref = {
        .type_id = KEY_SIGN_PUBLIC_TYPE_HYDROGEN_SIGN_PUBLIC,
        .as.hydrogen_sign_public = &public_key
    };

    aether_peer_key_entry_t storage[2];
    aether_peer_key_registry_t registry;

    aether_peer_key_registry_init(
        &registry,
        storage,
        2u);

    aether_uuid_t uid = {
        .msb = 1u,
        .lsb = 2u
    };

    assert(aether_peer_key_registry_register(
        &registry,
        uid,
        &ref) == AETHER_OK);

    assert(aether_peer_key_registry_matches(
        &ref,
        &storage[0]));

    public_key.data.data[0] = 99u;

    assert(!aether_peer_key_registry_matches(
        &ref,
        &storage[0]));
}

int main(void) {
    test_hydrogen_key_registry();
    return 0;
}

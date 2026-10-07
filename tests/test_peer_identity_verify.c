
#include "aether_peer_identity.h"

#include <assert.h>
#include <string.h>

static void test_invalid_identity_fails_without_root(void) {
    peer_identity_t identity;
    memset(&identity, 0, sizeof(identity));

    assert(!aether_peer_identity_verify_server_signature(
        &identity,
        NULL,
        0u,
        NULL,
        NULL));
}

int main(void) {
    test_invalid_identity_fails_without_root();
    return 0;
}

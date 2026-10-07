
#include "aether_peer_uap.h"

#include <assert.h>
#include <stdint.h>

static void test_empty_window(void) {
    aether_peer_uap_state_t state;
    aether_peer_uap_init(&state);

    assert(aether_peer_uap_is_empty(&state));
    assert(aether_peer_uap_is_open(&state, 100u));
    assert(aether_peer_uap_delay_until_open_ms(&state, 100u) == -1);
}

static void test_delayed_window(void) {
    aether_peer_uap_state_t state;
    aether_peer_uap_init(&state);

    assert(aether_peer_uap_set_window(
        &state,
        1000u,
        200,
        500) == AETHER_OK);

    assert(!aether_peer_uap_is_empty(&state));
    assert(!aether_peer_uap_is_open(&state, 1100u));
    assert(aether_peer_uap_delay_until_open_ms(&state, 1100u) == 100);
    assert(aether_peer_uap_is_open(&state, 1200u));
    assert(!aether_peer_uap_is_open(&state, 1500u));
}

static void test_cancel(void) {
    aether_peer_uap_state_t state;
    aether_peer_uap_init(&state);

    assert(aether_peer_uap_set_window(
        &state,
        0u,
        10,
        10) == AETHER_OK);

    assert(!aether_peer_uap_is_empty(&state));

    assert(aether_peer_uap_set_window(
        &state,
        0u,
        0,
        0) == AETHER_OK);

    assert(aether_peer_uap_is_empty(&state));
}

int main(void) {
    test_empty_window();
    test_delayed_window();
    test_cancel();
    return 0;
}

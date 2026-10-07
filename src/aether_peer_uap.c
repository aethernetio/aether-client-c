
#include "aether_peer_uap.h"

void aether_peer_uap_init(
    aether_peer_uap_state_t *state) {

    if (state == NULL) {
        return;
    }

    state->from_ms = 0u;
    state->until_ms = 0u;
}

static uint64_t saturating_add(
    uint64_t a,
    uint64_t b) {

    if (UINT64_MAX - a < b) {
        return UINT64_MAX;
    }

    return a + b;
}

aether_status_t aether_peer_uap_set_window(
    aether_peer_uap_state_t *state,
    uint64_t now_ms,
    int64_t starts_in_ms,
    int64_t duration_ms) {

    if (state == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (starts_in_ms < 0 || duration_ms < 0) {
        return AETHER_ERR_ARGUMENT;
    }

    if (duration_ms == 0) {
        state->from_ms = 0u;
        state->until_ms = 0u;
        return AETHER_OK;
    }

    state->from_ms =
        saturating_add(
            now_ms,
            (uint64_t) starts_in_ms);

    state->until_ms =
        saturating_add(
            state->from_ms,
            (uint64_t) duration_ms);

    return AETHER_OK;
}

bool aether_peer_uap_is_empty(
    const aether_peer_uap_state_t *state) {

    return state == NULL || state->until_ms == 0u;
}

bool aether_peer_uap_is_open(
    const aether_peer_uap_state_t *state,
    uint64_t now_ms) {

    if (aether_peer_uap_is_empty(state)) {
        return true;
    }

    return state->from_ms <= now_ms &&
           now_ms < state->until_ms;
}

int64_t aether_peer_uap_delay_until_open_ms(
    const aether_peer_uap_state_t *state,
    uint64_t now_ms) {

    if (aether_peer_uap_is_empty(state)) {
        return -1;
    }

    if (now_ms >= state->until_ms) {
        return -1;
    }

    if (now_ms >= state->from_ms) {
        return 0;
    }

    return (int64_t)(state->from_ms - now_ms);
}

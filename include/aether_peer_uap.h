
/*
 * Peer receive window state.
 *
 * Caller-owned, no heap. Transport-independent scheduling primitive.
 */
#ifndef AETHER_PEER_UAP_H
#define AETHER_PEER_UAP_H

#include "aether_client.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t from_ms;
    uint64_t until_ms;
} aether_peer_uap_state_t;

void aether_peer_uap_init(
    aether_peer_uap_state_t *state);

aether_status_t aether_peer_uap_set_window(
    aether_peer_uap_state_t *state,
    uint64_t now_ms,
    int64_t starts_in_ms,
    int64_t duration_ms);

bool aether_peer_uap_is_empty(
    const aether_peer_uap_state_t *state);

bool aether_peer_uap_is_open(
    const aether_peer_uap_state_t *state,
    uint64_t now_ms);

int64_t aether_peer_uap_delay_until_open_ms(
    const aether_peer_uap_state_t *state,
    uint64_t now_ms);

#ifdef __cplusplus
}
#endif

#endif

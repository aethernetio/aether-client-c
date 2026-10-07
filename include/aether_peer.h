
/*
 * Transport-independent peer protocol API.
 *
 * Peer objects are caller-owned. No heap allocation and no background threads.
 * The application drives progress explicitly and supplies byte-frame callbacks.
 */
#ifndef AETHER_PEER_H
#define AETHER_PEER_H

#include "aether_client.h"
#include "peer_api_types.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *ctx;

    aether_status_t (*send_frame)(
        void *ctx,
        const uint8_t *data,
        size_t size);

} aether_peer_frame_sender_t;

aether_status_t aether_peer_frame_encode(
    uint8_t *out,
    size_t capacity,
    size_t *length,
    const peer_frame_t *frame);

aether_status_t aether_peer_frame_decode(
    const uint8_t *data,
    size_t length,
    aether_meta_read_context_t *context,
    peer_frame_t *frame);

#ifdef __cplusplus
}
#endif

#endif

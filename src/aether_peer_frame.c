
/*
 * Peer frame codec.
 *
 * Implements the transport envelope around generated PeerFrame.
 * The payload inside the frame belongs to higher peer protocol layers.
 */

#include "aether_api.h"
#include "aether_dataio.h"

#include <string.h>

#define AETHER_PEER_FRAME_DOMAIN_SIZE 21u

static const uint8_t AETHER_PEER_FRAME_DOMAIN[AETHER_PEER_FRAME_DOMAIN_SIZE] = {
    'A','E','T','H','E','R','_','P','E','E','R','_',
    'F','R','A','M','E','_','V','1',0
};


aether_status_t aether_peer_frame_encode(
    uint8_t *out,
    size_t capacity,
    size_t *length,
    const peer_frame_t *frame) {

    if (out == NULL || length == NULL || frame == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (capacity < AETHER_PEER_FRAME_DOMAIN_SIZE) {
        return AETHER_ERR_OVERFLOW;
    }

    memcpy(out, AETHER_PEER_FRAME_DOMAIN,
           AETHER_PEER_FRAME_DOMAIN_SIZE);

    aether_data_out_t stream;
    aether_data_out_init(&stream, out, capacity);
    stream.pos = AETHER_PEER_FRAME_DOMAIN_SIZE;

    if (!peer_api_serialize_peer_frame(&stream, frame, 0u)) {
        return AETHER_ERR_OVERFLOW;
    }

    *length = stream.pos;
    return AETHER_OK;
}



aether_status_t aether_peer_frame_decode(
    const uint8_t *data,
    size_t length,
    peer_frame_t *frame) {

    if (data == NULL || frame == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (length < AETHER_PEER_FRAME_DOMAIN_SIZE ||
        memcmp(data, AETHER_PEER_FRAME_DOMAIN,
               AETHER_PEER_FRAME_DOMAIN_SIZE) != 0) {
        return AETHER_ERR_PROTOCOL;
    }

    aether_data_in_t stream;
    aether_data_in_init(&stream, data, length);
    stream.pos = AETHER_PEER_FRAME_DOMAIN_SIZE;

    if (!peer_api_deserialize_peer_frame(&stream, frame, 0u)) {
        return AETHER_ERR_PROTOCOL;
    }

    if (frame->payload.length == 0u) {
        return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

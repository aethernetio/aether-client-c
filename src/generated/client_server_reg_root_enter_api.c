#include "client_server_reg_root_enter_api.h"

#include <string.h>

aether_status_t registration_root_api_enter(
    registration_root_api_remote_t *api,
    crypto_lib_t cryptoLib,
    const server_registration_api_stream_t * stream) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (api->tx_capacity < 1u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 4u;
    if (pos + 1u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    api->tx[pos++] = (uint8_t)cryptoLib;
    if (stream == NULL) return AETHER_ERR_ARGUMENT;
    if (stream->length > stream->capacity) return AETHER_ERR_ARGUMENT;
    if ((*stream).length > 0u && (*stream).data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t stream_pack_value = (uint64_t)(*stream).length;
    if (stream_pack_value < UINT64_C(251)) {
        if (pos + 1u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
        api->tx[pos++] = (uint8_t)stream_pack_value;
    } else if (stream_pack_value < UINT64_C(1515)) {
        uint64_t stream_pack = stream_pack_value - UINT64_C(251);
        if (pos + 2u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
        api->tx[pos++] = (uint8_t)(((stream_pack >> 8u) & UINT64_C(0xff)) + UINT64_C(251));
        api->tx[pos++] = (uint8_t)(stream_pack & UINT64_C(0xff));
    } else if (stream_pack_value < UINT64_C(1049835)) {
        uint64_t stream_pack = stream_pack_value - UINT64_C(1515);
        if (pos + 4u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
        api->tx[pos++] = 255u;
        api->tx[pos++] = (uint8_t)(((stream_pack >> 16u) - UINT64_C(251)) + UINT64_C(1515));
        api->tx[pos++] = (uint8_t)(stream_pack & UINT64_C(0xff));
        api->tx[pos++] = (uint8_t)((stream_pack >> 8u) & UINT64_C(0xff));
    } else {
        const uint64_t stream_pack_limit = UINT64_C(1049835) + (UINT64_C(4294967296) * UINT64_C(256));
        if (stream_pack_value >= stream_pack_limit) return AETHER_ERR_OVERFLOW;
        uint64_t stream_pack = stream_pack_value - UINT64_C(1049835);
        if (pos + 8u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
        api->tx[pos++] = 255u;
        api->tx[pos++] = 255u;
        uint16_t stream_high = (uint16_t)(((stream_pack >> 32u) - UINT64_C(1515)) + UINT64_C(1049835));
        api->tx[pos++] = (uint8_t)(stream_high & 0xffu);
        api->tx[pos++] = (uint8_t)((stream_high >> 8u) & 0xffu);
        api->tx[pos++] = (uint8_t)(stream_pack & UINT64_C(0xff));
        api->tx[pos++] = (uint8_t)((stream_pack >> 8u) & UINT64_C(0xff));
        api->tx[pos++] = (uint8_t)((stream_pack >> 16u) & UINT64_C(0xff));
        api->tx[pos++] = (uint8_t)((stream_pack >> 24u) & UINT64_C(0xff));
    }
    if ((*stream).length > api->tx_capacity - pos) return AETHER_ERR_OVERFLOW;
    if ((*stream).length > 0u) memcpy(api->tx + pos, (*stream).data, (*stream).length);
    pos += (*stream).length;

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}


#include "client_server_api_api.h"

#include <string.h>

aether_status_t authorized_api_send_message_with_result(
    authorized_api_remote_t *api,
    uint32_t request_id,
    const message_t * msg) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (api->tx_capacity < 5u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 39u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    if (msg == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    api->tx[pos++] = (uint8_t)(msg->uid.msb >> 56u);
    api->tx[pos++] = (uint8_t)(msg->uid.msb >> 48u);
    api->tx[pos++] = (uint8_t)(msg->uid.msb >> 40u);
    api->tx[pos++] = (uint8_t)(msg->uid.msb >> 32u);
    api->tx[pos++] = (uint8_t)(msg->uid.msb >> 24u);
    api->tx[pos++] = (uint8_t)(msg->uid.msb >> 16u);
    api->tx[pos++] = (uint8_t)(msg->uid.msb >> 8u);
    api->tx[pos++] = (uint8_t)(msg->uid.msb >> 0u);
    api->tx[pos++] = (uint8_t)(msg->uid.lsb >> 56u);
    api->tx[pos++] = (uint8_t)(msg->uid.lsb >> 48u);
    api->tx[pos++] = (uint8_t)(msg->uid.lsb >> 40u);
    api->tx[pos++] = (uint8_t)(msg->uid.lsb >> 32u);
    api->tx[pos++] = (uint8_t)(msg->uid.lsb >> 24u);
    api->tx[pos++] = (uint8_t)(msg->uid.lsb >> 16u);
    api->tx[pos++] = (uint8_t)(msg->uid.lsb >> 8u);
    api->tx[pos++] = (uint8_t)(msg->uid.lsb >> 0u);
    if (msg->data.length > 0u && msg->data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t msg_data_pack_value = (uint64_t)msg->data.length;
    if (msg_data_pack_value < UINT64_C(251)) {
        if (pos + 1u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
        api->tx[pos++] = (uint8_t)msg_data_pack_value;
    } else if (msg_data_pack_value < UINT64_C(1515)) {
        uint64_t msg_data_pack = msg_data_pack_value - UINT64_C(251);
        if (pos + 2u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
        api->tx[pos++] = (uint8_t)(((msg_data_pack >> 8u) & UINT64_C(0xff)) + UINT64_C(251));
        api->tx[pos++] = (uint8_t)(msg_data_pack & UINT64_C(0xff));
    } else if (msg_data_pack_value < UINT64_C(1049835)) {
        uint64_t msg_data_pack = msg_data_pack_value - UINT64_C(1515);
        if (pos + 4u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
        api->tx[pos++] = 255u;
        api->tx[pos++] = (uint8_t)(((msg_data_pack >> 16u) - UINT64_C(251)) + UINT64_C(1515));
        api->tx[pos++] = (uint8_t)(msg_data_pack & UINT64_C(0xff));
        api->tx[pos++] = (uint8_t)((msg_data_pack >> 8u) & UINT64_C(0xff));
    } else {
        const uint64_t msg_data_pack_limit = UINT64_C(1049835) + (UINT64_C(4294967296) * UINT64_C(256));
        if (msg_data_pack_value >= msg_data_pack_limit) return AETHER_ERR_OVERFLOW;
        uint64_t msg_data_pack = msg_data_pack_value - UINT64_C(1049835);
        if (pos + 8u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
        api->tx[pos++] = 255u;
        api->tx[pos++] = 255u;
        uint16_t msg_data_high = (uint16_t)(((msg_data_pack >> 32u) - UINT64_C(1515)) + UINT64_C(1049835));
        api->tx[pos++] = (uint8_t)(msg_data_high & 0xffu);
        api->tx[pos++] = (uint8_t)((msg_data_high >> 8u) & 0xffu);
        api->tx[pos++] = (uint8_t)(msg_data_pack & UINT64_C(0xff));
        api->tx[pos++] = (uint8_t)((msg_data_pack >> 8u) & UINT64_C(0xff));
        api->tx[pos++] = (uint8_t)((msg_data_pack >> 16u) & UINT64_C(0xff));
        api->tx[pos++] = (uint8_t)((msg_data_pack >> 24u) & UINT64_C(0xff));
    }
    if (msg->data.length > api->tx_capacity - pos) return AETHER_ERR_OVERFLOW;
    if (msg->data.length > 0u) memcpy(api->tx + pos, msg->data.data, msg->data.length);
    pos += msg->data.length;

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}


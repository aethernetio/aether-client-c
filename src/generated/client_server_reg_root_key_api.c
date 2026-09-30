#include "client_server_reg_root_key_api.h"

#include <string.h>

aether_status_t registration_root_api_get_asymmetric_public_key(
    registration_root_api_remote_t *api,
    uint32_t request_id,
    crypto_lib_t cryptoLib) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (api->tx_capacity < 5u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 3u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    if (pos + 1u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    api->tx[pos++] = (uint8_t)cryptoLib;

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}


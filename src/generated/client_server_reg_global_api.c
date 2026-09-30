#include "client_server_reg_global_api.h"

#include <string.h>

aether_status_t global_reg_server_api_set_master_key(
    global_reg_server_api_remote_t *api,
    const key_ref_t * key) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (api->tx_capacity < 1u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 3u;
    if (key == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    api->tx[pos++] = (uint8_t)(*key).type_id;
    switch ((*key).type_id) {
    case 6u:
        if ((*key).as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*key).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*key).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*key).as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*key).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*key).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*key).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*key).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*key).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*key).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*key).as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*key).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*key).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*key).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*key).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*key).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > api->tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(api->tx + pos, (*key).as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t global_reg_server_api_finish(
    global_reg_server_api_remote_t *api,
    uint32_t request_id) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (api->tx_capacity < 5u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 4u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}


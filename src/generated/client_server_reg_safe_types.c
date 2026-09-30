#include "client_server_reg_safe_types.h"
#include "aether_meta_runtime.h"

#include <string.h>

aether_status_t client_server_reg_safe_serialize_key(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 6u:
        if ((*value).as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_key_symmetric(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_symmetric_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 6u:
        if ((*value).as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_key_asymmetric(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_key_asymmetric_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_public_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 2u:
        if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_key_asymmetric_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_private_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_key_sign(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 9u:
        if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 14u:
        if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_key_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_public_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 10u:
        if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 5u:
        if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_key_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_private_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 9u:
        if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 4u:
        if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_sodium_chacha20_poly1305(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_chacha20_poly1305_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_hydrogen_curve_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_curve_private_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_hydrogen_curve_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_curve_public_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_hydrogen_secret_box(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_secret_box_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_sodium_curve_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_curve_public_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_sodium_curve_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_curve_private_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_sodium_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_sign_private_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_sodium_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_sign_public_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_hydrogen_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_sign_private_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_hydrogen_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_sign_public_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_sign(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sign_a_e_e_d25519->data.data, 64u);
    pos += 64u;
        break;
    case 2u:
        if ((*value).as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sign_h_y_d_r_o_g_e_n->data.data, 64u);
    pos += 64u;
        break;
    case 3u:
        if ((*value).as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sign_p256_aes_gcm->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_sign_a_e_e_d25519(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_a_e_e_d25519_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_sign_h_y_d_r_o_g_e_n(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_h_y_d_r_o_g_e_n_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_signed_key(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const signed_key_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->key.type_id;
    switch (value->key.type_id) {
    case 6u:
        if (value->key.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->key.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->key.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->key.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->key.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->key.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->key.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->key.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->key.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->key.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->key.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->key.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->sign.type_id;
    switch (value->sign.type_id) {
    case 1u:
        if (value->sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->sign.as.sign_a_e_e_d25519->data.data, 64u);
    pos += 64u;
        break;
    case 2u:
        if (value->sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (value->sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->sign.as.sign_h_y_d_r_o_g_e_n->data.data, 64u);
    pos += 64u;
        break;
    case 3u:
        if (value->sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (value->sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->sign.as.sign_p256_aes_gcm->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_work_proof_config(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_config_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.work_proof_b_crypt == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.work_proof_b_crypt == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)(*value).as.work_proof_b_crypt->costBCrypt);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)(*value).as.work_proof_b_crypt->poolSize);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)(*value).as.work_proof_b_crypt->maxHashVal);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_work_proof_b_crypt(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_b_crypt_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)value->costBCrypt);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)value->poolSize);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->maxHashVal);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_work_proof_d_t_o(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_d_t_o_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->salt.data, value->salt.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->suffix.data, value->suffix.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)value->poolSize);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->maxHashVal);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->globalKey.key.type_id;
    switch (value->globalKey.key.type_id) {
    case 6u:
        if (value->globalKey.key.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->globalKey.key.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->globalKey.key.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->globalKey.key.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->globalKey.key.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->globalKey.key.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->globalKey.key.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->globalKey.key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->globalKey.key.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->globalKey.key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->globalKey.key.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->globalKey.key.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->globalKey.key.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->globalKey.key.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->globalKey.key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.key.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->globalKey.sign.type_id;
    switch (value->globalKey.sign.type_id) {
    case 1u:
        if (value->globalKey.sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.sign.as.sign_a_e_e_d25519->data.data, 64u);
    pos += 64u;
        break;
    case 2u:
        if (value->globalKey.sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.sign.as.sign_h_y_d_r_o_g_e_n->data.data, 64u);
    pos += 64u;
        break;
    case 3u:
        if (value->globalKey.sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (value->globalKey.sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->globalKey.sign.as.sign_p256_aes_gcm->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_i_p_address(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if ((*value).as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if ((*value).as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*value).as.i_p_address_web->data.data, (*value).as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_i_p_address_v4(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_v4_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 4u);
    pos += 4u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_i_p_address_v6(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_v6_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 16u);
    pos += 16u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_i_p_address_web(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_web_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->data.data, value->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_coder_and_port(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const coder_and_port_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->codec;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->port);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_i_p_address_and_ports(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_and_ports_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->address.type_id;
    switch (value->address.type_id) {
    case 1u:
        if (value->address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->address.as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if (value->address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->address.as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if (value->address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if (value->address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->address.as.i_p_address_web->data.data, value->address.as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (value->coderAndPorts.length > 0u && value->coderAndPorts.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_coderAndPorts_length = (uint64_t)value->coderAndPorts.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_coderAndPorts_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_coderAndPorts_index = 0u; value_coderAndPorts_index < value->coderAndPorts.length; ++value_coderAndPorts_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->coderAndPorts.data[value_coderAndPorts_index].codec;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->coderAndPorts.data[value_coderAndPorts_index].port);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_i_p_address_and_ports_list(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_and_ports_list_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (value->addresses.length > 0u && value->addresses.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_addresses_length = (uint64_t)value->addresses.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_addresses_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_addresses_index = 0u; value_addresses_index < value->addresses.length; ++value_addresses_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->addresses.data[value_addresses_index].address.type_id;
    switch (value->addresses.data[value_addresses_index].address.type_id) {
    case 1u:
        if (value->addresses.data[value_addresses_index].address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->addresses.data[value_addresses_index].address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->addresses.data[value_addresses_index].address.as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if (value->addresses.data[value_addresses_index].address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->addresses.data[value_addresses_index].address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->addresses.data[value_addresses_index].address.as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if (value->addresses.data[value_addresses_index].address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if (value->addresses.data[value_addresses_index].address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->addresses.data[value_addresses_index].address.as.i_p_address_web->data.data, value->addresses.data[value_addresses_index].address.as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (value->addresses.data[value_addresses_index].coderAndPorts.length > 0u && value->addresses.data[value_addresses_index].coderAndPorts.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_addresses_element_coderAndPorts_length = (uint64_t)value->addresses.data[value_addresses_index].coderAndPorts.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_addresses_element_coderAndPorts_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_addresses_element_coderAndPorts_index = 0u; value_addresses_element_coderAndPorts_index < value->addresses.data[value_addresses_index].coderAndPorts.length; ++value_addresses_element_coderAndPorts_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->addresses.data[value_addresses_index].coderAndPorts.data[value_addresses_element_coderAndPorts_index].codec;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->addresses.data[value_addresses_index].coderAndPorts.data[value_addresses_element_coderAndPorts_index].port);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_server_descriptor(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const server_descriptor_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->ipAddress.addresses.length > 0u && value->ipAddress.addresses.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_ipAddress_addresses_length = (uint64_t)value->ipAddress.addresses.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_ipAddress_addresses_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_ipAddress_addresses_index = 0u; value_ipAddress_addresses_index < value->ipAddress.addresses.length; ++value_ipAddress_addresses_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.type_id;
    switch (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.type_id) {
    case 1u:
        if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_web->data.data, value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.length > 0u && value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_ipAddress_addresses_element_coderAndPorts_length = (uint64_t)value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_ipAddress_addresses_element_coderAndPorts_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_ipAddress_addresses_element_coderAndPorts_index = 0u; value_ipAddress_addresses_element_coderAndPorts_index < value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.length; ++value_ipAddress_addresses_element_coderAndPorts_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.data[value_ipAddress_addresses_element_coderAndPorts_index].codec;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.data[value_ipAddress_addresses_element_coderAndPorts_index].port);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_server_descriptor_with_geo(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const server_descriptor_with_geo_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->time);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->ipAddress.addresses.length > 0u && value->ipAddress.addresses.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_ipAddress_addresses_length = (uint64_t)value->ipAddress.addresses.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_ipAddress_addresses_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_ipAddress_addresses_index = 0u; value_ipAddress_addresses_index < value->ipAddress.addresses.length; ++value_ipAddress_addresses_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.type_id;
    switch (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.type_id) {
    case 1u:
        if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_web->data.data, value->ipAddress.addresses.data[value_ipAddress_addresses_index].address.as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.length > 0u && value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_ipAddress_addresses_element_coderAndPorts_length = (uint64_t)value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_ipAddress_addresses_element_coderAndPorts_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_ipAddress_addresses_element_coderAndPorts_index = 0u; value_ipAddress_addresses_element_coderAndPorts_index < value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.length; ++value_ipAddress_addresses_element_coderAndPorts_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.data[value_ipAddress_addresses_element_coderAndPorts_index].codec;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->ipAddress.addresses.data[value_ipAddress_addresses_index].coderAndPorts.data[value_ipAddress_addresses_element_coderAndPorts_index].port);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    }
    {
        uint64_t bits;
        memcpy(&bits, &value->latitude, sizeof(bits));
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, bits);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint64_t bits;
        memcpy(&bits, &value->longitude, sizeof(bits));
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, bits);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->type;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_cloud(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const cloud_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (value->data.length > 0u && value->data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_data_length = (uint64_t)value->data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_data_index = 0u; value_data_index < value->data.length; ++value_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->data.data[value_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_money_operation(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const money_operation_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->from);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->to);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->amount);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->time);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)value->credit);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->status;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_access_group(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_group_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->time);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->owner);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->data.length > 0u && value->data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_data_length = (uint64_t)value->data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_data_index = 0u; value_data_index < value->data.length; ++value_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->data.data[value_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_u_u_i_d_and_cloud(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const u_u_i_d_and_cloud_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->cloud.data.length > 0u && value->cloud.data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_cloud_data_length = (uint64_t)value->cloud.data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_cloud_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_cloud_data_index = 0u; value_cloud_data_index < value->cloud.data.length; ++value_cloud_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->cloud.data.data[value_cloud_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_cloud_config(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const cloud_config_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->subjectUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->configVersion);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->cloud.data.length > 0u && value->cloud.data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_cloud_data_length = (uint64_t)value->cloud.data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_cloud_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_cloud_data_index = 0u; value_cloud_data_index < value->cloud.data.length; ++value_cloud_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->cloud.data.data[value_cloud_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_applied_config(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const applied_config_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->subjectUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->configVersion);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_pair_keys(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_ref_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 3u:
        if ((*value).as.pair_keys_sign == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_sign->privateKey.type_id;
    switch ((*value).as.pair_keys_sign->privateKey.type_id) {
    case 6u:
        if ((*value).as.pair_keys_sign->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_sign->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_sign->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_sign->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_sign->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_sign->publicKey.type_id;
    switch ((*value).as.pair_keys_sign->publicKey.type_id) {
    case 6u:
        if ((*value).as.pair_keys_sign->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_sign->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_sign->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_sign->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_sign->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
        break;
    case 1u:
        if ((*value).as.pair_keys_asym == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_asym->privateKey.type_id;
    switch ((*value).as.pair_keys_asym->privateKey.type_id) {
    case 6u:
        if ((*value).as.pair_keys_asym->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_asym->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_asym->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_asym->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_asym->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_asym->publicKey.type_id;
    switch ((*value).as.pair_keys_asym->publicKey.type_id) {
    case 6u:
        if ((*value).as.pair_keys_asym->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_asym->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_asym->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_asym->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_asym->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
        break;
    case 2u:
        if ((*value).as.pair_keys_asym_signed == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_asym_signed->privateKey.type_id;
    switch ((*value).as.pair_keys_asym_signed->privateKey.type_id) {
    case 6u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_asym_signed->publicKey.key.type_id;
    switch ((*value).as.pair_keys_asym_signed->publicKey.key.type_id) {
    case 6u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.key.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_asym_signed->publicKey.sign.type_id;
    switch ((*value).as.pair_keys_asym_signed->publicKey.sign.type_id) {
    case 1u:
        if ((*value).as.pair_keys_asym_signed->publicKey.sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.sign.as.sign_a_e_e_d25519->data.data, 64u);
    pos += 64u;
        break;
    case 2u:
        if ((*value).as.pair_keys_asym_signed->publicKey.sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.sign.as.sign_h_y_d_r_o_g_e_n->data.data, 64u);
    pos += 64u;
        break;
    case 3u:
        if ((*value).as.pair_keys_asym_signed->publicKey.sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->publicKey.sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->publicKey.sign.as.sign_p256_aes_gcm->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
        break;
    case 4u:
        if ((*value).as.pair_keys_sym == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_sym->clientToServer.type_id;
    switch ((*value).as.pair_keys_sym->clientToServer.type_id) {
    case 6u:
        if ((*value).as.pair_keys_sym->clientToServer.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->clientToServer.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->clientToServer.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_sym->clientToServer.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->clientToServer.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->clientToServer.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_sym->clientToServer.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->clientToServer.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->clientToServer.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_sym->serverToClient.type_id;
    switch ((*value).as.pair_keys_sym->serverToClient.type_id) {
    case 6u:
        if ((*value).as.pair_keys_sym->serverToClient.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->serverToClient.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->serverToClient.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_sym->serverToClient.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->serverToClient.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->serverToClient.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_sym->serverToClient.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->serverToClient.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->serverToClient.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_pair_keys_sign(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_sign_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->privateKey.type_id;
    switch (value->privateKey.type_id) {
    case 6u:
        if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->publicKey.type_id;
    switch (value->publicKey.type_id) {
    case 6u:
        if (value->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_pair_keys_asym(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_asym_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->privateKey.type_id;
    switch (value->privateKey.type_id) {
    case 6u:
        if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->publicKey.type_id;
    switch (value->publicKey.type_id) {
    case 6u:
        if (value->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_pair_keys_asym_signed(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_asym_signed_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->privateKey.type_id;
    switch (value->privateKey.type_id) {
    case 6u:
        if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->publicKey.key.type_id;
    switch (value->publicKey.key.type_id) {
    case 6u:
        if (value->publicKey.key.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->publicKey.key.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->publicKey.key.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->publicKey.key.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->publicKey.key.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->publicKey.key.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->publicKey.key.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->publicKey.key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->publicKey.key.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->publicKey.key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->publicKey.key.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->publicKey.key.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->publicKey.key.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->publicKey.key.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->publicKey.key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.key.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->publicKey.sign.type_id;
    switch (value->publicKey.sign.type_id) {
    case 1u:
        if (value->publicKey.sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.sign.as.sign_a_e_e_d25519->data.data, 64u);
    pos += 64u;
        break;
    case 2u:
        if (value->publicKey.sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.sign.as.sign_h_y_d_r_o_g_e_n->data.data, 64u);
    pos += 64u;
        break;
    case 3u:
        if (value->publicKey.sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.sign.as.sign_p256_aes_gcm->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_pair_keys_sym(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_sym_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->clientToServer.type_id;
    switch (value->clientToServer.type_id) {
    case 6u:
        if (value->clientToServer.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->clientToServer.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->clientToServer.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->clientToServer.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->clientToServer.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->clientToServer.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->clientToServer.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->clientToServer.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->clientToServer.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->serverToClient.type_id;
    switch (value->serverToClient.type_id) {
    case 6u:
        if (value->serverToClient.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->serverToClient.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->serverToClient.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->serverToClient.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->serverToClient.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->serverToClient.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->serverToClient.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->serverToClient.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->serverToClient.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_access_check_pair(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_check_pair_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->sourceUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->targetUid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_access_check_result(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_check_result_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->sourceUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->targetUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)value->hasAccess);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_ip_info(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const ip_info_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->ip.type_id;
    switch (value->ip.type_id) {
    case 1u:
        if (value->ip.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ip.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->ip.as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if (value->ip.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ip.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->ip.as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if (value->ip.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ip.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->ip.as.i_p_address_web->data.data, value->ip.as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->port);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint64_t bits;
        memcpy(&bits, &value->latitude, sizeof(bits));
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, bits);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint64_t bits;
        memcpy(&bits, &value->longitude, sizeof(bits));
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, bits);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_p256_aes_gcm_symmetric(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_symmetric_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_p256_aes_gcm_curve_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_curve_private_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_p256_aes_gcm_curve_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_curve_public_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_p256_aes_gcm_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_sign_private_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_p256_aes_gcm_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_sign_public_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_reg_safe_serialize_sign_p256_aes_gcm(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_p256_aes_gcm_t *value) {

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}


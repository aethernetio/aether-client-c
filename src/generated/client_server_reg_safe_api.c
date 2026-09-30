#include "client_server_reg_safe_api.h"
#include "aether_meta_runtime.h"

#include <string.h>

aether_status_t server_registration_api_registration(
    server_registration_api_remote_t *api,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const global_api_stream_t * globalApi) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    uint8_t *tx = api->tx;
    size_t tx_capacity = api->tx_capacity;

    if (api->tx_capacity < 1u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 3u;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, salt.data, salt.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, suffix.data, suffix.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (passwords.length > 0u && passwords.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t passwords_length = (uint64_t)passwords.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, passwords_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t passwords_index = 0u; passwords_index < passwords.length; ++passwords_index) {
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)passwords.data[passwords_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, parent);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (globalApi == NULL) return AETHER_ERR_ARGUMENT;
    if (globalApi->length > globalApi->capacity) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*globalApi).data, (*globalApi).length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_registration_api_request_work_proof_data(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t parent,
    pow_method_t powMethods) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    uint8_t *tx = api->tx;
    size_t tx_capacity = api->tx_capacity;

    if (api->tx_capacity < 5u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 4u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, parent);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)powMethods;

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_registration_api_resolve_servers(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    const cloud_t * serverIds) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    uint8_t *tx = api->tx;
    size_t tx_capacity = api->tx_capacity;

    if (api->tx_capacity < 5u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 5u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    if (serverIds == NULL) return AETHER_ERR_ARGUMENT;
    if (serverIds->data.length > 0u && serverIds->data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t serverIds_data_length = (uint64_t)serverIds->data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, serverIds_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t serverIds_data_index = 0u; serverIds_data_index < serverIds->data.length; ++serverIds_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)serverIds->data.data[serverIds_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_registration_api_set_return_key(
    server_registration_api_remote_t *api,
    const key_ref_t * key) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    uint8_t *tx = api->tx;
    size_t tx_capacity = api->tx_capacity;

    if (api->tx_capacity < 1u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 6u;
    {
        aether_status_t meta_status = client_server_reg_safe_serialize_key(
            tx,
            tx_capacity,
            &pos,
            key);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_registration_api_registration_direct(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const key_ref_t * masterKey) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    uint8_t *tx = api->tx;
    size_t tx_capacity = api->tx_capacity;

    if (api->tx_capacity < 5u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 7u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, salt.data, salt.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, suffix.data, suffix.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (passwords.length > 0u && passwords.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t passwords_length = (uint64_t)passwords.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, passwords_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t passwords_index = 0u; passwords_index < passwords.length; ++passwords_index) {
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)passwords.data[passwords_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, parent);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_reg_safe_serialize_key(
            tx,
            tx_capacity,
            &pos,
            masterKey);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}


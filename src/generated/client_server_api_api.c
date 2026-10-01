#include "client_server_api_api.h"
#include "aether_meta_runtime.h"

#include <string.h>

typedef struct {
    size_t *length;
    size_t remaining;
} client_server_api_stream_commit_t;

static aether_status_t client_server_api_stream_commit(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    (void)data;

    client_server_api_stream_commit_t *commit =
        ctx;

    if (commit == NULL ||
        commit->length == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (length > commit->remaining) {
        return AETHER_ERR_OVERFLOW;
    }

    *commit->length += length;

    return AETHER_OK;
}

aether_status_t client_api_unsafe_send_safe_api_data_multi(
    client_api_unsafe_remote_t *api,
    int8_t backId,
    const login_client_stream_t * data) {

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
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)backId);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (data == NULL) return AETHER_ERR_ARGUMENT;
    if (data->length > data->capacity) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*data).data, (*data).length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_unsafe_send_safe_api_data(
    client_api_unsafe_remote_t *api,
    const login_client_stream_t * data) {

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
    api->tx[pos++] = 4u;
    if (data == NULL) return AETHER_ERR_ARGUMENT;
    if (data->length > data->capacity) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*data).data, (*data).length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_change_parent(
    client_api_safe_remote_t *api,
    aether_uuid_t uid) {

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
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_change_alias(
    client_api_safe_remote_t *api,
    aether_uuid_t alias) {

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
    api->tx[pos++] = 4u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, alias);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_new_children(
    client_api_safe_remote_t *api,
    uuid_array_view_t uids) {

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
    api->tx[pos++] = 5u;
    if (uids.length > 0u && uids.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t uids_length = (uint64_t)uids.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, uids_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t uids_index = 0u; uids_index < uids.length; ++uids_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uids.data[uids_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_messages(
    client_api_safe_remote_t *api,
    message_array_view_t msg) {

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
    if (msg.length > 0u && msg.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t msg_length = (uint64_t)msg.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, msg_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t msg_index = 0u; msg_index < msg.length; ++msg_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, msg.data[msg_index].uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, msg.data[msg_index].data.data, msg.data[msg_index].data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_server_descriptor(
    client_api_safe_remote_t *api,
    const server_descriptor_t * serverDescriptor) {

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
    api->tx[pos++] = 7u;
    if (serverDescriptor == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)serverDescriptor->id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (serverDescriptor->ipAddress.addresses.length > 0u && serverDescriptor->ipAddress.addresses.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t serverDescriptor_ipAddress_addresses_length = (uint64_t)serverDescriptor->ipAddress.addresses.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, serverDescriptor_ipAddress_addresses_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t serverDescriptor_ipAddress_addresses_index = 0u; serverDescriptor_ipAddress_addresses_index < serverDescriptor->ipAddress.addresses.length; ++serverDescriptor_ipAddress_addresses_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.type_id;
    switch (serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.type_id) {
    case 1u:
        if (serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if (serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if (serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if (serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.as.i_p_address_web->data.data, serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].address.as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].coderAndPorts.length > 0u && serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].coderAndPorts.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t serverDescriptor_ipAddress_addresses_element_coderAndPorts_length = (uint64_t)serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].coderAndPorts.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, serverDescriptor_ipAddress_addresses_element_coderAndPorts_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t serverDescriptor_ipAddress_addresses_element_coderAndPorts_index = 0u; serverDescriptor_ipAddress_addresses_element_coderAndPorts_index < serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].coderAndPorts.length; ++serverDescriptor_ipAddress_addresses_element_coderAndPorts_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].coderAndPorts.data[serverDescriptor_ipAddress_addresses_element_coderAndPorts_index].codec;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)serverDescriptor->ipAddress.addresses.data[serverDescriptor_ipAddress_addresses_index].coderAndPorts.data[serverDescriptor_ipAddress_addresses_element_coderAndPorts_index].port);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_server_descriptors(
    client_api_safe_remote_t *api,
    server_descriptor_array_view_t serverDescriptors) {

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
    api->tx[pos++] = 8u;
    if (serverDescriptors.length > 0u && serverDescriptors.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t serverDescriptors_length = (uint64_t)serverDescriptors.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, serverDescriptors_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t serverDescriptors_index = 0u; serverDescriptors_index < serverDescriptors.length; ++serverDescriptors_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)serverDescriptors.data[serverDescriptors_index].id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.length > 0u && serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t serverDescriptors_element_ipAddress_addresses_length = (uint64_t)serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, serverDescriptors_element_ipAddress_addresses_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t serverDescriptors_element_ipAddress_addresses_index = 0u; serverDescriptors_element_ipAddress_addresses_index < serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.length; ++serverDescriptors_element_ipAddress_addresses_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.type_id;
    switch (serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.type_id) {
    case 1u:
        if (serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if (serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if (serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if (serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.as.i_p_address_web->data.data, serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].address.as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].coderAndPorts.length > 0u && serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].coderAndPorts.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t serverDescriptors_element_ipAddress_addresses_element_coderAndPorts_length = (uint64_t)serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].coderAndPorts.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, serverDescriptors_element_ipAddress_addresses_element_coderAndPorts_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t serverDescriptors_element_ipAddress_addresses_element_coderAndPorts_index = 0u; serverDescriptors_element_ipAddress_addresses_element_coderAndPorts_index < serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].coderAndPorts.length; ++serverDescriptors_element_ipAddress_addresses_element_coderAndPorts_index) {
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].coderAndPorts.data[serverDescriptors_element_ipAddress_addresses_element_coderAndPorts_index].codec;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)serverDescriptors.data[serverDescriptors_index].ipAddress.addresses.data[serverDescriptors_element_ipAddress_addresses_index].coderAndPorts.data[serverDescriptors_element_ipAddress_addresses_element_coderAndPorts_index].port);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_cloud(
    client_api_safe_remote_t *api,
    const u_u_i_d_and_cloud_t * uidAndCloud) {

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
    api->tx[pos++] = 9u;
    if (uidAndCloud == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uidAndCloud->uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (uidAndCloud->cloud.data.length > 0u && uidAndCloud->cloud.data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t uidAndCloud_cloud_data_length = (uint64_t)uidAndCloud->cloud.data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, uidAndCloud_cloud_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t uidAndCloud_cloud_data_index = 0u; uidAndCloud_cloud_data_index < uidAndCloud->cloud.data.length; ++uidAndCloud_cloud_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)uidAndCloud->cloud.data.data[uidAndCloud_cloud_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_clouds(
    client_api_safe_remote_t *api,
    u_u_i_d_and_cloud_array_view_t clouds) {

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
    api->tx[pos++] = 10u;
    if (clouds.length > 0u && clouds.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t clouds_length = (uint64_t)clouds.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, clouds_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t clouds_index = 0u; clouds_index < clouds.length; ++clouds_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, clouds.data[clouds_index].uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (clouds.data[clouds_index].cloud.data.length > 0u && clouds.data[clouds_index].cloud.data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t clouds_element_cloud_data_length = (uint64_t)clouds.data[clouds_index].cloud.data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, clouds_element_cloud_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t clouds_element_cloud_data_index = 0u; clouds_element_cloud_data_index < clouds.data[clouds_index].cloud.data.length; ++clouds_element_cloud_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)clouds.data[clouds_index].cloud.data.data[clouds_element_cloud_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_request_telemetry(
    client_api_safe_remote_t *api) {

    if (api == NULL ||
        api->send == NULL ||
        api->tx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (api->tx_capacity < 1u) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = 0u;
    api->tx[pos++] = 11u;

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_access_groups(
    client_api_safe_remote_t *api,
    access_group_array_view_t groups) {

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
    api->tx[pos++] = 12u;
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index].id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)groups.data[groups_index].time);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index].owner);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.data[groups_index].data.length > 0u && groups.data[groups_index].data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_element_data_length = (uint64_t)groups.data[groups_index].data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_element_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_element_data_index = 0u; groups_element_data_index < groups.data[groups_index].data.length; ++groups_element_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index].data.data[groups_element_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_access_group_for_client(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

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
    api->tx[pos++] = 13u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_add_items_to_access_group(
    client_api_safe_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups) {

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
    api->tx[pos++] = 14u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_remove_items_from_access_group(
    client_api_safe_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups) {

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
    api->tx[pos++] = 15u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_add_access_groups_to_client(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

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
    api->tx[pos++] = 16u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_remove_access_groups_from_client(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

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
    api->tx[pos++] = 17u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_all_accessed_clients(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t accessedClients) {

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
    api->tx[pos++] = 18u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (accessedClients.length > 0u && accessedClients.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t accessedClients_length = (uint64_t)accessedClients.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, accessedClients_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t accessedClients_index = 0u; accessedClients_index < accessedClients.length; ++accessedClients_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, accessedClients.data[accessedClients_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_access_check_results(
    client_api_safe_remote_t *api,
    access_check_result_array_view_t results) {

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
    api->tx[pos++] = 19u;
    if (results.length > 0u && results.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t results_length = (uint64_t)results.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, results_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t results_index = 0u; results_index < results.length; ++results_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, results.data[results_index].sourceUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, results.data[results_index].targetUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)results.data[results_index].hasAccess);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_message(
    client_api_safe_remote_t *api,
    const message_t * msg) {

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
    api->tx[pos++] = 20u;
    if (msg == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, msg->uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, msg->data.data, msg->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_send_cloud_configs(
    client_api_safe_remote_t *api,
    cloud_config_array_view_t configs) {

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
    api->tx[pos++] = 21u;
    if (configs.length > 0u && configs.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t configs_length = (uint64_t)configs.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, configs_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t configs_index = 0u; configs_index < configs.length; ++configs_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, configs.data[configs_index].subjectUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)configs.data[configs_index].configVersion);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (configs.data[configs_index].cloud.data.length > 0u && configs.data[configs_index].cloud.data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t configs_element_cloud_data_length = (uint64_t)configs.data[configs_index].cloud.data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, configs_element_cloud_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t configs_element_cloud_data_index = 0u; configs_element_cloud_data_index < configs.data[configs_index].cloud.data.length; ++configs_element_cloud_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)configs.data[configs_index].cloud.data.data[configs_element_cloud_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_client_interaction(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    const client_interaction_client_stream_t * stream) {

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
    api->tx[pos++] = 22u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (stream == NULL) return AETHER_ERR_ARGUMENT;
    if (stream->length > stream->capacity) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*stream).data, (*stream).length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_safe_probe_report(
    client_api_safe_remote_t *api,
    const probe_report_t * report) {

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
    api->tx[pos++] = 23u;
    if (report == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)report->testId);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)report->firstSequence);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)report->count);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (report->samples.length > 0u && report->samples.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t report_samples_length = (uint64_t)report->samples.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, report_samples_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t report_samples_index = 0u; report_samples_index < report->samples.length; ++report_samples_index) {
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)report->samples.data[report_samples_index].sequence);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)report->samples.data[report_samples_index].receivedAtMs);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_back_id(
    authorized_api_remote_t *api,
    int8_t id) {

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
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)id);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_ping(
    authorized_api_remote_t *api,
    uint32_t request_id,
    int64_t nextConnectMsDuration,
    int64_t rxWindowMs) {

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
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)nextConnectMsDuration);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)rxWindowMs);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    const client_api_stream_t * stream) {

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
    api->tx[pos++] = 5u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (stream == NULL) return AETHER_ERR_ARGUMENT;
    if (stream->length > stream->capacity) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*stream).data, (*stream).length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_send_message(
    authorized_api_remote_t *api,
    const message_t * msg) {

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
    if (msg == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, msg->uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, msg->data.data, msg->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_send_messages(
    authorized_api_remote_t *api,
    message_array_view_t msg) {

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
    api->tx[pos++] = 7u;
    if (msg.length > 0u && msg.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t msg_length = (uint64_t)msg.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, msg_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t msg_index = 0u; msg_index < msg.length; ++msg_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, msg.data[msg_index].uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, msg.data[msg_index].data.data, msg.data[msg_index].data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_send_multicast(
    authorized_api_remote_t *api,
    uuid_array_view_t uids,
    aether_bytes_view_t data) {

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
    api->tx[pos++] = 37u;
    if (uids.length > 0u && uids.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t uids_length = (uint64_t)uids.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, uids_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t uids_index = 0u; uids_index < uids.length; ++uids_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uids.data[uids_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, data.data, data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_send_message_with_result(
    authorized_api_remote_t *api,
    uint32_t request_id,
    const message_t * msg) {

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
    api->tx[pos++] = 39u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    if (msg == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, msg->uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, msg->data.data, msg->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_create_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t owner,
    uuid_array_view_t uids) {

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
    api->tx[pos++] = 8u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, owner);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (uids.length > 0u && uids.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t uids_length = (uint64_t)uids.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, uids_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t uids_index = 0u; uids_index < uids.length; ++uids_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uids.data[uids_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_add_to_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid) {

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
    api->tx[pos++] = 9u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groupId);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_remove_from_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid) {

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
    api->tx[pos++] = 10u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groupId);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_check_access_for_send_message(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid) {

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
    api->tx[pos++] = 11u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_resolver_servers(
    authorized_api_remote_t *api,
    short_array_view_t sid) {

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
    api->tx[pos++] = 12u;
    if (sid.length > 0u && sid.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t sid_length = (uint64_t)sid.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, sid_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t sid_index = 0u; sid_index < sid.length; ++sid_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)sid.data[sid_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_resolve_clouds(
    authorized_api_remote_t *api,
    uuid_array_view_t uids) {

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
    api->tx[pos++] = 13u;
    if (uids.length > 0u && uids.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t uids_length = (uint64_t)uids.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, uids_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t uids_index = 0u; uids_index < uids.length; ++uids_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uids.data[uids_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_report_applied_config(
    authorized_api_remote_t *api,
    applied_config_array_view_t configs) {

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
    api->tx[pos++] = 38u;
    if (configs.length > 0u && configs.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t configs_length = (uint64_t)configs.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, configs_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t configs_index = 0u; configs_index < configs.length; ++configs_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, configs.data[configs_index].subjectUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)configs.data[configs_index].configVersion);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_get_access_groups(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid) {

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
    api->tx[pos++] = 14u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_get_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId) {

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
    api->tx[pos++] = 15u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groupId);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_get_all_accessed_clients(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid) {

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
    api->tx[pos++] = 16u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_check_access_for_send_message2(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid1,
    aether_uuid_t uid2) {

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
    api->tx[pos++] = 17u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid1);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid2);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_send_telemetry(
    authorized_api_remote_t *api,
    const telemetry_ref_t * telemetry) {

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
    api->tx[pos++] = 18u;
    {
        aether_status_t meta_status = client_server_api_serialize_telemetry(
            tx,
            tx_capacity,
            &pos,
            telemetry);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_request_access_groups_for_clients(
    authorized_api_remote_t *api,
    uuid_array_view_t uids) {

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
    api->tx[pos++] = 19u;
    if (uids.length > 0u && uids.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t uids_length = (uint64_t)uids.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, uids_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t uids_index = 0u; uids_index < uids.length; ++uids_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uids.data[uids_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_request_access_groups_items(
    authorized_api_remote_t *api,
    uuid_array_view_t ids) {

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
    api->tx[pos++] = 20u;
    if (ids.length > 0u && ids.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t ids_length = (uint64_t)ids.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, ids_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t ids_index = 0u; ids_index < ids.length; ++ids_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, ids.data[ids_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_send_access_group_for_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

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
    api->tx[pos++] = 22u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_add_items_to_access_group(
    authorized_api_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups) {

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
    api->tx[pos++] = 23u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_remove_items_from_access_group(
    authorized_api_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups) {

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
    api->tx[pos++] = 24u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_add_access_groups_to_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

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
    api->tx[pos++] = 25u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_remove_access_groups_from_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

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
    api->tx[pos++] = 26u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (groups.length > 0u && groups.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t groups_length = (uint64_t)groups.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, groups_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t groups_index = 0u; groups_index < groups.length; ++groups_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groups.data[groups_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_request_all_accessed_clients(
    authorized_api_remote_t *api,
    uuid_array_view_t uids) {

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
    api->tx[pos++] = 27u;
    if (uids.length > 0u && uids.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t uids_length = (uint64_t)uids.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, uids_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t uids_index = 0u; uids_index < uids.length; ++uids_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uids.data[uids_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_request_access_check(
    authorized_api_remote_t *api,
    access_check_pair_array_view_t requests) {

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
    api->tx[pos++] = 28u;
    if (requests.length > 0u && requests.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t requests_length = (uint64_t)requests.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, requests_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t requests_index = 0u; requests_index < requests.length; ++requests_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, requests.data[requests_index].sourceUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, requests.data[requests_index].targetUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_get_client_activity(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit) {

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
    api->tx[pos++] = 29u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)fromTime);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)toTime);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)limit);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_search_client_logs(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    aether_bytes_view_t query,
    int32_t limit) {

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
    api->tx[pos++] = 30u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, query.data, query.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)limit);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_get_client_connections(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    int32_t limit) {

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
    api->tx[pos++] = 31u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)limit);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_get_client_messages(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit) {

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
    api->tx[pos++] = 32u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)fromTime);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)toTime);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)limit);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_set_next_read_delay(
    authorized_api_remote_t *api,
    int64_t delayMillis) {

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
    api->tx[pos++] = 33u;
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)delayMillis);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_get_uap(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid) {

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
    api->tx[pos++] = 34u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_request_web_rtc_session(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid) {

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
    api->tx[pos++] = 40u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_publish_web_rtc_session(
    authorized_api_remote_t *api,
    const web_rtc_session_t * session) {

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
    api->tx[pos++] = 41u;
    if (session == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, session->sdp.data, session->sdp.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (session->candidates.length > 0u && session->candidates.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t session_candidates_length = (uint64_t)session->candidates.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, session_candidates_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t session_candidates_index = 0u; session_candidates_index < session->candidates.length; ++session_candidates_index) {
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, session->candidates.data[session_candidates_index].data.data, session->candidates.data[session_candidates_index].data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_self_destruct(
    authorized_api_remote_t *api,
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
    api->tx[pos++] = 42u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_get_servers(
    authorized_api_remote_t *api,
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
    api->tx[pos++] = 43u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_get_client_timing(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid) {

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
    api->tx[pos++] = 35u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_open_receive_window(
    authorized_api_remote_t *api,
    uint32_t request_id,
    int64_t durationMs) {

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
    api->tx[pos++] = 36u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)durationMs);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_switch_version(
    authorized_api_remote_t *api,
    int32_t version) {

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
    api->tx[pos++] = 44u;
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)version);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_set_receive_window(
    authorized_api_remote_t *api,
    int64_t startsInMs,
    int64_t durationMs) {

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
    api->tx[pos++] = 45u;
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)startsInMs);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)durationMs);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_probe_packet(
    authorized_api_remote_t *api,
    int32_t testId,
    int32_t sequence,
    aether_bytes_view_t payload) {

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
    api->tx[pos++] = 46u;
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)testId);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)sequence);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, payload.data, payload.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_request_probe_report(
    authorized_api_remote_t *api,
    int32_t testId,
    int32_t firstSequence,
    int32_t count) {

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
    api->tx[pos++] = 47u;
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)testId);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)firstSequence);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)count);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_get_alias(
    authorized_api_remote_t *api,
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
    api->tx[pos++] = 48u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t authorized_api_set_alias(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t alias) {

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
    api->tx[pos++] = 49u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, alias);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t login_api_get_time_u_t_c(
    login_api_remote_t *api,
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
    api->tx[pos++] = 3u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t login_api_login_by_u_i_d(
    login_api_remote_t *api,
    aether_uuid_t uid,
    const login_stream_t * data) {

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
    api->tx[pos++] = 4u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (data == NULL) return AETHER_ERR_ARGUMENT;
    if (data->length > data->capacity) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*data).data, (*data).length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t login_api_login_by_alias(
    login_api_remote_t *api,
    aether_uuid_t alias,
    const login_stream_t * data) {

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
    api->tx[pos++] = 5u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, alias);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (data == NULL) return AETHER_ERR_ARGUMENT;
    if (data->length > data->capacity) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*data).data, (*data).length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t login_api_get_my_ip(
    login_api_remote_t *api,
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
    api->tx[pos++] = 6u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_get_balance(
    server_api_by_uid_remote_t *api,
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
    api->tx[pos++] = 3u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_set_parent(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid) {

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
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_block(
    server_api_by_uid_remote_t *api,
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
    api->tx[pos++] = 5u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_get_position(
    server_api_by_uid_remote_t *api,
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
    api->tx[pos++] = 6u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_get_parent(
    server_api_by_uid_remote_t *api,
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
    api->tx[pos++] = 7u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_get_beneficiary(
    server_api_by_uid_remote_t *api,
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
    api->tx[pos++] = 8u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_set_beneficiary(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid) {

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
    api->tx[pos++] = 9u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_get_block_time(
    server_api_by_uid_remote_t *api,
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
    api->tx[pos++] = 10u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_unblock(
    server_api_by_uid_remote_t *api,
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
    api->tx[pos++] = 11u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_create_time(
    server_api_by_uid_remote_t *api,
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
    api->tx[pos++] = 12u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_online_time(
    server_api_by_uid_remote_t *api,
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
    api->tx[pos++] = 13u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_add_access_group(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId) {

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
    api->tx[pos++] = 14u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groupId);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_remove_access_group(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId) {

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
    api->tx[pos++] = 15u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, groupId);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_set_msg_queue_limit(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    int32_t limit) {

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
    api->tx[pos++] = 16u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)limit);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_set_msg_time_limit(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    int32_t seconds) {

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
    api->tx[pos++] = 17u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)seconds);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_add_servers_to_cloud(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    short_array_view_t sids) {

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
    api->tx[pos++] = 18u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    if (sids.length > 0u && sids.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t sids_length = (uint64_t)sids.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, sids_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t sids_index = 0u; sids_index < sids.length; ++sids_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)sids.data[sids_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t server_api_by_uid_remove_servers_from_cloud(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    short_array_view_t sids) {

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
    api->tx[pos++] = 19u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    if (sids.length > 0u && sids.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t sids_length = (uint64_t)sids.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, sids_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t sids_index = 0u; sids_index < sids.length; ++sids_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)sids.data[sids_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_reg_unsafe_enter(
    client_api_reg_unsafe_remote_t *api,
    const client_api_reg_safe_stream_t * stream) {

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
    if (stream == NULL) return AETHER_ERR_ARGUMENT;
    if (stream->length > stream->capacity) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*stream).data, (*stream).length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t client_api_reg_unsafe_enter_global(
    client_api_reg_unsafe_remote_t *api,
    const global_reg_client_api_stream_t * stream) {

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
    api->tx[pos++] = 4u;
    if (stream == NULL) return AETHER_ERR_ARGUMENT;
    if (stream->length > stream->capacity) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*stream).data, (*stream).length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t global_reg_server_api_set_master_key(
    global_reg_server_api_remote_t *api,
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
    api->tx[pos++] = 3u;
    {
        aether_status_t meta_status = client_server_api_serialize_key(
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
        aether_status_t meta_status = client_server_api_serialize_key(
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
        aether_status_t meta_status = client_server_api_serialize_key(
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

aether_status_t server_registration_api_recovery(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
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
    api->tx[pos++] = 8u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_key(
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

aether_status_t registration_root_api_get_asymmetric_public_key(
    registration_root_api_remote_t *api,
    uint32_t request_id,
    crypto_lib_t cryptoLib) {

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
    api->tx[pos++] = 3u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)cryptoLib;

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t registration_root_api_enter(
    registration_root_api_remote_t *api,
    crypto_lib_t cryptoLib,
    const server_registration_api_stream_t * stream) {

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
    api->tx[pos++] = 4u;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)cryptoLib;
    if (stream == NULL) return AETHER_ERR_ARGUMENT;
    if (stream->length > stream->capacity) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*stream).data, (*stream).length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t registration_root_api_get_my_ip(
    registration_root_api_remote_t *api,
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
    api->tx[pos++] = 6u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

aether_status_t login_stream_authorized_api_back_id(
    login_stream_t *builder_stream,
    int8_t id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_back_id(
        &remote,
        id);
}

aether_status_t login_stream_authorized_api_ping(
    login_stream_t *builder_stream,
    uint32_t request_id,
    int64_t nextConnectMsDuration,
    int64_t rxWindowMs) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_ping(
        &remote,
        request_id,
        nextConnectMsDuration,
        rxWindowMs);
}

aether_status_t login_stream_authorized_api_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    const client_api_stream_t * stream) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_client(
        &remote,
        uid,
        stream);
}

aether_status_t login_stream_authorized_api_send_message(
    login_stream_t *builder_stream,
    const message_t * msg) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_send_message(
        &remote,
        msg);
}

aether_status_t login_stream_authorized_api_send_messages(
    login_stream_t *builder_stream,
    message_array_view_t msg) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_send_messages(
        &remote,
        msg);
}

aether_status_t login_stream_authorized_api_send_multicast(
    login_stream_t *builder_stream,
    uuid_array_view_t uids,
    aether_bytes_view_t data) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_send_multicast(
        &remote,
        uids,
        data);
}

aether_status_t login_stream_authorized_api_send_message_with_result(
    login_stream_t *builder_stream,
    uint32_t request_id,
    const message_t * msg) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_send_message_with_result(
        &remote,
        request_id,
        msg);
}

aether_status_t login_stream_authorized_api_create_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t owner,
    uuid_array_view_t uids) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_create_access_group(
        &remote,
        request_id,
        owner,
        uids);
}

aether_status_t login_stream_authorized_api_add_to_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_add_to_access_group(
        &remote,
        request_id,
        groupId,
        uid);
}

aether_status_t login_stream_authorized_api_remove_from_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_remove_from_access_group(
        &remote,
        request_id,
        groupId,
        uid);
}

aether_status_t login_stream_authorized_api_check_access_for_send_message(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_check_access_for_send_message(
        &remote,
        request_id,
        uid);
}

aether_status_t login_stream_authorized_api_resolver_servers(
    login_stream_t *builder_stream,
    short_array_view_t sid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_resolver_servers(
        &remote,
        sid);
}

aether_status_t login_stream_authorized_api_resolve_clouds(
    login_stream_t *builder_stream,
    uuid_array_view_t uids) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_resolve_clouds(
        &remote,
        uids);
}

aether_status_t login_stream_authorized_api_report_applied_config(
    login_stream_t *builder_stream,
    applied_config_array_view_t configs) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_report_applied_config(
        &remote,
        configs);
}

aether_status_t login_stream_authorized_api_get_access_groups(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_get_access_groups(
        &remote,
        request_id,
        uid);
}

aether_status_t login_stream_authorized_api_get_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_get_access_group(
        &remote,
        request_id,
        groupId);
}

aether_status_t login_stream_authorized_api_get_all_accessed_clients(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_get_all_accessed_clients(
        &remote,
        request_id,
        uid);
}

aether_status_t login_stream_authorized_api_check_access_for_send_message2(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid1,
    aether_uuid_t uid2) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_check_access_for_send_message2(
        &remote,
        request_id,
        uid1,
        uid2);
}

aether_status_t login_stream_authorized_api_send_telemetry(
    login_stream_t *builder_stream,
    const telemetry_ref_t * telemetry) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_send_telemetry(
        &remote,
        telemetry);
}

aether_status_t login_stream_authorized_api_request_access_groups_for_clients(
    login_stream_t *builder_stream,
    uuid_array_view_t uids) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_request_access_groups_for_clients(
        &remote,
        uids);
}

aether_status_t login_stream_authorized_api_request_access_groups_items(
    login_stream_t *builder_stream,
    uuid_array_view_t ids) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_request_access_groups_items(
        &remote,
        ids);
}

aether_status_t login_stream_authorized_api_send_access_group_for_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_send_access_group_for_client(
        &remote,
        uid,
        groups);
}

aether_status_t login_stream_authorized_api_add_items_to_access_group(
    login_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_add_items_to_access_group(
        &remote,
        id,
        groups);
}

aether_status_t login_stream_authorized_api_remove_items_from_access_group(
    login_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_remove_items_from_access_group(
        &remote,
        id,
        groups);
}

aether_status_t login_stream_authorized_api_add_access_groups_to_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_add_access_groups_to_client(
        &remote,
        uid,
        groups);
}

aether_status_t login_stream_authorized_api_remove_access_groups_from_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_remove_access_groups_from_client(
        &remote,
        uid,
        groups);
}

aether_status_t login_stream_authorized_api_request_all_accessed_clients(
    login_stream_t *builder_stream,
    uuid_array_view_t uids) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_request_all_accessed_clients(
        &remote,
        uids);
}

aether_status_t login_stream_authorized_api_request_access_check(
    login_stream_t *builder_stream,
    access_check_pair_array_view_t requests) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_request_access_check(
        &remote,
        requests);
}

aether_status_t login_stream_authorized_api_get_client_activity(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_get_client_activity(
        &remote,
        request_id,
        uid,
        fromTime,
        toTime,
        limit);
}

aether_status_t login_stream_authorized_api_search_client_logs(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    aether_bytes_view_t query,
    int32_t limit) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_search_client_logs(
        &remote,
        request_id,
        uid,
        query,
        limit);
}

aether_status_t login_stream_authorized_api_get_client_connections(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    int32_t limit) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_get_client_connections(
        &remote,
        request_id,
        uid,
        limit);
}

aether_status_t login_stream_authorized_api_get_client_messages(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_get_client_messages(
        &remote,
        request_id,
        uid,
        fromTime,
        toTime,
        limit);
}

aether_status_t login_stream_authorized_api_set_next_read_delay(
    login_stream_t *builder_stream,
    int64_t delayMillis) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_set_next_read_delay(
        &remote,
        delayMillis);
}

aether_status_t login_stream_authorized_api_get_uap(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_get_uap(
        &remote,
        request_id,
        uid);
}

aether_status_t login_stream_authorized_api_request_web_rtc_session(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_request_web_rtc_session(
        &remote,
        request_id,
        uid);
}

aether_status_t login_stream_authorized_api_publish_web_rtc_session(
    login_stream_t *builder_stream,
    const web_rtc_session_t * session) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_publish_web_rtc_session(
        &remote,
        session);
}

aether_status_t login_stream_authorized_api_self_destruct(
    login_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_self_destruct(
        &remote,
        request_id);
}

aether_status_t login_stream_authorized_api_get_servers(
    login_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_get_servers(
        &remote,
        request_id);
}

aether_status_t login_stream_authorized_api_get_client_timing(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_get_client_timing(
        &remote,
        request_id,
        uid);
}

aether_status_t login_stream_authorized_api_open_receive_window(
    login_stream_t *builder_stream,
    uint32_t request_id,
    int64_t durationMs) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_open_receive_window(
        &remote,
        request_id,
        durationMs);
}

aether_status_t login_stream_authorized_api_switch_version(
    login_stream_t *builder_stream,
    int32_t version) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_switch_version(
        &remote,
        version);
}

aether_status_t login_stream_authorized_api_set_receive_window(
    login_stream_t *builder_stream,
    int64_t startsInMs,
    int64_t durationMs) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_set_receive_window(
        &remote,
        startsInMs,
        durationMs);
}

aether_status_t login_stream_authorized_api_probe_packet(
    login_stream_t *builder_stream,
    int32_t testId,
    int32_t sequence,
    aether_bytes_view_t payload) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_probe_packet(
        &remote,
        testId,
        sequence,
        payload);
}

aether_status_t login_stream_authorized_api_request_probe_report(
    login_stream_t *builder_stream,
    int32_t testId,
    int32_t firstSequence,
    int32_t count) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_request_probe_report(
        &remote,
        testId,
        firstSequence,
        count);
}

aether_status_t login_stream_authorized_api_get_alias(
    login_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_get_alias(
        &remote,
        request_id);
}

aether_status_t login_stream_authorized_api_set_alias(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t alias) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    authorized_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return authorized_api_set_alias(
        &remote,
        request_id,
        alias);
}

aether_status_t login_client_stream_client_api_safe_change_parent(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_change_parent(
        &remote,
        uid);
}

aether_status_t login_client_stream_client_api_safe_change_alias(
    login_client_stream_t *builder_stream,
    aether_uuid_t alias) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_change_alias(
        &remote,
        alias);
}

aether_status_t login_client_stream_client_api_safe_new_children(
    login_client_stream_t *builder_stream,
    uuid_array_view_t uids) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_new_children(
        &remote,
        uids);
}

aether_status_t login_client_stream_client_api_safe_send_messages(
    login_client_stream_t *builder_stream,
    message_array_view_t msg) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_messages(
        &remote,
        msg);
}

aether_status_t login_client_stream_client_api_safe_send_server_descriptor(
    login_client_stream_t *builder_stream,
    const server_descriptor_t * serverDescriptor) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_server_descriptor(
        &remote,
        serverDescriptor);
}

aether_status_t login_client_stream_client_api_safe_send_server_descriptors(
    login_client_stream_t *builder_stream,
    server_descriptor_array_view_t serverDescriptors) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_server_descriptors(
        &remote,
        serverDescriptors);
}

aether_status_t login_client_stream_client_api_safe_send_cloud(
    login_client_stream_t *builder_stream,
    const u_u_i_d_and_cloud_t * uidAndCloud) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_cloud(
        &remote,
        uidAndCloud);
}

aether_status_t login_client_stream_client_api_safe_send_clouds(
    login_client_stream_t *builder_stream,
    u_u_i_d_and_cloud_array_view_t clouds) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_clouds(
        &remote,
        clouds);
}

aether_status_t login_client_stream_client_api_safe_request_telemetry(
    login_client_stream_t *builder_stream) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_request_telemetry(
        &remote);
}

aether_status_t login_client_stream_client_api_safe_send_access_groups(
    login_client_stream_t *builder_stream,
    access_group_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_access_groups(
        &remote,
        groups);
}

aether_status_t login_client_stream_client_api_safe_send_access_group_for_client(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_access_group_for_client(
        &remote,
        uid,
        groups);
}

aether_status_t login_client_stream_client_api_safe_add_items_to_access_group(
    login_client_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_add_items_to_access_group(
        &remote,
        id,
        groups);
}

aether_status_t login_client_stream_client_api_safe_remove_items_from_access_group(
    login_client_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_remove_items_from_access_group(
        &remote,
        id,
        groups);
}

aether_status_t login_client_stream_client_api_safe_add_access_groups_to_client(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_add_access_groups_to_client(
        &remote,
        uid,
        groups);
}

aether_status_t login_client_stream_client_api_safe_remove_access_groups_from_client(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_remove_access_groups_from_client(
        &remote,
        uid,
        groups);
}

aether_status_t login_client_stream_client_api_safe_send_all_accessed_clients(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t accessedClients) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_all_accessed_clients(
        &remote,
        uid,
        accessedClients);
}

aether_status_t login_client_stream_client_api_safe_send_access_check_results(
    login_client_stream_t *builder_stream,
    access_check_result_array_view_t results) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_access_check_results(
        &remote,
        results);
}

aether_status_t login_client_stream_client_api_safe_send_message(
    login_client_stream_t *builder_stream,
    const message_t * msg) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_message(
        &remote,
        msg);
}

aether_status_t login_client_stream_client_api_safe_send_cloud_configs(
    login_client_stream_t *builder_stream,
    cloud_config_array_view_t configs) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_send_cloud_configs(
        &remote,
        configs);
}

aether_status_t login_client_stream_client_api_safe_client_interaction(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    const client_interaction_client_stream_t * stream) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_client_interaction(
        &remote,
        uid,
        stream);
}

aether_status_t login_client_stream_client_api_safe_probe_report(
    login_client_stream_t *builder_stream,
    const probe_report_t * report) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    client_api_safe_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return client_api_safe_probe_report(
        &remote,
        report);
}

aether_status_t client_api_stream_server_api_by_uid_get_balance(
    client_api_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_get_balance(
        &remote,
        request_id);
}

aether_status_t client_api_stream_server_api_by_uid_set_parent(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_set_parent(
        &remote,
        request_id,
        uid);
}

aether_status_t client_api_stream_server_api_by_uid_block(
    client_api_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_block(
        &remote,
        request_id);
}

aether_status_t client_api_stream_server_api_by_uid_get_position(
    client_api_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_get_position(
        &remote,
        request_id);
}

aether_status_t client_api_stream_server_api_by_uid_get_parent(
    client_api_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_get_parent(
        &remote,
        request_id);
}

aether_status_t client_api_stream_server_api_by_uid_get_beneficiary(
    client_api_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_get_beneficiary(
        &remote,
        request_id);
}

aether_status_t client_api_stream_server_api_by_uid_set_beneficiary(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_set_beneficiary(
        &remote,
        request_id,
        uid);
}

aether_status_t client_api_stream_server_api_by_uid_get_block_time(
    client_api_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_get_block_time(
        &remote,
        request_id);
}

aether_status_t client_api_stream_server_api_by_uid_unblock(
    client_api_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_unblock(
        &remote,
        request_id);
}

aether_status_t client_api_stream_server_api_by_uid_create_time(
    client_api_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_create_time(
        &remote,
        request_id);
}

aether_status_t client_api_stream_server_api_by_uid_online_time(
    client_api_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_online_time(
        &remote,
        request_id);
}

aether_status_t client_api_stream_server_api_by_uid_add_access_group(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_add_access_group(
        &remote,
        request_id,
        groupId);
}

aether_status_t client_api_stream_server_api_by_uid_remove_access_group(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_remove_access_group(
        &remote,
        request_id,
        groupId);
}

aether_status_t client_api_stream_server_api_by_uid_set_msg_queue_limit(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    int32_t limit) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_set_msg_queue_limit(
        &remote,
        request_id,
        limit);
}

aether_status_t client_api_stream_server_api_by_uid_set_msg_time_limit(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    int32_t seconds) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_set_msg_time_limit(
        &remote,
        request_id,
        seconds);
}

aether_status_t client_api_stream_server_api_by_uid_add_servers_to_cloud(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    short_array_view_t sids) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_add_servers_to_cloud(
        &remote,
        request_id,
        sids);
}

aether_status_t client_api_stream_server_api_by_uid_remove_servers_from_cloud(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    short_array_view_t sids) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_api_by_uid_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_api_by_uid_remove_servers_from_cloud(
        &remote,
        request_id,
        sids);
}

aether_status_t global_api_stream_global_reg_server_api_set_master_key(
    global_api_stream_t *builder_stream,
    const key_ref_t * key) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    global_reg_server_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return global_reg_server_api_set_master_key(
        &remote,
        key);
}

aether_status_t global_api_stream_global_reg_server_api_finish(
    global_api_stream_t *builder_stream,
    uint32_t request_id) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    global_reg_server_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return global_reg_server_api_finish(
        &remote,
        request_id);
}

aether_status_t server_registration_api_stream_server_registration_api_registration(
    server_registration_api_stream_t *builder_stream,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const global_api_stream_t * globalApi) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_registration_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_registration_api_registration(
        &remote,
        salt,
        suffix,
        passwords,
        parent,
        globalApi);
}

aether_status_t server_registration_api_stream_server_registration_api_registration_direct(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const key_ref_t * masterKey) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_registration_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_registration_api_registration_direct(
        &remote,
        request_id,
        salt,
        suffix,
        passwords,
        parent,
        masterKey);
}

aether_status_t server_registration_api_stream_server_registration_api_request_work_proof_data(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t parent,
    pow_method_t powMethods) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_registration_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_registration_api_request_work_proof_data(
        &remote,
        request_id,
        parent,
        powMethods);
}

aether_status_t server_registration_api_stream_server_registration_api_resolve_servers(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    const cloud_t * serverIds) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_registration_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_registration_api_resolve_servers(
        &remote,
        request_id,
        serverIds);
}

aether_status_t server_registration_api_stream_server_registration_api_set_return_key(
    server_registration_api_stream_t *builder_stream,
    const key_ref_t * key) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_registration_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_registration_api_set_return_key(
        &remote,
        key);
}

aether_status_t server_registration_api_stream_server_registration_api_recovery(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    const key_ref_t * masterKey) {

    if (builder_stream == NULL ||
        builder_stream->length > builder_stream->capacity) {
        return AETHER_ERR_ARGUMENT;
    }

    if (builder_stream->capacity > 0u &&
        builder_stream->data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t remaining =
        builder_stream->capacity - builder_stream->length;

    if (remaining == 0u) {
        return AETHER_ERR_OVERFLOW;
    }

    client_server_api_stream_commit_t commit = {
        &builder_stream->length,
        remaining
    };

    server_registration_api_remote_t remote = {
        &commit,
        client_server_api_stream_commit,
        builder_stream->data + builder_stream->length,
        remaining
    };

    return server_registration_api_recovery(
        &remote,
        request_id,
        uid,
        masterKey);
}


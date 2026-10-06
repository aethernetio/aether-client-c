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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:140 api.ClientApiUnsafe.methods.sendSafeApiDataMulti */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:140 api.ClientApiUnsafe.methods.sendSafeApiDataMulti */
aether_status_t client_api_unsafe_send_safe_api_data_multi(
    client_api_unsafe_remote_t *api,
    int16_t backId,
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
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)backId);
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:145 api.ClientApiUnsafe.methods.sendSafeApiData */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:145 api.ClientApiUnsafe.methods.sendSafeApiData */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:152 api.ClientApiSafe.methods.changeParent */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:152 api.ClientApiSafe.methods.changeParent */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:156 api.ClientApiSafe.methods.changeAlias */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:156 api.ClientApiSafe.methods.changeAlias */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:160 api.ClientApiSafe.methods.newChildren */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:160 api.ClientApiSafe.methods.newChildren */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:164 api.ClientApiSafe.methods.sendMessages */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:164 api.ClientApiSafe.methods.sendMessages */
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
        aether_status_t meta_status = client_server_api_serialize_message(
            tx,
            tx_capacity,
            &pos,
            (&(msg.data[msg_index])));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:168 api.ClientApiSafe.methods.sendServerDescriptor */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:168 api.ClientApiSafe.methods.sendServerDescriptor */
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
    {
        aether_status_t meta_status = client_server_api_serialize_server_descriptor(
            tx,
            tx_capacity,
            &pos,
            serverDescriptor);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:172 api.ClientApiSafe.methods.sendServerDescriptors */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:172 api.ClientApiSafe.methods.sendServerDescriptors */
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
        aether_status_t meta_status = client_server_api_serialize_server_descriptor(
            tx,
            tx_capacity,
            &pos,
            (&(serverDescriptors.data[serverDescriptors_index])));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:176 api.ClientApiSafe.methods.sendCloud */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:176 api.ClientApiSafe.methods.sendCloud */
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
    {
        aether_status_t meta_status = client_server_api_serialize_u_u_i_d_and_cloud(
            tx,
            tx_capacity,
            &pos,
            uidAndCloud);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:180 api.ClientApiSafe.methods.sendClouds */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:180 api.ClientApiSafe.methods.sendClouds */
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
        aether_status_t meta_status = client_server_api_serialize_u_u_i_d_and_cloud(
            tx,
            tx_capacity,
            &pos,
            (&(clouds.data[clouds_index])));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:184 api.ClientApiSafe.methods.requestTelemetry */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:184 api.ClientApiSafe.methods.requestTelemetry */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:186 api.ClientApiSafe.methods.sendAccessGroups */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:186 api.ClientApiSafe.methods.sendAccessGroups */
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
        aether_status_t meta_status = client_server_api_serialize_access_group(
            tx,
            tx_capacity,
            &pos,
            (&(groups.data[groups_index])));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:190 api.ClientApiSafe.methods.sendAccessGroupForClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:190 api.ClientApiSafe.methods.sendAccessGroupForClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:195 api.ClientApiSafe.methods.addItemsToAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:195 api.ClientApiSafe.methods.addItemsToAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:200 api.ClientApiSafe.methods.removeItemsFromAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:200 api.ClientApiSafe.methods.removeItemsFromAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:205 api.ClientApiSafe.methods.addAccessGroupsToClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:205 api.ClientApiSafe.methods.addAccessGroupsToClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:210 api.ClientApiSafe.methods.removeAccessGroupsFromClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:210 api.ClientApiSafe.methods.removeAccessGroupsFromClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:215 api.ClientApiSafe.methods.sendAllAccessedClients */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:215 api.ClientApiSafe.methods.sendAllAccessedClients */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:220 api.ClientApiSafe.methods.sendAccessCheckResults */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:220 api.ClientApiSafe.methods.sendAccessCheckResults */
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
        aether_status_t meta_status = client_server_api_serialize_access_check_result(
            tx,
            tx_capacity,
            &pos,
            (&(results.data[results_index])));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:225 api.ClientApiSafe.methods.sendMessage */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:225 api.ClientApiSafe.methods.sendMessage */
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
    {
        aether_status_t meta_status = client_server_api_serialize_message(
            tx,
            tx_capacity,
            &pos,
            msg);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:230 api.ClientApiSafe.methods.sendCloudConfigs */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:230 api.ClientApiSafe.methods.sendCloudConfigs */
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
        aether_status_t meta_status = client_server_api_serialize_cloud_config(
            tx,
            tx_capacity,
            &pos,
            (&(configs.data[configs_index])));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:235 api.ClientApiSafe.methods.clientInteraction */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:235 api.ClientApiSafe.methods.clientInteraction */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:240 api.ClientApiSafe.methods.probeReport */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:240 api.ClientApiSafe.methods.probeReport */
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
    {
        aether_status_t meta_status = client_server_api_serialize_probe_report(
            tx,
            tx_capacity,
            &pos,
            report);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:247 api.AuthorizedApi.methods.backId */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:247 api.AuthorizedApi.methods.backId */
aether_status_t authorized_api_back_id(
    authorized_api_remote_t *api,
    int16_t id) {

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
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)id);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:252 api.AuthorizedApi.methods.ping */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:252 api.AuthorizedApi.methods.ping */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:259 api.AuthorizedApi.methods.client */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:259 api.AuthorizedApi.methods.client */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:264 api.AuthorizedApi.methods.sendMessage */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:264 api.AuthorizedApi.methods.sendMessage */
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
    {
        aether_status_t meta_status = client_server_api_serialize_message(
            tx,
            tx_capacity,
            &pos,
            msg);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:268 api.AuthorizedApi.methods.sendMessages */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:268 api.AuthorizedApi.methods.sendMessages */
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
        aether_status_t meta_status = client_server_api_serialize_message(
            tx,
            tx_capacity,
            &pos,
            (&(msg.data[msg_index])));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:273 api.AuthorizedApi.methods.sendMulticast */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:273 api.AuthorizedApi.methods.sendMulticast */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:279 api.AuthorizedApi.methods.sendMessageWithResult */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:279 api.AuthorizedApi.methods.sendMessageWithResult */
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
    {
        aether_status_t meta_status = client_server_api_serialize_message(
            tx,
            tx_capacity,
            &pos,
            msg);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:288 api.AuthorizedApi.methods.createAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:288 api.AuthorizedApi.methods.createAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:294 api.AuthorizedApi.methods.addToAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:294 api.AuthorizedApi.methods.addToAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:300 api.AuthorizedApi.methods.removeFromAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:300 api.AuthorizedApi.methods.removeFromAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:306 api.AuthorizedApi.methods.checkAccessForSendMessage */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:306 api.AuthorizedApi.methods.checkAccessForSendMessage */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:311 api.AuthorizedApi.methods.resolverServers */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:311 api.AuthorizedApi.methods.resolverServers */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:315 api.AuthorizedApi.methods.resolveClouds */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:315 api.AuthorizedApi.methods.resolveClouds */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:321 api.AuthorizedApi.methods.reportAppliedConfig */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:321 api.AuthorizedApi.methods.reportAppliedConfig */
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
        aether_status_t meta_status = client_server_api_serialize_applied_config(
            tx,
            tx_capacity,
            &pos,
            (&(configs.data[configs_index])));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:327 api.AuthorizedApi.methods.getAccessGroups */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:327 api.AuthorizedApi.methods.getAccessGroups */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:332 api.AuthorizedApi.methods.getAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:332 api.AuthorizedApi.methods.getAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:337 api.AuthorizedApi.methods.getAllAccessedClients */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:337 api.AuthorizedApi.methods.getAllAccessedClients */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:342 api.AuthorizedApi.methods.checkAccessForSendMessage2 */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:342 api.AuthorizedApi.methods.checkAccessForSendMessage2 */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:348 api.AuthorizedApi.methods.sendTelemetry */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:348 api.AuthorizedApi.methods.sendTelemetry */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:352 api.AuthorizedApi.methods.requestAccessGroupsForClients */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:352 api.AuthorizedApi.methods.requestAccessGroupsForClients */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:356 api.AuthorizedApi.methods.requestAccessGroupsItems */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:356 api.AuthorizedApi.methods.requestAccessGroupsItems */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:360 api.AuthorizedApi.methods.sendAccessGroupForClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:360 api.AuthorizedApi.methods.sendAccessGroupForClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:365 api.AuthorizedApi.methods.addItemsToAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:365 api.AuthorizedApi.methods.addItemsToAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:370 api.AuthorizedApi.methods.removeItemsFromAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:370 api.AuthorizedApi.methods.removeItemsFromAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:375 api.AuthorizedApi.methods.addAccessGroupsToClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:375 api.AuthorizedApi.methods.addAccessGroupsToClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:380 api.AuthorizedApi.methods.removeAccessGroupsFromClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:380 api.AuthorizedApi.methods.removeAccessGroupsFromClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:385 api.AuthorizedApi.methods.requestAllAccessedClients */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:385 api.AuthorizedApi.methods.requestAllAccessedClients */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:389 api.AuthorizedApi.methods.requestAccessCheck */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:389 api.AuthorizedApi.methods.requestAccessCheck */
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
        aether_status_t meta_status = client_server_api_serialize_access_check_pair(
            tx,
            tx_capacity,
            &pos,
            (&(requests.data[requests_index])));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:394 api.AuthorizedApi.methods.getClientActivity */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:394 api.AuthorizedApi.methods.getClientActivity */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:402 api.AuthorizedApi.methods.searchClientLogs */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:402 api.AuthorizedApi.methods.searchClientLogs */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:409 api.AuthorizedApi.methods.getClientConnections */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:409 api.AuthorizedApi.methods.getClientConnections */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:415 api.AuthorizedApi.methods.getClientMessages */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:415 api.AuthorizedApi.methods.getClientMessages */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:426 api.AuthorizedApi.methods.setNextReadDelay */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:426 api.AuthorizedApi.methods.setNextReadDelay */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:430 api.AuthorizedApi.methods.getUap */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:430 api.AuthorizedApi.methods.getUap */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:440 api.AuthorizedApi.methods.requestWebRtcSession */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:440 api.AuthorizedApi.methods.requestWebRtcSession */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:446 api.AuthorizedApi.methods.publishWebRtcSession */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:446 api.AuthorizedApi.methods.publishWebRtcSession */
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
    {
        aether_status_t meta_status = client_server_api_serialize_web_rtc_session(
            tx,
            tx_capacity,
            &pos,
            session);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:452 api.AuthorizedApi.methods.selfDestruct */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:452 api.AuthorizedApi.methods.selfDestruct */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:456 api.AuthorizedApi.methods.getServers */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:456 api.AuthorizedApi.methods.getServers */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:462 api.AuthorizedApi.methods.getClientTiming */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:462 api.AuthorizedApi.methods.getClientTiming */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:468 api.AuthorizedApi.methods.openReceiveWindow */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:468 api.AuthorizedApi.methods.openReceiveWindow */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:475 api.AuthorizedApi.methods.switchVersion */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:475 api.AuthorizedApi.methods.switchVersion */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:482 api.AuthorizedApi.methods.setReceiveWindow */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:482 api.AuthorizedApi.methods.setReceiveWindow */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:487 api.AuthorizedApi.methods.probePacket */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:487 api.AuthorizedApi.methods.probePacket */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:493 api.AuthorizedApi.methods.requestProbeReport */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:493 api.AuthorizedApi.methods.requestProbeReport */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:499 api.AuthorizedApi.methods.getAlias */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:499 api.AuthorizedApi.methods.getAlias */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:502 api.AuthorizedApi.methods.setAlias */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:502 api.AuthorizedApi.methods.setAlias */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:508 api.AuthorizedApi.methods.issuePeerIdentity */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:508 api.AuthorizedApi.methods.issuePeerIdentity */
aether_status_t authorized_api_issue_peer_identity(
    authorized_api_remote_t *api,
    uint32_t request_id,
    const key_sign_public_ref_t * key) {

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
    api->tx[pos++] = 50u;
    api->tx[pos++] = (uint8_t)((request_id >> 0u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    api->tx[pos++] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));
    {
        aether_status_t meta_status = client_server_api_serialize_key_sign_public(
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:516 api.LoginApi.methods.getTimeUTC */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:516 api.LoginApi.methods.getTimeUTC */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:519 api.LoginApi.methods.loginByUID */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:519 api.LoginApi.methods.loginByUID */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:524 api.LoginApi.methods.loginByAlias */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:524 api.LoginApi.methods.loginByAlias */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:530 api.LoginApi.methods.getMyIp */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:530 api.LoginApi.methods.getMyIp */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:535 api.LoginApi.methods.loginByUIDMulti */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:535 api.LoginApi.methods.loginByUIDMulti */
aether_status_t login_api_login_by_u_i_d_multi(
    login_api_remote_t *api,
    aether_uuid_t uid,
    int16_t backId,
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
    api->tx[pos++] = 7u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)backId);
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:541 api.LoginApi.methods.loginByAliasMulti */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:541 api.LoginApi.methods.loginByAliasMulti */
aether_status_t login_api_login_by_alias_multi(
    login_api_remote_t *api,
    aether_uuid_t alias,
    int16_t backId,
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
    api->tx[pos++] = 8u;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, alias);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)backId);
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:551 api.ServerApiByUid.methods.getBalance */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:551 api.ServerApiByUid.methods.getBalance */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:554 api.ServerApiByUid.methods.setParent */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:554 api.ServerApiByUid.methods.setParent */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:559 api.ServerApiByUid.methods.block */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:559 api.ServerApiByUid.methods.block */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:562 api.ServerApiByUid.methods.getPosition */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:562 api.ServerApiByUid.methods.getPosition */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:565 api.ServerApiByUid.methods.getParent */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:565 api.ServerApiByUid.methods.getParent */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:568 api.ServerApiByUid.methods.getBeneficiary */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:568 api.ServerApiByUid.methods.getBeneficiary */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:571 api.ServerApiByUid.methods.setBeneficiary */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:571 api.ServerApiByUid.methods.setBeneficiary */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:576 api.ServerApiByUid.methods.getBlockTime */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:576 api.ServerApiByUid.methods.getBlockTime */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:579 api.ServerApiByUid.methods.unblock */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:579 api.ServerApiByUid.methods.unblock */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:582 api.ServerApiByUid.methods.createTime */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:582 api.ServerApiByUid.methods.createTime */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:585 api.ServerApiByUid.methods.onlineTime */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:585 api.ServerApiByUid.methods.onlineTime */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:588 api.ServerApiByUid.methods.addAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:588 api.ServerApiByUid.methods.addAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:593 api.ServerApiByUid.methods.removeAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:593 api.ServerApiByUid.methods.removeAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:598 api.ServerApiByUid.methods.setMsgQueueLimit */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:598 api.ServerApiByUid.methods.setMsgQueueLimit */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:603 api.ServerApiByUid.methods.setMsgTimeLimit */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:603 api.ServerApiByUid.methods.setMsgTimeLimit */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:609 api.ServerApiByUid.methods.addServersToCloud */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:609 api.ServerApiByUid.methods.addServersToCloud */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:614 api.ServerApiByUid.methods.removeServersFromCloud */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:614 api.ServerApiByUid.methods.removeServersFromCloud */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:625 api.ClientApiRegUnsafe.methods.enter */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:625 api.ClientApiRegUnsafe.methods.enter */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:633 api.ClientApiRegUnsafe.methods.enterGlobal */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:633 api.ClientApiRegUnsafe.methods.enterGlobal */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:644 api.GlobalRegServerApi.methods.setMasterKey */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:644 api.GlobalRegServerApi.methods.setMasterKey */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:648 api.GlobalRegServerApi.methods.finish */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:648 api.GlobalRegServerApi.methods.finish */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:654 api.ServerRegistrationApi.methods.registration */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:654 api.ServerRegistrationApi.methods.registration */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:667 api.ServerRegistrationApi.methods.registrationDirect */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:667 api.ServerRegistrationApi.methods.registrationDirect */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:677 api.ServerRegistrationApi.methods.requestWorkProofData */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:677 api.ServerRegistrationApi.methods.requestWorkProofData */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:683 api.ServerRegistrationApi.methods.resolveServers */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:683 api.ServerRegistrationApi.methods.resolveServers */
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
    {
        aether_status_t meta_status = client_server_api_serialize_cloud(
            tx,
            tx_capacity,
            &pos,
            serverIds);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:688 api.ServerRegistrationApi.methods.setReturnKey */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:688 api.ServerRegistrationApi.methods.setReturnKey */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:693 api.ServerRegistrationApi.methods.recovery */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:693 api.ServerRegistrationApi.methods.recovery */
aether_status_t server_registration_api_recovery(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    const key_ref_t * oldMasterKey,
    const key_ref_t * newMasterKey) {

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
            oldMasterKey);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_key(
            tx,
            tx_capacity,
            &pos,
            newMasterKey);
        if (meta_status != AETHER_OK) return meta_status;
    }

    return api->send(
        api->send_ctx,
        api->tx,
        pos);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:704 api.RegistrationRootApi.methods.getAsymmetricPublicKey */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:704 api.RegistrationRootApi.methods.getAsymmetricPublicKey */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:709 api.RegistrationRootApi.methods.enter */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:709 api.RegistrationRootApi.methods.enter */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:718 api.RegistrationRootApi.methods.getMyIp */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:718 api.RegistrationRootApi.methods.getMyIp */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:247 api.AuthorizedApi.methods.backId */
aether_status_t login_stream_authorized_api_back_id(
    login_stream_t *builder_stream,
    int16_t id) {

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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:252 api.AuthorizedApi.methods.ping */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:259 api.AuthorizedApi.methods.client */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:264 api.AuthorizedApi.methods.sendMessage */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:268 api.AuthorizedApi.methods.sendMessages */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:273 api.AuthorizedApi.methods.sendMulticast */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:279 api.AuthorizedApi.methods.sendMessageWithResult */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:288 api.AuthorizedApi.methods.createAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:294 api.AuthorizedApi.methods.addToAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:300 api.AuthorizedApi.methods.removeFromAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:306 api.AuthorizedApi.methods.checkAccessForSendMessage */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:311 api.AuthorizedApi.methods.resolverServers */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:315 api.AuthorizedApi.methods.resolveClouds */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:321 api.AuthorizedApi.methods.reportAppliedConfig */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:327 api.AuthorizedApi.methods.getAccessGroups */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:332 api.AuthorizedApi.methods.getAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:337 api.AuthorizedApi.methods.getAllAccessedClients */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:342 api.AuthorizedApi.methods.checkAccessForSendMessage2 */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:348 api.AuthorizedApi.methods.sendTelemetry */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:352 api.AuthorizedApi.methods.requestAccessGroupsForClients */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:356 api.AuthorizedApi.methods.requestAccessGroupsItems */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:360 api.AuthorizedApi.methods.sendAccessGroupForClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:365 api.AuthorizedApi.methods.addItemsToAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:370 api.AuthorizedApi.methods.removeItemsFromAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:375 api.AuthorizedApi.methods.addAccessGroupsToClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:380 api.AuthorizedApi.methods.removeAccessGroupsFromClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:385 api.AuthorizedApi.methods.requestAllAccessedClients */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:389 api.AuthorizedApi.methods.requestAccessCheck */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:394 api.AuthorizedApi.methods.getClientActivity */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:402 api.AuthorizedApi.methods.searchClientLogs */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:409 api.AuthorizedApi.methods.getClientConnections */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:415 api.AuthorizedApi.methods.getClientMessages */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:426 api.AuthorizedApi.methods.setNextReadDelay */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:430 api.AuthorizedApi.methods.getUap */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:440 api.AuthorizedApi.methods.requestWebRtcSession */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:446 api.AuthorizedApi.methods.publishWebRtcSession */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:452 api.AuthorizedApi.methods.selfDestruct */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:456 api.AuthorizedApi.methods.getServers */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:462 api.AuthorizedApi.methods.getClientTiming */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:468 api.AuthorizedApi.methods.openReceiveWindow */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:475 api.AuthorizedApi.methods.switchVersion */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:482 api.AuthorizedApi.methods.setReceiveWindow */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:487 api.AuthorizedApi.methods.probePacket */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:493 api.AuthorizedApi.methods.requestProbeReport */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:499 api.AuthorizedApi.methods.getAlias */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:502 api.AuthorizedApi.methods.setAlias */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:508 api.AuthorizedApi.methods.issuePeerIdentity */
aether_status_t login_stream_authorized_api_issue_peer_identity(
    login_stream_t *builder_stream,
    uint32_t request_id,
    const key_sign_public_ref_t * key) {

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

    return authorized_api_issue_peer_identity(
        &remote,
        request_id,
        key);
}

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:152 api.ClientApiSafe.methods.changeParent */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:156 api.ClientApiSafe.methods.changeAlias */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:160 api.ClientApiSafe.methods.newChildren */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:164 api.ClientApiSafe.methods.sendMessages */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:168 api.ClientApiSafe.methods.sendServerDescriptor */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:172 api.ClientApiSafe.methods.sendServerDescriptors */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:176 api.ClientApiSafe.methods.sendCloud */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:180 api.ClientApiSafe.methods.sendClouds */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:184 api.ClientApiSafe.methods.requestTelemetry */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:186 api.ClientApiSafe.methods.sendAccessGroups */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:190 api.ClientApiSafe.methods.sendAccessGroupForClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:195 api.ClientApiSafe.methods.addItemsToAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:200 api.ClientApiSafe.methods.removeItemsFromAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:205 api.ClientApiSafe.methods.addAccessGroupsToClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:210 api.ClientApiSafe.methods.removeAccessGroupsFromClient */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:215 api.ClientApiSafe.methods.sendAllAccessedClients */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:220 api.ClientApiSafe.methods.sendAccessCheckResults */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:225 api.ClientApiSafe.methods.sendMessage */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:230 api.ClientApiSafe.methods.sendCloudConfigs */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:235 api.ClientApiSafe.methods.clientInteraction */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:240 api.ClientApiSafe.methods.probeReport */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:551 api.ServerApiByUid.methods.getBalance */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:554 api.ServerApiByUid.methods.setParent */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:559 api.ServerApiByUid.methods.block */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:562 api.ServerApiByUid.methods.getPosition */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:565 api.ServerApiByUid.methods.getParent */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:568 api.ServerApiByUid.methods.getBeneficiary */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:571 api.ServerApiByUid.methods.setBeneficiary */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:576 api.ServerApiByUid.methods.getBlockTime */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:579 api.ServerApiByUid.methods.unblock */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:582 api.ServerApiByUid.methods.createTime */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:585 api.ServerApiByUid.methods.onlineTime */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:588 api.ServerApiByUid.methods.addAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:593 api.ServerApiByUid.methods.removeAccessGroup */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:598 api.ServerApiByUid.methods.setMsgQueueLimit */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:603 api.ServerApiByUid.methods.setMsgTimeLimit */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:609 api.ServerApiByUid.methods.addServersToCloud */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:614 api.ServerApiByUid.methods.removeServersFromCloud */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:644 api.GlobalRegServerApi.methods.setMasterKey */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:648 api.GlobalRegServerApi.methods.finish */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:654 api.ServerRegistrationApi.methods.registration */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:667 api.ServerRegistrationApi.methods.registrationDirect */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:677 api.ServerRegistrationApi.methods.requestWorkProofData */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:683 api.ServerRegistrationApi.methods.resolveServers */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:688 api.ServerRegistrationApi.methods.setReturnKey */
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

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:693 api.ServerRegistrationApi.methods.recovery */
aether_status_t server_registration_api_stream_server_registration_api_recovery(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    const key_ref_t * oldMasterKey,
    const key_ref_t * newMasterKey) {

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
        oldMasterKey,
        newMasterKey);
}


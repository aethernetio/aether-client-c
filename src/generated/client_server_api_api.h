#ifndef CLIENTSERVERAPI_API_H
#define CLIENTSERVERAPI_API_H

#include "client_server_api_types.h"
#include "aether_meta_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef aether_status_t (*client_server_api_send_fn)(
    void *ctx,
    const uint8_t *data,
    size_t length);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} client_api_unsafe_remote_t;

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:140 api.ClientApiUnsafe.methods.sendSafeApiDataMulti */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:140 api.ClientApiUnsafe.methods.sendSafeApiDataMulti */
aether_status_t client_api_unsafe_send_safe_api_data_multi(
    client_api_unsafe_remote_t *api,
    int16_t backId,
    const login_client_stream_t * data);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:145 api.ClientApiUnsafe.methods.sendSafeApiData */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:145 api.ClientApiUnsafe.methods.sendSafeApiData */
aether_status_t client_api_unsafe_send_safe_api_data(
    client_api_unsafe_remote_t *api,
    const login_client_stream_t * data);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} client_api_safe_remote_t;

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:152 api.ClientApiSafe.methods.changeParent */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:152 api.ClientApiSafe.methods.changeParent */
aether_status_t client_api_safe_change_parent(
    client_api_safe_remote_t *api,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:156 api.ClientApiSafe.methods.changeAlias */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:156 api.ClientApiSafe.methods.changeAlias */
aether_status_t client_api_safe_change_alias(
    client_api_safe_remote_t *api,
    aether_uuid_t alias);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:160 api.ClientApiSafe.methods.newChildren */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:160 api.ClientApiSafe.methods.newChildren */
aether_status_t client_api_safe_new_children(
    client_api_safe_remote_t *api,
    uuid_array_view_t uids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:164 api.ClientApiSafe.methods.sendMessages */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:164 api.ClientApiSafe.methods.sendMessages */
aether_status_t client_api_safe_send_messages(
    client_api_safe_remote_t *api,
    message_array_view_t msg);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:168 api.ClientApiSafe.methods.sendServerDescriptor */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:168 api.ClientApiSafe.methods.sendServerDescriptor */
aether_status_t client_api_safe_send_server_descriptor(
    client_api_safe_remote_t *api,
    const server_descriptor_t * serverDescriptor);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:172 api.ClientApiSafe.methods.sendServerDescriptors */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:172 api.ClientApiSafe.methods.sendServerDescriptors */
aether_status_t client_api_safe_send_server_descriptors(
    client_api_safe_remote_t *api,
    server_descriptor_array_view_t serverDescriptors);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:176 api.ClientApiSafe.methods.sendCloud */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:176 api.ClientApiSafe.methods.sendCloud */
aether_status_t client_api_safe_send_cloud(
    client_api_safe_remote_t *api,
    const u_u_i_d_and_cloud_t * uidAndCloud);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:180 api.ClientApiSafe.methods.sendClouds */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:180 api.ClientApiSafe.methods.sendClouds */
aether_status_t client_api_safe_send_clouds(
    client_api_safe_remote_t *api,
    u_u_i_d_and_cloud_array_view_t clouds);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:184 api.ClientApiSafe.methods.requestTelemetry */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:184 api.ClientApiSafe.methods.requestTelemetry */
aether_status_t client_api_safe_request_telemetry(
    client_api_safe_remote_t *api);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:186 api.ClientApiSafe.methods.sendAccessGroups */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:186 api.ClientApiSafe.methods.sendAccessGroups */
aether_status_t client_api_safe_send_access_groups(
    client_api_safe_remote_t *api,
    access_group_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:190 api.ClientApiSafe.methods.sendAccessGroupForClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:190 api.ClientApiSafe.methods.sendAccessGroupForClient */
aether_status_t client_api_safe_send_access_group_for_client(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:195 api.ClientApiSafe.methods.addItemsToAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:195 api.ClientApiSafe.methods.addItemsToAccessGroup */
aether_status_t client_api_safe_add_items_to_access_group(
    client_api_safe_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:200 api.ClientApiSafe.methods.removeItemsFromAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:200 api.ClientApiSafe.methods.removeItemsFromAccessGroup */
aether_status_t client_api_safe_remove_items_from_access_group(
    client_api_safe_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:205 api.ClientApiSafe.methods.addAccessGroupsToClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:205 api.ClientApiSafe.methods.addAccessGroupsToClient */
aether_status_t client_api_safe_add_access_groups_to_client(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:210 api.ClientApiSafe.methods.removeAccessGroupsFromClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:210 api.ClientApiSafe.methods.removeAccessGroupsFromClient */
aether_status_t client_api_safe_remove_access_groups_from_client(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:215 api.ClientApiSafe.methods.sendAllAccessedClients */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:215 api.ClientApiSafe.methods.sendAllAccessedClients */
aether_status_t client_api_safe_send_all_accessed_clients(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t accessedClients);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:220 api.ClientApiSafe.methods.sendAccessCheckResults */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:220 api.ClientApiSafe.methods.sendAccessCheckResults */
aether_status_t client_api_safe_send_access_check_results(
    client_api_safe_remote_t *api,
    access_check_result_array_view_t results);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:225 api.ClientApiSafe.methods.sendMessage */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:225 api.ClientApiSafe.methods.sendMessage */
aether_status_t client_api_safe_send_message(
    client_api_safe_remote_t *api,
    const message_t * msg);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:230 api.ClientApiSafe.methods.sendCloudConfigs */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:230 api.ClientApiSafe.methods.sendCloudConfigs */
aether_status_t client_api_safe_send_cloud_configs(
    client_api_safe_remote_t *api,
    cloud_config_array_view_t configs);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:235 api.ClientApiSafe.methods.clientInteraction */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:235 api.ClientApiSafe.methods.clientInteraction */
aether_status_t client_api_safe_client_interaction(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    const client_interaction_client_stream_t * stream);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:240 api.ClientApiSafe.methods.probeReport */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:240 api.ClientApiSafe.methods.probeReport */
aether_status_t client_api_safe_probe_report(
    client_api_safe_remote_t *api,
    const probe_report_t * report);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} authorized_api_remote_t;

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:247 api.AuthorizedApi.methods.backId */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:247 api.AuthorizedApi.methods.backId */
aether_status_t authorized_api_back_id(
    authorized_api_remote_t *api,
    int16_t id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:252 api.AuthorizedApi.methods.ping */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:252 api.AuthorizedApi.methods.ping */
aether_status_t authorized_api_ping(
    authorized_api_remote_t *api,
    uint32_t request_id,
    int64_t nextConnectMsDuration,
    int64_t rxWindowMs);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:259 api.AuthorizedApi.methods.client */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:259 api.AuthorizedApi.methods.client */
aether_status_t authorized_api_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    const client_api_stream_t * stream);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:264 api.AuthorizedApi.methods.sendMessage */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:264 api.AuthorizedApi.methods.sendMessage */
aether_status_t authorized_api_send_message(
    authorized_api_remote_t *api,
    const message_t * msg);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:268 api.AuthorizedApi.methods.sendMessages */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:268 api.AuthorizedApi.methods.sendMessages */
aether_status_t authorized_api_send_messages(
    authorized_api_remote_t *api,
    message_array_view_t msg);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:273 api.AuthorizedApi.methods.sendMulticast */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:273 api.AuthorizedApi.methods.sendMulticast */
aether_status_t authorized_api_send_multicast(
    authorized_api_remote_t *api,
    uuid_array_view_t uids,
    aether_bytes_view_t data);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:279 api.AuthorizedApi.methods.sendMessageWithResult */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:279 api.AuthorizedApi.methods.sendMessageWithResult */
aether_status_t authorized_api_send_message_with_result(
    authorized_api_remote_t *api,
    uint32_t request_id,
    const message_t * msg);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:288 api.AuthorizedApi.methods.createAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:288 api.AuthorizedApi.methods.createAccessGroup */
aether_status_t authorized_api_create_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t owner,
    uuid_array_view_t uids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:294 api.AuthorizedApi.methods.addToAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:294 api.AuthorizedApi.methods.addToAccessGroup */
aether_status_t authorized_api_add_to_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:300 api.AuthorizedApi.methods.removeFromAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:300 api.AuthorizedApi.methods.removeFromAccessGroup */
aether_status_t authorized_api_remove_from_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:306 api.AuthorizedApi.methods.checkAccessForSendMessage */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:306 api.AuthorizedApi.methods.checkAccessForSendMessage */
aether_status_t authorized_api_check_access_for_send_message(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:311 api.AuthorizedApi.methods.resolverServers */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:311 api.AuthorizedApi.methods.resolverServers */
aether_status_t authorized_api_resolver_servers(
    authorized_api_remote_t *api,
    short_array_view_t sid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:315 api.AuthorizedApi.methods.resolveClouds */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:315 api.AuthorizedApi.methods.resolveClouds */
aether_status_t authorized_api_resolve_clouds(
    authorized_api_remote_t *api,
    uuid_array_view_t uids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:321 api.AuthorizedApi.methods.reportAppliedConfig */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:321 api.AuthorizedApi.methods.reportAppliedConfig */
aether_status_t authorized_api_report_applied_config(
    authorized_api_remote_t *api,
    applied_config_array_view_t configs);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:327 api.AuthorizedApi.methods.getAccessGroups */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:327 api.AuthorizedApi.methods.getAccessGroups */
aether_status_t authorized_api_get_access_groups(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:332 api.AuthorizedApi.methods.getAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:332 api.AuthorizedApi.methods.getAccessGroup */
aether_status_t authorized_api_get_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:337 api.AuthorizedApi.methods.getAllAccessedClients */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:337 api.AuthorizedApi.methods.getAllAccessedClients */
aether_status_t authorized_api_get_all_accessed_clients(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:342 api.AuthorizedApi.methods.checkAccessForSendMessage2 */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:342 api.AuthorizedApi.methods.checkAccessForSendMessage2 */
aether_status_t authorized_api_check_access_for_send_message2(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid1,
    aether_uuid_t uid2);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:348 api.AuthorizedApi.methods.sendTelemetry */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:348 api.AuthorizedApi.methods.sendTelemetry */
aether_status_t authorized_api_send_telemetry(
    authorized_api_remote_t *api,
    const telemetry_ref_t * telemetry);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:352 api.AuthorizedApi.methods.requestAccessGroupsForClients */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:352 api.AuthorizedApi.methods.requestAccessGroupsForClients */
aether_status_t authorized_api_request_access_groups_for_clients(
    authorized_api_remote_t *api,
    uuid_array_view_t uids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:356 api.AuthorizedApi.methods.requestAccessGroupsItems */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:356 api.AuthorizedApi.methods.requestAccessGroupsItems */
aether_status_t authorized_api_request_access_groups_items(
    authorized_api_remote_t *api,
    uuid_array_view_t ids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:360 api.AuthorizedApi.methods.sendAccessGroupForClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:360 api.AuthorizedApi.methods.sendAccessGroupForClient */
aether_status_t authorized_api_send_access_group_for_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:365 api.AuthorizedApi.methods.addItemsToAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:365 api.AuthorizedApi.methods.addItemsToAccessGroup */
aether_status_t authorized_api_add_items_to_access_group(
    authorized_api_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:370 api.AuthorizedApi.methods.removeItemsFromAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:370 api.AuthorizedApi.methods.removeItemsFromAccessGroup */
aether_status_t authorized_api_remove_items_from_access_group(
    authorized_api_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:375 api.AuthorizedApi.methods.addAccessGroupsToClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:375 api.AuthorizedApi.methods.addAccessGroupsToClient */
aether_status_t authorized_api_add_access_groups_to_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:380 api.AuthorizedApi.methods.removeAccessGroupsFromClient */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:380 api.AuthorizedApi.methods.removeAccessGroupsFromClient */
aether_status_t authorized_api_remove_access_groups_from_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:385 api.AuthorizedApi.methods.requestAllAccessedClients */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:385 api.AuthorizedApi.methods.requestAllAccessedClients */
aether_status_t authorized_api_request_all_accessed_clients(
    authorized_api_remote_t *api,
    uuid_array_view_t uids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:389 api.AuthorizedApi.methods.requestAccessCheck */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:389 api.AuthorizedApi.methods.requestAccessCheck */
aether_status_t authorized_api_request_access_check(
    authorized_api_remote_t *api,
    access_check_pair_array_view_t requests);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:394 api.AuthorizedApi.methods.getClientActivity */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:394 api.AuthorizedApi.methods.getClientActivity */
aether_status_t authorized_api_get_client_activity(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:402 api.AuthorizedApi.methods.searchClientLogs */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:402 api.AuthorizedApi.methods.searchClientLogs */
aether_status_t authorized_api_search_client_logs(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    aether_bytes_view_t query,
    int32_t limit);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:409 api.AuthorizedApi.methods.getClientConnections */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:409 api.AuthorizedApi.methods.getClientConnections */
aether_status_t authorized_api_get_client_connections(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    int32_t limit);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:415 api.AuthorizedApi.methods.getClientMessages */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:415 api.AuthorizedApi.methods.getClientMessages */
aether_status_t authorized_api_get_client_messages(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:426 api.AuthorizedApi.methods.setNextReadDelay */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:426 api.AuthorizedApi.methods.setNextReadDelay */
aether_status_t authorized_api_set_next_read_delay(
    authorized_api_remote_t *api,
    int64_t delayMillis);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:430 api.AuthorizedApi.methods.getUap */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:430 api.AuthorizedApi.methods.getUap */
aether_status_t authorized_api_get_uap(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:440 api.AuthorizedApi.methods.requestWebRtcSession */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:440 api.AuthorizedApi.methods.requestWebRtcSession */
aether_status_t authorized_api_request_web_rtc_session(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:446 api.AuthorizedApi.methods.publishWebRtcSession */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:446 api.AuthorizedApi.methods.publishWebRtcSession */
aether_status_t authorized_api_publish_web_rtc_session(
    authorized_api_remote_t *api,
    const web_rtc_session_t * session);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:452 api.AuthorizedApi.methods.selfDestruct */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:452 api.AuthorizedApi.methods.selfDestruct */
aether_status_t authorized_api_self_destruct(
    authorized_api_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:456 api.AuthorizedApi.methods.getServers */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:456 api.AuthorizedApi.methods.getServers */
aether_status_t authorized_api_get_servers(
    authorized_api_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:462 api.AuthorizedApi.methods.getClientTiming */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:462 api.AuthorizedApi.methods.getClientTiming */
aether_status_t authorized_api_get_client_timing(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:468 api.AuthorizedApi.methods.openReceiveWindow */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:468 api.AuthorizedApi.methods.openReceiveWindow */
aether_status_t authorized_api_open_receive_window(
    authorized_api_remote_t *api,
    uint32_t request_id,
    int64_t durationMs);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:475 api.AuthorizedApi.methods.switchVersion */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:475 api.AuthorizedApi.methods.switchVersion */
aether_status_t authorized_api_switch_version(
    authorized_api_remote_t *api,
    int32_t version);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:482 api.AuthorizedApi.methods.setReceiveWindow */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:482 api.AuthorizedApi.methods.setReceiveWindow */
aether_status_t authorized_api_set_receive_window(
    authorized_api_remote_t *api,
    int64_t startsInMs,
    int64_t durationMs);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:487 api.AuthorizedApi.methods.probePacket */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:487 api.AuthorizedApi.methods.probePacket */
aether_status_t authorized_api_probe_packet(
    authorized_api_remote_t *api,
    int32_t testId,
    int32_t sequence,
    aether_bytes_view_t payload);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:493 api.AuthorizedApi.methods.requestProbeReport */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:493 api.AuthorizedApi.methods.requestProbeReport */
aether_status_t authorized_api_request_probe_report(
    authorized_api_remote_t *api,
    int32_t testId,
    int32_t firstSequence,
    int32_t count);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:499 api.AuthorizedApi.methods.getAlias */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:499 api.AuthorizedApi.methods.getAlias */
aether_status_t authorized_api_get_alias(
    authorized_api_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:502 api.AuthorizedApi.methods.setAlias */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:502 api.AuthorizedApi.methods.setAlias */
aether_status_t authorized_api_set_alias(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t alias);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:508 api.AuthorizedApi.methods.issuePeerIdentity */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:508 api.AuthorizedApi.methods.issuePeerIdentity */
aether_status_t authorized_api_issue_peer_identity(
    authorized_api_remote_t *api,
    uint32_t request_id,
    const key_sign_public_ref_t * key);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} login_api_remote_t;

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:516 api.LoginApi.methods.getTimeUTC */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:516 api.LoginApi.methods.getTimeUTC */
aether_status_t login_api_get_time_u_t_c(
    login_api_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:519 api.LoginApi.methods.loginByUID */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:519 api.LoginApi.methods.loginByUID */
aether_status_t login_api_login_by_u_i_d(
    login_api_remote_t *api,
    aether_uuid_t uid,
    const login_stream_t * data);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:524 api.LoginApi.methods.loginByAlias */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:524 api.LoginApi.methods.loginByAlias */
aether_status_t login_api_login_by_alias(
    login_api_remote_t *api,
    aether_uuid_t alias,
    const login_stream_t * data);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:530 api.LoginApi.methods.getMyIp */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:530 api.LoginApi.methods.getMyIp */
aether_status_t login_api_get_my_ip(
    login_api_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:535 api.LoginApi.methods.loginByUIDMulti */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:535 api.LoginApi.methods.loginByUIDMulti */
aether_status_t login_api_login_by_u_i_d_multi(
    login_api_remote_t *api,
    aether_uuid_t uid,
    int16_t backId,
    const login_stream_t * data);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:541 api.LoginApi.methods.loginByAliasMulti */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:541 api.LoginApi.methods.loginByAliasMulti */
aether_status_t login_api_login_by_alias_multi(
    login_api_remote_t *api,
    aether_uuid_t alias,
    int16_t backId,
    const login_stream_t * data);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} server_api_by_uid_client_remote_t;

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} server_api_by_uid_remote_t;

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:551 api.ServerApiByUid.methods.getBalance */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:551 api.ServerApiByUid.methods.getBalance */
aether_status_t server_api_by_uid_get_balance(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:554 api.ServerApiByUid.methods.setParent */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:554 api.ServerApiByUid.methods.setParent */
aether_status_t server_api_by_uid_set_parent(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:559 api.ServerApiByUid.methods.block */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:559 api.ServerApiByUid.methods.block */
aether_status_t server_api_by_uid_block(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:562 api.ServerApiByUid.methods.getPosition */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:562 api.ServerApiByUid.methods.getPosition */
aether_status_t server_api_by_uid_get_position(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:565 api.ServerApiByUid.methods.getParent */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:565 api.ServerApiByUid.methods.getParent */
aether_status_t server_api_by_uid_get_parent(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:568 api.ServerApiByUid.methods.getBeneficiary */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:568 api.ServerApiByUid.methods.getBeneficiary */
aether_status_t server_api_by_uid_get_beneficiary(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:571 api.ServerApiByUid.methods.setBeneficiary */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:571 api.ServerApiByUid.methods.setBeneficiary */
aether_status_t server_api_by_uid_set_beneficiary(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:576 api.ServerApiByUid.methods.getBlockTime */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:576 api.ServerApiByUid.methods.getBlockTime */
aether_status_t server_api_by_uid_get_block_time(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:579 api.ServerApiByUid.methods.unblock */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:579 api.ServerApiByUid.methods.unblock */
aether_status_t server_api_by_uid_unblock(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:582 api.ServerApiByUid.methods.createTime */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:582 api.ServerApiByUid.methods.createTime */
aether_status_t server_api_by_uid_create_time(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:585 api.ServerApiByUid.methods.onlineTime */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:585 api.ServerApiByUid.methods.onlineTime */
aether_status_t server_api_by_uid_online_time(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:588 api.ServerApiByUid.methods.addAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:588 api.ServerApiByUid.methods.addAccessGroup */
aether_status_t server_api_by_uid_add_access_group(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:593 api.ServerApiByUid.methods.removeAccessGroup */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:593 api.ServerApiByUid.methods.removeAccessGroup */
aether_status_t server_api_by_uid_remove_access_group(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:598 api.ServerApiByUid.methods.setMsgQueueLimit */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:598 api.ServerApiByUid.methods.setMsgQueueLimit */
aether_status_t server_api_by_uid_set_msg_queue_limit(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    int32_t limit);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:603 api.ServerApiByUid.methods.setMsgTimeLimit */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:603 api.ServerApiByUid.methods.setMsgTimeLimit */
aether_status_t server_api_by_uid_set_msg_time_limit(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    int32_t seconds);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:609 api.ServerApiByUid.methods.addServersToCloud */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:609 api.ServerApiByUid.methods.addServersToCloud */
aether_status_t server_api_by_uid_add_servers_to_cloud(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    short_array_view_t sids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:614 api.ServerApiByUid.methods.removeServersFromCloud */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:614 api.ServerApiByUid.methods.removeServersFromCloud */
aether_status_t server_api_by_uid_remove_servers_from_cloud(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    short_array_view_t sids);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} client_api_reg_unsafe_remote_t;

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:625 api.ClientApiRegUnsafe.methods.enter */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:625 api.ClientApiRegUnsafe.methods.enter */
aether_status_t client_api_reg_unsafe_enter(
    client_api_reg_unsafe_remote_t *api,
    const client_api_reg_safe_stream_t * stream);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:633 api.ClientApiRegUnsafe.methods.enterGlobal */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:633 api.ClientApiRegUnsafe.methods.enterGlobal */
aether_status_t client_api_reg_unsafe_enter_global(
    client_api_reg_unsafe_remote_t *api,
    const global_reg_client_api_stream_t * stream);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} global_reg_server_api_remote_t;

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:644 api.GlobalRegServerApi.methods.setMasterKey */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:644 api.GlobalRegServerApi.methods.setMasterKey */
aether_status_t global_reg_server_api_set_master_key(
    global_reg_server_api_remote_t *api,
    const key_ref_t * key);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:648 api.GlobalRegServerApi.methods.finish */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:648 api.GlobalRegServerApi.methods.finish */
aether_status_t global_reg_server_api_finish(
    global_reg_server_api_remote_t *api,
    uint32_t request_id);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} server_registration_api_remote_t;

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:654 api.ServerRegistrationApi.methods.registration */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:654 api.ServerRegistrationApi.methods.registration */
aether_status_t server_registration_api_registration(
    server_registration_api_remote_t *api,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const global_api_stream_t * globalApi);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:667 api.ServerRegistrationApi.methods.registrationDirect */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:667 api.ServerRegistrationApi.methods.registrationDirect */
aether_status_t server_registration_api_registration_direct(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const key_ref_t * masterKey);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:677 api.ServerRegistrationApi.methods.requestWorkProofData */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:677 api.ServerRegistrationApi.methods.requestWorkProofData */
aether_status_t server_registration_api_request_work_proof_data(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t parent,
    pow_method_t powMethods);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:683 api.ServerRegistrationApi.methods.resolveServers */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:683 api.ServerRegistrationApi.methods.resolveServers */
aether_status_t server_registration_api_resolve_servers(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    const cloud_t * serverIds);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:688 api.ServerRegistrationApi.methods.setReturnKey */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:688 api.ServerRegistrationApi.methods.setReturnKey */
aether_status_t server_registration_api_set_return_key(
    server_registration_api_remote_t *api,
    const key_ref_t * key);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:693 api.ServerRegistrationApi.methods.recovery */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:693 api.ServerRegistrationApi.methods.recovery */
aether_status_t server_registration_api_recovery(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    const key_ref_t * oldMasterKey,
    const key_ref_t * newMasterKey);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} registration_root_api_remote_t;

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:704 api.RegistrationRootApi.methods.getAsymmetricPublicKey */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:704 api.RegistrationRootApi.methods.getAsymmetricPublicKey */
aether_status_t registration_root_api_get_asymmetric_public_key(
    registration_root_api_remote_t *api,
    uint32_t request_id,
    crypto_lib_t cryptoLib);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:709 api.RegistrationRootApi.methods.enter */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:709 api.RegistrationRootApi.methods.enter */
aether_status_t registration_root_api_enter(
    registration_root_api_remote_t *api,
    crypto_lib_t cryptoLib,
    const server_registration_api_stream_t * stream);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:718 api.RegistrationRootApi.methods.getMyIp */
/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:718 api.RegistrationRootApi.methods.getMyIp */
aether_status_t registration_root_api_get_my_ip(
    registration_root_api_remote_t *api,
    uint32_t request_id);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} client_api_reg_safe_remote_t;

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} global_reg_client_api_remote_t;

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:247 api.AuthorizedApi.methods.backId */
aether_status_t login_stream_authorized_api_back_id(
    login_stream_t *builder_stream,
    int16_t id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:252 api.AuthorizedApi.methods.ping */
aether_status_t login_stream_authorized_api_ping(
    login_stream_t *builder_stream,
    uint32_t request_id,
    int64_t nextConnectMsDuration,
    int64_t rxWindowMs);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:259 api.AuthorizedApi.methods.client */
aether_status_t login_stream_authorized_api_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    const client_api_stream_t * stream);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:264 api.AuthorizedApi.methods.sendMessage */
aether_status_t login_stream_authorized_api_send_message(
    login_stream_t *builder_stream,
    const message_t * msg);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:268 api.AuthorizedApi.methods.sendMessages */
aether_status_t login_stream_authorized_api_send_messages(
    login_stream_t *builder_stream,
    message_array_view_t msg);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:273 api.AuthorizedApi.methods.sendMulticast */
aether_status_t login_stream_authorized_api_send_multicast(
    login_stream_t *builder_stream,
    uuid_array_view_t uids,
    aether_bytes_view_t data);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:279 api.AuthorizedApi.methods.sendMessageWithResult */
aether_status_t login_stream_authorized_api_send_message_with_result(
    login_stream_t *builder_stream,
    uint32_t request_id,
    const message_t * msg);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:288 api.AuthorizedApi.methods.createAccessGroup */
aether_status_t login_stream_authorized_api_create_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t owner,
    uuid_array_view_t uids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:294 api.AuthorizedApi.methods.addToAccessGroup */
aether_status_t login_stream_authorized_api_add_to_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:300 api.AuthorizedApi.methods.removeFromAccessGroup */
aether_status_t login_stream_authorized_api_remove_from_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:306 api.AuthorizedApi.methods.checkAccessForSendMessage */
aether_status_t login_stream_authorized_api_check_access_for_send_message(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:311 api.AuthorizedApi.methods.resolverServers */
aether_status_t login_stream_authorized_api_resolver_servers(
    login_stream_t *builder_stream,
    short_array_view_t sid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:315 api.AuthorizedApi.methods.resolveClouds */
aether_status_t login_stream_authorized_api_resolve_clouds(
    login_stream_t *builder_stream,
    uuid_array_view_t uids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:321 api.AuthorizedApi.methods.reportAppliedConfig */
aether_status_t login_stream_authorized_api_report_applied_config(
    login_stream_t *builder_stream,
    applied_config_array_view_t configs);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:327 api.AuthorizedApi.methods.getAccessGroups */
aether_status_t login_stream_authorized_api_get_access_groups(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:332 api.AuthorizedApi.methods.getAccessGroup */
aether_status_t login_stream_authorized_api_get_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:337 api.AuthorizedApi.methods.getAllAccessedClients */
aether_status_t login_stream_authorized_api_get_all_accessed_clients(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:342 api.AuthorizedApi.methods.checkAccessForSendMessage2 */
aether_status_t login_stream_authorized_api_check_access_for_send_message2(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid1,
    aether_uuid_t uid2);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:348 api.AuthorizedApi.methods.sendTelemetry */
aether_status_t login_stream_authorized_api_send_telemetry(
    login_stream_t *builder_stream,
    const telemetry_ref_t * telemetry);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:352 api.AuthorizedApi.methods.requestAccessGroupsForClients */
aether_status_t login_stream_authorized_api_request_access_groups_for_clients(
    login_stream_t *builder_stream,
    uuid_array_view_t uids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:356 api.AuthorizedApi.methods.requestAccessGroupsItems */
aether_status_t login_stream_authorized_api_request_access_groups_items(
    login_stream_t *builder_stream,
    uuid_array_view_t ids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:360 api.AuthorizedApi.methods.sendAccessGroupForClient */
aether_status_t login_stream_authorized_api_send_access_group_for_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:365 api.AuthorizedApi.methods.addItemsToAccessGroup */
aether_status_t login_stream_authorized_api_add_items_to_access_group(
    login_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:370 api.AuthorizedApi.methods.removeItemsFromAccessGroup */
aether_status_t login_stream_authorized_api_remove_items_from_access_group(
    login_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:375 api.AuthorizedApi.methods.addAccessGroupsToClient */
aether_status_t login_stream_authorized_api_add_access_groups_to_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:380 api.AuthorizedApi.methods.removeAccessGroupsFromClient */
aether_status_t login_stream_authorized_api_remove_access_groups_from_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:385 api.AuthorizedApi.methods.requestAllAccessedClients */
aether_status_t login_stream_authorized_api_request_all_accessed_clients(
    login_stream_t *builder_stream,
    uuid_array_view_t uids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:389 api.AuthorizedApi.methods.requestAccessCheck */
aether_status_t login_stream_authorized_api_request_access_check(
    login_stream_t *builder_stream,
    access_check_pair_array_view_t requests);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:394 api.AuthorizedApi.methods.getClientActivity */
aether_status_t login_stream_authorized_api_get_client_activity(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:402 api.AuthorizedApi.methods.searchClientLogs */
aether_status_t login_stream_authorized_api_search_client_logs(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    aether_bytes_view_t query,
    int32_t limit);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:409 api.AuthorizedApi.methods.getClientConnections */
aether_status_t login_stream_authorized_api_get_client_connections(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    int32_t limit);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:415 api.AuthorizedApi.methods.getClientMessages */
aether_status_t login_stream_authorized_api_get_client_messages(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:426 api.AuthorizedApi.methods.setNextReadDelay */
aether_status_t login_stream_authorized_api_set_next_read_delay(
    login_stream_t *builder_stream,
    int64_t delayMillis);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:430 api.AuthorizedApi.methods.getUap */
aether_status_t login_stream_authorized_api_get_uap(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:440 api.AuthorizedApi.methods.requestWebRtcSession */
aether_status_t login_stream_authorized_api_request_web_rtc_session(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:446 api.AuthorizedApi.methods.publishWebRtcSession */
aether_status_t login_stream_authorized_api_publish_web_rtc_session(
    login_stream_t *builder_stream,
    const web_rtc_session_t * session);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:452 api.AuthorizedApi.methods.selfDestruct */
aether_status_t login_stream_authorized_api_self_destruct(
    login_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:456 api.AuthorizedApi.methods.getServers */
aether_status_t login_stream_authorized_api_get_servers(
    login_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:462 api.AuthorizedApi.methods.getClientTiming */
aether_status_t login_stream_authorized_api_get_client_timing(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:468 api.AuthorizedApi.methods.openReceiveWindow */
aether_status_t login_stream_authorized_api_open_receive_window(
    login_stream_t *builder_stream,
    uint32_t request_id,
    int64_t durationMs);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:475 api.AuthorizedApi.methods.switchVersion */
aether_status_t login_stream_authorized_api_switch_version(
    login_stream_t *builder_stream,
    int32_t version);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:482 api.AuthorizedApi.methods.setReceiveWindow */
aether_status_t login_stream_authorized_api_set_receive_window(
    login_stream_t *builder_stream,
    int64_t startsInMs,
    int64_t durationMs);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:487 api.AuthorizedApi.methods.probePacket */
aether_status_t login_stream_authorized_api_probe_packet(
    login_stream_t *builder_stream,
    int32_t testId,
    int32_t sequence,
    aether_bytes_view_t payload);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:493 api.AuthorizedApi.methods.requestProbeReport */
aether_status_t login_stream_authorized_api_request_probe_report(
    login_stream_t *builder_stream,
    int32_t testId,
    int32_t firstSequence,
    int32_t count);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:499 api.AuthorizedApi.methods.getAlias */
aether_status_t login_stream_authorized_api_get_alias(
    login_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:502 api.AuthorizedApi.methods.setAlias */
aether_status_t login_stream_authorized_api_set_alias(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t alias);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:508 api.AuthorizedApi.methods.issuePeerIdentity */
aether_status_t login_stream_authorized_api_issue_peer_identity(
    login_stream_t *builder_stream,
    uint32_t request_id,
    const key_sign_public_ref_t * key);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:152 api.ClientApiSafe.methods.changeParent */
aether_status_t login_client_stream_client_api_safe_change_parent(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:156 api.ClientApiSafe.methods.changeAlias */
aether_status_t login_client_stream_client_api_safe_change_alias(
    login_client_stream_t *builder_stream,
    aether_uuid_t alias);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:160 api.ClientApiSafe.methods.newChildren */
aether_status_t login_client_stream_client_api_safe_new_children(
    login_client_stream_t *builder_stream,
    uuid_array_view_t uids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:164 api.ClientApiSafe.methods.sendMessages */
aether_status_t login_client_stream_client_api_safe_send_messages(
    login_client_stream_t *builder_stream,
    message_array_view_t msg);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:168 api.ClientApiSafe.methods.sendServerDescriptor */
aether_status_t login_client_stream_client_api_safe_send_server_descriptor(
    login_client_stream_t *builder_stream,
    const server_descriptor_t * serverDescriptor);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:172 api.ClientApiSafe.methods.sendServerDescriptors */
aether_status_t login_client_stream_client_api_safe_send_server_descriptors(
    login_client_stream_t *builder_stream,
    server_descriptor_array_view_t serverDescriptors);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:176 api.ClientApiSafe.methods.sendCloud */
aether_status_t login_client_stream_client_api_safe_send_cloud(
    login_client_stream_t *builder_stream,
    const u_u_i_d_and_cloud_t * uidAndCloud);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:180 api.ClientApiSafe.methods.sendClouds */
aether_status_t login_client_stream_client_api_safe_send_clouds(
    login_client_stream_t *builder_stream,
    u_u_i_d_and_cloud_array_view_t clouds);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:184 api.ClientApiSafe.methods.requestTelemetry */
aether_status_t login_client_stream_client_api_safe_request_telemetry(
    login_client_stream_t *builder_stream);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:186 api.ClientApiSafe.methods.sendAccessGroups */
aether_status_t login_client_stream_client_api_safe_send_access_groups(
    login_client_stream_t *builder_stream,
    access_group_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:190 api.ClientApiSafe.methods.sendAccessGroupForClient */
aether_status_t login_client_stream_client_api_safe_send_access_group_for_client(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:195 api.ClientApiSafe.methods.addItemsToAccessGroup */
aether_status_t login_client_stream_client_api_safe_add_items_to_access_group(
    login_client_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:200 api.ClientApiSafe.methods.removeItemsFromAccessGroup */
aether_status_t login_client_stream_client_api_safe_remove_items_from_access_group(
    login_client_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:205 api.ClientApiSafe.methods.addAccessGroupsToClient */
aether_status_t login_client_stream_client_api_safe_add_access_groups_to_client(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:210 api.ClientApiSafe.methods.removeAccessGroupsFromClient */
aether_status_t login_client_stream_client_api_safe_remove_access_groups_from_client(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:215 api.ClientApiSafe.methods.sendAllAccessedClients */
aether_status_t login_client_stream_client_api_safe_send_all_accessed_clients(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t accessedClients);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:220 api.ClientApiSafe.methods.sendAccessCheckResults */
aether_status_t login_client_stream_client_api_safe_send_access_check_results(
    login_client_stream_t *builder_stream,
    access_check_result_array_view_t results);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:225 api.ClientApiSafe.methods.sendMessage */
aether_status_t login_client_stream_client_api_safe_send_message(
    login_client_stream_t *builder_stream,
    const message_t * msg);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:230 api.ClientApiSafe.methods.sendCloudConfigs */
aether_status_t login_client_stream_client_api_safe_send_cloud_configs(
    login_client_stream_t *builder_stream,
    cloud_config_array_view_t configs);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:235 api.ClientApiSafe.methods.clientInteraction */
aether_status_t login_client_stream_client_api_safe_client_interaction(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    const client_interaction_client_stream_t * stream);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:240 api.ClientApiSafe.methods.probeReport */
aether_status_t login_client_stream_client_api_safe_probe_report(
    login_client_stream_t *builder_stream,
    const probe_report_t * report);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:551 api.ServerApiByUid.methods.getBalance */
aether_status_t client_api_stream_server_api_by_uid_get_balance(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:554 api.ServerApiByUid.methods.setParent */
aether_status_t client_api_stream_server_api_by_uid_set_parent(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:559 api.ServerApiByUid.methods.block */
aether_status_t client_api_stream_server_api_by_uid_block(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:562 api.ServerApiByUid.methods.getPosition */
aether_status_t client_api_stream_server_api_by_uid_get_position(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:565 api.ServerApiByUid.methods.getParent */
aether_status_t client_api_stream_server_api_by_uid_get_parent(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:568 api.ServerApiByUid.methods.getBeneficiary */
aether_status_t client_api_stream_server_api_by_uid_get_beneficiary(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:571 api.ServerApiByUid.methods.setBeneficiary */
aether_status_t client_api_stream_server_api_by_uid_set_beneficiary(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:576 api.ServerApiByUid.methods.getBlockTime */
aether_status_t client_api_stream_server_api_by_uid_get_block_time(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:579 api.ServerApiByUid.methods.unblock */
aether_status_t client_api_stream_server_api_by_uid_unblock(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:582 api.ServerApiByUid.methods.createTime */
aether_status_t client_api_stream_server_api_by_uid_create_time(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:585 api.ServerApiByUid.methods.onlineTime */
aether_status_t client_api_stream_server_api_by_uid_online_time(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:588 api.ServerApiByUid.methods.addAccessGroup */
aether_status_t client_api_stream_server_api_by_uid_add_access_group(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:593 api.ServerApiByUid.methods.removeAccessGroup */
aether_status_t client_api_stream_server_api_by_uid_remove_access_group(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:598 api.ServerApiByUid.methods.setMsgQueueLimit */
aether_status_t client_api_stream_server_api_by_uid_set_msg_queue_limit(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    int32_t limit);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:603 api.ServerApiByUid.methods.setMsgTimeLimit */
aether_status_t client_api_stream_server_api_by_uid_set_msg_time_limit(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    int32_t seconds);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:609 api.ServerApiByUid.methods.addServersToCloud */
aether_status_t client_api_stream_server_api_by_uid_add_servers_to_cloud(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    short_array_view_t sids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:614 api.ServerApiByUid.methods.removeServersFromCloud */
aether_status_t client_api_stream_server_api_by_uid_remove_servers_from_cloud(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    short_array_view_t sids);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:644 api.GlobalRegServerApi.methods.setMasterKey */
aether_status_t global_api_stream_global_reg_server_api_set_master_key(
    global_api_stream_t *builder_stream,
    const key_ref_t * key);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:648 api.GlobalRegServerApi.methods.finish */
aether_status_t global_api_stream_global_reg_server_api_finish(
    global_api_stream_t *builder_stream,
    uint32_t request_id);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:654 api.ServerRegistrationApi.methods.registration */
aether_status_t server_registration_api_stream_server_registration_api_registration(
    server_registration_api_stream_t *builder_stream,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const global_api_stream_t * globalApi);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:667 api.ServerRegistrationApi.methods.registrationDirect */
aether_status_t server_registration_api_stream_server_registration_api_registration_direct(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const key_ref_t * masterKey);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:677 api.ServerRegistrationApi.methods.requestWorkProofData */
aether_status_t server_registration_api_stream_server_registration_api_request_work_proof_data(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t parent,
    pow_method_t powMethods);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:683 api.ServerRegistrationApi.methods.resolveServers */
aether_status_t server_registration_api_stream_server_registration_api_resolve_servers(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    const cloud_t * serverIds);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:688 api.ServerRegistrationApi.methods.setReturnKey */
aether_status_t server_registration_api_stream_server_registration_api_set_return_key(
    server_registration_api_stream_t *builder_stream,
    const key_ref_t * key);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:693 api.ServerRegistrationApi.methods.recovery */
aether_status_t server_registration_api_stream_server_registration_api_recovery(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    const key_ref_t * oldMasterKey,
    const key_ref_t * newMasterKey);

#ifdef __cplusplus
}
#endif

#endif

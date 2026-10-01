#ifndef CLIENTSERVERAPI_API_H
#define CLIENTSERVERAPI_API_H

#include "client_server_api_types.h"

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

aether_status_t client_api_unsafe_send_safe_api_data_multi(
    client_api_unsafe_remote_t *api,
    int8_t backId,
    const login_client_stream_t * data);

aether_status_t client_api_unsafe_send_safe_api_data(
    client_api_unsafe_remote_t *api,
    const login_client_stream_t * data);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} client_api_safe_remote_t;

aether_status_t client_api_safe_change_parent(
    client_api_safe_remote_t *api,
    aether_uuid_t uid);

aether_status_t client_api_safe_change_alias(
    client_api_safe_remote_t *api,
    aether_uuid_t alias);

aether_status_t client_api_safe_new_children(
    client_api_safe_remote_t *api,
    uuid_array_view_t uids);

aether_status_t client_api_safe_send_messages(
    client_api_safe_remote_t *api,
    message_array_view_t msg);

aether_status_t client_api_safe_send_server_descriptor(
    client_api_safe_remote_t *api,
    const server_descriptor_t * serverDescriptor);

aether_status_t client_api_safe_send_server_descriptors(
    client_api_safe_remote_t *api,
    server_descriptor_array_view_t serverDescriptors);

aether_status_t client_api_safe_send_cloud(
    client_api_safe_remote_t *api,
    const u_u_i_d_and_cloud_t * uidAndCloud);

aether_status_t client_api_safe_send_clouds(
    client_api_safe_remote_t *api,
    u_u_i_d_and_cloud_array_view_t clouds);

aether_status_t client_api_safe_request_telemetry(
    client_api_safe_remote_t *api);

aether_status_t client_api_safe_send_access_groups(
    client_api_safe_remote_t *api,
    access_group_array_view_t groups);

aether_status_t client_api_safe_send_access_group_for_client(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t client_api_safe_add_items_to_access_group(
    client_api_safe_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups);

aether_status_t client_api_safe_remove_items_from_access_group(
    client_api_safe_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups);

aether_status_t client_api_safe_add_access_groups_to_client(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t client_api_safe_remove_access_groups_from_client(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t client_api_safe_send_all_accessed_clients(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t accessedClients);

aether_status_t client_api_safe_send_access_check_results(
    client_api_safe_remote_t *api,
    access_check_result_array_view_t results);

aether_status_t client_api_safe_send_message(
    client_api_safe_remote_t *api,
    const message_t * msg);

aether_status_t client_api_safe_send_cloud_configs(
    client_api_safe_remote_t *api,
    cloud_config_array_view_t configs);

aether_status_t client_api_safe_client_interaction(
    client_api_safe_remote_t *api,
    aether_uuid_t uid,
    const client_interaction_client_stream_t * stream);

aether_status_t client_api_safe_probe_report(
    client_api_safe_remote_t *api,
    const probe_report_t * report);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} authorized_api_remote_t;

aether_status_t authorized_api_back_id(
    authorized_api_remote_t *api,
    int8_t id);

aether_status_t authorized_api_ping(
    authorized_api_remote_t *api,
    uint32_t request_id,
    int64_t nextConnectMsDuration,
    int64_t rxWindowMs);

aether_status_t authorized_api_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    const client_api_stream_t * stream);

aether_status_t authorized_api_send_message(
    authorized_api_remote_t *api,
    const message_t * msg);

aether_status_t authorized_api_send_messages(
    authorized_api_remote_t *api,
    message_array_view_t msg);

aether_status_t authorized_api_send_multicast(
    authorized_api_remote_t *api,
    uuid_array_view_t uids,
    aether_bytes_view_t data);

aether_status_t authorized_api_send_message_with_result(
    authorized_api_remote_t *api,
    uint32_t request_id,
    const message_t * msg);

aether_status_t authorized_api_create_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t owner,
    uuid_array_view_t uids);

aether_status_t authorized_api_add_to_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid);

aether_status_t authorized_api_remove_from_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid);

aether_status_t authorized_api_check_access_for_send_message(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t authorized_api_resolver_servers(
    authorized_api_remote_t *api,
    short_array_view_t sid);

aether_status_t authorized_api_resolve_clouds(
    authorized_api_remote_t *api,
    uuid_array_view_t uids);

aether_status_t authorized_api_report_applied_config(
    authorized_api_remote_t *api,
    applied_config_array_view_t configs);

aether_status_t authorized_api_get_access_groups(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t authorized_api_get_access_group(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId);

aether_status_t authorized_api_get_all_accessed_clients(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t authorized_api_check_access_for_send_message2(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid1,
    aether_uuid_t uid2);

aether_status_t authorized_api_send_telemetry(
    authorized_api_remote_t *api,
    const telemetry_ref_t * telemetry);

aether_status_t authorized_api_request_access_groups_for_clients(
    authorized_api_remote_t *api,
    uuid_array_view_t uids);

aether_status_t authorized_api_request_access_groups_items(
    authorized_api_remote_t *api,
    uuid_array_view_t ids);

aether_status_t authorized_api_send_access_group_for_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t authorized_api_add_items_to_access_group(
    authorized_api_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups);

aether_status_t authorized_api_remove_items_from_access_group(
    authorized_api_remote_t *api,
    aether_uuid_t id,
    uuid_array_view_t groups);

aether_status_t authorized_api_add_access_groups_to_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t authorized_api_remove_access_groups_from_client(
    authorized_api_remote_t *api,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t authorized_api_request_all_accessed_clients(
    authorized_api_remote_t *api,
    uuid_array_view_t uids);

aether_status_t authorized_api_request_access_check(
    authorized_api_remote_t *api,
    access_check_pair_array_view_t requests);

aether_status_t authorized_api_get_client_activity(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit);

aether_status_t authorized_api_search_client_logs(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    aether_bytes_view_t query,
    int32_t limit);

aether_status_t authorized_api_get_client_connections(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    int32_t limit);

aether_status_t authorized_api_get_client_messages(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit);

aether_status_t authorized_api_set_next_read_delay(
    authorized_api_remote_t *api,
    int64_t delayMillis);

aether_status_t authorized_api_get_uap(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t authorized_api_request_web_rtc_session(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t authorized_api_publish_web_rtc_session(
    authorized_api_remote_t *api,
    const web_rtc_session_t * session);

aether_status_t authorized_api_self_destruct(
    authorized_api_remote_t *api,
    uint32_t request_id);

aether_status_t authorized_api_get_servers(
    authorized_api_remote_t *api,
    uint32_t request_id);

aether_status_t authorized_api_get_client_timing(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t authorized_api_open_receive_window(
    authorized_api_remote_t *api,
    uint32_t request_id,
    int64_t durationMs);

aether_status_t authorized_api_switch_version(
    authorized_api_remote_t *api,
    int32_t version);

aether_status_t authorized_api_set_receive_window(
    authorized_api_remote_t *api,
    int64_t startsInMs,
    int64_t durationMs);

aether_status_t authorized_api_probe_packet(
    authorized_api_remote_t *api,
    int32_t testId,
    int32_t sequence,
    aether_bytes_view_t payload);

aether_status_t authorized_api_request_probe_report(
    authorized_api_remote_t *api,
    int32_t testId,
    int32_t firstSequence,
    int32_t count);

aether_status_t authorized_api_get_alias(
    authorized_api_remote_t *api,
    uint32_t request_id);

aether_status_t authorized_api_set_alias(
    authorized_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t alias);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} login_api_remote_t;

aether_status_t login_api_get_time_u_t_c(
    login_api_remote_t *api,
    uint32_t request_id);

aether_status_t login_api_login_by_u_i_d(
    login_api_remote_t *api,
    aether_uuid_t uid,
    const login_stream_t * data);

aether_status_t login_api_login_by_alias(
    login_api_remote_t *api,
    aether_uuid_t alias,
    const login_stream_t * data);

aether_status_t login_api_get_my_ip(
    login_api_remote_t *api,
    uint32_t request_id);

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

aether_status_t server_api_by_uid_get_balance(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

aether_status_t server_api_by_uid_set_parent(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t server_api_by_uid_block(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

aether_status_t server_api_by_uid_get_position(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

aether_status_t server_api_by_uid_get_parent(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

aether_status_t server_api_by_uid_get_beneficiary(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

aether_status_t server_api_by_uid_set_beneficiary(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t server_api_by_uid_get_block_time(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

aether_status_t server_api_by_uid_unblock(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

aether_status_t server_api_by_uid_create_time(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

aether_status_t server_api_by_uid_online_time(
    server_api_by_uid_remote_t *api,
    uint32_t request_id);

aether_status_t server_api_by_uid_add_access_group(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId);

aether_status_t server_api_by_uid_remove_access_group(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    aether_uuid_t groupId);

aether_status_t server_api_by_uid_set_msg_queue_limit(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    int32_t limit);

aether_status_t server_api_by_uid_set_msg_time_limit(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    int32_t seconds);

aether_status_t server_api_by_uid_add_servers_to_cloud(
    server_api_by_uid_remote_t *api,
    uint32_t request_id,
    short_array_view_t sids);

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

aether_status_t client_api_reg_unsafe_enter(
    client_api_reg_unsafe_remote_t *api,
    const client_api_reg_safe_stream_t * stream);

aether_status_t client_api_reg_unsafe_enter_global(
    client_api_reg_unsafe_remote_t *api,
    const global_reg_client_api_stream_t * stream);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} global_reg_server_api_remote_t;

aether_status_t global_reg_server_api_set_master_key(
    global_reg_server_api_remote_t *api,
    const key_ref_t * key);

aether_status_t global_reg_server_api_finish(
    global_reg_server_api_remote_t *api,
    uint32_t request_id);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} server_registration_api_remote_t;

aether_status_t server_registration_api_registration(
    server_registration_api_remote_t *api,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const global_api_stream_t * globalApi);

aether_status_t server_registration_api_registration_direct(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const key_ref_t * masterKey);

aether_status_t server_registration_api_request_work_proof_data(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t parent,
    pow_method_t powMethods);

aether_status_t server_registration_api_resolve_servers(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    const cloud_t * serverIds);

aether_status_t server_registration_api_set_return_key(
    server_registration_api_remote_t *api,
    const key_ref_t * key);

aether_status_t server_registration_api_recovery(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_uuid_t uid,
    const key_ref_t * masterKey);

typedef struct {
    void *send_ctx;
    client_server_api_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} registration_root_api_remote_t;

aether_status_t registration_root_api_get_asymmetric_public_key(
    registration_root_api_remote_t *api,
    uint32_t request_id,
    crypto_lib_t cryptoLib);

aether_status_t registration_root_api_enter(
    registration_root_api_remote_t *api,
    crypto_lib_t cryptoLib,
    const server_registration_api_stream_t * stream);

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

aether_status_t login_stream_authorized_api_back_id(
    login_stream_t *builder_stream,
    int8_t id);

aether_status_t login_stream_authorized_api_ping(
    login_stream_t *builder_stream,
    uint32_t request_id,
    int64_t nextConnectMsDuration,
    int64_t rxWindowMs);

aether_status_t login_stream_authorized_api_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    const client_api_stream_t * stream);

aether_status_t login_stream_authorized_api_send_message(
    login_stream_t *builder_stream,
    const message_t * msg);

aether_status_t login_stream_authorized_api_send_messages(
    login_stream_t *builder_stream,
    message_array_view_t msg);

aether_status_t login_stream_authorized_api_send_multicast(
    login_stream_t *builder_stream,
    uuid_array_view_t uids,
    aether_bytes_view_t data);

aether_status_t login_stream_authorized_api_send_message_with_result(
    login_stream_t *builder_stream,
    uint32_t request_id,
    const message_t * msg);

aether_status_t login_stream_authorized_api_create_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t owner,
    uuid_array_view_t uids);

aether_status_t login_stream_authorized_api_add_to_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid);

aether_status_t login_stream_authorized_api_remove_from_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId,
    aether_uuid_t uid);

aether_status_t login_stream_authorized_api_check_access_for_send_message(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t login_stream_authorized_api_resolver_servers(
    login_stream_t *builder_stream,
    short_array_view_t sid);

aether_status_t login_stream_authorized_api_resolve_clouds(
    login_stream_t *builder_stream,
    uuid_array_view_t uids);

aether_status_t login_stream_authorized_api_report_applied_config(
    login_stream_t *builder_stream,
    applied_config_array_view_t configs);

aether_status_t login_stream_authorized_api_get_access_groups(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t login_stream_authorized_api_get_access_group(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId);

aether_status_t login_stream_authorized_api_get_all_accessed_clients(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t login_stream_authorized_api_check_access_for_send_message2(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid1,
    aether_uuid_t uid2);

aether_status_t login_stream_authorized_api_send_telemetry(
    login_stream_t *builder_stream,
    const telemetry_ref_t * telemetry);

aether_status_t login_stream_authorized_api_request_access_groups_for_clients(
    login_stream_t *builder_stream,
    uuid_array_view_t uids);

aether_status_t login_stream_authorized_api_request_access_groups_items(
    login_stream_t *builder_stream,
    uuid_array_view_t ids);

aether_status_t login_stream_authorized_api_send_access_group_for_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t login_stream_authorized_api_add_items_to_access_group(
    login_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups);

aether_status_t login_stream_authorized_api_remove_items_from_access_group(
    login_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups);

aether_status_t login_stream_authorized_api_add_access_groups_to_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t login_stream_authorized_api_remove_access_groups_from_client(
    login_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t login_stream_authorized_api_request_all_accessed_clients(
    login_stream_t *builder_stream,
    uuid_array_view_t uids);

aether_status_t login_stream_authorized_api_request_access_check(
    login_stream_t *builder_stream,
    access_check_pair_array_view_t requests);

aether_status_t login_stream_authorized_api_get_client_activity(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit);

aether_status_t login_stream_authorized_api_search_client_logs(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    aether_bytes_view_t query,
    int32_t limit);

aether_status_t login_stream_authorized_api_get_client_connections(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    int32_t limit);

aether_status_t login_stream_authorized_api_get_client_messages(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    int64_t fromTime,
    int64_t toTime,
    int32_t limit);

aether_status_t login_stream_authorized_api_set_next_read_delay(
    login_stream_t *builder_stream,
    int64_t delayMillis);

aether_status_t login_stream_authorized_api_get_uap(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t login_stream_authorized_api_request_web_rtc_session(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t login_stream_authorized_api_publish_web_rtc_session(
    login_stream_t *builder_stream,
    const web_rtc_session_t * session);

aether_status_t login_stream_authorized_api_self_destruct(
    login_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t login_stream_authorized_api_get_servers(
    login_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t login_stream_authorized_api_get_client_timing(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t login_stream_authorized_api_open_receive_window(
    login_stream_t *builder_stream,
    uint32_t request_id,
    int64_t durationMs);

aether_status_t login_stream_authorized_api_switch_version(
    login_stream_t *builder_stream,
    int32_t version);

aether_status_t login_stream_authorized_api_set_receive_window(
    login_stream_t *builder_stream,
    int64_t startsInMs,
    int64_t durationMs);

aether_status_t login_stream_authorized_api_probe_packet(
    login_stream_t *builder_stream,
    int32_t testId,
    int32_t sequence,
    aether_bytes_view_t payload);

aether_status_t login_stream_authorized_api_request_probe_report(
    login_stream_t *builder_stream,
    int32_t testId,
    int32_t firstSequence,
    int32_t count);

aether_status_t login_stream_authorized_api_get_alias(
    login_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t login_stream_authorized_api_set_alias(
    login_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t alias);

aether_status_t login_client_stream_client_api_safe_change_parent(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid);

aether_status_t login_client_stream_client_api_safe_change_alias(
    login_client_stream_t *builder_stream,
    aether_uuid_t alias);

aether_status_t login_client_stream_client_api_safe_new_children(
    login_client_stream_t *builder_stream,
    uuid_array_view_t uids);

aether_status_t login_client_stream_client_api_safe_send_messages(
    login_client_stream_t *builder_stream,
    message_array_view_t msg);

aether_status_t login_client_stream_client_api_safe_send_server_descriptor(
    login_client_stream_t *builder_stream,
    const server_descriptor_t * serverDescriptor);

aether_status_t login_client_stream_client_api_safe_send_server_descriptors(
    login_client_stream_t *builder_stream,
    server_descriptor_array_view_t serverDescriptors);

aether_status_t login_client_stream_client_api_safe_send_cloud(
    login_client_stream_t *builder_stream,
    const u_u_i_d_and_cloud_t * uidAndCloud);

aether_status_t login_client_stream_client_api_safe_send_clouds(
    login_client_stream_t *builder_stream,
    u_u_i_d_and_cloud_array_view_t clouds);

aether_status_t login_client_stream_client_api_safe_request_telemetry(
    login_client_stream_t *builder_stream);

aether_status_t login_client_stream_client_api_safe_send_access_groups(
    login_client_stream_t *builder_stream,
    access_group_array_view_t groups);

aether_status_t login_client_stream_client_api_safe_send_access_group_for_client(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t login_client_stream_client_api_safe_add_items_to_access_group(
    login_client_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups);

aether_status_t login_client_stream_client_api_safe_remove_items_from_access_group(
    login_client_stream_t *builder_stream,
    aether_uuid_t id,
    uuid_array_view_t groups);

aether_status_t login_client_stream_client_api_safe_add_access_groups_to_client(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t login_client_stream_client_api_safe_remove_access_groups_from_client(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t groups);

aether_status_t login_client_stream_client_api_safe_send_all_accessed_clients(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    uuid_array_view_t accessedClients);

aether_status_t login_client_stream_client_api_safe_send_access_check_results(
    login_client_stream_t *builder_stream,
    access_check_result_array_view_t results);

aether_status_t login_client_stream_client_api_safe_send_message(
    login_client_stream_t *builder_stream,
    const message_t * msg);

aether_status_t login_client_stream_client_api_safe_send_cloud_configs(
    login_client_stream_t *builder_stream,
    cloud_config_array_view_t configs);

aether_status_t login_client_stream_client_api_safe_client_interaction(
    login_client_stream_t *builder_stream,
    aether_uuid_t uid,
    const client_interaction_client_stream_t * stream);

aether_status_t login_client_stream_client_api_safe_probe_report(
    login_client_stream_t *builder_stream,
    const probe_report_t * report);

aether_status_t client_api_stream_server_api_by_uid_get_balance(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t client_api_stream_server_api_by_uid_set_parent(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t client_api_stream_server_api_by_uid_block(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t client_api_stream_server_api_by_uid_get_position(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t client_api_stream_server_api_by_uid_get_parent(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t client_api_stream_server_api_by_uid_get_beneficiary(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t client_api_stream_server_api_by_uid_set_beneficiary(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid);

aether_status_t client_api_stream_server_api_by_uid_get_block_time(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t client_api_stream_server_api_by_uid_unblock(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t client_api_stream_server_api_by_uid_create_time(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t client_api_stream_server_api_by_uid_online_time(
    client_api_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t client_api_stream_server_api_by_uid_add_access_group(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId);

aether_status_t client_api_stream_server_api_by_uid_remove_access_group(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t groupId);

aether_status_t client_api_stream_server_api_by_uid_set_msg_queue_limit(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    int32_t limit);

aether_status_t client_api_stream_server_api_by_uid_set_msg_time_limit(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    int32_t seconds);

aether_status_t client_api_stream_server_api_by_uid_add_servers_to_cloud(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    short_array_view_t sids);

aether_status_t client_api_stream_server_api_by_uid_remove_servers_from_cloud(
    client_api_stream_t *builder_stream,
    uint32_t request_id,
    short_array_view_t sids);

aether_status_t global_api_stream_global_reg_server_api_set_master_key(
    global_api_stream_t *builder_stream,
    const key_ref_t * key);

aether_status_t global_api_stream_global_reg_server_api_finish(
    global_api_stream_t *builder_stream,
    uint32_t request_id);

aether_status_t server_registration_api_stream_server_registration_api_registration(
    server_registration_api_stream_t *builder_stream,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const global_api_stream_t * globalApi);

aether_status_t server_registration_api_stream_server_registration_api_registration_direct(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const key_ref_t * masterKey);

aether_status_t server_registration_api_stream_server_registration_api_request_work_proof_data(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t parent,
    pow_method_t powMethods);

aether_status_t server_registration_api_stream_server_registration_api_resolve_servers(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    const cloud_t * serverIds);

aether_status_t server_registration_api_stream_server_registration_api_set_return_key(
    server_registration_api_stream_t *builder_stream,
    const key_ref_t * key);

aether_status_t server_registration_api_stream_server_registration_api_recovery(
    server_registration_api_stream_t *builder_stream,
    uint32_t request_id,
    aether_uuid_t uid,
    const key_ref_t * masterKey);

#ifdef __cplusplus
}
#endif

#endif

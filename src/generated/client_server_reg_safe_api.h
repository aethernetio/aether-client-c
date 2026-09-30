#ifndef CLIENT_SERVER_REG_SAFE_API_H
#define CLIENT_SERVER_REG_SAFE_API_H

#include "client_server_reg_safe_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef aether_status_t (*client_server_reg_safe_send_fn)(
    void *ctx,
    const uint8_t *data,
    size_t length);

typedef struct {
    void *send_ctx;
    client_server_reg_safe_send_fn send;
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

aether_status_t server_registration_api_registration_direct(
    server_registration_api_remote_t *api,
    uint32_t request_id,
    aether_bytes_view_t salt,
    aether_bytes_view_t suffix,
    int_array_view_t passwords,
    aether_uuid_t parent,
    const key_ref_t * masterKey);

#ifdef __cplusplus
}
#endif

#endif

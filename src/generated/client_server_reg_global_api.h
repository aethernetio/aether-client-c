#ifndef CLIENTSERVERREGGLOBAL_API_H
#define CLIENTSERVERREGGLOBAL_API_H

#include "client_server_reg_global_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef aether_status_t (*client_server_reg_global_send_fn)(
    void *ctx,
    const uint8_t *data,
    size_t length);

typedef struct {
    void *send_ctx;
    client_server_reg_global_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} global_reg_server_api_remote_t;

aether_status_t global_reg_server_api_set_master_key(
    global_reg_server_api_remote_t *api,
    const key_ref_t * key);

aether_status_t global_reg_server_api_finish(
    global_reg_server_api_remote_t *api,
    uint32_t request_id);

#ifdef __cplusplus
}
#endif

#endif

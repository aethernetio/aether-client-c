#ifndef CLIENTSERVERREGROOTENTER_API_H
#define CLIENTSERVERREGROOTENTER_API_H

#include "client_server_reg_root_enter_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef aether_status_t (*client_server_reg_root_enter_send_fn)(
    void *ctx,
    const uint8_t *data,
    size_t length);

typedef struct {
    void *send_ctx;
    client_server_reg_root_enter_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} registration_root_api_remote_t;

aether_status_t registration_root_api_enter(
    registration_root_api_remote_t *api,
    crypto_lib_t cryptoLib,
    const server_registration_api_stream_t * stream);

#ifdef __cplusplus
}
#endif

#endif

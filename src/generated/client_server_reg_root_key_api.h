#ifndef CLIENTSERVERREGROOTKEY_API_H
#define CLIENTSERVERREGROOTKEY_API_H

#include "client_server_reg_root_key_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef aether_status_t (*client_server_reg_root_key_send_fn)(
    void *ctx,
    const uint8_t *data,
    size_t length);

typedef struct {
    void *send_ctx;
    client_server_reg_root_key_send_fn send;
    uint8_t *tx;
    size_t tx_capacity;
} registration_root_api_remote_t;

aether_status_t registration_root_api_get_asymmetric_public_key(
    registration_root_api_remote_t *api,
    uint32_t request_id,
    crypto_lib_t cryptoLib);

#ifdef __cplusplus
}
#endif

#endif

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
} authorized_api_remote_t;

aether_status_t authorized_api_send_message_with_result(
    authorized_api_remote_t *api,
    uint32_t request_id,
    const message_t * msg);

#ifdef __cplusplus
}
#endif

#endif

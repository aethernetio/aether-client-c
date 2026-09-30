#ifndef CLIENTSERVERREGUNSAFELOCAL_LOCAL_H
#define CLIENTSERVERREGUNSAFELOCAL_LOCAL_H

#include "client_server_reg_unsafe_local_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef aether_status_t (*client_api_reg_unsafe_enter_fn)(
    void *ctx,
    const client_api_reg_safe_stream_t *stream);

typedef aether_status_t (*client_api_reg_unsafe_enter_global_fn)(
    void *ctx,
    const global_reg_client_api_stream_t *stream);

typedef struct {
    void *ctx;
    client_api_reg_unsafe_enter_fn enter;
    client_api_reg_unsafe_enter_global_fn enter_global;
} client_api_reg_unsafe_local_t;

aether_status_t client_api_reg_unsafe_dispatch(
    client_api_reg_unsafe_local_t *api,
    const uint8_t *data,
    size_t length,
    size_t *consumed);

#ifdef __cplusplus
}
#endif

#endif

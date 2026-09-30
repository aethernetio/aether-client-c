#ifndef CLIENT_SERVER_REG_DIRECT_RESPONSE_RESPONSE_H
#define CLIENT_SERVER_REG_DIRECT_RESPONSE_RESPONSE_H

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    aether_uuid_t alias;
    aether_uuid_t uid;
    struct {
        struct { int16_t *data; size_t length; } data;
    } cloud;
} server_registration_api_registration_direct_result_t;

typedef struct {
    int16_t *cloud_data_storage;
    size_t cloud_data_capacity;
} server_registration_api_registration_direct_response_storage_t;

typedef enum {
    SERVER_REGISTRATION_API_REGISTRATION_DIRECT_RESPONSE_RESULT = 0,
    SERVER_REGISTRATION_API_REGISTRATION_DIRECT_RESPONSE_ERROR = 1
} server_registration_api_registration_direct_response_kind_t;

typedef struct {
    server_registration_api_registration_direct_response_kind_t kind;
    uint32_t request_id;
    server_registration_api_registration_direct_result_t result;
} server_registration_api_registration_direct_response_frame_t;

aether_status_t server_registration_api_registration_direct_response_decode(
    const server_registration_api_registration_direct_response_storage_t *api,
    const uint8_t *data,
    size_t length,
    server_registration_api_registration_direct_response_frame_t *frame,
    size_t *consumed);

#ifdef __cplusplus
}
#endif

#endif

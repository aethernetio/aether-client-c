#include "client_server_reg_direct_response_response.h"
#include "aether_meta_runtime.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef aether_meta_reader_t client_server_reg_direct_response_response_reader_t;
#define client_server_reg_direct_response_response_read_u8 aether_meta_read_u8
#define client_server_reg_direct_response_response_read_u16le aether_meta_read_u16le
#define client_server_reg_direct_response_response_read_u32le aether_meta_read_u32le
#define client_server_reg_direct_response_response_read_u64le aether_meta_read_u64le

#define client_server_reg_direct_response_response_read_i16le aether_meta_read_i16le
#define client_server_reg_direct_response_response_read_i32le aether_meta_read_i32le
#define client_server_reg_direct_response_response_read_i64le aether_meta_read_i64le

#define client_server_reg_direct_response_response_read_pack aether_meta_read_pack

#define client_server_reg_direct_response_response_read_view aether_meta_read_view
#define client_server_reg_direct_response_response_read_uuid aether_meta_read_uuid

aether_status_t server_registration_api_registration_direct_response_decode(
    const server_registration_api_registration_direct_response_storage_t *api,
    const uint8_t *data,
    size_t length,
    server_registration_api_registration_direct_response_frame_t *frame,
    size_t *consumed) {

    if (frame == NULL || consumed == NULL || (length != 0u && data == NULL) || api == NULL) return AETHER_ERR_ARGUMENT;
    *frame = (server_registration_api_registration_direct_response_frame_t){0};
    *consumed = 0u;
    client_server_reg_direct_response_response_reader_t reader = { data, length, 0u };
    uint8_t command;
    uint32_t request_id;
    if (!client_server_reg_direct_response_response_read_u8(&reader, &command) || !client_server_reg_direct_response_response_read_u32le(&reader, &request_id)) return AETHER_ERR_PROTOCOL;
    frame->request_id = request_id;
    if (command == 1u) { frame->kind = SERVER_REGISTRATION_API_REGISTRATION_DIRECT_RESPONSE_ERROR; *consumed = reader.position; return AETHER_OK; }
    if (command != 0u) return AETHER_ERR_PROTOCOL;
    server_registration_api_registration_direct_result_t result = {0};

        if (!client_server_reg_direct_response_response_read_uuid(
                &reader,
                &result.alias)) {
            return AETHER_ERR_PROTOCOL;
        }

        if (!client_server_reg_direct_response_response_read_uuid(
                &reader,
                &result.uid)) {
            return AETHER_ERR_PROTOCOL;
        }

        {
            uint64_t count64 = 0u;
            if (!client_server_reg_direct_response_response_read_pack(
                    &reader,
                    &count64) ||
                count64 > (uint64_t)SIZE_MAX) {
                return AETHER_ERR_PROTOCOL;
            }

            size_t count =
                (size_t)count64;

            if (count > api->cloud_data_capacity ||
                (count != 0u &&
                 api->cloud_data_storage == NULL)) {
                return AETHER_ERR_OVERFLOW;
            }

            result.cloud.data.data =
                api->cloud_data_storage;
            result.cloud.data.length =
                count;

            for (size_t i = 0u;
                 i < count;
                 ++i) {
                if (!client_server_reg_direct_response_response_read_i16le(
                        &reader,
                        &api->cloud_data_storage[i])) {
                    return AETHER_ERR_PROTOCOL;
                }
            }
        }

    frame->kind = SERVER_REGISTRATION_API_REGISTRATION_DIRECT_RESPONSE_RESULT;
    frame->result = result;
    *consumed = reader.position;
    return AETHER_OK;
}

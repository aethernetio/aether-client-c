
#include "aether_reg_root_enter.h"

#include "client_server_reg_root_enter_api.h"


typedef struct {
    size_t *length;
    size_t capacity;
} aether_reg_root_enter_commit_t;


static aether_status_t aether_reg_root_enter_commit(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    (void)data;

    aether_reg_root_enter_commit_t *commit =
        ctx;

    if (commit == NULL ||
        commit->length == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (length > commit->capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    *commit->length =
        length;

    return AETHER_OK;
}


aether_status_t aether_generated_reg_root_build_enter(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t crypto_lib,
    const uint8_t *encrypted_stream,
    size_t encrypted_stream_length) {

    if (length == NULL ||
        (capacity > 0u &&
         data == NULL) ||
        (encrypted_stream == NULL &&
         encrypted_stream_length != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    *length =
        0u;

    aether_reg_root_enter_commit_t commit = {
        .length = length,
        .capacity = capacity
    };

    registration_root_api_remote_t remote = {
        .send_ctx = &commit,
        .send = aether_reg_root_enter_commit,
        .tx = data,
        .tx_capacity = capacity
    };

    server_registration_api_stream_t stream = {
        .data =
            (uint8_t *)encrypted_stream,
        .capacity =
            encrypted_stream_length,
        .length =
            encrypted_stream_length
    };

    return
        registration_root_api_enter(
            &remote,
            (crypto_lib_t)crypto_lib,
            &stream);
}


#include "aether_reg_root_key.h"

#include "client_server_reg_root_key_api.h"


typedef struct {
    size_t *length;
    size_t capacity;
} aether_reg_root_key_commit_t;


static aether_status_t aether_reg_root_key_commit(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    (void)data;

    aether_reg_root_key_commit_t *commit =
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


aether_status_t aether_generated_reg_root_build_get_key(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint32_t request_id,
    uint8_t crypto_lib) {

    if (length == NULL ||
        (capacity > 0u &&
         data == NULL)) {

        return AETHER_ERR_ARGUMENT;
    }

    *length =
        0u;

    aether_reg_root_key_commit_t commit = {
        .length = length,
        .capacity = capacity
    };

    registration_root_api_remote_t remote = {
        .send_ctx = &commit,
        .send = aether_reg_root_key_commit,
        .tx = data,
        .tx_capacity = capacity
    };

    return
        registration_root_api_get_asymmetric_public_key(
            &remote,
            request_id,
            (crypto_lib_t)crypto_lib);
}

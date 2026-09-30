
#include "aether_reg_global.h"

#include "client_server_reg_global_api.h"

#include <string.h>


typedef struct {
    size_t *length;
    size_t remaining;
} aether_reg_global_commit_t;


static aether_status_t aether_reg_global_commit(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    (void)data;

    aether_reg_global_commit_t *commit =
        ctx;

    if (commit == NULL ||
        commit->length == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (length > commit->remaining) {
        return AETHER_ERR_OVERFLOW;
    }

    *commit->length +=
        length;

    return AETHER_OK;
}


aether_status_t aether_generated_reg_global_build(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *key,
    size_t key_length,
    uint32_t request_id) {

    if (length == NULL ||
        key == NULL ||
        key_length != 32u ||
        (capacity > 0u &&
         data == NULL)) {

        return AETHER_ERR_ARGUMENT;
    }

    union {
        sodium_chacha20_poly1305_t sodium;
        hydrogen_secret_box_t hydrogen;
        p256_aes_gcm_symmetric_t p256;
    } key_storage;

    memset(
        &key_storage,
        0,
        sizeof(key_storage));

    key_ref_t key_ref = {
        .type_id =
            symmetric_key_type
    };

    switch (symmetric_key_type) {
    case KEY_SYMMETRIC_TYPE_SODIUM_CHACHA20_POLY1305:
        memcpy(
            key_storage.sodium.data.data,
            key,
            32u);

        key_ref.as.sodium_chacha20_poly1305 =
            &key_storage.sodium;
        break;

    case KEY_SYMMETRIC_TYPE_HYDROGEN_SECRET_BOX:
        memcpy(
            key_storage.hydrogen.data.data,
            key,
            32u);

        key_ref.as.hydrogen_secret_box =
            &key_storage.hydrogen;
        break;

    case KEY_SYMMETRIC_TYPE_P256_AES_GCM_SYMMETRIC:
        memcpy(
            key_storage.p256.data.data,
            key,
            32u);

        key_ref.as.p256_aes_gcm_symmetric =
            &key_storage.p256;
        break;

    default:
        return AETHER_ERR_ARGUMENT;
    }

    size_t produced =
        0u;

    aether_reg_global_commit_t commit = {
        .length = &produced,
        .remaining = capacity
    };

    global_reg_server_api_remote_t remote = {
        .send_ctx = &commit,
        .send = aether_reg_global_commit,
        .tx = data,
        .tx_capacity = capacity
    };

    aether_status_t status =
        global_reg_server_api_set_master_key(
            &remote,
            &key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    if (produced > capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    commit.remaining =
        capacity - produced;

    remote.tx =
        data + produced;

    remote.tx_capacity =
        capacity - produced;

    status =
        global_reg_server_api_finish(
            &remote,
            request_id);

    if (status != AETHER_OK) {
        return status;
    }

    *length =
        produced;

    return AETHER_OK;
}

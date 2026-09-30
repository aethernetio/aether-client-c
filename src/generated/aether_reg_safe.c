
#include "aether_reg_safe.h"

#include "client_server_reg_safe_api.h"

#include <string.h>


typedef struct {
    size_t *length;
    size_t remaining;
} aether_reg_safe_commit_t;


typedef union {
    sodium_chacha20_poly1305_t sodium;
    hydrogen_secret_box_t hydrogen;
    p256_aes_gcm_symmetric_t p256;
} aether_reg_safe_key_storage_t;


static aether_status_t aether_reg_safe_commit(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    (void)data;

    aether_reg_safe_commit_t *commit =
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


static aether_status_t aether_reg_safe_key(
    uint8_t symmetric_key_type,
    const uint8_t *key,
    size_t key_length,
    aether_reg_safe_key_storage_t *storage,
    key_ref_t *key_ref) {

    if (key == NULL ||
        storage == NULL ||
        key_ref == NULL ||
        key_length != 32u) {

        return AETHER_ERR_ARGUMENT;
    }

    memset(
        storage,
        0,
        sizeof(*storage));

    memset(
        key_ref,
        0,
        sizeof(*key_ref));

    key_ref->type_id =
        symmetric_key_type;

    switch (symmetric_key_type) {
    case KEY_SYMMETRIC_TYPE_SODIUM_CHACHA20_POLY1305:
        memcpy(
            storage->sodium.data.data,
            key,
            32u);

        key_ref->as.sodium_chacha20_poly1305 =
            &storage->sodium;
        return AETHER_OK;

    case KEY_SYMMETRIC_TYPE_HYDROGEN_SECRET_BOX:
        memcpy(
            storage->hydrogen.data.data,
            key,
            32u);

        key_ref->as.hydrogen_secret_box =
            &storage->hydrogen;
        return AETHER_OK;

    case KEY_SYMMETRIC_TYPE_P256_AES_GCM_SYMMETRIC:
        memcpy(
            storage->p256.data.data,
            key,
            32u);

        key_ref->as.p256_aes_gcm_symmetric =
            &storage->p256;
        return AETHER_OK;

    default:
        return AETHER_ERR_ARGUMENT;
    }
}


static void aether_reg_safe_advance(
    server_registration_api_remote_t *remote,
    aether_reg_safe_commit_t *commit,
    uint8_t *data,
    size_t capacity,
    size_t produced) {

    commit->remaining =
        capacity - produced;

    remote->tx =
        data != NULL
            ? data + produced
            : NULL;

    remote->tx_capacity =
        capacity - produced;
}


static aether_status_t aether_reg_safe_remote(
    uint8_t *data,
    size_t capacity,
    size_t *produced,
    aether_reg_safe_commit_t *commit,
    server_registration_api_remote_t *remote) {

    if (produced == NULL ||
        commit == NULL ||
        remote == NULL ||
        (capacity > 0u &&
         data == NULL)) {

        return AETHER_ERR_ARGUMENT;
    }

    *produced =
        0u;

    commit->length =
        produced;

    commit->remaining =
        capacity;

    remote->send_ctx =
        commit;

    remote->send =
        aether_reg_safe_commit;

    remote->tx =
        data;

    remote->tx_capacity =
        capacity;

    return AETHER_OK;
}


aether_status_t aether_generated_reg_safe_build_pow(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *key,
    size_t key_length,
    uint32_t request_id,
    aether_uuid_t parent) {

    if (length == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    aether_reg_safe_key_storage_t key_storage;
    key_ref_t key_ref;

    aether_status_t status =
        aether_reg_safe_key(
            symmetric_key_type,
            key,
            key_length,
            &key_storage,
            &key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    size_t produced;
    aether_reg_safe_commit_t commit;
    server_registration_api_remote_t remote;

    status =
        aether_reg_safe_remote(
            data,
            capacity,
            &produced,
            &commit,
            &remote);

    if (status != AETHER_OK) {
        return status;
    }

    status =
        server_registration_api_set_return_key(
            &remote,
            &key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    aether_reg_safe_advance(
        &remote,
        &commit,
        data,
        capacity,
        produced);

    status =
        server_registration_api_request_work_proof_data(
            &remote,
            request_id,
            parent,
            pow_method_AE_BCRYPT_CRC32);

    if (status != AETHER_OK) {
        return status;
    }

    *length =
        produced;

    return AETHER_OK;
}


aether_status_t aether_generated_reg_safe_build_registration(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *return_key,
    size_t return_key_length,
    const uint8_t *salt,
    size_t salt_length,
    const uint8_t *suffix,
    size_t suffix_length,
    const int32_t *passwords,
    size_t password_count,
    aether_uuid_t parent,
    const uint8_t *global_cipher,
    size_t global_cipher_length) {

    if (length == NULL ||
        (salt == NULL &&
         salt_length != 0u) ||
        (suffix == NULL &&
         suffix_length != 0u) ||
        (passwords == NULL &&
         password_count != 0u) ||
        (global_cipher == NULL &&
         global_cipher_length != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    aether_reg_safe_key_storage_t key_storage;
    key_ref_t key_ref;

    aether_status_t status =
        aether_reg_safe_key(
            symmetric_key_type,
            return_key,
            return_key_length,
            &key_storage,
            &key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    size_t produced;
    aether_reg_safe_commit_t commit;
    server_registration_api_remote_t remote;

    status =
        aether_reg_safe_remote(
            data,
            capacity,
            &produced,
            &commit,
            &remote);

    if (status != AETHER_OK) {
        return status;
    }

    status =
        server_registration_api_set_return_key(
            &remote,
            &key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    aether_reg_safe_advance(
        &remote,
        &commit,
        data,
        capacity,
        produced);

    aether_bytes_view_t salt_view = {
        .data = salt,
        .length = salt_length
    };

    aether_bytes_view_t suffix_view = {
        .data = suffix,
        .length = suffix_length
    };

    int_array_view_t password_view = {
        .data = passwords,
        .length = password_count
    };

    global_api_stream_t global_api = {
        .data =
            (uint8_t *)global_cipher,
        .capacity =
            global_cipher_length,
        .length =
            global_cipher_length
    };

    status =
        server_registration_api_registration(
            &remote,
            salt_view,
            suffix_view,
            password_view,
            parent,
            &global_api);

    if (status != AETHER_OK) {
        return status;
    }

    *length =
        produced;

    return AETHER_OK;
}



aether_status_t aether_generated_reg_safe_build_registration_direct(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *return_key,
    size_t return_key_length,
    const uint8_t *master_key,
    size_t master_key_length,
    uint32_t request_id,
    const uint8_t *salt,
    size_t salt_length,
    const uint8_t *suffix,
    size_t suffix_length,
    const int32_t *passwords,
    size_t password_count,
    aether_uuid_t parent) {

    if (length == NULL ||
        (salt == NULL && salt_length != 0u) ||
        (suffix == NULL && suffix_length != 0u) ||
        (passwords == NULL && password_count != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    aether_reg_safe_key_storage_t return_key_storage;
    key_ref_t return_key_ref;

    aether_status_t status =
        aether_reg_safe_key(
            symmetric_key_type,
            return_key,
            return_key_length,
            &return_key_storage,
            &return_key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    aether_reg_safe_key_storage_t master_key_storage;
    key_ref_t master_key_ref;

    status =
        aether_reg_safe_key(
            symmetric_key_type,
            master_key,
            master_key_length,
            &master_key_storage,
            &master_key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    size_t produced;
    aether_reg_safe_commit_t commit;
    server_registration_api_remote_t remote;

    status =
        aether_reg_safe_remote(
            data,
            capacity,
            &produced,
            &commit,
            &remote);

    if (status != AETHER_OK) {
        return status;
    }

    status =
        server_registration_api_set_return_key(
            &remote,
            &return_key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    aether_reg_safe_advance(
        &remote,
        &commit,
        data,
        capacity,
        produced);

    aether_bytes_view_t salt_view = {
        .data = salt,
        .length = salt_length
    };

    aether_bytes_view_t suffix_view = {
        .data = suffix,
        .length = suffix_length
    };

    int_array_view_t password_view = {
        .data = passwords,
        .length = password_count
    };

    status =
        server_registration_api_registration_direct(
            &remote,
            request_id,
            salt_view,
            suffix_view,
            password_view,
            parent,
            &master_key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    *length =
        produced;

    return AETHER_OK;
}



aether_status_t aether_generated_reg_safe_build_resolve(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint32_t request_id,
    const int16_t *server_ids,
    size_t server_count) {

    if (length == NULL ||
        (server_ids == NULL &&
         server_count != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    size_t produced;
    aether_reg_safe_commit_t commit;
    server_registration_api_remote_t remote;

    aether_status_t status =
        aether_reg_safe_remote(
            data,
            capacity,
            &produced,
            &commit,
            &remote);

    if (status != AETHER_OK) {
        return status;
    }

    short_array_view_t server_id_view = {
        .data = server_ids,
        .length = server_count
    };

    cloud_t cloud = {
        .data = server_id_view
    };

    status =
        server_registration_api_resolve_servers(
            &remote,
            request_id,
            &cloud);

    if (status != AETHER_OK) {
        return status;
    }

    *length =
        produced;

    return AETHER_OK;
}
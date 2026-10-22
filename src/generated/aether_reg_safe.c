
#include "aether_reg_safe.h"

#include "aether_api.h"

#include <string.h>


typedef struct {
    size_t *length;
} aether_reg_safe_commit_t;


typedef union {
    common_sodium_chacha20_poly1305_t sodium;
    common_hydrogen_secret_box_t hydrogen;
    common_p256_aes_gcm_symmetric_t p256;
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

    *commit->length =
        length;

    return AETHER_OK;
}


static aether_status_t aether_reg_safe_key(
    uint8_t symmetric_key_type,
    const uint8_t *key,
    size_t key_length,
    aether_reg_safe_key_storage_t *storage,
    common_key_ref_t *key_ref) {

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
    case COMMON_KEY_SYMMETRIC_TYPE_COMMON_SODIUM_CHACHA20_POLY1305:
        memcpy(
            storage->sodium.data.data,
            key,
            32u);

        key_ref->as.common_sodium_chacha20_poly1305 =
            &storage->sodium;
        return AETHER_OK;

    case COMMON_KEY_SYMMETRIC_TYPE_COMMON_HYDROGEN_SECRET_BOX:
        memcpy(
            storage->hydrogen.data.data,
            key,
            32u);

        key_ref->as.common_hydrogen_secret_box =
            &storage->hydrogen;
        return AETHER_OK;

    case COMMON_KEY_SYMMETRIC_TYPE_COMMON_P256_AES_GCM_SYMMETRIC:
        memcpy(
            storage->p256.data.data,
            key,
            32u);

        key_ref->as.common_p256_aes_gcm_symmetric =
            &storage->p256;
        return AETHER_OK;

    default:
        return AETHER_ERR_ARGUMENT;
    }
}


aether_status_t aether_generated_reg_safe_build_pow(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *key,
    size_t key_length,
    aether_uuid_t parent) {

    if (length == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    aether_reg_safe_key_storage_t key_storage;
    common_key_ref_t key_ref;

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

    aether_data_out_t out;
    aether_data_out_init(&out, data, capacity);

    size_t produced = 0u;

    aether_reg_safe_commit_t commit = {
        .length = &produced
    };

    csa_server_registration_api_remote_t remote = {
        .send_ctx = &commit,
        .send = aether_reg_safe_commit,
        .out = &out
    };

    status =
        csa_server_registration_api_set_return_key(
            &remote,
            &key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    status =
        csa_server_registration_api_request_work_proof_data(
            &remote,
            parent,
            common_pow_method_AE_BCRYPT_CRC32);

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

    (void)data;
    (void)capacity;
    (void)length;
    (void)symmetric_key_type;
    (void)return_key;
    (void)return_key_length;
    (void)salt;
    (void)salt_length;
    (void)suffix;
    (void)suffix_length;
    (void)passwords;
    (void)password_count;
    (void)parent;
    (void)global_cipher;
    (void)global_cipher_length;

    return AETHER_ERR_UNSUPPORTED;
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
    common_key_ref_t return_key_ref;

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
    common_key_ref_t master_key_ref;

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

    aether_data_out_t out;
    aether_data_out_init(&out, data, capacity);

    size_t produced = 0u;

    aether_reg_safe_commit_t commit = {
        .length = &produced
    };

    csa_server_registration_api_remote_t remote = {
        .send_ctx = &commit,
        .send = aether_reg_safe_commit,
        .out = &out
    };

    status =
        csa_server_registration_api_set_return_key(
            &remote,
            &return_key_ref);

    if (status != AETHER_OK) {
        return status;
    }

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
        csa_server_registration_api_registration_direct(
            &remote,
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


aether_status_t aether_generated_reg_safe_build_recovery(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *return_key,
    size_t return_key_length,
    uint8_t old_master_key_type,
    const uint8_t *old_master_key,
    size_t old_master_key_length,
    const uint8_t *new_master_key,
    size_t new_master_key_length,
    aether_uuid_t uid) {

    if (length == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    aether_reg_safe_key_storage_t return_key_storage;
    common_key_ref_t return_key_ref;

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

    aether_reg_safe_key_storage_t old_master_key_storage;
    common_key_ref_t old_master_key_ref;

    status =
        aether_reg_safe_key(
            old_master_key_type,
            old_master_key,
            old_master_key_length,
            &old_master_key_storage,
            &old_master_key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    aether_reg_safe_key_storage_t new_master_key_storage;
    common_key_ref_t new_master_key_ref;

    status =
        aether_reg_safe_key(
            symmetric_key_type,
            new_master_key,
            new_master_key_length,
            &new_master_key_storage,
            &new_master_key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    aether_data_out_t out;
    aether_data_out_init(&out, data, capacity);

    size_t produced = 0u;

    aether_reg_safe_commit_t commit = {
        .length = &produced
    };

    csa_server_registration_api_remote_t remote = {
        .send_ctx = &commit,
        .send = aether_reg_safe_commit,
        .out = &out
    };

    status =
        csa_server_registration_api_set_return_key(
            &remote,
            &return_key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    status =
        csa_server_registration_api_recovery(
            &remote,
            uid,
            &old_master_key_ref,
            &new_master_key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    *length =
        produced;

    return AETHER_OK;
}


aether_status_t aether_generated_reg_safe_build_resolve_with_return_key(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *return_key,
    size_t return_key_length,
    const int16_t *server_ids,
    size_t server_count) {

    if (length == NULL ||
        (server_ids == NULL &&
         server_count != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    aether_reg_safe_key_storage_t key_storage;
    common_key_ref_t key_ref;

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

    aether_data_out_t out;
    aether_data_out_init(&out, data, capacity);

    size_t produced = 0u;

    aether_reg_safe_commit_t commit = {
        .length = &produced
    };

    csa_server_registration_api_remote_t remote = {
        .send_ctx = &commit,
        .send = aether_reg_safe_commit,
        .out = &out
    };

    status =
        csa_server_registration_api_set_return_key(
            &remote,
            &key_ref);

    if (status != AETHER_OK) {
        return status;
    }

    short_array_view_t server_id_view = {
        .data = server_ids,
        .length = server_count
    };

    common_cloud_t cloud = {
        .data = server_id_view
    };

    status =
        csa_server_registration_api_resolve_servers(
            &remote,
            &cloud);

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
    const int16_t *server_ids,
    size_t server_count) {

    if (length == NULL ||
        (server_ids == NULL &&
         server_count != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    aether_data_out_t out;
    aether_data_out_init(&out, data, capacity);

    size_t produced = 0u;

    aether_reg_safe_commit_t commit = {
        .length = &produced
    };

    csa_server_registration_api_remote_t remote = {
        .send_ctx = &commit,
        .send = aether_reg_safe_commit,
        .out = &out
    };

    short_array_view_t server_id_view = {
        .data = server_ids,
        .length = server_count
    };

    common_cloud_t cloud = {
        .data = server_id_view
    };

    aether_status_t status =
        csa_server_registration_api_resolve_servers(
            &remote,
            &cloud);

    if (status != AETHER_OK) {
        return status;
    }

    *length =
        produced;

    return AETHER_OK;
}

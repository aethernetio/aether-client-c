#include "aether_test_crypto_hydrogen.h"

#include "aether_hydrogen.h"


static aether_status_t hydro_status(int result) {
    return result == 0 ? AETHER_OK : AETHER_ERR_CRYPTO;
}


static aether_status_t initialize(void *ctx) {
    (void)ctx;
    return hydro_status(aether_hydrogen_init());
}


static aether_status_t random_symmetric_key(
    void *ctx,
    uint8_t *out_key,
    size_t key_len) {

    (void)ctx;

    if (out_key == NULL ||
        key_len != AETHER_HYDROGEN_KEY_BYTES) {
        return AETHER_ERR_ARGUMENT;
    }

    if (aether_hydrogen_init() != 0) {
        return AETHER_ERR_CRYPTO;
    }

    aether_hydrogen_random_symmetric_key(out_key);
    return AETHER_OK;
}


static aether_status_t verify_signature(
    void *ctx,
    const uint8_t *public_key,
    size_t public_key_len,
    const uint8_t *signature,
    size_t signature_len,
    const uint8_t *data,
    size_t data_len) {

    (void)ctx;

    if (public_key_len != AETHER_HYDROGEN_KEY_BYTES ||
        signature_len != AETHER_HYDROGEN_SIGNATURE_BYTES) {
        return AETHER_ERR_ARGUMENT;
    }

    return hydro_status(
        aether_hydrogen_verify_signature(
            public_key,
            signature,
            data,
            data_len));
}


static aether_status_t asymmetric_encrypt(
    void *ctx,
    const uint8_t *public_key,
    size_t public_key_len,
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len) {

    (void)ctx;

    if (public_key_len != AETHER_HYDROGEN_KEY_BYTES) {
        return AETHER_ERR_ARGUMENT;
    }

    return hydro_status(
        aether_hydrogen_asymmetric_encrypt(
            public_key,
            plain,
            plain_len,
            out,
            out_capacity,
            out_len));
}


static aether_status_t symmetric_encrypt(
    void *ctx,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len) {

    (void)ctx;

    if (key_len != AETHER_HYDROGEN_KEY_BYTES) {
        return AETHER_ERR_ARGUMENT;
    }

    return hydro_status(
        aether_hydrogen_symmetric_encrypt(
            key,
            plain,
            plain_len,
            out,
            out_capacity,
            out_len));
}


static aether_status_t symmetric_decrypt(
    void *ctx,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *cipher,
    size_t cipher_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len) {

    (void)ctx;

    if (key_len != AETHER_HYDROGEN_KEY_BYTES) {
        return AETHER_ERR_ARGUMENT;
    }

    return hydro_status(
        aether_hydrogen_symmetric_decrypt(
            key,
            cipher,
            cipher_len,
            out,
            out_capacity,
            out_len));
}


static aether_status_t derive_server_keys(
    void *ctx,
    const uint8_t *master_key,
    size_t master_key_len,
    int32_t server_id,
    uint32_t key_number,
    uint8_t *client_to_server,
    uint8_t *server_to_client,
    size_t derived_key_len) {

    (void)ctx;

    if (master_key_len != AETHER_HYDROGEN_KEY_BYTES ||
        derived_key_len != AETHER_HYDROGEN_KEY_BYTES) {
        return AETHER_ERR_ARGUMENT;
    }

    return hydro_status(
        aether_hydrogen_derive_server_keys(
            master_key,
            server_id,
            key_number,
            client_to_server,
            server_to_client));
}


/*
 * Values are the current canonical ADSL values:
 * CryptoLib.HYDROGEN ordinal = 1
 * HydrogenSecretBox id       = 3
 * HydrogenCurvePublic id     = 2
 * SignHYDROGEN id            = 2
 */
const aether_crypto_vtable_t AETHER_TEST_HYDROGEN_CRYPTO = {
    .profile = {
        .crypto_lib = 1u,
        .symmetric_key_type = 3u,
        .asymmetric_public_key_type = 2u,
        .sign_type = 2u,
        .symmetric_key_bytes = 32u,
        .asymmetric_public_key_bytes = 32u,
        .sign_public_key_bytes = 32u,
        .signature_bytes = 64u
    },
    .initialize = initialize,
    .random_symmetric_key = random_symmetric_key,
    .verify_signature = verify_signature,
    .asymmetric_encrypt = asymmetric_encrypt,
    .symmetric_encrypt = symmetric_encrypt,
    .symmetric_decrypt = symmetric_decrypt,
    .derive_server_keys = derive_server_keys
};

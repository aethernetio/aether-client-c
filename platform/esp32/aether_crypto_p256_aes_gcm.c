
#include "aether_crypto_p256_aes_gcm.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "psa/crypto.h"


#define P256_AES_KEY_BYTES 32u
#define P256_PRIVATE_KEY_BYTES 32u
#define P256_PUBLIC_KEY_BYTES 64u
#define P256_PSA_PUBLIC_KEY_BYTES 65u
#define P256_NONCE_BYTES 12u
#define P256_TAG_BYTES 16u
#define P256_SIGNATURE_BYTES 64u
#define P256_SHARED_SECRET_BYTES 32u
#define P256_SHA256_BYTES 32u

#define P256_CRYPTO_LIB_ORDINAL 2u
#define P256_SYMMETRIC_KEY_TYPE 11u
#define P256_ASYMMETRIC_PUBLIC_KEY_TYPE 13u
#define P256_SIGN_TYPE 3u


_Static_assert(
    P256_AES_KEY_BYTES == 32u,
    "P256_AES_GCM key wire size changed");

_Static_assert(
    P256_PUBLIC_KEY_BYTES == 64u,
    "P256_AES_GCM public key wire size changed");

_Static_assert(
    P256_NONCE_BYTES == 12u,
    "P256_AES_GCM nonce wire size changed");

_Static_assert(
    P256_TAG_BYTES == 16u,
    "P256_AES_GCM tag wire size changed");

_Static_assert(
    P256_SIGNATURE_BYTES == 64u,
    "P256_AES_GCM signature wire size changed");


static const uint8_t ASYMMETRIC_HKDF_SALT[] =
    "aether-p256-asym-salt-v1";

static const uint8_t ASYMMETRIC_HKDF_INFO[] =
    "aether-p256-asym-key-v1";

static const uint8_t SERVER_HKDF_SALT[] =
    "aether-p256-server-salt-v1";

static const uint8_t SERVER_HKDF_INFO[] =
    "aether-p256-server-keys-v1";

static const uint8_t SIGN_CONTEXT[] =
    "aether-p256-sign-v1";


static aether_status_t status_from_psa(
    psa_status_t status
) {
    return status == PSA_SUCCESS
        ? AETHER_OK
        : AETHER_ERR_CRYPTO;
}


static void destroy_key(
    psa_key_id_t *key
) {
    if (key != NULL && *key != 0) {
        (void)psa_destroy_key(*key);
        *key = 0;
    }
}


static psa_status_t import_key(
    psa_key_type_t type,
    size_t bits,
    psa_key_usage_t usage,
    psa_algorithm_t algorithm,
    const uint8_t *data,
    size_t data_len,
    psa_key_id_t *out_key
) {
    if (data == NULL || out_key == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_key_attributes_t attributes =
        PSA_KEY_ATTRIBUTES_INIT;

    psa_set_key_type(
        &attributes,
        type
    );

    psa_set_key_bits(
        &attributes,
        bits
    );

    psa_set_key_usage_flags(
        &attributes,
        usage
    );

    psa_set_key_algorithm(
        &attributes,
        algorithm
    );

    psa_status_t status =
        psa_import_key(
            &attributes,
            data,
            data_len,
            out_key
        );

    psa_reset_key_attributes(
        &attributes
    );

    return status;
}


static psa_status_t import_aes_key(
    const uint8_t *key,
    psa_key_usage_t usage,
    psa_key_id_t *out_key
) {
    return import_key(
        PSA_KEY_TYPE_AES,
        P256_AES_KEY_BYTES * 8u,
        usage,
        PSA_ALG_GCM,
        key,
        P256_AES_KEY_BYTES,
        out_key
    );
}


static psa_status_t hmac_sha256(
    const uint8_t *key,
    size_t key_len,
    const uint8_t *data,
    size_t data_len,
    uint8_t out[P256_SHA256_BYTES]
) {
    if (key == NULL
            || key_len == 0u
            || (data == NULL && data_len != 0u)
            || out == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_key_id_t key_id = 0;

    psa_status_t status =
        import_key(
            PSA_KEY_TYPE_HMAC,
            key_len * 8u,
            PSA_KEY_USAGE_SIGN_MESSAGE,
            PSA_ALG_HMAC(
                PSA_ALG_SHA_256
            ),
            key,
            key_len,
            &key_id
        );

    if (status != PSA_SUCCESS) {
        return status;
    }

    size_t output_len = 0u;

    status =
        psa_mac_compute(
            key_id,
            PSA_ALG_HMAC(
                PSA_ALG_SHA_256
            ),
            data,
            data_len,
            out,
            P256_SHA256_BYTES,
            &output_len
        );

    destroy_key(
        &key_id
    );

    if (status != PSA_SUCCESS) {
        return status;
    }

    return output_len ==
            P256_SHA256_BYTES
        ? PSA_SUCCESS
        : PSA_ERROR_CORRUPTION_DETECTED;
}


static psa_status_t hkdf_sha256(
    const uint8_t *ikm,
    size_t ikm_len,
    const uint8_t *salt,
    size_t salt_len,
    const uint8_t *info,
    size_t info_len,
    uint8_t *out,
    size_t out_len
) {
    if (ikm == NULL
            || ikm_len == 0u
            || salt == NULL
            || salt_len == 0u
            || (info == NULL
                && info_len != 0u)
            || out == NULL
            || out_len == 0u
            || out_len >
                P256_SHA256_BYTES * 2u
            || info_len > 95u) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    uint8_t prk[
        P256_SHA256_BYTES
    ];

    uint8_t previous[
        P256_SHA256_BYTES
    ];

    uint8_t input[
        P256_SHA256_BYTES
            + 95u
            + 1u
    ];

    psa_status_t status =
        hmac_sha256(
            salt,
            salt_len,
            ikm,
            ikm_len,
            prk
        );

    if (status != PSA_SUCCESS) {
        return status;
    }

    size_t produced = 0u;
    size_t previous_len = 0u;
    uint8_t counter = 1u;

    while (produced < out_len) {
        size_t input_len = 0u;

        if (previous_len != 0u) {
            memcpy(
                input + input_len,
                previous,
                previous_len
            );

            input_len +=
                previous_len;
        }

        if (info_len != 0u) {
            memcpy(
                input + input_len,
                info,
                info_len
            );

            input_len +=
                info_len;
        }

        input[input_len++] =
            counter;

        status =
            hmac_sha256(
                prk,
                sizeof(prk),
                input,
                input_len,
                previous
            );

        if (status != PSA_SUCCESS) {
            memset(
                prk,
                0,
                sizeof(prk)
            );

            memset(
                previous,
                0,
                sizeof(previous)
            );

            memset(
                input,
                0,
                sizeof(input)
            );

            return status;
        }

        previous_len =
            sizeof(previous);

        size_t remaining =
            out_len - produced;

        size_t copy_len =
            remaining <
                    previous_len
                ? remaining
                : previous_len;

        memcpy(
            out + produced,
            previous,
            copy_len
        );

        produced +=
            copy_len;

        counter++;
    }

    memset(
        prk,
        0,
        sizeof(prk)
    );

    memset(
        previous,
        0,
        sizeof(previous)
    );

    memset(
        input,
        0,
        sizeof(input)
    );

    return PSA_SUCCESS;
}


static void raw_public_to_psa(
    const uint8_t raw[
        P256_PUBLIC_KEY_BYTES
    ],
    uint8_t psa[
        P256_PSA_PUBLIC_KEY_BYTES
    ]
) {
    psa[0] = 0x04u;

    memcpy(
        psa + 1u,
        raw,
        P256_PUBLIC_KEY_BYTES
    );
}


static psa_status_t aes_gcm_encrypt(
    const uint8_t *key,
    const uint8_t *nonce,
    const uint8_t *aad,
    size_t aad_len,
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len
) {
    if (key == NULL
            || nonce == NULL
            || (aad == NULL
                && aad_len != 0u)
            || (plain == NULL
                && plain_len != 0u)
            || out == NULL
            || out_len == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (out_capacity <
            plain_len
                + P256_TAG_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    psa_key_id_t key_id = 0;

    psa_status_t status =
        import_aes_key(
            key,
            PSA_KEY_USAGE_ENCRYPT,
            &key_id
        );

    if (status != PSA_SUCCESS) {
        return status;
    }

    status =
        psa_aead_encrypt(
            key_id,
            PSA_ALG_GCM,
            nonce,
            P256_NONCE_BYTES,
            aad,
            aad_len,
            plain,
            plain_len,
            out,
            out_capacity,
            out_len
        );

    destroy_key(
        &key_id
    );

    return status;
}


static psa_status_t aes_gcm_decrypt(
    const uint8_t *key,
    const uint8_t *nonce,
    const uint8_t *aad,
    size_t aad_len,
    const uint8_t *cipher,
    size_t cipher_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len
) {
    if (key == NULL
            || nonce == NULL
            || (aad == NULL
                && aad_len != 0u)
            || cipher == NULL
            || cipher_len <
                P256_TAG_BYTES
            || out == NULL
            || out_len == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (out_capacity <
            cipher_len
                - P256_TAG_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    psa_key_id_t key_id = 0;

    psa_status_t status =
        import_aes_key(
            key,
            PSA_KEY_USAGE_DECRYPT,
            &key_id
        );

    if (status != PSA_SUCCESS) {
        return status;
    }

    status =
        psa_aead_decrypt(
            key_id,
            PSA_ALG_GCM,
            nonce,
            P256_NONCE_BYTES,
            aad,
            aad_len,
            cipher,
            cipher_len,
            out,
            out_capacity,
            out_len
        );

    destroy_key(
        &key_id
    );

    return status;
}


static psa_status_t hash_signed_data(
    const uint8_t *data,
    size_t data_len,
    uint8_t hash[
        P256_SHA256_BYTES
    ]
) {
    if ((data == NULL
            && data_len != 0u)
            || hash == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_hash_operation_t operation =
        PSA_HASH_OPERATION_INIT;

    psa_status_t status =
        psa_hash_setup(
            &operation,
            PSA_ALG_SHA_256
        );

    if (status != PSA_SUCCESS) {
        return status;
    }

    status =
        psa_hash_update(
            &operation,
            SIGN_CONTEXT,
            sizeof(SIGN_CONTEXT) - 1u
        );

    if (status == PSA_SUCCESS
            && data_len != 0u) {
        status =
            psa_hash_update(
                &operation,
                data,
                data_len
            );
    }

    size_t hash_len = 0u;

    if (status == PSA_SUCCESS) {
        status =
            psa_hash_finish(
                &operation,
                hash,
                P256_SHA256_BYTES,
                &hash_len
            );
    } else {
        (void)psa_hash_abort(
            &operation
        );
    }

    if (status != PSA_SUCCESS) {
        return status;
    }

    return hash_len ==
            P256_SHA256_BYTES
        ? PSA_SUCCESS
        : PSA_ERROR_CORRUPTION_DETECTED;
}


static aether_status_t initialize(
    void *ctx
) {
    (void)ctx;

    return status_from_psa(
        psa_crypto_init()
    );
}


static aether_status_t random_symmetric_key(
    void *ctx,
    uint8_t *out_key,
    size_t key_len
) {
    (void)ctx;

    if (out_key == NULL
            || key_len !=
                P256_AES_KEY_BYTES) {
        return AETHER_ERR_ARGUMENT;
    }

    return status_from_psa(
        psa_generate_random(
            out_key,
            key_len
        )
    );
}


static aether_status_t verify_signature(
    void *ctx,
    const uint8_t *public_key,
    size_t public_key_len,
    const uint8_t *signature,
    size_t signature_len,
    const uint8_t *data,
    size_t data_len
) {
    (void)ctx;

    if (public_key == NULL
            || public_key_len !=
                P256_PUBLIC_KEY_BYTES
            || signature == NULL
            || signature_len !=
                P256_SIGNATURE_BYTES
            || (data == NULL
                && data_len != 0u)) {
        return AETHER_ERR_ARGUMENT;
    }

    uint8_t psa_public[
        P256_PSA_PUBLIC_KEY_BYTES
    ];

    raw_public_to_psa(
        public_key,
        psa_public
    );

    uint8_t hash[
        P256_SHA256_BYTES
    ];

    psa_status_t status =
        hash_signed_data(
            data,
            data_len,
            hash
        );

    if (status != PSA_SUCCESS) {
        return AETHER_ERR_CRYPTO;
    }

    psa_key_id_t key_id = 0;

    status =
        import_key(
            PSA_KEY_TYPE_ECC_PUBLIC_KEY(
                PSA_ECC_FAMILY_SECP_R1
            ),
            P256_PRIVATE_KEY_BYTES
                * 8u,
            PSA_KEY_USAGE_VERIFY_HASH,
            PSA_ALG_ECDSA(
                PSA_ALG_SHA_256
            ),
            psa_public,
            sizeof(psa_public),
            &key_id
        );

    if (status == PSA_SUCCESS) {
        status =
            psa_verify_hash(
                key_id,
                PSA_ALG_ECDSA(
                    PSA_ALG_SHA_256
                ),
                hash,
                sizeof(hash),
                signature,
                signature_len
            );
    }

    destroy_key(
        &key_id
    );

    memset(
        hash,
        0,
        sizeof(hash)
    );

    return status_from_psa(
        status
    );
}


static psa_status_t generate_ephemeral_key(
    psa_key_id_t *out_key
) {
    if (out_key == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_key_attributes_t attributes =
        PSA_KEY_ATTRIBUTES_INIT;

    psa_set_key_type(
        &attributes,
        PSA_KEY_TYPE_ECC_KEY_PAIR(
            PSA_ECC_FAMILY_SECP_R1
        )
    );

    psa_set_key_bits(
        &attributes,
        P256_PRIVATE_KEY_BYTES * 8u
    );

    psa_set_key_usage_flags(
        &attributes,
        PSA_KEY_USAGE_DERIVE
    );

    psa_set_key_algorithm(
        &attributes,
        PSA_ALG_ECDH
    );

    psa_status_t status =
        psa_generate_key(
            &attributes,
            out_key
        );

    psa_reset_key_attributes(
        &attributes
    );

    return status;
}


static aether_status_t asymmetric_encrypt(
    void *ctx,
    const uint8_t *public_key,
    size_t public_key_len,
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len
) {
    (void)ctx;

    if (public_key == NULL
            || public_key_len !=
                P256_PUBLIC_KEY_BYTES
            || (plain == NULL
                && plain_len != 0u)
            || out == NULL
            || out_len == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    const size_t prefix_len =
        P256_PUBLIC_KEY_BYTES
            + P256_NONCE_BYTES;

    if (out_capacity <
            prefix_len
                + plain_len
                + P256_TAG_BYTES) {
        return AETHER_ERR_OVERFLOW;
    }

    psa_key_id_t ephemeral_key = 0;

    psa_status_t status =
        generate_ephemeral_key(
            &ephemeral_key
        );

    if (status != PSA_SUCCESS) {
        return AETHER_ERR_CRYPTO;
    }

    uint8_t ephemeral_psa[
        P256_PSA_PUBLIC_KEY_BYTES
    ];

    size_t ephemeral_psa_len = 0u;

    status =
        psa_export_public_key(
            ephemeral_key,
            ephemeral_psa,
            sizeof(ephemeral_psa),
            &ephemeral_psa_len
        );

    if (status != PSA_SUCCESS
            || ephemeral_psa_len !=
                P256_PSA_PUBLIC_KEY_BYTES
            || ephemeral_psa[0]
                != 0x04u) {
        destroy_key(
            &ephemeral_key
        );

        return AETHER_ERR_CRYPTO;
    }

    memcpy(
        out,
        ephemeral_psa + 1u,
        P256_PUBLIC_KEY_BYTES
    );

    uint8_t recipient_psa[
        P256_PSA_PUBLIC_KEY_BYTES
    ];

    raw_public_to_psa(
        public_key,
        recipient_psa
    );

    uint8_t shared_secret[
        P256_SHARED_SECRET_BYTES
    ];

    size_t shared_secret_len = 0u;

    status =
        psa_raw_key_agreement(
            PSA_ALG_ECDH,
            ephemeral_key,
            recipient_psa,
            sizeof(recipient_psa),
            shared_secret,
            sizeof(shared_secret),
            &shared_secret_len
        );

    destroy_key(
        &ephemeral_key
    );

    if (status != PSA_SUCCESS
            || shared_secret_len !=
                P256_SHARED_SECRET_BYTES) {
        memset(
            shared_secret,
            0,
            sizeof(shared_secret)
        );

        return AETHER_ERR_CRYPTO;
    }

    uint8_t session_key[
        P256_AES_KEY_BYTES
    ];

    status =
        hkdf_sha256(
            shared_secret,
            sizeof(shared_secret),
            ASYMMETRIC_HKDF_SALT,
            sizeof(
                ASYMMETRIC_HKDF_SALT
            ) - 1u,
            ASYMMETRIC_HKDF_INFO,
            sizeof(
                ASYMMETRIC_HKDF_INFO
            ) - 1u,
            session_key,
            sizeof(session_key)
        );

    memset(
        shared_secret,
        0,
        sizeof(shared_secret)
    );

    if (status != PSA_SUCCESS) {
        memset(
            session_key,
            0,
            sizeof(session_key)
        );

        return AETHER_ERR_CRYPTO;
    }

    uint8_t *nonce =
        out + P256_PUBLIC_KEY_BYTES;

    status =
        psa_generate_random(
            nonce,
            P256_NONCE_BYTES
        );

    if (status != PSA_SUCCESS) {
        memset(
            session_key,
            0,
            sizeof(session_key)
        );

        return AETHER_ERR_CRYPTO;
    }

    size_t encrypted_len = 0u;

    status =
        aes_gcm_encrypt(
            session_key,
            nonce,
            out,
            P256_PUBLIC_KEY_BYTES,
            plain,
            plain_len,
            out + prefix_len,
            out_capacity - prefix_len,
            &encrypted_len
        );

    memset(
        session_key,
        0,
        sizeof(session_key)
    );

    if (status != PSA_SUCCESS) {
        return AETHER_ERR_CRYPTO;
    }

    *out_len =
        prefix_len
            + encrypted_len;

    return AETHER_OK;
}


static aether_status_t symmetric_encrypt(
    void *ctx,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len
) {
    (void)ctx;

    if (key == NULL
            || key_len !=
                P256_AES_KEY_BYTES
            || (plain == NULL
                && plain_len != 0u)
            || out == NULL
            || out_len == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (out_capacity <
            P256_NONCE_BYTES
                + plain_len
                + P256_TAG_BYTES) {
        return AETHER_ERR_OVERFLOW;
    }

    psa_status_t status =
        psa_generate_random(
            out,
            P256_NONCE_BYTES
        );

    if (status != PSA_SUCCESS) {
        return AETHER_ERR_CRYPTO;
    }

    size_t encrypted_len = 0u;

    status =
        aes_gcm_encrypt(
            key,
            out,
            NULL,
            0u,
            plain,
            plain_len,
            out + P256_NONCE_BYTES,
            out_capacity -
                P256_NONCE_BYTES,
            &encrypted_len
        );

    if (status != PSA_SUCCESS) {
        return AETHER_ERR_CRYPTO;
    }

    *out_len =
        P256_NONCE_BYTES
            + encrypted_len;

    return AETHER_OK;
}


static aether_status_t symmetric_decrypt(
    void *ctx,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *cipher,
    size_t cipher_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len
) {
    (void)ctx;

    if (key == NULL
            || key_len !=
                P256_AES_KEY_BYTES
            || cipher == NULL
            || out == NULL
            || out_len == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (cipher_len <
            P256_NONCE_BYTES
                + P256_TAG_BYTES) {
        return AETHER_ERR_CRYPTO;
    }

    const uint8_t *nonce =
        cipher;

    const uint8_t *encrypted =
        cipher + P256_NONCE_BYTES;

    const size_t encrypted_len =
        cipher_len
            - P256_NONCE_BYTES;

    if (out_capacity <
            encrypted_len
                - P256_TAG_BYTES) {
        return AETHER_ERR_OVERFLOW;
    }

    return status_from_psa(
        aes_gcm_decrypt(
            key,
            nonce,
            NULL,
            0u,
            encrypted,
            encrypted_len,
            out,
            out_capacity,
            out_len
        )
    );
}


static void put_u32_be(
    uint8_t out[4],
    uint32_t value
) {
    out[0] =
        (uint8_t)(value >> 24u);

    out[1] =
        (uint8_t)(value >> 16u);

    out[2] =
        (uint8_t)(value >> 8u);

    out[3] =
        (uint8_t)value;
}


static aether_status_t derive_server_keys(
    void *ctx,
    const uint8_t *master_key,
    size_t master_key_len,
    int32_t server_id,
    uint32_t key_number,
    uint8_t *client_to_server,
    uint8_t *server_to_client,
    size_t derived_key_len
) {
    (void)ctx;

    if (master_key == NULL
            || master_key_len !=
                P256_AES_KEY_BYTES
            || client_to_server == NULL
            || server_to_client == NULL
            || derived_key_len !=
                P256_AES_KEY_BYTES) {
        return AETHER_ERR_ARGUMENT;
    }

    uint8_t info[
        sizeof(SERVER_HKDF_INFO)
            - 1u
            + 8u
    ];

    const size_t prefix_len =
        sizeof(SERVER_HKDF_INFO)
            - 1u;

    memcpy(
        info,
        SERVER_HKDF_INFO,
        prefix_len
    );

    put_u32_be(
        info + prefix_len,
        (uint32_t)server_id
    );

    put_u32_be(
        info + prefix_len + 4u,
        key_number
    );

    uint8_t derived[
        P256_AES_KEY_BYTES * 2u
    ];

    psa_status_t status =
        hkdf_sha256(
            master_key,
            master_key_len,
            SERVER_HKDF_SALT,
            sizeof(
                SERVER_HKDF_SALT
            ) - 1u,
            info,
            sizeof(info),
            derived,
            sizeof(derived)
        );

    memset(
        info,
        0,
        sizeof(info)
    );

    if (status != PSA_SUCCESS) {
        memset(
            derived,
            0,
            sizeof(derived)
        );

        return AETHER_ERR_CRYPTO;
    }

    memcpy(
        client_to_server,
        derived,
        P256_AES_KEY_BYTES
    );

    memcpy(
        server_to_client,
        derived + P256_AES_KEY_BYTES,
        P256_AES_KEY_BYTES
    );

    memset(
        derived,
        0,
        sizeof(derived)
    );

    return AETHER_OK;
}


/*
 * Canonical common.adsl.yaml values:
 *
 * CryptoLib.P256_AES_GCM ordinal = 2
 * P256AesGcmSymmetric id         = 11
 * P256AesGcmCurvePublic id       = 13
 * SignP256AesGcm id              = 3
 */
const aether_crypto_vtable_t
    AETHER_ESP32_P256_AES_GCM_CRYPTO = {

    .profile = {
        .crypto_lib =
            P256_CRYPTO_LIB_ORDINAL,

        .symmetric_key_type =
            P256_SYMMETRIC_KEY_TYPE,

        .asymmetric_public_key_type =
            P256_ASYMMETRIC_PUBLIC_KEY_TYPE,

        .sign_type =
            P256_SIGN_TYPE,

        .symmetric_key_bytes =
            P256_AES_KEY_BYTES,

        .asymmetric_public_key_bytes =
            P256_PUBLIC_KEY_BYTES,

        .sign_public_key_bytes =
            P256_PUBLIC_KEY_BYTES,

        .signature_bytes =
            P256_SIGNATURE_BYTES
    },

    .initialize =
        initialize,

    .random_symmetric_key =
        random_symmetric_key,

    .verify_signature =
        verify_signature,

    .asymmetric_encrypt =
        asymmetric_encrypt,

    .symmetric_encrypt =
        symmetric_encrypt,

    .symmetric_decrypt =
        symmetric_decrypt,

    .derive_server_keys =
        derive_server_keys
};


const aether_crypto_vtable_t *
aether_esp32_p256_aes_gcm_crypto(
    void
) {
    return
        &AETHER_ESP32_P256_AES_GCM_CRYPTO;
}

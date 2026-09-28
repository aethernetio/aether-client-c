
#ifdef AETHER_P256_VECTOR_TEST

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../../platform/esp32/aether_crypto_p256_aes_gcm.c"


static int hex_value(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }

    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }

    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }

    return -1;
}


static int decode_hex(
    const char *hex,
    uint8_t *out,
    size_t out_len
) {
    if (hex == NULL || out == NULL) {
        return -1;
    }

    for (size_t i = 0u; i < out_len; ++i) {
        int high =
            hex_value(hex[i * 2u]);

        int low =
            hex_value(hex[i * 2u + 1u]);

        if (high < 0 || low < 0) {
            return -1;
        }

        out[i] =
            (uint8_t)(
                (high << 4) | low
            );
    }

    return hex[out_len * 2u] == '\0'
        ? 0
        : -1;
}


static int test_profile(void) {
    const aether_crypto_profile_t *profile =
        &AETHER_ESP32_P256_AES_GCM_CRYPTO.profile;

    if (profile->crypto_lib != 2u) {
        return 101;
    }

    if (profile->symmetric_key_type != 11u) {
        return 102;
    }

    if (profile->asymmetric_public_key_type != 13u) {
        return 103;
    }

    if (profile->sign_type != 3u) {
        return 104;
    }

    if (profile->symmetric_key_bytes != 32u
            || profile->asymmetric_public_key_bytes != 64u
            || profile->sign_public_key_bytes != 64u
            || profile->signature_bytes != 64u) {
        return 105;
    }

    return 0;
}


static int test_server_kdf_vector(void) {
    uint8_t master[32];
    uint8_t expected[64];
    uint8_t client_to_server[32];
    uint8_t server_to_client[32];
    uint8_t actual[64];

    if (decode_hex(
            "000102030405060708090A0B0C0D0E0F"
            "101112131415161718191A1B1C1D1E1F",
            master,
            sizeof(master)) != 0) {
        return 201;
    }

    if (decode_hex(
            "82CE351797368E89315A2DFC091E5D48"
            "075F46BBFEC93AD837C7C2D92B16E8A3"
            "D4FEAB9795EE8450D6F9CAE3D49ECEF2"
            "E8ACFE3E304767CAC2A56C2C0B523EF5",
            expected,
            sizeof(expected)) != 0) {
        return 202;
    }

    aether_status_t status =
        AETHER_ESP32_P256_AES_GCM_CRYPTO
            .derive_server_keys(
                NULL,
                master,
                sizeof(master),
                0x10203040,
                7u,
                client_to_server,
                server_to_client,
                32u
            );

    if (status != AETHER_OK) {
        return 203;
    }

    memcpy(
        actual,
        client_to_server,
        32u
    );

    memcpy(
        actual + 32u,
        server_to_client,
        32u
    );

    return memcmp(
        actual,
        expected,
        sizeof(expected)
    ) == 0
        ? 0
        : 204;
}


static int test_signature_vector(void) {
    uint8_t public_key[64];
    uint8_t signature[64];

    static const uint8_t message[] =
        "signed-server-key";

    if (decode_hex(
            "5ECBE4D1A6330A44C8F7EF951D4BF165"
            "E6C6B721EFADA985FB41661BC6E7FD6C"
            "8734640C4998FF7E374B06CE1A64A2EC"
            "D82AB036384FB83D9A79B127A27D5032",
            public_key,
            sizeof(public_key)) != 0) {
        return 301;
    }

    if (decode_hex(
            "DF3B58AC194A5776E7C30521CC08C8E9"
            "FF5724F8DB8260066DE5CEA6D45CE68E"
            "943329754C68160D5CB9733EC83DCF8B"
            "5F12C8F2204D9DC3AA3BB924EFC9C5DC",
            signature,
            sizeof(signature)) != 0) {
        return 302;
    }

    aether_status_t status =
        AETHER_ESP32_P256_AES_GCM_CRYPTO
            .verify_signature(
                NULL,
                public_key,
                sizeof(public_key),
                signature,
                sizeof(signature),
                message,
                sizeof(message) - 1u
            );

    if (status != AETHER_OK) {
        return 303;
    }

    signature[0] ^= 1u;

    status =
        AETHER_ESP32_P256_AES_GCM_CRYPTO
            .verify_signature(
                NULL,
                public_key,
                sizeof(public_key),
                signature,
                sizeof(signature),
                message,
                sizeof(message) - 1u
            );

    return status != AETHER_OK
        ? 0
        : 304;
}


static int test_symmetric_vector(void) {
    uint8_t key[32];
    uint8_t nonce[12];
    uint8_t expected[42];
    uint8_t actual[42];
    uint8_t decrypted[32];

    static const uint8_t plain[] =
        "aether-interop";

    if (decode_hex(
            "000102030405060708090A0B0C0D0E0F"
            "101112131415161718191A1B1C1D1E1F",
            key,
            sizeof(key)) != 0) {
        return 401;
    }

    if (decode_hex(
            "A0A1A2A3A4A5A6A7A8A9AAAB",
            nonce,
            sizeof(nonce)) != 0) {
        return 402;
    }

    if (decode_hex(
            "A0A1A2A3A4A5A6A7A8A9AAAB"
            "877D084520B92FD60C11E2A1680A"
            "3602A747C8D423757B3E6EC60A7862A4",
            expected,
            sizeof(expected)) != 0) {
        return 403;
    }

    memcpy(
        actual,
        nonce,
        sizeof(nonce)
    );

    size_t encrypted_len = 0u;

    psa_status_t status =
        aes_gcm_encrypt(
            key,
            nonce,
            NULL,
            0u,
            plain,
            sizeof(plain) - 1u,
            actual + sizeof(nonce),
            sizeof(actual) - sizeof(nonce),
            &encrypted_len
        );

    if (status != PSA_SUCCESS
            || encrypted_len !=
                sizeof(actual) - sizeof(nonce)) {
        return 404;
    }

    if (memcmp(
            actual,
            expected,
            sizeof(expected)) != 0) {
        return 405;
    }

    size_t plain_len = 0u;

    status =
        aes_gcm_decrypt(
            key,
            expected,
            NULL,
            0u,
            expected + sizeof(nonce),
            sizeof(expected) - sizeof(nonce),
            decrypted,
            sizeof(decrypted),
            &plain_len
        );

    if (status != PSA_SUCCESS) {
        return 406;
    }

    if (plain_len != sizeof(plain) - 1u) {
        return 407;
    }

    return memcmp(
        decrypted,
        plain,
        plain_len
    ) == 0
        ? 0
        : 408;
}


static int test_asymmetric_vector(void) {
    uint8_t recipient_public[64];
    uint8_t ephemeral_private[32];
    uint8_t ephemeral_public[64];
    uint8_t nonce[12];
    uint8_t expected[111];
    uint8_t actual[111];
    uint8_t recipient_psa[65];
    uint8_t shared[32];
    uint8_t session_key[32];

    static const uint8_t plain[] =
        "registration-vector";

    if (decode_hex(
            "6B17D1F2E12C4247F8BCE6E563A440F2"
            "77037D812DEB33A0F4A13945D898C296"
            "4FE342E2FE1A7F9B8EE7EB4A7C0F9E16"
            "2BCE33576B315ECECBB6406837BF51F5",
            recipient_public,
            sizeof(recipient_public)) != 0) {
        return 501;
    }

    if (decode_hex(
            "00000000000000000000000000000000"
            "00000000000000000000000000000002",
            ephemeral_private,
            sizeof(ephemeral_private)) != 0) {
        return 502;
    }

    if (decode_hex(
            "7CF27B188D034F7E8A52380304B51AC3"
            "C08969E277F21B35A60B48FC47669978"
            "07775510DB8ED040293D9AC69F7430DB"
            "BA7DADE63CE982299E04B79D227873D1",
            ephemeral_public,
            sizeof(ephemeral_public)) != 0) {
        return 503;
    }

    if (decode_hex(
            "303132333435363738393A3B",
            nonce,
            sizeof(nonce)) != 0) {
        return 504;
    }

    if (decode_hex(
            "7CF27B188D034F7E8A52380304B51AC3"
            "C08969E277F21B35A60B48FC47669978"
            "07775510DB8ED040293D9AC69F7430DB"
            "BA7DADE63CE982299E04B79D227873D1"
            "303132333435363738393A3B"
            "897A3CDCF12A9FC0C4A4AF43056AD684"
            "91A23F077634EFBA4FFFF633177603B81A85D0",
            expected,
            sizeof(expected)) != 0) {
        return 505;
    }

    psa_key_id_t ephemeral_key = 0;

    psa_status_t status =
        import_key(
            PSA_KEY_TYPE_ECC_KEY_PAIR(
                PSA_ECC_FAMILY_SECP_R1
            ),
            256u,
            PSA_KEY_USAGE_DERIVE,
            PSA_ALG_ECDH,
            ephemeral_private,
            sizeof(ephemeral_private),
            &ephemeral_key
        );

    if (status != PSA_SUCCESS) {
        return 506;
    }

    raw_public_to_psa(
        recipient_public,
        recipient_psa
    );

    size_t shared_len = 0u;

    status =
        psa_raw_key_agreement(
            PSA_ALG_ECDH,
            ephemeral_key,
            recipient_psa,
            sizeof(recipient_psa),
            shared,
            sizeof(shared),
            &shared_len
        );

    destroy_key(
        &ephemeral_key
    );

    if (status != PSA_SUCCESS
            || shared_len != sizeof(shared)) {
        return 507;
    }

    status =
        hkdf_sha256(
            shared,
            sizeof(shared),
            ASYMMETRIC_HKDF_SALT,
            sizeof(ASYMMETRIC_HKDF_SALT) - 1u,
            ASYMMETRIC_HKDF_INFO,
            sizeof(ASYMMETRIC_HKDF_INFO) - 1u,
            session_key,
            sizeof(session_key)
        );

    if (status != PSA_SUCCESS) {
        return 508;
    }

    memcpy(
        actual,
        ephemeral_public,
        sizeof(ephemeral_public)
    );

    memcpy(
        actual + sizeof(ephemeral_public),
        nonce,
        sizeof(nonce)
    );

    size_t encrypted_len = 0u;

    status =
        aes_gcm_encrypt(
            session_key,
            nonce,
            ephemeral_public,
            sizeof(ephemeral_public),
            plain,
            sizeof(plain) - 1u,
            actual
                + sizeof(ephemeral_public)
                + sizeof(nonce),
            sizeof(actual)
                - sizeof(ephemeral_public)
                - sizeof(nonce),
            &encrypted_len
        );

    memset(
        shared,
        0,
        sizeof(shared)
    );

    memset(
        session_key,
        0,
        sizeof(session_key)
    );

    if (status != PSA_SUCCESS) {
        return 509;
    }

    if (encrypted_len !=
            sizeof(actual)
                - sizeof(ephemeral_public)
                - sizeof(nonce)) {
        return 510;
    }

    return memcmp(
        actual,
        expected,
        sizeof(expected)
    ) == 0
        ? 0
        : 511;
}


int aether_p256_vector_test_run(void) {
    if (psa_crypto_init() != PSA_SUCCESS) {
        return 1;
    }

    int result =
        test_profile();

    if (result != 0) {
        return result;
    }

    result =
        test_server_kdf_vector();

    if (result != 0) {
        return result;
    }

    result =
        test_signature_vector();

    if (result != 0) {
        return result;
    }

    result =
        test_symmetric_vector();

    if (result != 0) {
        return result;
    }

    return
        test_asymmetric_vector();
}

#endif


#ifdef AETHER_SIZE_WITH_P256

#include <stddef.h>
#include <stdint.h>

#include "psa/crypto.h"

static void aether_psa_api_compile_probe(void) {
    psa_key_attributes_t aes_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t ecdh_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t sign_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t verify_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t hmac_attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_key_id_t aes_key = 0;
    psa_key_id_t ecdh_key = 0;
    psa_key_id_t sign_key = 0;
    psa_key_id_t verify_key = 0;
    psa_key_id_t hmac_key = 0;

    uint8_t random_data[32] = {0};
    uint8_t symmetric_key[32] = {0};
    uint8_t private_key[32] = {0};
    uint8_t public_key[65] = {0};
    uint8_t nonce[12] = {0};
    uint8_t plain[32] = {0};
    uint8_t cipher[64] = {0};
    uint8_t decrypted[32] = {0};
    uint8_t shared_secret[32] = {0};
    uint8_t exported_public[65] = {0};
    uint8_t signature[64] = {0};
    uint8_t mac[32] = {0};
    uint8_t hash[32] = {0};

    size_t cipher_len = 0;
    size_t decrypted_len = 0;
    size_t shared_secret_len = 0;
    size_t exported_public_len = 0;
    size_t signature_len = 0;
    size_t mac_len = 0;
    size_t hash_len = 0;

    /*
     * SEC1 uncompressed representation required by PSA for Weierstrass
     * public keys. Aether wire omits only this leading 0x04 byte.
     */
    public_key[0] = 0x04;

    (void)psa_crypto_init();
    (void)psa_generate_random(random_data, sizeof(random_data));

    /*
     * AES-256-GCM.
     */
    psa_set_key_type(&aes_attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&aes_attributes, 256);
    psa_set_key_usage_flags(
        &aes_attributes,
        PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&aes_attributes, PSA_ALG_GCM);

    (void)psa_import_key(
        &aes_attributes,
        symmetric_key,
        sizeof(symmetric_key),
        &aes_key);

    (void)psa_aead_encrypt(
        aes_key,
        PSA_ALG_GCM,
        nonce,
        sizeof(nonce),
        NULL,
        0,
        plain,
        sizeof(plain),
        cipher,
        sizeof(cipher),
        &cipher_len);

    (void)psa_aead_decrypt(
        aes_key,
        PSA_ALG_GCM,
        nonce,
        sizeof(nonce),
        NULL,
        0,
        cipher,
        cipher_len,
        decrypted,
        sizeof(decrypted),
        &decrypted_len);

    /*
     * P-256 ECDH. Private wire representation is a raw 32-byte scalar.
     */
    psa_set_key_type(
        &ecdh_attributes,
        PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&ecdh_attributes, 256);
    psa_set_key_usage_flags(
        &ecdh_attributes,
        PSA_KEY_USAGE_DERIVE | PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(
        &ecdh_attributes,
        PSA_ALG_ECDH);

    (void)psa_import_key(
        &ecdh_attributes,
        private_key,
        sizeof(private_key),
        &ecdh_key);

    (void)psa_export_public_key(
        ecdh_key,
        exported_public,
        sizeof(exported_public),
        &exported_public_len);

    (void)psa_raw_key_agreement(
        PSA_ALG_ECDH,
        ecdh_key,
        public_key,
        sizeof(public_key),
        shared_secret,
        sizeof(shared_secret),
        &shared_secret_len);

    /*
     * ECDSA P-256/SHA-256. PSA ECDSA signatures are fixed raw r||s,
     * exactly the 64-byte Aether wire representation.
     */
    psa_set_key_type(
        &sign_attributes,
        PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&sign_attributes, 256);
    psa_set_key_usage_flags(
        &sign_attributes,
        PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(
        &sign_attributes,
        PSA_ALG_ECDSA(PSA_ALG_SHA_256));

    (void)psa_import_key(
        &sign_attributes,
        private_key,
        sizeof(private_key),
        &sign_key);

    (void)psa_sign_message(
        sign_key,
        PSA_ALG_ECDSA(PSA_ALG_SHA_256),
        plain,
        sizeof(plain),
        signature,
        sizeof(signature),
        &signature_len);

    psa_set_key_type(
        &verify_attributes,
        PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&verify_attributes, 256);
    psa_set_key_usage_flags(
        &verify_attributes,
        PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(
        &verify_attributes,
        PSA_ALG_ECDSA(PSA_ALG_SHA_256));

    (void)psa_import_key(
        &verify_attributes,
        public_key,
        sizeof(public_key),
        &verify_key);

    (void)psa_verify_message(
        verify_key,
        PSA_ALG_ECDSA(PSA_ALG_SHA_256),
        plain,
        sizeof(plain),
        signature,
        signature_len);

    /*
     * SHA-256 and HMAC-SHA256 needed by HKDF-SHA256.
     */
    (void)psa_hash_compute(
        PSA_ALG_SHA_256,
        plain,
        sizeof(plain),
        hash,
        sizeof(hash),
        &hash_len);

    psa_set_key_type(
        &hmac_attributes,
        PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&hmac_attributes, 256);
    psa_set_key_usage_flags(
        &hmac_attributes,
        PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(
        &hmac_attributes,
        PSA_ALG_HMAC(PSA_ALG_SHA_256));

    (void)psa_import_key(
        &hmac_attributes,
        symmetric_key,
        sizeof(symmetric_key),
        &hmac_key);

    (void)psa_mac_compute(
        hmac_key,
        PSA_ALG_HMAC(PSA_ALG_SHA_256),
        plain,
        sizeof(plain),
        mac,
        sizeof(mac),
        &mac_len);

    (void)psa_destroy_key(hmac_key);
    (void)psa_destroy_key(verify_key);
    (void)psa_destroy_key(sign_key);
    (void)psa_destroy_key(ecdh_key);
    (void)psa_destroy_key(aes_key);

    psa_reset_key_attributes(&hmac_attributes);
    psa_reset_key_attributes(&verify_attributes);
    psa_reset_key_attributes(&sign_attributes);
    psa_reset_key_attributes(&ecdh_attributes);
    psa_reset_key_attributes(&aes_attributes);

    /*
     * Keep compile-time dataflow visible to the optimizer.
     */
    if (random_data[0] ==
            (uint8_t)(
                cipher_len
                + decrypted_len
                + shared_secret_len
                + exported_public_len
                + signature_len
                + mac_len
                + hash_len)) {
        random_data[0] ^= 1u;
    }
}

void aether_psa_api_probe_reference(void) {
    aether_psa_api_compile_probe();
}

#endif
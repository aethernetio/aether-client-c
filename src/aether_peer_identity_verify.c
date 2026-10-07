

#include "aether_peer_identity.h"

#include <stddef.h>

static const uint8_t *sign_bytes(
    const sign_ref_t *sign) {

    if (sign == NULL) {
        return NULL;
    }

    switch (sign->type_id) {
        case SIGN_TYPE_SIGN_HYDROGEN:
            return (const uint8_t *)sign->as.sign_hydrogen->data.data;

        case SIGN_TYPE_SIGN_P256AES_GCM:
            return (const uint8_t *)sign->as.sign_p256aes_gcm->data.data;

        case SIGN_TYPE_SIGN_AE_ED25519:
            return (const uint8_t *)sign->as.sign_ae_ed25519->data.data;

        default:
            return NULL;
    }
}

static const uint8_t *key_bytes(
    const key_ref_t *key) {

    if (key == NULL) {
        return NULL;
    }

    switch (key->type_id) {
        case KEY_TYPE_HYDROGEN_CURVE_PUBLIC:
            return (const uint8_t *)key->as.hydrogen_curve_public->data.data;

        case KEY_TYPE_SODIUM_CURVE_PUBLIC:
            return (const uint8_t *)key->as.sodium_curve_public->data.data;

        case KEY_TYPE_P256AES_GCM_CURVE_PUBLIC:
            return (const uint8_t *)key->as.p256aes_gcm_curve_public->data.data;

        default:
            return NULL;
    }
}

bool aether_peer_identity_verify_server_signature(
    const peer_identity_t *identity,
    const uint8_t *trusted_keys,
    size_t trusted_key_count,
    const aether_crypto_vtable_t *crypto,
    void *crypto_ctx) {

    if (identity == NULL ||
        trusted_keys == NULL ||
        crypto == NULL ||
        crypto->verify_signature == NULL) {
        return false;
    }

    const uint8_t *signature =
        sign_bytes(&identity->serverSigningKey.sign);

    const uint8_t *key =
        key_bytes(&identity->serverSigningKey.key);

    if (signature == NULL || key == NULL) {
        return false;
    }

    for (size_t i = 0u; i < trusted_key_count; ++i) {
        const uint8_t *root =
            trusted_keys +
            i * crypto->profile.sign_public_key_bytes;

        if (crypto->verify_signature(
                crypto_ctx,
                root,
                crypto->profile.sign_public_key_bytes,
                signature,
                crypto->profile.signature_bytes,
                key,
                crypto->profile.asymmetric_public_key_bytes) ==
            AETHER_OK) {
            return true;
        }
    }

    return false;
}
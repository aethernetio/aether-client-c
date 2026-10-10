
/*
 * Process-wide registry of cryptographic backends.
 *
 * See include/aether_crypto_registry.h for the contract. The registry
 * is a fixed-size array with no heap and no hidden state. Registration
 * is explicit at startup and idempotent.
 */
#include "aether_crypto_registry.h"



static aether_crypto_provider_t
    crypto_registry[AETHER_MAX_CRYPTO_PROVIDERS];

static size_t crypto_registry_count = 0u;


aether_status_t aether_crypto_register(
    const aether_crypto_vtable_t *vtable,
    void *ctx) {

    if (vtable == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    uint8_t crypto_lib = vtable->profile.crypto_lib;

    /*
     * Idempotent: re-registering the same crypto_lib is allowed and
     * replaces vtable/ctx in place. This makes startup tolerant of
     * duplicate registration without burdening the caller.
     */
    for (size_t i = 0u; i < crypto_registry_count; ++i) {
        if (crypto_registry[i].crypto_lib == crypto_lib) {
            crypto_registry[i].vtable = vtable;
            crypto_registry[i].ctx = ctx;
            return AETHER_OK;
        }
    }

    if (crypto_registry_count >= AETHER_MAX_CRYPTO_PROVIDERS) {
        return AETHER_ERR_OVERFLOW;
    }

    crypto_registry[crypto_registry_count].crypto_lib = crypto_lib;
    crypto_registry[crypto_registry_count].vtable = vtable;
    crypto_registry[crypto_registry_count].ctx = ctx;
    crypto_registry_count++;

    return AETHER_OK;
}


const aether_crypto_provider_t *aether_crypto_find(
    uint8_t crypto_lib) {

    for (size_t i = 0u; i < crypto_registry_count; ++i) {
        if (crypto_registry[i].crypto_lib == crypto_lib) {
            return &crypto_registry[i];
        }
    }

    return NULL;
}


const aether_crypto_provider_t *aether_crypto_find_by_wire_types(
    uint8_t asymmetric_public_key_type,
    uint8_t sign_type) {

    for (size_t i = 0u; i < crypto_registry_count; ++i) {

        const aether_crypto_profile_t *profile =
            &crypto_registry[i].vtable->profile;

        if (profile->asymmetric_public_key_type ==
                asymmetric_public_key_type &&
            profile->sign_type == sign_type) {

            return &crypto_registry[i];
        }
    }

    return NULL;
}




const aether_crypto_provider_t *
aether_crypto_find_by_asymmetric_public_key_type(
    uint8_t asymmetric_public_key_type) {

    for (size_t i = 0u; i < crypto_registry_count; ++i) {

        if (crypto_registry[i].vtable->profile.asymmetric_public_key_type ==
                asymmetric_public_key_type) {

            return &crypto_registry[i];
        }
    }

    return NULL;
}


void aether_crypto_registry_reset(void) {

    for (size_t i = 0u; i < crypto_registry_count; ++i) {
        crypto_registry[i].crypto_lib = 0u;
        crypto_registry[i].vtable = NULL;
        crypto_registry[i].ctx = NULL;
    }

    crypto_registry_count = 0u;
}

/*
 * Process-wide registry of cryptographic backends.
 *
 * The wire format carries a one-byte crypto_lib discriminator inside
 * every key and signed key. A client may need to handle several backends
 * simultaneously (for example during provider migration or when a peer
 * uses a different library), so the single aether_crypto_vtable_t in the
 * client config is not enough.
 *
 * This registry is the C analogue of the Java CryptoProviderFactory. It
 * is a fixed-size process-wide table. Registration happens explicitly at
 * startup; no constructors, no linker tricks, no hidden initialisation.
 * Idempotent: re-registering the same crypto_lib is a no-op.
 *
 * The vtable and its ctx must remain valid for the process lifetime.
 */
#ifndef AETHER_CRYPTO_REGISTRY_H
#define AETHER_CRYPTO_REGISTRY_H

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AETHER_MAX_CRYPTO_PROVIDERS
#define AETHER_MAX_CRYPTO_PROVIDERS 8u
#endif

typedef struct {
    uint8_t crypto_lib;
    const aether_crypto_vtable_t *vtable;
    void *ctx;
} aether_crypto_provider_t;

/*
 * Register a backend under vtable->profile.crypto_lib.
 *
 * Returns:
 *   AETHER_OK             on success (including re-registration of the
 *                         same crypto_lib);
 *   AETHER_ERR_ARGUMENT   if vtable is NULL;
 *   AETHER_ERR_OVERFLOW   if the registry is full.
 */
aether_status_t aether_crypto_register(
    const aether_crypto_vtable_t *vtable,
    void *ctx);

/*
 * Look up a provider by wire-level crypto_lib discriminator.
 * Returns NULL when no provider is registered for that id.
 */
const aether_crypto_provider_t *aether_crypto_find(
    uint8_t crypto_lib);

/*
 * Look up a provider by the pair of wire types that appear inside
 * aether_signed_key_t. This is the only way to decide which backend
 * must verify a SignedKey received from a peer, because the wire
 * carries key_type and sign_type but not crypto_lib itself.
 */
const aether_crypto_provider_t *aether_crypto_find_by_wire_types(
    uint8_t asymmetric_public_key_type,
    uint8_t sign_type);


/*
 * Look up a provider by the wire-level asymmetric public key type
 * alone. The wire carries only the ADSL type id (for example
 * HydrogenCurvePublic = 2, SodiumCurvePublic = 8,
 * P256AesGcmCurvePublic = 13) and those ids are unique across all
 * registered providers, so a single id is enough to pick the
 * backend that knows the correct key and signature sizes.
 */
const aether_crypto_provider_t *
aether_crypto_find_by_asymmetric_public_key_type(
    uint8_t asymmetric_public_key_type);


/*
 * Reset the registry. Intended for tests only. Production code
 * registers once at startup and keeps the registry untouched.
 */
void aether_crypto_registry_reset(void);

#ifdef __cplusplus
}
#endif

#endif
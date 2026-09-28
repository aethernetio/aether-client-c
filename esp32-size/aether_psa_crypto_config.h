


#ifndef AETHER_PSA_CRYPTO_CONFIG_H
#define AETHER_PSA_CRYPTO_CONFIG_H

/*
 * Start from the normal TF-PSA capability profile.
 */
#include "psa/crypto_config.h"

/*
 * Apply ESP-IDF's platform/user configuration here, before Aether's final
 * footprint reductions. build_info.h will include mbedtls/esp_config.h again
 * through TF_PSA_CRYPTO_USER_CONFIG_FILE, but esp_config.h is include-guarded,
 * so that later include is intentionally a no-op.
 */

#include "mbedtls/esp_config.h"




/*
 * Aether does not use RSA and this esp32-port profile disables Wi-Fi
 * Enterprise. Remove the complete RSA capability group only after ESP-IDF has
 * applied its compatibility Kconfig mappings.
 */
#undef PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY
#undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
#undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT
#undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT
#undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE

#undef PSA_WANT_ALG_RSA_PKCS1V15_CRYPT
#undef PSA_WANT_ALG_RSA_PKCS1V15_SIGN
#undef PSA_WANT_ALG_RSA_OAEP

#undef PSA_WANT_ALG_RSA_PSS

/*
 * This ESP32 footprint profile intentionally supports only the elliptic curve
 * required by Aether and the enabled Wi-Fi SAE/OWE path: secp256r1 / group 19.
 * DPP, NAN and Wi-Fi Enterprise are disabled, so the alternate ECC families,
 * finite-field DH groups and their larger hashes are unnecessary.
 *
 * Keep SHA-1/CCM/CMAC because the WPA stack still uses them. Keep SHA-256,
 * ECDH/ECDSA and SECP_R1_256 for both Aether and Wi-Fi.
 */
#undef PSA_WANT_ALG_SHA_384
#undef PSA_WANT_ALG_SHA_512

#undef PSA_WANT_ECC_BRAINPOOL_P_R1_256
#undef PSA_WANT_ECC_BRAINPOOL_P_R1_384
#undef PSA_WANT_ECC_BRAINPOOL_P_R1_512
#undef PSA_WANT_ECC_MONTGOMERY_255
#undef PSA_WANT_ECC_MONTGOMERY_448
#undef PSA_WANT_ECC_SECP_K1_256
#undef PSA_WANT_ECC_SECP_R1_384
#undef PSA_WANT_ECC_SECP_R1_521

#undef PSA_WANT_ALG_FFDH

#undef PSA_WANT_DH_RFC7919_2048
#undef PSA_WANT_DH_RFC7919_3072
#undef PSA_WANT_DH_RFC7919_4096
#undef PSA_WANT_DH_RFC7919_6144
#undef PSA_WANT_DH_RFC7919_8192

#undef PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE

#endif /* AETHER_PSA_CRYPTO_CONFIG_H */
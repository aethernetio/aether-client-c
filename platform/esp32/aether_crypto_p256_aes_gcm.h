
#ifndef AETHER_CRYPTO_P256_AES_GCM_H
#define AETHER_CRYPTO_P256_AES_GCM_H

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const aether_crypto_vtable_t
    AETHER_ESP32_P256_AES_GCM_CRYPTO;

const aether_crypto_vtable_t *
aether_esp32_p256_aes_gcm_crypto(void);

#ifdef __cplusplus
}
#endif

#endif

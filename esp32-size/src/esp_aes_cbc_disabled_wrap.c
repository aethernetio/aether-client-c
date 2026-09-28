
#if defined(AETHER_ESP32_DISABLE_CBC)

#include <stddef.h>

#include "aes/esp_aes.h"

int __wrap_esp_aes_crypt_cbc(
    esp_aes_context *ctx,
    int mode,
    size_t length,
    unsigned char iv[16],
    const unsigned char *input,
    unsigned char *output)
{
    (void) ctx;
    (void) mode;
    (void) length;
    (void) iv;
    (void) input;
    (void) output;

    return ERR_ESP_AES_INVALID_INPUT_LENGTH;
}

#endif
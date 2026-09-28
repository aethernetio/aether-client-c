
#ifndef AETHER_PLATFORM_ESP32_H
#define AETHER_PLATFORM_ESP32_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Platform-package trust hook.
 *
 * This is NOT application configuration and is not part of aether.h.
 * A production ESP32 package may override this weak symbol in its platform
 * integration without exposing trust plumbing to application code.
 */
const uint8_t *aether_esp32_trusted_sign_keys(
    size_t *count);

#ifdef __cplusplus
}
#endif

#endif

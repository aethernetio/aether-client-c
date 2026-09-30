
#ifndef AETHER_REG_ROOT_ENTER_H
#define AETHER_REG_ROOT_ENTER_H

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif

aether_status_t aether_generated_reg_root_build_enter(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t crypto_lib,
    const uint8_t *encrypted_stream,
    size_t encrypted_stream_length);

#ifdef __cplusplus
}
#endif

#endif

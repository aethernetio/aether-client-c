
#ifndef AETHER_REG_ROOT_KEY_H
#define AETHER_REG_ROOT_KEY_H

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif

aether_status_t aether_generated_reg_root_build_get_key(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint32_t request_id,
    uint8_t crypto_lib);

#ifdef __cplusplus
}
#endif

#endif

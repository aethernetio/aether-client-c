
#ifndef AETHER_REG_GLOBAL_H
#define AETHER_REG_GLOBAL_H

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif

aether_status_t aether_generated_reg_global_build(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *key,
    size_t key_length,
    uint32_t request_id);

#ifdef __cplusplus
}
#endif

#endif

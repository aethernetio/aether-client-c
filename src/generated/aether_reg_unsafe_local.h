
#ifndef AETHER_REG_UNSAFE_LOCAL_H
#define AETHER_REG_UNSAFE_LOCAL_H

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AETHER_REG_UNSAFE_FRAME_ENTER = 1,
    AETHER_REG_UNSAFE_FRAME_ENTER_GLOBAL = 2
} aether_reg_unsafe_frame_kind_t;


typedef struct {
    aether_reg_unsafe_frame_kind_t kind;
    const uint8_t *data;
    size_t length;
} aether_reg_unsafe_frame_t;


aether_status_t aether_generated_reg_unsafe_decode(
    aether_reg_unsafe_frame_t *frame,
    const uint8_t *data,
    size_t length,
    size_t *consumed);

#ifdef __cplusplus
}
#endif

#endif

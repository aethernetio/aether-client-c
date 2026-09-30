
#ifndef AETHER_REG_SAFE_H
#define AETHER_REG_SAFE_H

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif

aether_status_t aether_generated_reg_safe_build_pow(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *key,
    size_t key_length,
    uint32_t request_id,
    aether_uuid_t parent);

aether_status_t aether_generated_reg_safe_build_registration(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *return_key,
    size_t return_key_length,
    const uint8_t *salt,
    size_t salt_length,
    const uint8_t *suffix,
    size_t suffix_length,
    const int32_t *passwords,
    size_t password_count,
    aether_uuid_t parent,
    const uint8_t *global_cipher,
    size_t global_cipher_length);


aether_status_t aether_generated_reg_safe_build_registration_direct(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t symmetric_key_type,
    const uint8_t *return_key,
    size_t return_key_length,
    const uint8_t *master_key,
    size_t master_key_length,
    uint32_t request_id,
    const uint8_t *salt,
    size_t salt_length,
    const uint8_t *suffix,
    size_t suffix_length,
    const int32_t *passwords,
    size_t password_count,
    aether_uuid_t parent);


aether_status_t aether_generated_reg_safe_build_resolve(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint32_t request_id,
    const int16_t *server_ids,
    size_t server_count);

#ifdef __cplusplus
}
#endif

#endif
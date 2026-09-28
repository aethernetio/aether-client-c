
#ifndef AETHER_HYDROGEN_H
#define AETHER_HYDROGEN_H

#include <stddef.h>
#include <stdint.h>

#define AETHER_HYDROGEN_KEY_BYTES 32u
#define AETHER_HYDROGEN_SIGNATURE_BYTES 64u
#define AETHER_HYDROGEN_KX_PACKET_BYTES 48u
#define AETHER_HYDROGEN_SECRETBOX_HEADER_BYTES 36u
#define AETHER_HYDROGEN_MSG_ID_BYTES 8u

int aether_hydrogen_init(void);

void aether_hydrogen_random_symmetric_key(
    uint8_t out[AETHER_HYDROGEN_KEY_BYTES]);

int aether_hydrogen_symmetric_encrypt(
    const uint8_t key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len);

int aether_hydrogen_symmetric_decrypt(
    const uint8_t key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t *cipher,
    size_t cipher_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len);

int aether_hydrogen_asymmetric_encrypt(
    const uint8_t public_key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len);

int aether_hydrogen_asymmetric_decrypt(
    const uint8_t public_key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t private_key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t *cipher,
    size_t cipher_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len);

int aether_hydrogen_derive_server_keys(
    const uint8_t master_key[AETHER_HYDROGEN_KEY_BYTES],
    int32_t server_id,
    uint32_t key_number,
    uint8_t client_to_server[AETHER_HYDROGEN_KEY_BYTES],
    uint8_t server_to_client[AETHER_HYDROGEN_KEY_BYTES]);

int aether_hydrogen_verify_signature(
    const uint8_t public_key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t signature[AETHER_HYDROGEN_SIGNATURE_BYTES],
    const uint8_t *data,
    size_t data_len);

#endif

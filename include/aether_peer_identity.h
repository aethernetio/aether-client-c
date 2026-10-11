
/*
 * Peer identity helpers and pinned key registry.
 *
 * Storage is caller-owned. Keys are copied into registry entries.
 * No heap allocation.
 */
#ifndef AETHER_PEER_IDENTITY_H
#define AETHER_PEER_IDENTITY_H

#include "aether_client.h"
#include "aether_api.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AETHER_PEER_MAX_KEY_BYTES 64u

typedef struct {
    bool used;
    aether_uuid_t uid;
    uint8_t key_type;
    uint8_t key_bytes[AETHER_PEER_MAX_KEY_BYTES];
    size_t key_size;
} aether_peer_key_entry_t;

typedef struct {
    aether_peer_key_entry_t *entries;
    size_t capacity;
} aether_peer_key_registry_t;

void aether_peer_key_registry_init(
    aether_peer_key_registry_t *registry,
    aether_peer_key_entry_t *storage,
    size_t capacity);

aether_status_t aether_peer_key_registry_register(
    aether_peer_key_registry_t *registry,
    aether_uuid_t uid,
    const key_sign_public_ref_t *key);

bool aether_peer_key_registry_matches(
    const key_sign_public_ref_t *key,
    const aether_peer_key_entry_t *entry);

 
bool aether_peer_identity_verify_server_signature(
    const peer_identity_t *identity,
    const uint8_t *trusted_keys,
    size_t trusted_key_count,
    const aether_crypto_vtable_t *crypto,
    void *crypto_ctx);


#ifdef __cplusplus
}
#endif

#endif
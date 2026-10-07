
#include "aether_peer_identity.h"

#include <string.h>

static size_t copy_public_key_bytes(
    const key_sign_public_ref_t *key,
    uint8_t *out,
    size_t capacity) {

    if (key == NULL || out == NULL) {
        return 0u;
    }

    switch (key->type_id) {
        case KEY_SIGN_PUBLIC_TYPE_HYDROGEN_SIGN_PUBLIC:
            if (capacity < 32u) return 0u;
            memcpy(out,
                   key->as.hydrogen_sign_public->data.data,
                   32u);
            return 32u;

        case KEY_SIGN_PUBLIC_TYPE_SODIUM_SIGN_PUBLIC:
            if (capacity < 32u) return 0u;
            memcpy(out,
                   key->as.sodium_sign_public->data.data,
                   32u);
            return 32u;

        case KEY_SIGN_PUBLIC_TYPE_P256AES_GCM_SIGN_PUBLIC:
            if (capacity < 64u) return 0u;
            memcpy(out,
                   key->as.p256aes_gcm_sign_public->data.data,
                   64u);
            return 64u;

        default:
            return 0u;
    }
}

void aether_peer_key_registry_init(
    aether_peer_key_registry_t *registry,
    aether_peer_key_entry_t *storage,
    size_t capacity) {

    if (registry == NULL) {
        return;
    }

    registry->entries = storage;
    registry->capacity = capacity;

    if (storage != NULL) {
        memset(storage,
               0,
               sizeof(aether_peer_key_entry_t) * capacity);
    }
}

aether_status_t aether_peer_key_registry_register(
    aether_peer_key_registry_t *registry,
    aether_uuid_t uid,
    const key_sign_public_ref_t *key) {

    if (registry == NULL || registry->entries == NULL || key == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t key_size = 0u;
    uint8_t key_bytes[AETHER_PEER_MAX_KEY_BYTES];

    key_size = copy_public_key_bytes(
        key,
        key_bytes,
        sizeof(key_bytes));

    if (key_size == 0u) {
        return AETHER_ERR_UNSUPPORTED;
    }

    for (size_t i = 0u; i < registry->capacity; ++i) {
        if (registry->entries[i].used &&
            registry->entries[i].uid.msb == uid.msb &&
            registry->entries[i].uid.lsb == uid.lsb) {

            registry->entries[i].key_type = key->type_id;
            registry->entries[i].key_size = key_size;
            memcpy(registry->entries[i].key_bytes,
                   key_bytes,
                   key_size);
            return AETHER_OK;
        }
    }

    for (size_t i = 0u; i < registry->capacity; ++i) {
        if (!registry->entries[i].used) {
            registry->entries[i].used = true;
            registry->entries[i].uid = uid;
            registry->entries[i].key_type = key->type_id;
            registry->entries[i].key_size = key_size;
            memcpy(registry->entries[i].key_bytes,
                   key_bytes,
                   key_size);
            return AETHER_OK;
        }
    }

    return AETHER_ERR_OVERFLOW;
}

bool aether_peer_key_registry_matches(
    const key_sign_public_ref_t *key,
    const aether_peer_key_entry_t *entry) {

    if (key == NULL || entry == NULL || !entry->used) {
        return false;
    }

    if (key->type_id != entry->key_type) {
        return false;
    }

    uint8_t bytes[AETHER_PEER_MAX_KEY_BYTES];

    size_t size = copy_public_key_bytes(
        key,
        bytes,
        sizeof(bytes));

    return size == entry->key_size &&
           memcmp(bytes,
                  entry->key_bytes,
                  size) == 0;
}

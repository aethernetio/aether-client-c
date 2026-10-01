

/*
 * Supply UID + master key for a client that does not have credentials yet.
 *
 * This API is intentionally one-shot/idempotent:
 * - if credentials are absent, UID and master key are installed;
 * - if the client already has credentials, AETHER_OK is returned and the
 *   existing UID/master key are left unchanged.
 *
 * Routing recovery is mandatory core behavior and does not require a
 * caller-owned credentials component.
 */
#ifndef AETHER_CREDENTIALS_H
#define AETHER_CREDENTIALS_H

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif

aether_status_t aether_set_credentials(
    aether_client_t *client,
    aether_uuid_t uid,
    const uint8_t master_key[AETHER_KEY_BYTES]);

#ifdef __cplusplus
}
#endif

#endif

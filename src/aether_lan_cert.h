
/*
 * Internal certificate verification helpers.
 *
 * Exposed only for the certificate-chain regression test; not part of
 * the public API.
 */
#ifndef AETHER_LAN_CERT_H
#define AETHER_LAN_CERT_H

#include "aether.h"
#include "aether_lan.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Verify authority certificate against trusted roots. */
bool aether_lan_verify_authority_cert(
    aether_t *client,
    const uint8_t cert[AETHER_LAN_AUTHORITY_CERT_BYTES]);

/* Verify peer certificate against a trusted authority, checking UID. */
bool aether_lan_verify_peer_cert(
    aether_t *client,
    const uint8_t authority_cert[AETHER_LAN_AUTHORITY_CERT_BYTES],
    aether_uuid_t expected_uid,
    const uint8_t cert[AETHER_LAN_PEER_CERT_BYTES],
    uint8_t out_sign_pk[32]);

#ifdef __cplusplus
}
#endif

#endif

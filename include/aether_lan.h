
/*
 * LAN direct delivery: local-network discovery and direct message transport.
 *
 * Two independent concerns live behind this single caller-owned context:
 *
 *   - Discovery: periodic multicast beacons carry UID, certificate
 *     fingerprint and direct port. Discovery is unauthenticated: it can
 *     only move a peer from EMPTY to DISCOVERED.
 *
 *   - Direct transport: mutual authentication with Noise XX, identity
 *     certificates and an encrypted session. Only this path may move a
 *     peer to VERIFIED, and only VERIFIED peers are eligible for direct
 *     message delivery.
 *
 * The component is driven by aether_work() as an ordinary registered
 * component. It performs no blocking I/O. No heap; all storage is
 * caller-owned.
 *
 * The transport-independent parts (UID, peer identity certificate,
 * Noise XX, session keys, replay window) are designed so that future
 * LoRa / BLE / mesh components can reuse the same identity model with
 * different endpoints and framing.
 *
 * Wire and protocol details are documented in ai.txt (research v6).
 */
#ifndef AETHER_LAN_H
#define AETHER_LAN_H


#include "aether.h"
#include "aether_delivery.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Certificate wire sizes. Not transmitted separately from handshake
 * packets; provided for callers that want to size their own buffers.
 */
#define AETHER_LAN_PEER_CERT_BYTES      113u
#define AETHER_LAN_AUTHORITY_CERT_BYTES 134u

/*
 * Peer identity material. Caller-owned.
 */

typedef struct {
    uint8_t sign_sk[64];      /* hydro_sign secret key */
    uint8_t cert[AETHER_LAN_PEER_CERT_BYTES];
    uint8_t authority_cert[AETHER_LAN_AUTHORITY_CERT_BYTES];
    uint8_t kx_pk[32];        /* hydro_kx static public key  */
    uint8_t kx_sk[32];        /* hydro_kx static secret key  */
} aether_lan_identity_t;


typedef enum {
    AETHER_LAN_PEER_EMPTY = 0,
    AETHER_LAN_PEER_DISCOVERED,
    AETHER_LAN_PEER_VERIFYING,
    AETHER_LAN_PEER_VERIFIED
} aether_lan_peer_state_t;

typedef struct {
    aether_uuid_t uid;
    uint8_t  ipv4[4];
    uint16_t direct_port;
    aether_lan_peer_state_t state;
    uint32_t last_seen_ms;
    uint8_t  rx_key[32];
    uint8_t  tx_key[32];
    uint8_t  session_id[8];
    uint64_t tx_seq;
    uint64_t rx_highest;
    uint64_t rx_seen;
    uint32_t reserved;
} aether_lan_peer_t;


/*
 * Handshake substate and role. Only one handshake is in flight globally.
 */
typedef enum {
    AETHER_LAN_HS_IDLE = 0,
    AETHER_LAN_HS_WAIT_B,
    AETHER_LAN_HS_WAIT_C,
    AETHER_LAN_HS_WAIT_D
} aether_lan_hs_state_t;

typedef enum {
    AETHER_LAN_HS_ROLE_NONE = 0,
    AETHER_LAN_HS_ROLE_INITIATOR,
    AETHER_LAN_HS_ROLE_RESPONDER
} aether_lan_hs_role_t;

/*
 * Scratch storage big enough to hold one hydro_kx_state plus a little
 * padding. Size is asserted in aether_lan.c at compile time.
 */
#define AETHER_LAN_HS_SCRATCH_BYTES 400u


typedef struct {
    uint8_t  active;
    uint8_t  state;
    uint8_t  role;
    uint8_t  reserved;
    uint32_t started_ms;
    aether_uuid_t peer_uid;
    uint8_t  ipv4[4];
    uint16_t direct_port;
    uint16_t reserved2;
    uint8_t  scratch[AETHER_LAN_HS_SCRATCH_BYTES];
    uint8_t  p1[48];
    uint8_t  p2[96];
    uint8_t  p3[64];
    uint8_t  rx_key[32];
    uint8_t  tx_key[32];
    uint8_t  session_id[8];
    /* Verified peer identity for this handshake. */
    uint8_t  peer_sign_pk[32];
    uint8_t  peer_cert_hash[32];
    /* Own cert hash, derived once at start. */
    uint8_t  own_cert_hash[32];
    uint8_t  reserved3[24];
} aether_lan_handshake_t;


/*
 * Caller-owned LAN component. Public so it can be embedded or statically
 * allocated; fields are internal and must not be touched by callers.
 */
typedef struct aether_lan {
    aether_t *client;
    const aether_lan_identity_t *identity;

    aether_lan_peer_t *peers;
    size_t peer_count;

    uint8_t *rx_buffer;
    size_t rx_capacity;
    uint8_t *tx_buffer;
    size_t tx_capacity;

    int discovery_fd;
    int direct_fd;
    uint16_t direct_port;

    uint32_t next_beacon_ms;
    size_t rr_cursor;

    aether_lan_handshake_t handshake;
} aether_lan_t;



/*
 * Initialize LAN component.
 *
 * All storage is caller-owned:
 *   peers        -> peer_count entries
 *   rx_buffer    -> at least AETHER_LAN_RX_BYTES
 *   tx_buffer    -> at least AETHER_LAN_TX_BYTES
 *   identity     -> identity material, must outlive the context
 *
 * One multicast discovery socket and one unicast direct socket are
 * opened internally. No heap.
 */
#define AETHER_LAN_RX_BYTES 576u
#define AETHER_LAN_TX_BYTES 576u

aether_status_t aether_lan_init(
    aether_lan_t *lan,
    aether_t *client,
    const aether_lan_identity_t *identity,
    aether_lan_peer_t *peers,
    size_t peer_count,
    uint8_t *rx_buffer,
    size_t rx_capacity,
    uint8_t *tx_buffer,
    size_t tx_capacity);

aether_status_t aether_lan_work(
    aether_t *client,
    void *context);

typedef struct {
    aether_lan_t *lan;
} aether_lan_provider_context_t;

aether_delivery_attempt_t aether_lan_delivery_try(
    void *context,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length);

aether_delivery_provider_t aether_lan_delivery_provider(
    aether_lan_provider_context_t *provider_context,
    aether_lan_t *lan);

aether_lan_peer_state_t aether_lan_peer_state(
    const aether_lan_t *lan,
    aether_uuid_t uid);

void aether_lan_deinit(aether_lan_t *lan);







#ifdef __cplusplus
}
#endif

#endif
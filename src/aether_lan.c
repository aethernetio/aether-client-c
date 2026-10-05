
/*


 * LAN direct delivery component.
 * See include/aether_lan.h and ai.txt for the public contract.
 */

#include <stdio.h>


#include "aether_lan.h"
#include "aether_internal.h"

#include <string.h>

#include "aether_lan_codec.h"
#include "hydrogen.h"


#if defined(ESP_PLATFORM)
#  include <lwip/sockets.h>
#  include <lwip/inet.h>
#else
#  include <arpa/inet.h>
#  include <errno.h>
#  include <fcntl.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif



#define AETHER_LAN_MCAST_A    239u
#define AETHER_LAN_MCAST_B    255u
#define AETHER_LAN_MCAST_C    72u
#define AETHER_LAN_MCAST_D    65u
#define AETHER_LAN_MCAST_PORT 57321u

#define AETHER_LAN_BEACON_MS        5000u
#define AETHER_LAN_BEACON_JITTER_MS 1000u
#define AETHER_LAN_TTL_DISCOVERED_MS 45000u
#define AETHER_LAN_TTL_VERIFIED_MS  120000u
#define AETHER_LAN_HS_TIMEOUT_MS    5000u
#define AETHER_LAN_MAX_DGRAMS_TICK  8u



static bool uuid_eq(aether_uuid_t a, aether_uuid_t b) {
    return a.msb == b.msb && a.lsb == b.lsb;
}

static bool uuid_is_zero(aether_uuid_t u) {
    return u.msb == 0u && u.lsb == 0u;
}

static bool uuid_less(aether_uuid_t a, aether_uuid_t b) {
    if (a.msb != b.msb) return a.msb < b.msb;
    return a.lsb < b.lsb;
}


/* ---------- peer table ---------- */

static aether_lan_peer_t *lan_peer_find(
    aether_lan_t *lan, aether_uuid_t uid) {

    for (size_t i = 0; i < lan->peer_count; ++i) {
        if (uuid_eq(lan->peers[i].uid, uid) &&
            lan->peers[i].state != AETHER_LAN_PEER_EMPTY) {
            return &lan->peers[i];
        }
    }
    return NULL;
}

static aether_lan_peer_t *lan_peer_alloc(
    aether_lan_t *lan) {

    for (size_t i = 0; i < lan->peer_count; ++i) {
        if (lan->peers[i].state == AETHER_LAN_PEER_EMPTY) {
            return &lan->peers[i];
        }
    }
    return NULL;
}

static void lan_peer_clear(aether_lan_peer_t *p) {
    memset(p, 0, sizeof(*p));
}


static void lan_peer_expire(aether_lan_t *lan, uint32_t now_ms) {

    /* If the active handshake timed out, drop transient state. */
    if (lan->handshake.active &&
        (now_ms - lan->handshake.started_ms) > AETHER_LAN_HS_TIMEOUT_MS) {

        aether_lan_peer_t *hp = lan_peer_find(lan, lan->handshake.peer_uid);
        if (hp != NULL && hp->state == AETHER_LAN_PEER_VERIFYING) {
            /* Keep the discovery entry alive but drop the verification. */
            hp->state = AETHER_LAN_PEER_DISCOVERED;
        }
        memset(&lan->handshake, 0, sizeof(lan->handshake));
    }

    for (size_t i = 0; i < lan->peer_count; ++i) {
        aether_lan_peer_t *p = &lan->peers[i];

        if (p->state == AETHER_LAN_PEER_EMPTY) continue;

        uint32_t age = now_ms - p->last_seen_ms;

        if (p->state == AETHER_LAN_PEER_DISCOVERED &&
            age > AETHER_LAN_TTL_DISCOVERED_MS) {
            lan_peer_clear(p);
        } else if (p->state == AETHER_LAN_PEER_VERIFIED &&
                   age > AETHER_LAN_TTL_VERIFIED_MS) {
            lan_peer_clear(p);
        } else if (p->state == AETHER_LAN_PEER_VERIFYING &&
                   age > AETHER_LAN_TTL_DISCOVERED_MS) {
            /* Verification stalled and discovery is stale too. */
            lan_peer_clear(p);
        }
    }
}



/* ---------- socket helpers ---------- */

static int lan_set_nonblocking(int fd) {
#if defined(ESP_PLATFORM)
    int flags = 1;
    return lwip_ioctl(fd, FIONBIO, &flags);
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
}

static int lan_open_discovery_socket(void) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return -1;

    int one = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
#if !defined(ESP_PLATFORM)
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));
#endif

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(AETHER_LAN_MCAST_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }

    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr.s_addr =
        htonl((uint32_t)AETHER_LAN_MCAST_A << 24u |
              (uint32_t)AETHER_LAN_MCAST_B << 16u |
              (uint32_t)AETHER_LAN_MCAST_C << 8u |
              (uint32_t)AETHER_LAN_MCAST_D);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   &mreq, sizeof(mreq)) < 0) {
        close(fd);
        return -1;
    }

    uint8_t ttl = 1u;
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

    if (lan_set_nonblocking(fd) != 0) {
        close(fd);
        return -1;
    }

    return fd;
}

static int lan_open_direct_socket(uint16_t *out_port) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }

    struct sockaddr_in local;
    socklen_t llen = sizeof(local);
    if (getsockname(fd, (struct sockaddr *)&local, &llen) != 0) {
        close(fd);
        return -1;
    }

    if (lan_set_nonblocking(fd) != 0) {
        close(fd);
        return -1;
    }

    *out_port = ntohs(local.sin_port);
    return fd;
}

/* ---------- LAN helpers ---------- */


static uint64_t lan_now_ms(aether_t *client) {
    if (client->app.platform.now_ms == NULL) return 0u;
    return client->app.platform.now_ms(client->app.platform.ctx);
}




/*
 * Canonical certificate hash. Used both for the discovery beacon
 * fingerprint and for the handshake transcript hashes, so that all
 * peers compute the same value for the same certificate.
 */
static bool lan_hash32(const uint8_t *data, size_t len, uint8_t out[32]) {
    static const char ctx[8] = { 'A','E','T','H','F','P','1',0 };
    return hydro_hash_hash(out, 32u, data, len, ctx, NULL) == 0;
}


static void lan_build_mcast_addr(struct sockaddr_in *out) {
    memset(out, 0, sizeof(*out));
    out->sin_family = AF_INET;
    out->sin_port = htons(AETHER_LAN_MCAST_PORT);
    out->sin_addr.s_addr = htonl(
        ((uint32_t)AETHER_LAN_MCAST_A << 24u) |
        ((uint32_t)AETHER_LAN_MCAST_B << 16u) |
        ((uint32_t)AETHER_LAN_MCAST_C <<  8u) |
         (uint32_t)AETHER_LAN_MCAST_D);
}

static void lan_send_beacon(aether_lan_t *lan) {
    uint8_t buf[AETHER_LAN_BEACON_BYTES];
    uint8_t fp[8];

    /* Fingerprint of the peer certificate: first 8 bytes of a hash. */
    uint8_t digest[32];
    if (!lan_hash32(lan->identity->cert, AETHER_LAN_PEER_CERT_BYTES, digest)) {
        return;
    }
    memcpy(fp, digest, 8);

    aether_uuid_t me = aether_uid(lan->client);
    aether_lan_beacon_encode(buf, me, fp, lan->direct_port);

    struct sockaddr_in dst;
    lan_build_mcast_addr(&dst);
    (void)sendto(lan->discovery_fd, buf, sizeof(buf), 0,
                 (struct sockaddr *)&dst, sizeof(dst));
}

static void lan_handle_beacon(
    aether_lan_t *lan,
    const uint8_t *buf, size_t len,
    const uint8_t src_ipv4[4]) {

    aether_uuid_t uid;
    uint8_t cert_fp[8];
    uint16_t port;
    if (!aether_lan_beacon_decode(buf, len, &uid, cert_fp, &port)) return;

    aether_uuid_t me = aether_uid(lan->client);
    if (uuid_eq(uid, me) || uuid_is_zero(uid)) return;

    aether_lan_peer_t *p = lan_peer_find(lan, uid);
    if (p == NULL) {
        p = lan_peer_alloc(lan);
        if (p == NULL) return;
        memset(p, 0, sizeof(*p));
        p->uid = uid;
        p->state = AETHER_LAN_PEER_DISCOVERED;
    }


    /* Beacon NEVER overwrites a VERIFIED endpoint. */
    if (p->state != AETHER_LAN_PEER_VERIFIED &&
        p->state != AETHER_LAN_PEER_VERIFYING) {
        memcpy(p->ipv4, src_ipv4, 4);
        p->direct_port = port;
    }

    /*
     * For DISCOVERED/VERIFYING, a beacon refreshes discovery presence.
     * For VERIFIED, the authenticated TTL is refreshed ONLY by a
     * successful direct packet, so an unauthenticated beacon must not
     * extend the session lifetime.
     */
    if (p->state != AETHER_LAN_PEER_VERIFIED) {
        p->last_seen_ms = (uint32_t)lan_now_ms(lan->client);
    }

    if (p->state == AETHER_LAN_PEER_EMPTY) {
        p->state = AETHER_LAN_PEER_DISCOVERED;
    }
}

/* ---------- session/transcript ---------- */



static bool lan_kx_keypair_from_identity(
    const aether_lan_identity_t *id,
    hydro_kx_keypair *out) {

    /*
     * The identity holds an explicit static KX keypair; the seed form
     * of hydro_kx_keygen_deterministic would silently derive a DIFFERENT
     * keypair, so we must copy the caller-owned pair verbatim.
     */
    memcpy(out->pk, id->kx_pk, hydro_kx_PUBLICKEYBYTES);
    memcpy(out->sk, id->kx_sk, hydro_kx_SECRETKEYBYTES);
    return true;
}


static bool lan_cert_fingerprint(
    const uint8_t cert[AETHER_LAN_PEER_CERT_BYTES],
    uint8_t out32[32]) {

    static const char ctx[8] = { 'A','E','T','H','F','P','1',0 };
    return hydro_hash_hash(out32, 32u, cert, AETHER_LAN_PEER_CERT_BYTES,
                           ctx, NULL) == 0;
}

/*
 * Verify a peer identity certificate.
 *
 * Authority certificate layout (134 bytes):
 *   [0]      version
 *   [1]      purpose
 *   [2..5]   authority_id
 *   [6..69]  authority_p256_sign_pk (64)
 *   [70..133] root_signature (64)
 * Signed input: "AETHERAU" || cert[0..69]
 *
 * Peer identity certificate layout (113 bytes):
 *   [0]      version
 *   [1..16]  uid (16)
 *   [17..48] peer_identity_sign_pk (32)
 *   [49..112] authority_signature (64)
 * Signed input: "AETHERPE" || cert[0..48]
 */

static const uint8_t AETHER_LAN_AUTH_SIG_PREFIX[8] = {
    'A','E','T','H','E','R','I','A'
};

static const uint8_t AETHER_LAN_PEER_SIG_PREFIX[8] = {
    'A','E','T','H','E','R','P','E'
};




bool aether_lan_verify_authority_cert(
    aether_t *client,
    const uint8_t cert[AETHER_LAN_AUTHORITY_CERT_BYTES]) {

    /* Structural validation first. */
    if (cert[0] != 0x01u) return false;
    if (cert[1] != 0x01u) return false;  /* PEER_IDENTITY_ISSUER */

    if (client->app.platform.crypto == NULL ||
        client->app.platform.crypto->verify_signature == NULL) {
        return false;
    }

    const uint8_t *trusted = client->app.platform.trusted_sign_keys;
    size_t trusted_count = client->app.platform.trusted_sign_key_count;
    if (trusted == NULL || trusted_count == 0u) return false;

    /*
     * Signed bytes: "AETHERIA" || version || purpose || authority_id
     *             || authority_p256_sign_pk  (prefix + cert[0..69]).
     * Signature:   cert[70..133] (64 bytes, P256).
     * Trusted root keys: P256, 64 bytes each.
     */
    uint8_t signed_input[8u + 70u];
    memcpy(signed_input, AETHER_LAN_AUTH_SIG_PREFIX, 8u);
    memcpy(signed_input + 8u, cert, 70u);

    const uint8_t *sig = cert + 70u;

    for (size_t i = 0u; i < trusted_count; ++i) {
        const uint8_t *pk = trusted + i * 64u;
        if (client->app.platform.crypto->verify_signature(
                client->app.platform.crypto_ctx,
                pk, 64u,
                sig, 64u,
                signed_input, sizeof(signed_input)) == AETHER_OK) {
            return true;
        }
    }
    return false;
}

/*
 * Verify a peer identity certificate against a trusted authority
 * certificate. On success, writes the certified hydro_sign public key
 * to out_sign_pk[32].
 *
 * Peer cert layout (113 bytes):
 *   [0]       version=1
 *   [1..16]   uid (16 bytes, canonical BE msb||lsb)
 *   [17..48]  peer_identity_sign_pk (32 bytes, hydro_sign)
 *   [49..112] authority_signature   (64 bytes)
 *
 * Signed bytes: "AETHERPE" || cert[0..48] (prefix + 49 bytes).
 * Authority signature is P256 verified with the authority's P256 public
 * key from the authority cert (bytes 6..69).
 */
bool aether_lan_verify_peer_cert(
    aether_t *client,
    const uint8_t authority_cert[AETHER_LAN_AUTHORITY_CERT_BYTES],
    aether_uuid_t expected_uid,
    const uint8_t cert[AETHER_LAN_PEER_CERT_BYTES],
    uint8_t out_sign_pk[32]) {

    if (cert[0] != 0x01u) return false;

    /* UID inside cert must match the claimed/expected UID. */
    uint8_t expected_bytes[16];
    for (unsigned i = 0u; i < 8u; ++i) {
        expected_bytes[i] =
            (uint8_t)(expected_uid.msb >> (56u - 8u * i));
        expected_bytes[8u + i] =
            (uint8_t)(expected_uid.lsb >> (56u - 8u * i));
    }
    if (memcmp(expected_bytes, cert + 1, 16) != 0) return false;

    if (client->app.platform.crypto == NULL ||
        client->app.platform.crypto->verify_signature == NULL) {
        return false;
    }

    /* Authority P256 public key lives at bytes 6..69 of authority cert. */
    const uint8_t *authority_pk = authority_cert + 6u;

    uint8_t signed_input[8u + 49u];
    memcpy(signed_input, AETHER_LAN_PEER_SIG_PREFIX, 8u);
    memcpy(signed_input + 8u, cert, 49u);

    const uint8_t *sig = cert + 49u;

    if (client->app.platform.crypto->verify_signature(
            client->app.platform.crypto_ctx,
            authority_pk, 64u,
            sig, 64u,
            signed_input, sizeof(signed_input)) != AETHER_OK) {
        return false;
    }

    memcpy(out_sign_pk, cert + 17u, 32u);
    return true;
}


static void lan_transcript_hashes(
    const uint8_t cert_i[AETHER_LAN_PEER_CERT_BYTES],
    const uint8_t cert_r[AETHER_LAN_PEER_CERT_BYTES],
    uint8_t h_cert_i[32],
    uint8_t h_cert_r[32]) {

    (void)lan_cert_fingerprint(cert_i, h_cert_i);
    (void)lan_cert_fingerprint(cert_r, h_cert_r);
}

/* sig_b transcript:
 *   "AETHERPB" || version(1) || uid_i(16) || uid_r(16)
 *             || H(cert_i)(32) || H(cert_r)(32)
 *             || H(p1)(32) || H(p2)(32)
 */
static void lan_sig_b_input(
    uint8_t out[8u + 1u + 16u + 16u + 32u*4u],
    aether_uuid_t uid_i, aether_uuid_t uid_r,
    const uint8_t h_cert_i[32], const uint8_t h_cert_r[32],
    const uint8_t p1[48], const uint8_t p2[96]) {

    static const char prefix[8] = { 'A','E','T','H','E','R','P','B' };
    uint8_t h_p1[32]; uint8_t h_p2[32];
    static const char hctx[8] = { 'A','E','T','H','H','S','0','0' };
    (void)hydro_hash_hash(h_p1, 32u, p1, 48u, hctx, NULL);
    (void)hydro_hash_hash(h_p2, 32u, p2, 96u, hctx, NULL);

    size_t off = 0u;
    memcpy(out + off, prefix, 8u); off += 8u;
    out[off++] = 0x01u;
    for (unsigned i = 0; i < 8u; ++i)
        out[off++] = (uint8_t)(uid_i.msb >> (56u - 8u*i));
    for (unsigned i = 0; i < 8u; ++i)
        out[off++] = (uint8_t)(uid_i.lsb >> (56u - 8u*i));
    for (unsigned i = 0; i < 8u; ++i)
        out[off++] = (uint8_t)(uid_r.msb >> (56u - 8u*i));
    for (unsigned i = 0; i < 8u; ++i)
        out[off++] = (uint8_t)(uid_r.lsb >> (56u - 8u*i));
    memcpy(out + off, h_cert_i, 32u); off += 32u;
    memcpy(out + off, h_cert_r, 32u); off += 32u;
    memcpy(out + off, h_p1, 32u); off += 32u;
    memcpy(out + off, h_p2, 32u); off += 32u;
}

static void lan_sig_a_input(
    uint8_t out[8u + 1u + 16u + 16u + 32u*5u],
    aether_uuid_t uid_i, aether_uuid_t uid_r,
    const uint8_t h_cert_i[32], const uint8_t h_cert_r[32],
    const uint8_t p1[48], const uint8_t p2[96], const uint8_t p3[64]) {

    static const char prefix[8] = { 'A','E','T','H','E','R','P','A' };
    uint8_t h_p1[32]; uint8_t h_p2[32]; uint8_t h_p3[32];
    static const char hctx[8] = { 'A','E','T','H','H','S','0','0' };
    (void)hydro_hash_hash(h_p1, 32u, p1, 48u, hctx, NULL);
    (void)hydro_hash_hash(h_p2, 32u, p2, 96u, hctx, NULL);
    (void)hydro_hash_hash(h_p3, 32u, p3, 64u, hctx, NULL);

    size_t off = 0u;
    memcpy(out + off, prefix, 8u); off += 8u;
    out[off++] = 0x01u;
    for (unsigned i = 0; i < 8u; ++i)
        out[off++] = (uint8_t)(uid_i.msb >> (56u - 8u*i));
    for (unsigned i = 0; i < 8u; ++i)
        out[off++] = (uint8_t)(uid_i.lsb >> (56u - 8u*i));
    for (unsigned i = 0; i < 8u; ++i)
        out[off++] = (uint8_t)(uid_r.msb >> (56u - 8u*i));
    for (unsigned i = 0; i < 8u; ++i)
        out[off++] = (uint8_t)(uid_r.lsb >> (56u - 8u*i));
    memcpy(out + off, h_cert_i, 32u); off += 32u;
    memcpy(out + off, h_cert_r, 32u); off += 32u;
    memcpy(out + off, h_p1, 32u); off += 32u;
    memcpy(out + off, h_p2, 32u); off += 32u;
    memcpy(out + off, h_p3, 32u); off += 32u;
}

/* ---------- handshake: send helpers ---------- */

static bool lan_send_to_peer(
    aether_lan_t *lan,
    const uint8_t ipv4[4], uint16_t port,
    const uint8_t *buf, size_t len) {

    struct sockaddr_in dst;
    memset(&dst, 0, sizeof(dst));
    dst.sin_family = AF_INET;
    dst.sin_port = htons(port);
    memcpy(&dst.sin_addr, ipv4, 4);

    ssize_t n = sendto(lan->direct_fd, buf, len, 0,
                       (struct sockaddr *)&dst, sizeof(dst));
    return n == (ssize_t)len;
}



/*
 * Endpoint equality helper.
 */
static bool lan_ep_eq(const uint8_t a[4], uint16_t ap,
                       const uint8_t b[4], uint16_t bp) {
    return ap == bp && memcmp(a, b, 4) == 0;
}

/*
 * Store verified peer identity into handshake scratch: certified sign_pk
 * and cert hash. Used by responder during handle_a so that sig_a can be
 * verified later in handle_c.
 */
static void lan_hs_store_peer_identity(
    aether_lan_handshake_t *hs,
    const uint8_t sign_pk[32],
    const uint8_t cert[AETHER_LAN_PEER_CERT_BYTES]) {

    memcpy(hs->peer_sign_pk, sign_pk, 32);
    (void)lan_cert_fingerprint(cert, hs->peer_cert_hash);
}

/* ---------- handshake: A ---------- */

static void lan_hs_handle_a(
    aether_lan_t *lan,
    const uint8_t *buf, size_t len,
    const uint8_t src_ipv4[4], uint16_t src_port) {



    aether_uuid_t uid_i;
    uint8_t cert_i[AETHER_LAN_PEER_CERT_BYTES];
    uint8_t p1[hydro_kx_XX_PACKET1BYTES];



    if (!aether_lan_pkt_a_decode(buf, len, &uid_i, cert_i, p1)) return;

    aether_uuid_t me = aether_uid(lan->client);
    if (uuid_eq(uid_i, me) || uuid_is_zero(uid_i)) return;




    /* Verify peer certificate: root -> authority -> peer. */
    if (!aether_lan_verify_authority_cert(lan->client, lan->identity->authority_cert)) {
        return;
    }

    uint8_t peer_sign_pk[32];
    if (!aether_lan_verify_peer_cert(lan->client, lan->identity->authority_cert,
                              uid_i, cert_i, peer_sign_pk)) {
        return;
    }



    aether_lan_peer_t *existing = lan_peer_find(lan, uid_i);




    /*
     * Simultaneous handshake tie-break applies ONLY to A from the
     * peer we are already handshaking with. A from an unrelated peer
     * must not disturb the current attempt.
     */
    if (lan->handshake.active &&
        lan->handshake.role == AETHER_LAN_HS_ROLE_INITIATOR &&
        (lan->handshake.state == AETHER_LAN_HS_WAIT_B ||
         lan->handshake.state == AETHER_LAN_HS_WAIT_D)) {

        if (!uuid_eq(uid_i, lan->handshake.peer_uid)) {
            return;
        }
        if (uuid_less(me, uid_i)) {
            /* I am lower: stay initiator, ignore their A. */
            return;
        }
        /* I am higher: abandon my attempt, become responder. */
        lan->handshake.active = 0u;
        if (existing != NULL && existing->state == AETHER_LAN_PEER_VERIFYING) {
            existing->state = AETHER_LAN_PEER_DISCOVERED;
        }
    }


    if (lan->handshake.active) return;
    if (lan->identity == NULL) return;

    hydro_kx_state *st = (hydro_kx_state *)lan->handshake.scratch.bytes;
    memset(st, 0, sizeof(*st));

    hydro_kx_keypair my_kp;
    if (!lan_kx_keypair_from_identity(lan->identity, &my_kp)) return;

    uint8_t p2[hydro_kx_XX_PACKET2BYTES];
    if (hydro_kx_xx_2(st, p2, p1, NULL, &my_kp) != 0) return;

    uint8_t h_cert_i[32]; uint8_t h_cert_r[32];
    lan_transcript_hashes(cert_i, lan->identity->cert, h_cert_i, h_cert_r);

    uint8_t sig_input[8u + 1u + 16u + 16u + 32u*4u];
    lan_sig_b_input(sig_input, uid_i, me, h_cert_i, h_cert_r, p1, p2);

    uint8_t sig_b[hydro_sign_BYTES];
    static const char sign_ctx[8] = { 'A','E','T','H','S','I','G','N' };
    if (hydro_sign_create(sig_b, sig_input, sizeof(sig_input),
                          sign_ctx, lan->identity->sign_sk) != 0) {
        return;
    }

    uint8_t out[AETHER_LAN_PKT_B_BYTES];
    aether_lan_pkt_b_encode(out, me, lan->identity->cert, p2, sig_b);

    if (!lan_send_to_peer(lan, src_ipv4, src_port, out, sizeof(out))) {
        return;
    }


    /*
     * Peer entry update.
     *   NEW:        allocate, VERIFYING, candidate endpoint.
     *   DISCOVERED: promote, candidate endpoint.
     *   VERIFIED/VERIFYING: DO NOT touch endpoint, session, TTL or state.
     * Candidate endpoint for a VERIFIED peer lives in handshake scratch
     * and is installed atomically only when the fresh handshake succeeds.
     */
    aether_lan_peer_t *p = existing;
    if (p == NULL) {
        p = lan_peer_alloc(lan);
        if (p == NULL) return;
        memset(p, 0, sizeof(*p));
        p->uid = uid_i;
        memcpy(p->ipv4, src_ipv4, 4);
        p->direct_port = src_port;
        p->last_seen_ms = (uint32_t)lan_now_ms(lan->client);
        p->state = AETHER_LAN_PEER_VERIFYING;
    } else if (p->state == AETHER_LAN_PEER_DISCOVERED) {
        memcpy(p->ipv4, src_ipv4, 4);
        p->direct_port = src_port;
        p->last_seen_ms = (uint32_t)lan_now_ms(lan->client);
        p->state = AETHER_LAN_PEER_VERIFYING;
    }


    aether_lan_handshake_t *hs = &lan->handshake;
    hs->active = 1u;
    hs->state = AETHER_LAN_HS_WAIT_C;
    hs->role = AETHER_LAN_HS_ROLE_RESPONDER;
    hs->started_ms = (uint32_t)lan_now_ms(lan->client);
    hs->peer_uid = uid_i;
    memcpy(hs->ipv4, src_ipv4, 4);
    hs->direct_port = src_port;
    memcpy(hs->p1, p1, sizeof(p1));
    memcpy(hs->p2, p2, sizeof(p2));
    memcpy(hs->own_cert_hash, h_cert_r, 32);
    lan_hs_store_peer_identity(hs, peer_sign_pk, cert_i);
}

/* ---------- handshake: initiator start ---------- */

static bool lan_hs_start_initiator(aether_lan_t *lan, aether_lan_peer_t *p) {
    if (lan->handshake.active) return false;

    hydro_kx_state *st = (hydro_kx_state *)lan->handshake.scratch.bytes;
    memset(st, 0, sizeof(*st));

    uint8_t p1[hydro_kx_XX_PACKET1BYTES];
    if (hydro_kx_xx_1(st, p1, NULL) != 0) return false;

    uint8_t buf[AETHER_LAN_PKT_A_BYTES];
    aether_uuid_t me = aether_uid(lan->client);
    aether_lan_pkt_a_encode(buf, me, lan->identity->cert, p1);

    if (!lan_send_to_peer(lan, p->ipv4, p->direct_port, buf, sizeof(buf))) {
        return false;
    }

    aether_lan_handshake_t *hs = &lan->handshake;
    hs->active = 1u;
    hs->state = AETHER_LAN_HS_WAIT_B;
    hs->role = AETHER_LAN_HS_ROLE_INITIATOR;
    hs->started_ms = (uint32_t)lan_now_ms(lan->client);
    hs->peer_uid = p->uid;
    memcpy(hs->ipv4, p->ipv4, 4);
    hs->direct_port = p->direct_port;
    memcpy(hs->p1, p1, sizeof(p1));

    /* Cache own cert hash so B/C transcript is symmetric. */
    (void)lan_cert_fingerprint(lan->identity->cert, hs->own_cert_hash);

    if (p->state == AETHER_LAN_PEER_DISCOVERED) {
        p->state = AETHER_LAN_PEER_VERIFYING;
    }
    return true;
}

/* ---------- handshake: B ---------- */

static void lan_hs_handle_b(
    aether_lan_t *lan,
    const uint8_t *buf, size_t len,
    const uint8_t src_ipv4[4], uint16_t src_port) {

    if (!lan->handshake.active ||
        lan->handshake.role != AETHER_LAN_HS_ROLE_INITIATOR ||
        lan->handshake.state != AETHER_LAN_HS_WAIT_B) {
        return;
    }

    /* Endpoint must match the address we sent A to. */
    if (!lan_ep_eq(lan->handshake.ipv4, lan->handshake.direct_port,
                   src_ipv4, src_port)) {
        return;
    }

    aether_uuid_t uid_r;
    uint8_t cert_r[AETHER_LAN_PEER_CERT_BYTES];
    uint8_t p2[hydro_kx_XX_PACKET2BYTES];
    uint8_t sig_b[hydro_sign_BYTES];

    if (!aether_lan_pkt_b_decode(buf, len, &uid_r, cert_r, p2, sig_b)) return;
    if (!uuid_eq(uid_r, lan->handshake.peer_uid)) return;

    /* Verify peer certificate chain first. */
    if (!aether_lan_verify_authority_cert(lan->client, lan->identity->authority_cert)) {
        return;
    }
    uint8_t peer_sign_pk[32];
    if (!aether_lan_verify_peer_cert(lan->client, lan->identity->authority_cert,
                              uid_r, cert_r, peer_sign_pk)) {
        return;
    }

    /* Verify sig_b using certified peer sign pk. */
    uint8_t h_cert_i[32]; uint8_t h_cert_r[32];
    lan_transcript_hashes(lan->identity->cert, cert_r, h_cert_i, h_cert_r);

    aether_uuid_t me = aether_uid(lan->client);
    uint8_t sig_input[8u + 1u + 16u + 16u + 32u*4u];
    lan_sig_b_input(sig_input, me, uid_r, h_cert_i, h_cert_r,
                    lan->handshake.p1, p2);

    static const char sign_ctx[8] = { 'A','E','T','H','S','I','G','N' };
    if (hydro_sign_verify(sig_b, sig_input, sizeof(sig_input),
                          sign_ctx, peer_sign_pk) != 0) {
        return;
    }

    /* Complete initiator: xx_3 produces p3 and provisional keys. */
    hydro_kx_state *st = (hydro_kx_state *)lan->handshake.scratch.bytes;

    hydro_kx_keypair my_kp;
    if (!lan_kx_keypair_from_identity(lan->identity, &my_kp)) return;

    hydro_kx_session_keypair kp;
    uint8_t p3[hydro_kx_XX_PACKET3BYTES];
    uint8_t peer_static_pk[32];
    if (hydro_kx_xx_3(st, &kp, p3, peer_static_pk, p2, NULL, &my_kp) != 0) {
        return;
    }

    uint8_t sig_a_input[8u + 1u + 16u + 16u + 32u*5u];
    lan_sig_a_input(sig_a_input, me, uid_r, h_cert_i, h_cert_r,
                    lan->handshake.p1, p2, p3);

    uint8_t sig_a[hydro_sign_BYTES];
    if (hydro_sign_create(sig_a, sig_a_input, sizeof(sig_a_input),
                          sign_ctx, lan->identity->sign_sk) != 0) {
        return;
    }

    uint8_t out[AETHER_LAN_PKT_C_BYTES];
    aether_lan_pkt_c_encode(out, p3, sig_a);

    if (!lan_send_to_peer(lan, src_ipv4, src_port, out, sizeof(out))) {
        return;
    }

    uint8_t session_id[8];
    if (!aether_lan_session_id_compute(lan->handshake.p1, p2, p3, session_id)) {
        return;
    }

    /* Store provisional keys in handshake scratch; peer is NOT VERIFIED yet. */
    aether_lan_handshake_t *hs = &lan->handshake;
    memcpy(hs->p2, p2, sizeof(p2));
    memcpy(hs->p3, p3, sizeof(p3));
    memcpy(hs->rx_key, kp.rx, 32);
    memcpy(hs->tx_key, kp.tx, 32);
    memcpy(hs->session_id, session_id, 8);
    memcpy(hs->peer_sign_pk, peer_sign_pk, 32);
    memcpy(hs->peer_cert_hash, h_cert_r, 32);
    hs->state = AETHER_LAN_HS_WAIT_D;
}

/* ---------- handshake: C (responder side) ---------- */

static void lan_hs_handle_c(
    aether_lan_t *lan,
    const uint8_t *buf, size_t len,
    const uint8_t src_ipv4[4], uint16_t src_port) {

    if (!lan->handshake.active ||
        lan->handshake.role != AETHER_LAN_HS_ROLE_RESPONDER ||
        lan->handshake.state != AETHER_LAN_HS_WAIT_C) {
        return;
    }

    /* Continuation must come from the endpoint we sent B to. */
    if (!lan_ep_eq(lan->handshake.ipv4, lan->handshake.direct_port,
                   src_ipv4, src_port)) {
        return;
    }

    uint8_t p3[hydro_kx_XX_PACKET3BYTES];
    uint8_t sig_a[hydro_sign_BYTES];
    if (!aether_lan_pkt_c_decode(buf, len, p3, sig_a)) return;

    aether_uuid_t me = aether_uid(lan->client);
    aether_uuid_t uid_i = lan->handshake.peer_uid;

    /* Verify sig_a against certified peer sign pk over full transcript. */
    uint8_t sig_a_input[8u + 1u + 16u + 16u + 32u*5u];
    lan_sig_a_input(sig_a_input, uid_i, me,
                    lan->handshake.peer_cert_hash,
                    lan->handshake.own_cert_hash,
                    lan->handshake.p1, lan->handshake.p2, p3);

    static const char sign_ctx[8] = { 'A','E','T','H','S','I','G','N' };
    if (hydro_sign_verify(sig_a, sig_a_input, sizeof(sig_a_input),
                          sign_ctx, lan->handshake.peer_sign_pk) != 0) {
        return;
    }

    hydro_kx_state *st = (hydro_kx_state *)lan->handshake.scratch.bytes;

    hydro_kx_session_keypair kp;
    uint8_t peer_static_pk[32];
    if (hydro_kx_xx_4(st, &kp, peer_static_pk, p3, NULL) != 0) {
        return;
    }

    uint8_t session_id[8];
    if (!aether_lan_session_id_compute(lan->handshake.p1,
                                       lan->handshake.p2, p3, session_id)) {
        return;
    }

    /* Send Packet D: B->A seq = 1, empty plaintext secretbox. */
    uint8_t sbox[hydro_secretbox_HEADERBYTES];
    if (hydro_secretbox_encrypt(sbox, NULL, 0u, 1u,
                                "AETHERlb", kp.tx) != 0) {
        return;
    }

    uint8_t out[AETHER_LAN_PKT_D_BYTES];
    aether_lan_pkt_d_encode(out, session_id, 1u, sbox);

    if (!lan_send_to_peer(lan, src_ipv4, src_port, out, sizeof(out))) {
        return;
    }

    /* Now atomically install session into peer table. */
    aether_lan_peer_t *p = lan_peer_find(lan, uid_i);
    if (p == NULL) {
        p = lan_peer_alloc(lan);
        if (p == NULL) return;
        memset(p, 0, sizeof(*p));
        p->uid = uid_i;
    }

    memcpy(p->ipv4, src_ipv4, 4);
    p->direct_port = src_port;
    memcpy(p->rx_key, kp.rx, 32);
    memcpy(p->tx_key, kp.tx, 32);
    memcpy(p->session_id, session_id, 8);
    p->tx_seq = 2u;      /* first B->A data = seq 2 (D used seq 1) */
    p->rx_highest = 0u;
    p->rx_seen = 0u;
    p->state = AETHER_LAN_PEER_VERIFIED;
    p->last_seen_ms = (uint32_t)lan_now_ms(lan->client);

    lan->handshake.active = 0u;
}

/* ---------- handshake: D (initiator side) ---------- */

static void lan_hs_handle_d(
    aether_lan_t *lan,
    const uint8_t *buf, size_t len,
    const uint8_t src_ipv4[4], uint16_t src_port) {

    if (!lan->handshake.active ||
        lan->handshake.role != AETHER_LAN_HS_ROLE_INITIATOR ||
        lan->handshake.state != AETHER_LAN_HS_WAIT_D) {
        return;
    }

    if (!lan_ep_eq(lan->handshake.ipv4, lan->handshake.direct_port,
                   src_ipv4, src_port)) {
        return;
    }

    uint8_t session_id[8];
    uint64_t sequence = 0u;
    uint8_t sbox[hydro_secretbox_HEADERBYTES];
    if (!aether_lan_pkt_d_decode(buf, len, session_id, &sequence, sbox)) return;

    if (memcmp(session_id, lan->handshake.session_id, 8) != 0) return;


    /* Verify and decrypt empty plaintext. */
    if (sequence != 1u) return;
    uint8_t plain_out[1];
    size_t plain_len = 0u;
    if (hydro_secretbox_decrypt(plain_out, sbox, sizeof(sbox), sequence,
                                "AETHERlb", lan->handshake.rx_key) != 0) {
        return;
    }
    (void)plain_len;


    /* Atomically install provisional session now that D authenticated. */
    aether_lan_peer_t *p = lan_peer_find(lan, lan->handshake.peer_uid);
    if (p == NULL) {
        p = lan_peer_alloc(lan);
        if (p == NULL) return;
        memset(p, 0, sizeof(*p));
        p->uid = lan->handshake.peer_uid;
    }

    memcpy(p->ipv4, lan->handshake.ipv4, 4);
    p->direct_port = lan->handshake.direct_port;
    memcpy(p->rx_key, lan->handshake.rx_key, 32);
    memcpy(p->tx_key, lan->handshake.tx_key, 32);
    memcpy(p->session_id, lan->handshake.session_id, 8);
    p->tx_seq = 1u;      /* first A->B data = seq 1 */
    p->rx_highest = 0u;
    p->rx_seen = 0u;

    /* Replay commit for D seq 1 only after successful authentication. */
    aether_lan_replay_commit(&p->rx_highest, &p->rx_seen, 1u);

    p->state = AETHER_LAN_PEER_VERIFIED;
    p->last_seen_ms = (uint32_t)lan_now_ms(lan->client);

    lan->handshake.active = 0u;
}


/*
 * Receive and process one datagram from the direct socket.
 * src_ipv4 / src_port identify the source endpoint.
 */
static void lan_process_direct(
    aether_lan_t *lan,
    const uint8_t *buf, size_t len,
    const uint8_t src_ipv4[4], uint16_t src_port) {

    if (len < 6u) return;



    /* Handshake packets share the "AELH" prefix. */
    if (buf[0] == 'A' && buf[1] == 'E' && buf[2] == 'L' && buf[3] == 'H') {
        switch (buf[5]) {
        case AETHER_LAN_TYPE_HS_A:
            lan_hs_handle_a(lan, buf, len, src_ipv4, src_port);
            return;
        case AETHER_LAN_TYPE_HS_B:
            lan_hs_handle_b(lan, buf, len, src_ipv4, src_port);
            return;
        case AETHER_LAN_TYPE_HS_C:
            lan_hs_handle_c(lan, buf, len, src_ipv4, src_port);
            return;
        case AETHER_LAN_TYPE_HS_D:
            lan_hs_handle_d(lan, buf, len, src_ipv4, src_port);
            return;
        default:
            return;
        }
    }

    if (buf[0] == 'A' && buf[1] == 'E' && buf[2] == 'L' && buf[3] == 'D') {
        uint8_t session_id[8];
        uint64_t seq = 0u;
        const uint8_t *sbox = NULL;
        size_t sbox_len = 0u;

        if (!aether_lan_direct_decode(buf, len, session_id, &seq,
                                      &sbox, &sbox_len)) {
            return;
        }
        if (sbox_len < hydro_secretbox_HEADERBYTES) return;

        /* Find a VERIFIED peer whose session_id and endpoint match. */
        for (size_t i = 0u; i < lan->peer_count; ++i) {
            aether_lan_peer_t *p = &lan->peers[i];
            if (p->state != AETHER_LAN_PEER_VERIFIED) continue;
            if (memcmp(p->session_id, session_id, 8) != 0) continue;
            if (!lan_ep_eq(p->ipv4, p->direct_port, src_ipv4, src_port)) continue;

            if (!aether_lan_replay_precheck(p->rx_highest, p->rx_seen, seq)) {
                return;
            }

            size_t plain_len = sbox_len - hydro_secretbox_HEADERBYTES;
            if (plain_len > lan->rx_capacity) return;


            /* Use tx_buffer as plaintext scratch: TX side is idle during RX
             * drain (single-threaded scheduler); using rx_buffer would
             * overlap ciphertext and plaintext. */
            if (hydro_secretbox_decrypt(lan->tx_buffer, sbox, sbox_len, seq,
                                        "AETHERld", p->rx_key) != 0) {
                return;
            }

            aether_lan_replay_commit(&p->rx_highest, &p->rx_seen, seq);
            p->last_seen_ms = (uint32_t)lan_now_ms(lan->client);

            (void)aether_dispatch_message_internal(
                lan->client, p->uid, lan->tx_buffer, plain_len);

            return;
        }
    }
}

/* ---------- tick: RX drain (bounded) ---------- */

static void lan_drain_discovery(aether_lan_t *lan) {
    for (unsigned i = 0u; i < AETHER_LAN_MAX_DGRAMS_TICK; ++i) {
        struct sockaddr_in src;
        socklen_t slen = sizeof(src);
        ssize_t n = recvfrom(lan->discovery_fd, lan->rx_buffer,
                             lan->rx_capacity, 0,
                             (struct sockaddr *)&src, &slen);
        if (n <= 0) break;

        uint8_t ipv4[4];
        memcpy(ipv4, &src.sin_addr, 4);
        lan_handle_beacon(lan, lan->rx_buffer, (size_t)n, ipv4);
    }
}

static void lan_drain_direct(aether_lan_t *lan) {
    for (unsigned i = 0u; i < AETHER_LAN_MAX_DGRAMS_TICK; ++i) {
        struct sockaddr_in src;
        socklen_t slen = sizeof(src);
        ssize_t n = recvfrom(lan->direct_fd, lan->rx_buffer,
                             lan->rx_capacity, 0,
                             (struct sockaddr *)&src, &slen);
        if (n <= 0) break;

        uint8_t ipv4[4];
        memcpy(ipv4, &src.sin_addr, 4);
        uint16_t port = ntohs(src.sin_port);

        lan_process_direct(lan, lan->rx_buffer, (size_t)n, ipv4, port);
    }
}

/* ---------- fairness: pick next DISCOVERED peer ---------- */

static aether_lan_peer_t *lan_pick_discovered(aether_lan_t *lan) {
    if (lan->peer_count == 0u) return NULL;

    for (size_t k = 0u; k < lan->peer_count; ++k) {
        size_t idx = (lan->rr_cursor + k) % lan->peer_count;
        aether_lan_peer_t *p = &lan->peers[idx];
        if (p->state == AETHER_LAN_PEER_DISCOVERED && !uuid_is_zero(p->uid)) {
            lan->rr_cursor = (idx + 1u) % lan->peer_count;
            return p;
        }
    }
    return NULL;
}

/* ---------- public API ---------- */

_Static_assert(sizeof(hydro_kx_state) <= AETHER_LAN_HS_SCRATCH_BYTES,
               "handshake scratch too small for hydro_kx_state");

aether_status_t aether_lan_init(
    aether_lan_t *lan,
    aether_t *client,
    const aether_lan_identity_t *identity,
    aether_lan_peer_t *peers,
    size_t peer_count,
    uint8_t *rx_buffer,
    size_t rx_capacity,
    uint8_t *tx_buffer,
    size_t tx_capacity) {

    if (lan == NULL || client == NULL || identity == NULL ||
        peers == NULL || peer_count == 0u ||
        rx_buffer == NULL || rx_capacity < AETHER_LAN_RX_BYTES ||
        tx_buffer == NULL || tx_capacity < AETHER_LAN_TX_BYTES) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(lan, 0, sizeof(*lan));
    memset(peers, 0, peer_count * sizeof(*peers));

    lan->client = client;
    lan->identity = identity;
    lan->peers = peers;
    lan->peer_count = peer_count;
    lan->rx_buffer = rx_buffer;
    lan->rx_capacity = rx_capacity;
    lan->tx_buffer = tx_buffer;
    lan->tx_capacity = tx_capacity;
    lan->discovery_fd = -1;
    lan->direct_fd = -1;

    /*
     * Validate own identity: own certificate authority chain must verify
     * and cert UID must match client UID. Fail init if inconsistent.
     */
    aether_uuid_t me = aether_uid(client);
    if (uuid_is_zero(me)) {
        return AETHER_ERR_STATE;
    }

    if (!aether_lan_verify_authority_cert(client, identity->authority_cert)) {
        return AETHER_ERR_CRYPTO;
    }

    uint8_t own_sign_pk[32];
    if (!aether_lan_verify_peer_cert(client, identity->authority_cert,
                              me, identity->cert, own_sign_pk)) {
        return AETHER_ERR_CRYPTO;
    }

    /* kx keypair material is caller-supplied; nothing to derive. */

    int dfd = lan_open_discovery_socket();
    if (dfd < 0) {
        return AETHER_ERR_TRANSPORT;
    }

    uint16_t port = 0u;
    int cfd = lan_open_direct_socket(&port);
    if (cfd < 0) {
        close(dfd);
        return AETHER_ERR_TRANSPORT;
    }

    lan->discovery_fd = dfd;
    lan->direct_fd = cfd;
    lan->direct_port = port;
    lan->next_beacon_ms = 0u;
    lan->rr_cursor = 0u;

    return AETHER_OK;
}

void aether_lan_deinit(aether_lan_t *lan) {
    if (lan == NULL) return;
    if (lan->discovery_fd >= 0) {
        close(lan->discovery_fd);
        lan->discovery_fd = -1;
    }
    if (lan->direct_fd >= 0) {
        close(lan->direct_fd);
        lan->direct_fd = -1;
    }
    memset(&lan->handshake, 0, sizeof(lan->handshake));
}

aether_status_t aether_lan_work(
    aether_t *client,
    void *context) {

    if (client == NULL) return AETHER_ERR_ARGUMENT;
    aether_lan_t *lan = context;
    if (lan == NULL || lan->client != client) return AETHER_ERR_ARGUMENT;
    if (lan->discovery_fd < 0 || lan->direct_fd < 0) return AETHER_OK;

    uint32_t now = (uint32_t)lan_now_ms(client);

    lan_peer_expire(lan, now);

    /* Periodic beacon. */
    if (lan->next_beacon_ms == 0u || now >= lan->next_beacon_ms) {
        lan_send_beacon(lan);
        /* Simple fixed-period for v1 (jitter is future). */
        lan->next_beacon_ms = now + AETHER_LAN_BEACON_MS;
    }

    /* Drain RX bounded. */
    lan_drain_discovery(lan);
    lan_drain_direct(lan);

    /* Start a handshake if idle and a DISCOVERED peer is waiting. */
    if (!lan->handshake.active) {
        aether_lan_peer_t *p = lan_pick_discovered(lan);
        if (p != NULL) {
            (void)lan_hs_start_initiator(lan, p);
        }
    }






    return AETHER_OK;

}

aether_lan_peer_state_t aether_lan_peer_state(
    const aether_lan_t *lan,
    aether_uuid_t uid) {

    if (lan == NULL) return AETHER_LAN_PEER_EMPTY;
    for (size_t i = 0u; i < lan->peer_count; ++i) {
        if (uuid_eq(lan->peers[i].uid, uid) &&
            lan->peers[i].state != AETHER_LAN_PEER_EMPTY) {
            return lan->peers[i].state;
        }
    }
    return AETHER_LAN_PEER_EMPTY;
}
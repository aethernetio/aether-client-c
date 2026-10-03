
/*
 * LAN wire codec implementation. Pure functions only.
 */
#include "aether_lan_codec.h"

#include "hydrogen.h"

#include <string.h>

static void put_u16be(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v >> 8u);
    p[1] = (uint8_t)(v & 0xffu);
}

static uint16_t get_u16be(const uint8_t *p) {
    return (uint16_t)(((uint16_t)p[0] << 8u) | (uint16_t)p[1]);
}

static void put_u64be(uint8_t *p, uint64_t v) {
    for (unsigned i = 0; i < 8u; ++i) {
        p[i] = (uint8_t)(v >> (56u - 8u * i));
    }
}

static uint64_t get_u64be(const uint8_t *p) {
    uint64_t v = 0u;
    for (unsigned i = 0; i < 8u; ++i) {
        v = (v << 8u) | (uint64_t)p[i];
    }
    return v;
}

static void put_uuid(uint8_t *p, aether_uuid_t uid) {
    put_u64be(p, uid.msb);
    put_u64be(p + 8, uid.lsb);
}

static aether_uuid_t get_uuid(const uint8_t *p) {
    aether_uuid_t u;
    u.msb = get_u64be(p);
    u.lsb = get_u64be(p + 8);
    return u;
}

static bool has_prefix(const uint8_t *b, char c0, char c1, char c2, char c3) {
    return b[0] == (uint8_t)c0 && b[1] == (uint8_t)c1 &&
           b[2] == (uint8_t)c2 && b[3] == (uint8_t)c3;
}

void aether_lan_beacon_encode(
    uint8_t out[AETHER_LAN_BEACON_BYTES],
    aether_uuid_t uid,
    const uint8_t cert_fp[8],
    uint16_t direct_port) {

    memset(out, 0, AETHER_LAN_BEACON_BYTES);
    out[0] = 'A'; out[1] = 'E'; out[2] = 'L'; out[3] = 'N';
    out[4] = AETHER_LAN_PROTO_V1;
    out[5] = AETHER_LAN_TYPE_BEACON;
    out[6] = 0u;
    put_uuid(out + 7, uid);
    memcpy(out + 23, cert_fp, 8);
    put_u16be(out + 31, direct_port);
}

bool aether_lan_beacon_decode(
    const uint8_t *buf, size_t len,
    aether_uuid_t *out_uid,
    uint8_t out_cert_fp[8],
    uint16_t *out_port) {

    if (buf == NULL || len != AETHER_LAN_BEACON_BYTES) return false;
    if (!has_prefix(buf, 'A', 'E', 'L', 'N')) return false;
    if (buf[4] != AETHER_LAN_PROTO_V1) return false;
    if (buf[5] != AETHER_LAN_TYPE_BEACON) return false;

    if (out_uid != NULL) *out_uid = get_uuid(buf + 7);
    if (out_cert_fp != NULL) memcpy(out_cert_fp, buf + 23, 8);
    if (out_port != NULL) *out_port = get_u16be(buf + 31);
    return true;
}

/* ---------- handshake packets ---------- */

void aether_lan_pkt_a_encode(
    uint8_t out[AETHER_LAN_PKT_A_BYTES],
    aether_uuid_t uid_i,
    const uint8_t cert_i[113],
    const uint8_t p1[48]) {

    memset(out, 0, AETHER_LAN_PKT_A_BYTES);
    out[0] = 'A'; out[1] = 'E'; out[2] = 'L'; out[3] = 'H';
    out[4] = AETHER_LAN_PROTO_V1;
    out[5] = AETHER_LAN_TYPE_HS_A;
    out[6] = 0u;
    put_uuid(out + 7, uid_i);
    memcpy(out + 23, cert_i, 113);
    memcpy(out + 136, p1, 48);
}

bool aether_lan_pkt_a_decode(
    const uint8_t *buf, size_t len,
    aether_uuid_t *out_uid,
    uint8_t out_cert[113],
    uint8_t out_p1[48]) {

    if (buf == NULL || len != AETHER_LAN_PKT_A_BYTES) return false;
    if (!has_prefix(buf, 'A', 'E', 'L', 'H')) return false;
    if (buf[4] != AETHER_LAN_PROTO_V1) return false;

    if (buf[5] != AETHER_LAN_TYPE_HS_A) return false;
    if (buf[6] != 0u) return false;


    if (out_uid != NULL) *out_uid = get_uuid(buf + 7);
    if (out_cert != NULL) memcpy(out_cert, buf + 23, 113);
    if (out_p1 != NULL) memcpy(out_p1, buf + 136, 48);
    return true;
}

void aether_lan_pkt_b_encode(
    uint8_t out[AETHER_LAN_PKT_B_BYTES],
    aether_uuid_t uid_r,
    const uint8_t cert_r[113],
    const uint8_t p2[96],
    const uint8_t sig_b[64]) {

    memset(out, 0, AETHER_LAN_PKT_B_BYTES);
    out[0] = 'A'; out[1] = 'E'; out[2] = 'L'; out[3] = 'H';
    out[4] = AETHER_LAN_PROTO_V1;
    out[5] = AETHER_LAN_TYPE_HS_B;
    out[6] = 0u;
    put_uuid(out + 7, uid_r);
    memcpy(out + 23, cert_r, 113);
    memcpy(out + 136, p2, 96);
    memcpy(out + 232, sig_b, 64);
}

bool aether_lan_pkt_b_decode(
    const uint8_t *buf, size_t len,
    aether_uuid_t *out_uid,
    uint8_t out_cert[113],
    uint8_t out_p2[96],
    uint8_t out_sig_b[64]) {

    if (buf == NULL || len != AETHER_LAN_PKT_B_BYTES) return false;
    if (!has_prefix(buf, 'A', 'E', 'L', 'H')) return false;
    if (buf[4] != AETHER_LAN_PROTO_V1) return false;

    if (buf[5] != AETHER_LAN_TYPE_HS_B) return false;
    if (buf[6] != 0u) return false;


    if (out_uid != NULL) *out_uid = get_uuid(buf + 7);
    if (out_cert != NULL) memcpy(out_cert, buf + 23, 113);
    if (out_p2 != NULL) memcpy(out_p2, buf + 136, 96);
    if (out_sig_b != NULL) memcpy(out_sig_b, buf + 232, 64);
    return true;
}

void aether_lan_pkt_c_encode(
    uint8_t out[AETHER_LAN_PKT_C_BYTES],
    const uint8_t p3[64],
    const uint8_t sig_a[64]) {

    memset(out, 0, AETHER_LAN_PKT_C_BYTES);
    out[0] = 'A'; out[1] = 'E'; out[2] = 'L'; out[3] = 'H';
    out[4] = AETHER_LAN_PROTO_V1;
    out[5] = AETHER_LAN_TYPE_HS_C;
    out[6] = 0u;
    memcpy(out + 7, p3, 64);
    memcpy(out + 71, sig_a, 64);
}

bool aether_lan_pkt_c_decode(
    const uint8_t *buf, size_t len,
    uint8_t out_p3[64],
    uint8_t out_sig_a[64]) {

    if (buf == NULL || len != AETHER_LAN_PKT_C_BYTES) return false;
    if (!has_prefix(buf, 'A', 'E', 'L', 'H')) return false;
    if (buf[4] != AETHER_LAN_PROTO_V1) return false;

    if (buf[5] != AETHER_LAN_TYPE_HS_C) return false;
    if (buf[6] != 0u) return false;


    if (out_p3 != NULL) memcpy(out_p3, buf + 7, 64);
    if (out_sig_a != NULL) memcpy(out_sig_a, buf + 71, 64);
    return true;
}

void aether_lan_pkt_d_encode(
    uint8_t out[AETHER_LAN_PKT_D_BYTES],
    const uint8_t session_id[8],
    uint64_t sequence,
    const uint8_t secretbox[36]) {

    memset(out, 0, AETHER_LAN_PKT_D_BYTES);
    out[0] = 'A'; out[1] = 'E'; out[2] = 'L'; out[3] = 'H';
    out[4] = AETHER_LAN_PROTO_V1;
    out[5] = AETHER_LAN_TYPE_HS_D;
    out[6] = 0u;
    memcpy(out + 7, session_id, 8);
    put_u64be(out + 15, sequence);
    memcpy(out + 23, secretbox, 36);
}

bool aether_lan_pkt_d_decode(
    const uint8_t *buf, size_t len,
    uint8_t out_session_id[8],
    uint64_t *out_sequence,
    uint8_t out_secretbox[36]) {

    if (buf == NULL || len != AETHER_LAN_PKT_D_BYTES) return false;
    if (!has_prefix(buf, 'A', 'E', 'L', 'H')) return false;
    if (buf[4] != AETHER_LAN_PROTO_V1) return false;

    if (buf[5] != AETHER_LAN_TYPE_HS_D) return false;
    if (buf[6] != 0u) return false;


    if (out_session_id != NULL) memcpy(out_session_id, buf + 7, 8);
    if (out_sequence != NULL) *out_sequence = get_u64be(buf + 15);
    if (out_secretbox != NULL) memcpy(out_secretbox, buf + 23, 36);
    return true;
}

/* ---------- direct data ---------- */

size_t aether_lan_direct_frame_bytes(size_t payload_len) {
    return AETHER_LAN_DIRECT_HEADER_BYTES +
           hydro_secretbox_HEADERBYTES +
           payload_len;
}


void aether_lan_direct_encode(
    uint8_t *out,
    const uint8_t session_id[8],
    uint64_t sequence,
    const uint8_t *secretbox,
    size_t secretbox_len) {

    out[0] = 'A'; out[1] = 'E'; out[2] = 'L'; out[3] = 'D';
    out[4] = AETHER_LAN_PROTO_V1;
    out[5] = AETHER_LAN_TYPE_DATA;
    memcpy(out + 6, session_id, 8);
    put_u64be(out + 14, sequence);
    if (secretbox_len != 0u) {
        memcpy(out + 22, secretbox, secretbox_len);
    }
}



bool aether_lan_direct_decode(
    const uint8_t *buf, size_t len,
    uint8_t out_session_id[8],
    uint64_t *out_sequence,
    const uint8_t **out_secretbox,
    size_t *out_secretbox_len) {

    if (buf == NULL || len < AETHER_LAN_DIRECT_HEADER_BYTES) return false;
    if (!has_prefix(buf, 'A', 'E', 'L', 'D')) return false;
    if (buf[4] != AETHER_LAN_PROTO_V1) return false;
    if (buf[5] != AETHER_LAN_TYPE_DATA) return false;

    if (out_session_id != NULL) memcpy(out_session_id, buf + 6, 8);
    if (out_sequence != NULL) *out_sequence = get_u64be(buf + 14);
    if (out_secretbox != NULL) *out_secretbox = buf + 22;
    if (out_secretbox_len != NULL) {
        *out_secretbox_len = len - AETHER_LAN_DIRECT_HEADER_BYTES;
    }
    return true;
}


/* ---------- replay window ---------- */

bool aether_lan_replay_precheck(
    uint64_t rx_highest, uint64_t rx_seen, uint64_t seq) {

    if (seq == 0u) return false;
    if (seq > rx_highest) return true;

    uint64_t delta = rx_highest - seq;
    if (delta >= 64u) return false;
    return (rx_seen & (UINT64_C(1) << delta)) == 0u;
}

void aether_lan_replay_commit(
    uint64_t *rx_highest, uint64_t *rx_seen, uint64_t seq) {

    if (seq > *rx_highest) {
        uint64_t shift = seq - *rx_highest;
        if (shift >= 64u) {
            *rx_seen = 1u;
        } else {
            *rx_seen = (*rx_seen << shift) | UINT64_C(1);
        }
        *rx_highest = seq;
    } else {
        uint64_t delta = *rx_highest - seq;
        if (delta < 64u) {
            *rx_seen |= (UINT64_C(1) << delta);
        }
    }
}

/* ---------- transcript / session ID ---------- */

bool aether_lan_session_id_compute(
    const uint8_t p1[48],
    const uint8_t p2[96],
    const uint8_t p3[64],
    uint8_t out8[8]) {

    static const char ctx[hydro_hash_CONTEXTBYTES] = {
        'A', 'E', 'T', 'H', 'E', 'R', 'L', 'N'
    };

    hydro_hash_state st;
    if (hydro_hash_init(&st, ctx, NULL) != 0) return false;
    if (hydro_hash_update(&st, p1, 48) != 0) return false;
    if (hydro_hash_update(&st, p2, 96) != 0) return false;
    if (hydro_hash_update(&st, p3, 64) != 0) return false;

    uint8_t digest[hydro_hash_BYTES];
    if (hydro_hash_final(&st, digest, sizeof(digest)) != 0) return false;

    memcpy(out8, digest, 8);
    return true;
}
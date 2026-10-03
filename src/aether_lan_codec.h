
#ifndef AETHER_LAN_CODEC_H
#define AETHER_LAN_CODEC_H

#include "aether.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AETHER_LAN_BEACON_BYTES        33u
#define AETHER_LAN_PKT_A_BYTES         184u
#define AETHER_LAN_PKT_B_BYTES         296u
#define AETHER_LAN_PKT_C_BYTES         136u
#define AETHER_LAN_PKT_D_BYTES         59u
#define AETHER_LAN_DIRECT_HEADER_BYTES 22u
#define AETHER_LAN_DIRECT_SBOX_BYTES   36u
#define AETHER_LAN_DIRECT_OVERHEAD     58u

#define AETHER_LAN_PROTO_V1     0x01u
#define AETHER_LAN_TYPE_BEACON  0x01u
#define AETHER_LAN_TYPE_HS_A    0x10u
#define AETHER_LAN_TYPE_HS_B    0x11u
#define AETHER_LAN_TYPE_HS_C    0x12u
#define AETHER_LAN_TYPE_HS_D    0x13u
#define AETHER_LAN_TYPE_DATA    0x20u

void aether_lan_beacon_encode(
    uint8_t out[AETHER_LAN_BEACON_BYTES],
    aether_uuid_t uid,
    const uint8_t cert_fp[8],
    uint16_t direct_port);

bool aether_lan_beacon_decode(
    const uint8_t *buf, size_t len,
    aether_uuid_t *out_uid,
    uint8_t out_cert_fp[8],
    uint16_t *out_port);

void aether_lan_pkt_a_encode(
    uint8_t out[AETHER_LAN_PKT_A_BYTES],
    aether_uuid_t uid_i,
    const uint8_t cert_i[113],
    const uint8_t p1[48]);

bool aether_lan_pkt_a_decode(
    const uint8_t *buf, size_t len,
    aether_uuid_t *out_uid,
    uint8_t out_cert[113],
    uint8_t out_p1[48]);

void aether_lan_pkt_b_encode(
    uint8_t out[AETHER_LAN_PKT_B_BYTES],
    aether_uuid_t uid_r,
    const uint8_t cert_r[113],
    const uint8_t p2[96],
    const uint8_t sig_b[64]);

bool aether_lan_pkt_b_decode(
    const uint8_t *buf, size_t len,
    aether_uuid_t *out_uid,
    uint8_t out_cert[113],
    uint8_t out_p2[96],
    uint8_t out_sig_b[64]);

void aether_lan_pkt_c_encode(
    uint8_t out[AETHER_LAN_PKT_C_BYTES],
    const uint8_t p3[64],
    const uint8_t sig_a[64]);

bool aether_lan_pkt_c_decode(
    const uint8_t *buf, size_t len,
    uint8_t out_p3[64],
    uint8_t out_sig_a[64]);

void aether_lan_pkt_d_encode(
    uint8_t out[AETHER_LAN_PKT_D_BYTES],
    const uint8_t session_id[8],
    uint64_t sequence,
    const uint8_t secretbox[36]);

bool aether_lan_pkt_d_decode(
    const uint8_t *buf, size_t len,
    uint8_t out_session_id[8],
    uint64_t *out_sequence,
    uint8_t out_secretbox[36]);

size_t aether_lan_direct_frame_bytes(size_t payload_len);

void aether_lan_direct_encode(
    uint8_t *out,
    const uint8_t session_id[8],
    uint64_t sequence,
    const uint8_t *secretbox,
    size_t secretbox_len);

bool aether_lan_direct_decode(
    const uint8_t *buf, size_t len,
    uint8_t out_session_id[8],
    uint64_t *out_sequence,
    const uint8_t **out_secretbox,
    size_t *out_secretbox_len);

bool aether_lan_replay_precheck(
    uint64_t rx_highest, uint64_t rx_seen, uint64_t seq);
void aether_lan_replay_commit(
    uint64_t *rx_highest, uint64_t *rx_seen, uint64_t seq);

bool aether_lan_session_id_compute(
    const uint8_t p1[48],
    const uint8_t p2[96],
    const uint8_t p3[64],
    uint8_t out8[8]);

#ifdef __cplusplus
}
#endif

#endif

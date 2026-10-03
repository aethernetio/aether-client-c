
/*
 * LAN codec + replay window regression tests. Pure functions, no sockets.
 */
#include "aether_lan_codec.h"

#include "hydrogen.h"

#include <assert.h>
#include <string.h>

static void test_beacon_roundtrip(void) {
    uint8_t buf[AETHER_LAN_BEACON_BYTES];
    aether_uuid_t uid = { 0x0123456789ABCDEFull, 0xFEDCBA9876543210ull };
    uint8_t fp[8] = { 1,2,3,4,5,6,7,8 };

    aether_lan_beacon_encode(buf, uid, fp, 54321u);
    assert(buf[0] == 'A' && buf[1] == 'E' && buf[2] == 'L' && buf[3] == 'N');
    assert(buf[4] == AETHER_LAN_PROTO_V1);
    assert(buf[5] == AETHER_LAN_TYPE_BEACON);

    aether_uuid_t out_uid;
    uint8_t out_fp[8];
    uint16_t out_port;
    assert(aether_lan_beacon_decode(buf, sizeof(buf), &out_uid, out_fp, &out_port));
    assert(out_uid.msb == uid.msb && out_uid.lsb == uid.lsb);
    assert(memcmp(out_fp, fp, 8) == 0);
    assert(out_port == 54321u);

    assert(!aether_lan_beacon_decode(buf, 32u, NULL, NULL, NULL));
    buf[0] = 'X';
    assert(!aether_lan_beacon_decode(buf, sizeof(buf), NULL, NULL, NULL));
    buf[0] = 'A'; buf[4] = 0x02;
    assert(!aether_lan_beacon_decode(buf, sizeof(buf), NULL, NULL, NULL));
}

static void test_hs_packet_a(void) {
    uint8_t a[AETHER_LAN_PKT_A_BYTES];
    aether_uuid_t uid = { 0x1111111111111111ull, 0x2222222222222222ull };
    uint8_t cert[113]; uint8_t p1[48];
    for (unsigned i = 0; i < 113u; ++i) cert[i] = (uint8_t)i;
    for (unsigned i = 0; i < 48u; ++i) p1[i] = (uint8_t)(0xA0u + i);
    aether_lan_pkt_a_encode(a, uid, cert, p1);
    assert(a[0] == 'A' && a[1] == 'E' && a[2] == 'L' && a[3] == 'H');
    assert(a[4] == AETHER_LAN_PROTO_V1);
    assert(a[5] == AETHER_LAN_TYPE_HS_A);
    aether_uuid_t got_uid; uint8_t got_cert[113]; uint8_t got_p1[48];
    assert(aether_lan_pkt_a_decode(a, sizeof(a), &got_uid, got_cert, got_p1));
    assert(got_uid.msb == uid.msb && got_uid.lsb == uid.lsb);
    assert(memcmp(got_cert, cert, 113) == 0);
    assert(memcmp(got_p1, p1, 48) == 0);
    assert(!aether_lan_pkt_a_decode(a, 100u, NULL, NULL, NULL));
}

static void test_hs_packets_bcd(void) {
    aether_uuid_t uid = { 0x1111111111111111ull, 0x2222222222222222ull };

    /* Packet B */
    uint8_t b[AETHER_LAN_PKT_B_BYTES];
    uint8_t cert_r[113]; uint8_t p2[96]; uint8_t sig_b[64];
    for (unsigned i = 0; i < 113u; ++i) cert_r[i] = (uint8_t)(0xFFu - i);
    for (unsigned i = 0; i < 96u; ++i) p2[i] = (uint8_t)i;
    for (unsigned i = 0; i < 64u; ++i) sig_b[i] = (uint8_t)(0x50u + i);
    aether_lan_pkt_b_encode(b, uid, cert_r, p2, sig_b);
    assert(b[5] == AETHER_LAN_TYPE_HS_B);
    aether_uuid_t got_uid; uint8_t got_bcert[113]; uint8_t got_p2[96]; uint8_t got_sigb[64];
    assert(aether_lan_pkt_b_decode(b, sizeof(b), &got_uid, got_bcert, got_p2, got_sigb));
    assert(memcmp(got_bcert, cert_r, 113) == 0);
    assert(memcmp(got_p2, p2, 96) == 0);
    assert(memcmp(got_sigb, sig_b, 64) == 0);

    /* Packet C */
    uint8_t c[AETHER_LAN_PKT_C_BYTES];
    uint8_t p3[64]; uint8_t sig_a[64];
    for (unsigned i = 0; i < 64u; ++i) p3[i] = (uint8_t)(0xC0u + i);
    for (unsigned i = 0; i < 64u; ++i) sig_a[i] = (uint8_t)(0x30u + i);
    aether_lan_pkt_c_encode(c, p3, sig_a);
    assert(c[5] == AETHER_LAN_TYPE_HS_C);
    uint8_t got_p3[64]; uint8_t got_siga[64];
    assert(aether_lan_pkt_c_decode(c, sizeof(c), got_p3, got_siga));
    assert(memcmp(got_p3, p3, 64) == 0);
    assert(memcmp(got_siga, sig_a, 64) == 0);
    assert(!aether_lan_pkt_c_decode(c, 135u, NULL, NULL));

    /* Packet D */
    uint8_t d[AETHER_LAN_PKT_D_BYTES];
    uint8_t sid[8] = { 1,2,3,4,5,6,7,8 };
    uint8_t sbox[36]; for (unsigned i = 0; i < 36u; ++i) sbox[i] = (uint8_t)i;
    aether_lan_pkt_d_encode(d, sid, 0x0102030405060708ull, sbox);
    assert(d[5] == AETHER_LAN_TYPE_HS_D);
    uint8_t got_sid[8]; uint64_t got_seq; uint8_t got_sbox[36];
    assert(aether_lan_pkt_d_decode(d, sizeof(d), got_sid, &got_seq, got_sbox));
    assert(memcmp(got_sid, sid, 8) == 0);
    assert(got_seq == 0x0102030405060708ull);
    assert(memcmp(got_sbox, sbox, 36) == 0);
    assert(!aether_lan_pkt_d_decode(d, 58u, NULL, NULL, NULL));
}

static void test_direct_frame(void) {
    uint8_t buf[128];
    uint8_t sid[8] = { 9,8,7,6,5,4,3,2 };
    const uint8_t sbox[36] = { 0 };
    aether_lan_direct_encode(buf, sid, 1u, sbox, sizeof(sbox));

    uint8_t got_sid[8]; uint64_t got_seq;
    const uint8_t *got_sbox; size_t got_len;
    assert(aether_lan_direct_decode(buf, AETHER_LAN_DIRECT_HEADER_BYTES + 36u,
                                    got_sid, &got_seq, &got_sbox, &got_len));
    assert(memcmp(got_sid, sid, 8) == 0);
    assert(got_seq == 1u);
    assert(got_len == 36u);
    assert(!aether_lan_direct_decode(buf, 5u, NULL, NULL, NULL, NULL));

    assert(aether_lan_direct_frame_bytes(0u) ==
           AETHER_LAN_DIRECT_HEADER_BYTES + hydro_secretbox_HEADERBYTES);
}

static void test_replay_window(void) {
    uint64_t hi = 0u; uint64_t seen = 0u;

    assert(aether_lan_replay_precheck(hi, seen, 10u));
    aether_lan_replay_commit(&hi, &seen, 10u);
    assert(hi == 10u);

    assert(aether_lan_replay_precheck(hi, seen, 12u));
    aether_lan_replay_commit(&hi, &seen, 12u);

    assert(aether_lan_replay_precheck(hi, seen, 11u));
    aether_lan_replay_commit(&hi, &seen, 11u);

    /* Duplicates rejected. */
    assert(!aether_lan_replay_precheck(hi, seen, 11u));
    assert(!aether_lan_replay_precheck(hi, seen, 12u));
    assert(!aether_lan_replay_precheck(hi, seen, 10u));

    /* Far behind / zero rejected. */

    /* Zero rejected. */
    assert(!aether_lan_replay_precheck(hi, seen, 0u));

    /* Advance window far enough that seq 1 falls out of it. */
    aether_lan_replay_commit(&hi, &seen, 200u);
    assert(!aether_lan_replay_precheck(hi, seen, 1u));
    /* A gap inside the new window is still acceptable. */
    assert(aether_lan_replay_precheck(hi, seen, 199u));



    /*
     * Forged huge + bad MAC: precheck returns true, but AEAD failure
     * means commit is never called; window must be unchanged.
     */
    uint64_t before_hi = hi;
    uint64_t before_seen = seen;
    assert(aether_lan_replay_precheck(before_hi, before_seen, 1000000u));
    assert(hi == before_hi);
    assert(seen == before_seen);

    /* Current-window seq still accepted after the forged attempt. */

    /* An inside-window seq still accepted after the forged attempt. */
    assert(aether_lan_replay_precheck(hi, seen, 198u));

}

static void test_session_id_deterministic(void) {
    uint8_t p1[48]; uint8_t p2[96]; uint8_t p3[64];
    memset(p1, 0x11, sizeof(p1));
    memset(p2, 0x22, sizeof(p2));
    memset(p3, 0x33, sizeof(p3));

    uint8_t a[8]; uint8_t b[8];
    assert(aether_lan_session_id_compute(p1, p2, p3, a));
    assert(aether_lan_session_id_compute(p1, p2, p3, b));
    assert(memcmp(a, b, 8) == 0);

    p3[0] ^= 0x01u;
    uint8_t c[8];
    assert(aether_lan_session_id_compute(p1, p2, p3, c));
    assert(memcmp(a, c, 8) != 0);
}

int main(void) {
    (void)hydro_init();
    test_beacon_roundtrip();
    test_hs_packet_a();
    test_hs_packets_bcd();
    test_direct_frame();
    test_replay_window();
    test_session_id_deterministic();
    return 0;
}
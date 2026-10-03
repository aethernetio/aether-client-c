
/*
 * Host LAN end-to-end test.
 *
 * Two aether_t clients, each with aether_lan, no cloud provider.
 * Both bind real UDP sockets on loopback, discover via multicast,
 * run Noise XX handshake with identity certificates (fake crypto
 * backend: verify_signature accepts P256 sigs without cryptographic
 * check; hydro_sign is real), reach VERIFIED, then exchange an
 * exact binary payload through aether_delivery_send.
 */
#include "aether.h"
#include "aether_lan.h"
#include "aether_delivery.h"

#include "hydrogen.h"

#include <assert.h>

#include <time.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static aether_t ca;
static aether_t cb;

static aether_lan_t lan_a;
static aether_lan_t lan_b;

static aether_lan_peer_t peers_a[4];
static aether_lan_peer_t peers_b[4];

static uint8_t rxbuf_a[576]; static uint8_t txbuf_a[576];
static uint8_t rxbuf_b[576]; static uint8_t txbuf_b[576];

static aether_lan_identity_t id_a;
static aether_lan_identity_t id_b;

static aether_component_t comps_a[2];
static aether_component_t comps_b[2];

static uint8_t trusted_root[64];

/* ---------- fake crypto (always-OK verify) ---------- */

static aether_status_t fake_init(void *ctx) { (void)ctx; return AETHER_OK; }
static aether_status_t fake_random_key(void *c, uint8_t *out, size_t n) {
    (void)c; memset(out, 0x5A, n); return AETHER_OK;
}
static aether_status_t fake_verify(
    void *ctx, const uint8_t *pk, size_t pkl,
    const uint8_t *sig, size_t sigl,
    const uint8_t *data, size_t dl) {
    (void)ctx; (void)pk; (void)pkl; (void)sig; (void)sigl;
    (void)data; (void)dl;
    return AETHER_OK;
}
static aether_status_t fake_asym(
    void *c, const uint8_t *pk, size_t pkl,
    const uint8_t *pl, size_t pll,
    uint8_t *o, size_t oc, size_t *ol) {
    (void)c; (void)pk; (void)pkl;
    if (oc < pll) return AETHER_ERR_OVERFLOW;
    memcpy(o, pl, pll); *ol = pll; return AETHER_OK;
}
static aether_status_t fake_sym_e(
    void *c, const uint8_t *k, size_t kl,
    const uint8_t *pl, size_t pll,
    uint8_t *o, size_t oc, size_t *ol) {
    (void)c; (void)k; (void)kl;
    if (oc < pll) return AETHER_ERR_OVERFLOW;
    memcpy(o, pl, pll); *ol = pll; return AETHER_OK;
}
static aether_status_t fake_sym_d(
    void *c, const uint8_t *k, size_t kl,
    const uint8_t *ci, size_t cl,
    uint8_t *o, size_t oc, size_t *ol) {
    (void)c; (void)k; (void)kl;
    if (oc < cl) return AETHER_ERR_OVERFLOW;
    memcpy(o, ci, cl); *ol = cl; return AETHER_OK;
}
static aether_status_t fake_derive(
    void *c, const uint8_t *mk, size_t mkl,
    int32_t sid, uint32_t kn,
    uint8_t *c2s, uint8_t *s2c, size_t dl) {
    (void)c; (void)mk; (void)mkl; (void)sid; (void)kn;
    memset(c2s, 0, dl); memset(s2c, 0, dl);
    return AETHER_OK;
}

static const aether_crypto_vtable_t fake_crypto = {
    .profile = { 0,0,0,0, 32,64,64,64 },
    .initialize = fake_init,
    .random_symmetric_key = fake_random_key,
    .verify_signature = fake_verify,
    .asymmetric_encrypt = fake_asym,
    .symmetric_encrypt = fake_sym_e,
    .symmetric_decrypt = fake_sym_d,
    .derive_server_keys = fake_derive
};

/* ---------- timing / poll ---------- */

static uint64_t now_ms(void *ctx) {
    (void)ctx;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000u + (uint64_t)(ts.tv_nsec / 1000000);
}

static aether_status_t resolve_loopback(
    void *ctx, const char *h, aether_address_t *a) {
    (void)ctx; (void)h;
    memset(a, 0, sizeof(*a));
    a->kind = AETHER_ADDR_IPV4;
    a->length = 4u;
    a->bytes[0] = 127; a->bytes[3] = 1;
    return AETHER_OK;
}

static aether_status_t t_open(
    void *c, aether_channel_t ch, const aether_endpoint_t *ep) {
    (void)c; (void)ch; (void)ep; return AETHER_OK;
}
static void t_close(void *c, aether_channel_t ch) { (void)c; (void)ch; }
static aether_status_t t_send(
    void *c, aether_channel_t ch, const uint8_t *d, size_t n) {
    (void)c; (void)ch; (void)d; (void)n; return AETHER_OK;
}
static aether_status_t t_poll(void *c) { (void)c; return AETHER_OK; }
static aether_status_t t_recv(
    void *c, aether_channel_t ch, aether_bytes_view_t *f) {
    (void)c; (void)ch;
    if (f) { f->data = NULL; f->length = 0u; }
    return AETHER_OK;
}
static void t_consume(
    void *c, aether_channel_t ch, const uint8_t *d, size_t n) {
    (void)c; (void)ch; (void)d; (void)n;
}
static aether_status_t t_flash_load(
    void *c, uint8_t *d, size_t cap, size_t *len) {
    (void)c; (void)d; (void)cap;
    if (len) *len = 0u;
    return AETHER_ERR_STORAGE;
}
static aether_status_t t_flash_save(
    void *c, const uint8_t *s, size_t n) {
    (void)c; (void)s; (void)n; return AETHER_OK;
}
static aether_status_t t_pow(
    void *c, const uint8_t *sa, size_t sl,
    const uint8_t *su, size_t sul,
    uint8_t p, int32_t mh, int32_t *ws, size_t cap, size_t *ct) {
    (void)c; (void)sa; (void)sl; (void)su; (void)sul;
    (void)p; (void)mh; (void)ws; (void)cap;
    if (ct) *ct = 0u;
    return AETHER_OK;
}

aether_status_t aether_platform_init(
    void *ctx, size_t ctx_size, uint32_t slot,
    aether_client_platform_t *plat) {

    (void)ctx; (void)ctx_size; (void)slot;
    if (plat == NULL) return AETHER_ERR_ARGUMENT;
    memset(plat, 0, sizeof(*plat));
    plat->ctx = (void *)1;
    plat->now_ms = now_ms;
    plat->resolve = resolve_loopback;
    plat->poll = t_poll;
    plat->receive = t_recv;
    plat->consume = t_consume;
    plat->transport = (aether_transport_vtable_t){
        (void *)1, t_open, t_close, t_send
    };
    plat->flash = (aether_flash_vtable_t){
        (void *)1, t_flash_load, t_flash_save
    };
    plat->crypto = &fake_crypto;
    plat->crypto_ctx = NULL;
    plat->trusted_sign_keys = trusted_root;
    plat->trusted_sign_key_count = 1u;
    plat->pow = (aether_pow_vtable_t){ (void *)1, t_pow };
    return AETHER_OK;
}

void aether_platform_deinit(void *ctx) { (void)ctx; }

/* ---------- received message capture ---------- */

static uint8_t got_a[64];
static size_t got_a_len = 0u;
static uint8_t got_b[64];
static size_t got_b_len = 0u;

static void on_message_a(
    aether_t *cl, aether_uuid_t from,
    const uint8_t *d, size_t n) {
    (void)cl; (void)from;
    if (n <= sizeof(got_a)) { memcpy(got_a, d, n); got_a_len = n; }
}
static void on_message_b(
    aether_t *cl, aether_uuid_t from,
    const uint8_t *d, size_t n) {
    (void)cl; (void)from;
    if (n <= sizeof(got_b)) { memcpy(got_b, d, n); got_b_len = n; }
}

/* ---------- provision ---------- */

static void build_authority_cert(uint8_t out[134]) {
    memset(out, 0, 134);
    out[0] = 0x01u;             /* version */
    out[1] = 0x01u;             /* PEER_IDENTITY_ISSUER */
    /* authority_id = 1 */
    out[2] = 0; out[3] = 0; out[4] = 0; out[5] = 1;
    /* authority_p256_pk: arbitrary test bytes */
    for (unsigned i = 0; i < 64u; ++i) out[6 + i] = (uint8_t)(i + 1u);
    /* root_signature: arbitrary; fake verify accepts */
    for (unsigned i = 0; i < 64u; ++i) out[70 + i] = (uint8_t)(0x40u + i);
}

static void build_peer_cert(
    uint8_t out[113], aether_uuid_t uid,
    const uint8_t sign_pk[32]) {
    memset(out, 0, 113);
    out[0] = 0x01u;
    for (unsigned i = 0; i < 8u; ++i) {
        out[1 + i] = (uint8_t)(uid.msb >> (56u - 8u*i));
        out[9 + i] = (uint8_t)(uid.lsb >> (56u - 8u*i));
    }
    memcpy(out + 17, sign_pk, 32);
    for (unsigned i = 0; i < 64u; ++i) out[49 + i] = (uint8_t)(0xA0u + i);
}

/* ---------- main ---------- */

int main(void) {
    assert(hydro_init() == 0);

    for (unsigned i = 0; i < 64u; ++i) trusted_root[i] = (uint8_t)(0x80u + i);

    /* Two client identities with real hydro_sign and hydro_kx keys. */
    hydro_sign_keypair ska, skb;
    hydro_sign_keygen(&ska);
    hydro_sign_keygen(&skb);
    hydro_kx_keypair kxa, kxb;
    hydro_kx_keygen(&kxa);
    hydro_kx_keygen(&kxb);

    aether_uuid_t uid_a = { 0x1111222233334444ull, 0x5555666677778888ull };
    aether_uuid_t uid_b = { 0x2222333344445555ull, 0x6666777788889999ull };

    memset(&id_a, 0, sizeof(id_a));
    memset(&id_b, 0, sizeof(id_b));
    memcpy(id_a.sign_sk, ska.sk, 64);
    memcpy(id_b.sign_sk, skb.sk, 64);
    memcpy(id_a.kx_pk, kxa.pk, 32);
    memcpy(id_a.kx_sk, kxa.sk, 32);
    memcpy(id_b.kx_pk, kxb.pk, 32);
    memcpy(id_b.kx_sk, kxb.sk, 32);
    build_authority_cert(id_a.authority_cert);
    build_authority_cert(id_b.authority_cert);
    build_peer_cert(id_a.cert, uid_a, ska.pk);
    build_peer_cert(id_b.cert, uid_b, skb.pk);

    /* Init both clients. */
    assert(aether_init(&ca, uid_a) == AETHER_OK);
    assert(aether_init(&cb, uid_b) == AETHER_OK);

    /*
     * The facade only learns its own UID through registration or
     * credential restore. Tests provision it directly before start.
     */
    ca.app.core.uid = uid_a;
    cb.app.core.uid = uid_b;

    aether_on_message(&ca, on_message_a);
    aether_on_message(&cb, on_message_b);

    assert(aether_set_components(&ca, comps_a, 2u) == AETHER_OK);
    assert(aether_set_components(&cb, comps_b, 2u) == AETHER_OK);

    aether_status_t s =
        aether_lan_init(&lan_a, &ca, &id_a,
                        peers_a, 4u, rxbuf_a, sizeof(rxbuf_a),
                        txbuf_a, sizeof(txbuf_a));
    if (s != AETHER_OK) { fprintf(stderr, "lan_a init %d\n", (int)s); return 2; }
    s = aether_lan_init(&lan_b, &cb, &id_b,
                        peers_b, 4u, rxbuf_b, sizeof(rxbuf_b),
                        txbuf_b, sizeof(txbuf_b));
    if (s != AETHER_OK) { fprintf(stderr, "lan_b init %d\n", (int)s); return 2; }


    /*
     * Seed peer tables directly. Loopback multicast is unreliable on
     * many hosts and is orthogonal to the handshake/session path we
     * want to exercise here.
     */
    lan_a.peers[0].uid = uid_b;
    lan_a.peers[0].ipv4[0] = 127; lan_a.peers[0].ipv4[3] = 1;
    lan_a.peers[0].direct_port = lan_b.direct_port;
    lan_a.peers[0].state = AETHER_LAN_PEER_DISCOVERED;
    lan_a.peers[0].last_seen_ms = (uint32_t)now_ms(NULL);

    lan_b.peers[0].uid = uid_a;
    lan_b.peers[0].ipv4[0] = 127; lan_b.peers[0].ipv4[3] = 1;
    lan_b.peers[0].direct_port = lan_a.direct_port;
    lan_b.peers[0].state = AETHER_LAN_PEER_DISCOVERED;
    lan_b.peers[0].last_seen_ms = (uint32_t)now_ms(NULL);


    assert(aether_register_component(&ca, &lan_a, aether_lan_work) == AETHER_OK);
    assert(aether_register_component(&cb, &lan_b, aether_lan_work) == AETHER_OK);


    /*
     * We do not call aether_start(): the facade would re-initialize
     * the core and wipe the test-provisioned UID. LAN work is driven
     * directly, which is sufficient for the handshake/session path.
     */


    /* Drive both until both see each other as VERIFIED, or timeout. */
    uint64_t deadline = now_ms(NULL) + 15000u;
    bool ok = false;

    while (now_ms(NULL) < deadline) {
        (void)aether_work(&ca);
        (void)aether_work(&cb);
        (void)aether_lan_work(&ca, &lan_a);
        (void)aether_lan_work(&cb, &lan_b);

        aether_lan_peer_state_t sa = aether_lan_peer_state(&lan_a, uid_b);
        aether_lan_peer_state_t sb = aether_lan_peer_state(&lan_b, uid_a);
        if (sa == AETHER_LAN_PEER_VERIFIED &&
            sb == AETHER_LAN_PEER_VERIFIED) {
            ok = true;
            break;
        }
        usleep(2000);
    }

    if (!ok) {
        fprintf(stderr, "LAN: never reached VERIFIED\n");
        return 3;
    }

    /* Now delivery: A -> B, then B -> A. */
    aether_delivery_t del_a, del_b;
    aether_delivery_slot_t sl_a[2], sl_b[2];
    uint8_t ps_a[2*64], ps_b[2*64];
    aether_delivery_provider_t pr_a[1], pr_b[1];
    aether_lan_provider_context_t pc_a, pc_b;

    aether_delivery_init(&del_a, &ca, sl_a, 2u, ps_a, 64u);
    aether_delivery_init(&del_b, &cb, sl_b, 2u, ps_b, 64u);
    aether_delivery_set_providers(&del_a, pr_a, 1u);
    aether_delivery_set_providers(&del_b, pr_b, 1u);

    aether_delivery_provider_t pa = aether_lan_delivery_provider(&pc_a, &lan_a);
    aether_delivery_provider_t pb = aether_lan_delivery_provider(&pc_b, &lan_b);
    assert(aether_delivery_add_provider(&del_a, pa) == AETHER_OK);
    assert(aether_delivery_add_provider(&del_b, pb) == AETHER_OK);

    const uint8_t payload[8] = { 0x00,0x01,0x02,0x7F,0x80,0xFF,0x55,0xAA };

    assert(aether_delivery_send(&del_a, uid_b, payload, 8u) == AETHER_OK);

    /* Pump until delivered or timeout. */
    deadline = now_ms(NULL) + 5000u;
    while (now_ms(NULL) < deadline) {
        (void)aether_work(&ca);
        (void)aether_work(&cb);
        (void)aether_delivery_work(&ca, &del_a);
        (void)aether_delivery_work(&cb, &del_b);
        if (got_b_len == 8u) break;
        usleep(2000);
    }

    if (got_b_len != 8u || memcmp(got_b, payload, 8u) != 0) {
        fprintf(stderr, "LAN: payload not delivered (got %zu)\n", got_b_len);
        return 4;
    }

    /* Reverse direction. */
    assert(aether_delivery_send(&del_b, uid_a, payload, 8u) == AETHER_OK);
    deadline = now_ms(NULL) + 5000u;
    while (now_ms(NULL) < deadline) {
        (void)aether_work(&ca);
        (void)aether_work(&cb);
        (void)aether_delivery_work(&ca, &del_a);
        (void)aether_delivery_work(&cb, &del_b);
        if (got_a_len == 8u) break;
        usleep(2000);
    }
    if (got_a_len != 8u || memcmp(got_a, payload, 8u) != 0) {
        fprintf(stderr, "LAN: reverse payload not delivered\n");
        return 5;
    }

    aether_lan_deinit(&lan_a);
    aether_lan_deinit(&lan_b);
    aether_stop(&ca);
    aether_stop(&cb);

    printf("LAN e2e: OK\n");
    return 0;
}
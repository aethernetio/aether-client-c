
/*
 * LAN component-scheduler integration.
 *
 * Confirms aether_lan_work() is reachable through the registered
 * component dispatch of aether_work().
 */

#include <stdio.h>

#include "aether.h"
#include "aether_lan.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static aether_t c;
static aether_lan_t lan;
static aether_lan_peer_t peers[2];
static uint8_t rx[576];
static uint8_t tx[576];
static aether_lan_identity_t id;
static aether_component_t comps[1];

static uint64_t fake_now_ms(void *ctx) { (void)ctx; return 0u; }
static aether_status_t fake_resolve(
    void *ctx, const char *h, aether_address_t *a) {
    (void)ctx; (void)h;
    memset(a, 0, sizeof(*a));
    a->kind = AETHER_ADDR_IPV4;
    a->length = 4u;
    a->bytes[0] = 127; a->bytes[3] = 1;
    return AETHER_OK;
}
static aether_status_t fake_open(
    void *c, aether_channel_t ch, const aether_endpoint_t *e) {
    (void)c; (void)ch; (void)e; return AETHER_OK;
}
static void fake_close(void *c, aether_channel_t ch) { (void)c; (void)ch; }
static aether_status_t fake_send(
    void *c, aether_channel_t ch, const uint8_t *d, size_t n) {
    (void)c; (void)ch; (void)d; (void)n; return AETHER_OK;
}
static aether_status_t fake_poll(void *c) { (void)c; return AETHER_OK; }
static aether_status_t fake_recv(
    void *c, aether_channel_t ch, aether_bytes_view_t *f) {
    (void)c; (void)ch;
    if (f) { f->data = NULL; f->length = 0u; }
    return AETHER_OK;
}
static void fake_consume(
    void *c, aether_channel_t ch, const uint8_t *d, size_t n) {
    (void)c; (void)ch; (void)d; (void)n;
}
static aether_status_t fake_flash_load(
    void *c, uint8_t *d, size_t cap, size_t *len) {
    (void)c; (void)d; (void)cap;
    if (len) *len = 0u;
    return AETHER_ERR_STORAGE;
}
static aether_status_t fake_flash_save(
    void *c, const uint8_t *s, size_t n) {
    (void)c; (void)s; (void)n; return AETHER_OK;
}
static aether_status_t fake_pow(
    void *c, const uint8_t *sa, size_t sl,
    const uint8_t *su, size_t sul,
    uint8_t p, int32_t mh, int32_t *ws, size_t cap, size_t *ct) {
    (void)c; (void)sa; (void)sl; (void)su; (void)sul;
    (void)p; (void)mh; (void)ws; (void)cap;
    if (ct) *ct = 0u;
    return AETHER_OK;
}
static aether_status_t crypto_stub(void *c) { (void)c; return AETHER_OK; }
static aether_status_t crypto_rand(void *c, uint8_t *o, size_t n) {
    (void)c; memset(o, 0x11, n); return AETHER_OK;
}
static aether_status_t crypto_verify(
    void *c, const uint8_t *pk, size_t pkl,
    const uint8_t *s, size_t sl,
    const uint8_t *d, size_t dl) {
    (void)c; (void)pk; (void)pkl; (void)s; (void)sl; (void)d; (void)dl;
    return AETHER_OK;
}
static aether_status_t crypto_asym(
    void *c, const uint8_t *pk, size_t pkl,
    const uint8_t *pl, size_t pll,
    uint8_t *o, size_t oc, size_t *ol) {
    (void)c; (void)pk; (void)pkl;
    if (oc < pll) return AETHER_ERR_OVERFLOW;
    memcpy(o, pl, pll); *ol = pll; return AETHER_OK;
}
static aether_status_t crypto_se(
    void *c, const uint8_t *k, size_t kl,
    const uint8_t *pl, size_t pll,
    uint8_t *o, size_t oc, size_t *ol) {
    (void)c; (void)k; (void)kl;
    if (oc < pll) return AETHER_ERR_OVERFLOW;
    memcpy(o, pl, pll); *ol = pll; return AETHER_OK;
}
static aether_status_t crypto_sd(
    void *c, const uint8_t *k, size_t kl,
    const uint8_t *ci, size_t cl,
    uint8_t *o, size_t oc, size_t *ol) {
    (void)c; (void)k; (void)kl;
    if (oc < cl) return AETHER_ERR_OVERFLOW;
    memcpy(o, ci, cl); *ol = cl; return AETHER_OK;
}
static aether_status_t crypto_derive(
    void *c, const uint8_t *mk, size_t mkl,
    int32_t sid, uint32_t kn,
    uint8_t *a, uint8_t *b, size_t dl) {
    (void)c; (void)mk; (void)mkl; (void)sid; (void)kn;
    memset(a, 0, dl); memset(b, 0, dl); return AETHER_OK;
}

static const aether_crypto_vtable_t vtable = {
    .profile = { 0,0,0,0, 32,64,64,64 },
    .initialize = crypto_stub,
    .random_symmetric_key = crypto_rand,
    .verify_signature = crypto_verify,
    .asymmetric_encrypt = crypto_asym,
    .symmetric_encrypt = crypto_se,
    .symmetric_decrypt = crypto_sd,
    .derive_server_keys = crypto_derive
};

static uint8_t trusted_root[64];

aether_status_t aether_platform_init(
    void *ctx, size_t sz, uint32_t slot,
    aether_client_platform_t *p) {
    (void)ctx; (void)sz; (void)slot;
    if (p == NULL) return AETHER_ERR_ARGUMENT;
    memset(p, 0, sizeof(*p));
    p->ctx = (void*)1;
    p->now_ms = fake_now_ms;

    p->transport = (aether_transport_vtable_t){
        (void*)1, fake_open, fake_close, fake_send,
        fake_resolve, fake_poll, fake_recv, fake_consume,
        NULL
    };

    p->flash = (aether_flash_vtable_t){
        (void*)1, fake_flash_load, fake_flash_save
    };
    p->crypto = &vtable;
    p->trusted_sign_keys = trusted_root;
    p->trusted_sign_key_count = 1u;
    p->pow = (aether_pow_vtable_t){ (void*)1, fake_pow };
    return AETHER_OK;
}

void aether_platform_deinit(void *ctx) { (void)ctx; }

int main(void) {
    aether_uuid_t uid = { 0xAAu, 0xBBu };

    for (unsigned i = 0; i < 64u; ++i) trusted_root[i] = (uint8_t)i;

    assert(aether_init(&c, uid) == AETHER_OK);

    memset(&id, 0, sizeof(id));
    id.authority_cert[0] = 0x01u;
    id.authority_cert[1] = 0x01u;
    for (unsigned i = 0; i < 64u; ++i) id.authority_cert[6 + i] = (uint8_t)i;
    id.cert[0] = 0x01u;
    for (unsigned i = 0; i < 8u; ++i) {
        id.cert[1 + i] = (uint8_t)(uid.msb >> (56u - 8u*i));
        id.cert[9 + i] = (uint8_t)(uid.lsb >> (56u - 8u*i));
    }

    c.app.core.uid = uid;


    assert(aether_set_components(&c, comps, 1u) == AETHER_OK);

    /* Start the client; then re-install the test UID (aether_start()
     * re-initializes the core unless caller credentials were set). */
    assert(aether_start(&c) == AETHER_OK);
    c.app.core.uid = uid;


    aether_status_t s = aether_lan_init(
        &lan, &c, &id, peers, 2u, rx, sizeof(rx), tx, sizeof(tx));
    assert(s == AETHER_OK);


    assert(aether_register_component(&c, &lan, aether_lan_work) == AETHER_OK);

    /* The LAN component is now registered under aether_work(). */




    /*
     * Observable proof: aether_lan_work() emits the first beacon on its
     * initial tick, moving next_beacon_ms from 0 to a nonzero deadline.
     * If aether_work() does not dispatch the registered LAN component,
     * next_beacon_ms stays 0 and the assertion fails.
     */
    assert(lan.next_beacon_ms == 0u);
    (void)aether_work(&c);
    assert(lan.next_beacon_ms != 0u);


    aether_lan_deinit(&lan);

    printf("LAN component: OK\n");
    return 0;
}
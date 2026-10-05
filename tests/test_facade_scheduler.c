
/*
 * Facade scheduler / semantic transport integration tests.
 *
 * Verifies that aether_work() owns the batch boundary:
 *   - aether_send_message() only queues commands,
 *   - aether_work() performs at most one flush() and one tick() per pass,
 *   - component callbacks participate in the same batch,
 *   - a command queued from on_flush_done stays BUILDING until the next
 *     aether_work() call (no second flush in the same pass),
 *   - callback/RX guard blocks send_message.
 *
 * The test uses a small fake semantic transport so it can directly
 * count send/flush/tick invocations. It does NOT try to fake protocol
 * core readiness: the real lifecycle (aether_init + aether_start) is
 * driven, and the core stays in NO_IDENTITY as expected for an
 * unprovisioned client.
 */

#include "aether.h"
#include "aether_platform.h"
#include "aether_transport.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


/* ---- fake platform ---- */

static uint64_t fake_now_ms(void *ctx) { (void)ctx; return 1000u; }

static aether_status_t fake_resolve(
    void *c, const char *h, aether_address_t *a) {
    (void)c; (void)h;
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

static const aether_crypto_vtable_t fake_vtable = {
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
    p->ctx = (void *)1;
    p->now_ms = fake_now_ms;
    p->transport_io = (aether_transport_io_vtable_t){
        (void *)1, fake_open, fake_close, fake_send,
        fake_resolve, fake_poll, fake_recv, fake_consume,
        NULL
    };
    p->flash = (aether_flash_vtable_t){
        (void *)1, fake_flash_load, fake_flash_save
    };
    p->crypto = &fake_vtable;
    p->trusted_sign_keys = trusted_root;
    p->trusted_sign_key_count = 1u;
    p->pow = (aether_pow_vtable_t){ (void *)1, fake_pow };
    return AETHER_OK;
}

void aether_platform_deinit(void *ctx) { (void)ctx; }


/* ---- fake semantic transport ---- */

typedef struct {
    aether_transport_status_t status;
    aether_transport_listener_t listener;

    unsigned send_calls;
    unsigned flush_calls;
    unsigned tick_calls;
    unsigned reset_calls;
} fake_semantic_transport_t;

static fake_semantic_transport_t fake_transport;
static bool listener_resend;


static aether_transport_status_t fake_status(void *ctx) {
    fake_semantic_transport_t *f = ctx;
    return f->status;
}

static void fake_set_listener(
    void *ctx, const aether_transport_listener_t *l) {
    fake_semantic_transport_t *f = ctx;
    if (l == NULL) {
        memset(&f->listener, 0, sizeof(f->listener));
        return;
    }
    f->listener = *l;
}

static aether_status_t fake_send_message(
    void *ctx, aether_uuid_t uid,
    const uint8_t *data, size_t length) {
    (void)uid; (void)data; (void)length;
    fake_semantic_transport_t *f = ctx;

    if (f->status == AETHER_TRANSPORT_FLUSHING) {
        return AETHER_ERR_BUSY;
    }

    f->send_calls += 1u;
    f->status = AETHER_TRANSPORT_BUILDING;
    return AETHER_OK;
}

static aether_status_t fake_flush(void *ctx) {
    fake_semantic_transport_t *f = ctx;

    if (f->status == AETHER_TRANSPORT_FLUSHING) {
        return AETHER_ERR_BUSY;
    }
    if (f->status == AETHER_TRANSPORT_IDLE) {
        return AETHER_OK;
    }

    f->flush_calls += 1u;
    f->status = AETHER_TRANSPORT_FLUSHING;
    return AETHER_OK;
}

static aether_status_t fake_tick(void *ctx) {
    fake_semantic_transport_t *f = ctx;
    f->tick_calls += 1u;

    if (f->status == AETHER_TRANSPORT_FLUSHING) {
        f->status = AETHER_TRANSPORT_IDLE;

        if (f->listener.on_flush_done != NULL) {
            f->listener.on_flush_done(
                f->listener.ctx,
                AETHER_OK);
        }
    }

    return AETHER_OK;
}

static void fake_reset(void *ctx) {
    fake_semantic_transport_t *f = ctx;
    f->reset_calls += 1u;
    f->status = AETHER_TRANSPORT_IDLE;
}

static const aether_transport_vtable_t FAKE_VTABLE = {
    fake_status,
    fake_flush,
    fake_reset,
    fake_set_listener,
    fake_tick,
    fake_send_message
};


/* ---- fixture ---- */

static aether_t client;

static void on_flush_done(
    void *ctx, aether_status_t status) {
    (void)ctx;
    (void)status;

    if (listener_resend) {
        listener_resend = false;
        (void)aether_send_message(
            &client,
            (aether_uuid_t){9u, 9u},
            (const uint8_t *)"x",
            1u);
    }
}

static uint8_t payload[4] = { 1u, 2u, 3u, 4u };

static unsigned component_send_calls;
static unsigned component_flush_calls_seen;

static aether_status_t component_work(
    aether_t *c, void *ctx) {
    (void)ctx;

    component_send_calls += 1u;
    component_flush_calls_seen = fake_transport.flush_calls;

    (void)aether_send_message(
        c,
        (aether_uuid_t){7u, 8u},
        payload,
        2u);

    return AETHER_OK;
}

static aether_component_t components[1];

static void setup(size_t component_count) {

    listener_resend = false;
    component_send_calls = 0u;
    component_flush_calls_seen = 0u;

    assert(aether_init(&client, (aether_uuid_t){0u, 0u}) == AETHER_OK);

    if (component_count != 0u) {
        assert(aether_set_components(
            &client, components, component_count) == AETHER_OK);
    }

    assert(aether_start(&client) == AETHER_OK);

    /*
     * Replace the real server transport with a fake implementation so
     * the test can count scheduler invocations directly. The real
     * lifecycle has already configured the core, so this is a valid
     * override of the semantic boundary only.
     */
    memset(&fake_transport, 0, sizeof(fake_transport));
    client.app.transport.ctx = &fake_transport;
    client.app.transport.vtable = &FAKE_VTABLE;

    aether_transport_listener_t listener = { NULL, on_flush_done };
    aether_transport_set_listener(
        &client.app.transport,
        &listener);
}


static void test_case_a_send_only_queues(void) {

    setup(0u);

    assert(aether_send_message(
        &client,
        (aether_uuid_t){1u, 2u},
        payload, 2u) == AETHER_OK);

    assert(fake_transport.send_calls == 1u);
    assert(fake_transport.flush_calls == 0u);
    assert(fake_transport.tick_calls == 0u);
    assert(fake_transport.status == AETHER_TRANSPORT_BUILDING);

    assert(aether_send_message(
        &client,
        (aether_uuid_t){3u, 4u},
        payload, 2u) == AETHER_OK);

    assert(fake_transport.send_calls == 2u);
    assert(fake_transport.flush_calls == 0u);
    assert(fake_transport.tick_calls == 0u);
    assert(fake_transport.status == AETHER_TRANSPORT_BUILDING);

    aether_stop(&client);
}


static void test_case_b_one_flush_per_work(void) {

    setup(0u);

    assert(aether_send_message(
        &client,
        (aether_uuid_t){1u, 2u},
        payload, 2u) == AETHER_OK);
    assert(aether_send_message(
        &client,
        (aether_uuid_t){3u, 4u},
        payload, 2u) == AETHER_OK);

    assert(fake_transport.flush_calls == 0u);
    assert(fake_transport.tick_calls == 0u);

    (void)aether_work(&client);

    assert(fake_transport.flush_calls == 1u);
    assert(fake_transport.tick_calls == 1u);
    assert(fake_transport.status == AETHER_TRANSPORT_IDLE);

    /* Second work without new commands must not flush again. */
    (void)aether_work(&client);
    assert(fake_transport.flush_calls == 1u);
    assert(fake_transport.tick_calls == 2u);

    aether_stop(&client);
}


static void test_case_c_component_joins_batch(void) {

    setup(1u);

    assert(aether_register_component(
        &client,
        &components[0],
        component_work) == AETHER_OK);

    assert(aether_send_message(
        &client,
        (aether_uuid_t){1u, 2u},
        payload, 2u) == AETHER_OK);

    assert(fake_transport.flush_calls == 0u);

    (void)aether_work(&client);

    /* Component ran before flush and saw zero flush calls. */
    assert(component_send_calls == 1u);
    assert(component_flush_calls_seen == 0u);

    /* The single batch covered both commands. */
    assert(fake_transport.send_calls == 2u);
    assert(fake_transport.flush_calls == 1u);
    assert(fake_transport.tick_calls == 1u);
    assert(fake_transport.status == AETHER_TRANSPORT_IDLE);

    aether_stop(&client);
}


static void test_case_d_listener_resend_stays_building(void) {

    setup(0u);

    assert(aether_send_message(
        &client,
        (aether_uuid_t){1u, 2u},
        payload, 2u) == AETHER_OK);

    listener_resend = true;

    (void)aether_work(&client);

    assert(fake_transport.flush_calls == 1u);
    assert(fake_transport.tick_calls == 1u);
    assert(fake_transport.send_calls == 2u);

    /*
     * New command was queued from the listener. It must stay BUILDING
     * until the next aether_work() call.
     */
    assert(fake_transport.status == AETHER_TRANSPORT_BUILDING);

    (void)aether_work(&client);

    assert(fake_transport.flush_calls == 2u);
    assert(fake_transport.tick_calls == 2u);
    assert(fake_transport.status == AETHER_TRANSPORT_IDLE);

    aether_stop(&client);
}


static void test_case_e_flushing_blocks_new_sends(void) {

    setup(0u);

    assert(aether_send_message(
        &client,
        (aether_uuid_t){1u, 2u},
        payload, 2u) == AETHER_OK);

    assert(aether_transport_flush(&client.app.transport) == AETHER_OK);
    assert(fake_transport.status == AETHER_TRANSPORT_FLUSHING);

    /* While FLUSHING, new commands must be rejected. */
    assert(aether_send_message(
        &client,
        (aether_uuid_t){3u, 4u},
        payload, 2u) == AETHER_ERR_BUSY);
    assert(fake_transport.send_calls == 1u);

    (void)aether_work(&client);

    /* No second flush, exactly one tick. */
    assert(fake_transport.flush_calls == 1u);
    assert(fake_transport.tick_calls == 1u);
    assert(fake_transport.status == AETHER_TRANSPORT_IDLE);

    aether_stop(&client);
}



static void test_case_f_send_from_callback_allowed(void) {

    /*
     * The semantic transport does not care where send_message() is
     * called from; only the lifetime of the specific borrowed memory
     * range matters. A callback_active flag alone must not block it.
     */
    setup(0u);

    client.app.core.callback_active = true;

    assert(aether_send_message(
        &client,
        (aether_uuid_t){1u, 2u},
        payload, 2u) == AETHER_OK);

    assert(fake_transport.send_calls == 1u);
    assert(fake_transport.status == AETHER_TRANSPORT_BUILDING);

    client.app.core.callback_active = false;

    (void)aether_work(&client);

    assert(fake_transport.flush_calls == 1u);
    assert(fake_transport.tick_calls == 1u);
    assert(fake_transport.status == AETHER_TRANSPORT_IDLE);

    aether_stop(&client);
}



int main(void) {

    test_case_a_send_only_queues();
    test_case_b_one_flush_per_work();
    test_case_c_component_joins_batch();
    test_case_d_listener_resend_stays_building();
    test_case_e_flushing_blocks_new_sends();
    test_case_f_send_from_callback_allowed();

    printf("facade scheduler: OK\n");
    return 0;
}
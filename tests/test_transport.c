
/*
 * Semantic transport (server implementation) lifecycle tests.
 *
 * Covers batch state machine, borrowed-input lifetime, reset semantics
 * and server-readiness gating.
 *
 * The server transport has no command queue: send_message() goes
 * directly through aether_client_app_send_with_id(), which expects a
 * fully initialized core (crypto, transport_io). We therefore drive the
 * real lifecycle (aether_init + aether_start), then mark the core as
 * READY to satisfy the transport's server-readiness gate. The
 * transport_io and crypto backends are real but no-op; no packets leave
 * the test.
 */

#include "aether.h"
#include "aether_transport.h"
#include "aether_transport_server.h"
#include "aether_client_app.h"
#include "aether_internal.h"

#include "aether_test_crypto_hydrogen.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>


static unsigned flush_done_calls;
static aether_status_t last_flush_status;
static unsigned io_send_calls;


static void on_flush_done(
    void *ctx,
    aether_status_t status) {

    (void)ctx;

    flush_done_calls += 1u;
    last_flush_status = status;
}


static void reset_fixture(void) {

    flush_done_calls = 0u;
    last_flush_status = AETHER_OK;
    io_send_calls = 0u;
}


/* ---- platform for the low-level core ---- */

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
    (void)c; (void)ch; (void)d; (void)n;
    io_send_calls += 1u;
    return AETHER_OK;
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
    p->crypto = &AETHER_TEST_HYDROGEN_CRYPTO;
    p->crypto_ctx = NULL;
    p->trusted_sign_keys = trusted_root;
    p->trusted_sign_key_count = 1u;
    p->pow = (aether_pow_vtable_t){ (void *)1, fake_pow };
    return AETHER_OK;
}

void aether_platform_deinit(void *ctx) { (void)ctx; }


/* ---- fixture ---- */

static aether_t client;


static void setup_ready(void) {

    reset_fixture();

    assert(aether_init(&client, (aether_uuid_t){0u, 0u}) == AETHER_OK);
    assert(aether_start(&client) == AETHER_OK);

    /*
     * The transport's server-readiness gate expects AETHER_STATE_READY.
     * We synthesize it here to isolate the transport from registration.
     * The transport_io, crypto and config are already configured by the
     * real lifecycle above.
     */
    client.app.core.state = AETHER_STATE_READY;

    aether_transport_listener_t listener = { NULL, on_flush_done };
    aether_transport_set_listener(
        &client.app.transport,
        &listener);
}


static void teardown(void) {

    aether_stop(&client);
}


static void test_initial_state(void) {

    setup_ready();

    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_IDLE);

    teardown();
}


static void test_first_command_to_building(void) {

    setup_ready();

    uint8_t payload[4] = { 1u, 2u, 3u, 4u };
    aether_uuid_t uid = { 1u, 2u };

    assert(
        aether_transport_send_message(
            &client.app.transport, uid, payload, sizeof(payload)) ==
        AETHER_OK);

    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_BUILDING);

    teardown();
}


static void test_multiple_commands_stay_building(void) {

    setup_ready();

    uint8_t payload[2] = { 0xAAu, 0xBBu };
    aether_uuid_t uid = { 1u, 2u };

    for (unsigned i = 0u; i < 5u; ++i) {
        assert(
            aether_transport_send_message(
                &client.app.transport,
                uid,
                payload,
                sizeof(payload)) ==
            AETHER_OK);
    }

    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_BUILDING);

    teardown();
}


static void test_flush_from_idle_is_noop(void) {

    setup_ready();

    assert(
        aether_transport_flush(&client.app.transport) ==
        AETHER_OK);

    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_IDLE);

    assert(flush_done_calls == 0u);

    teardown();
}


static void test_flush_transitions_to_flushing(void) {

    setup_ready();

    uint8_t payload[2] = { 0xAAu, 0xBBu };
    aether_uuid_t uid = { 1u, 2u };

    aether_transport_send_message(
        &client.app.transport, uid, payload, sizeof(payload));

    assert(
        aether_transport_flush(&client.app.transport) ==
        AETHER_OK);

    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_FLUSHING);

    teardown();
}


static void test_tick_completes_batch(void) {

    setup_ready();

    uint8_t payload[2] = { 0xAAu, 0xBBu };
    aether_uuid_t uid = { 1u, 2u };

    aether_transport_send_message(
        &client.app.transport, uid, payload, sizeof(payload));
    aether_transport_flush(&client.app.transport);

    assert(
        aether_transport_tick(&client.app.transport) ==
        AETHER_OK);

    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_IDLE);

    assert(flush_done_calls == 1u);
    assert(last_flush_status == AETHER_OK);

    teardown();
}


static void test_send_message_busy_while_flushing(void) {

    setup_ready();

    uint8_t payload[2] = { 0xAAu, 0xBBu };
    aether_uuid_t uid = { 1u, 2u };

    aether_transport_send_message(
        &client.app.transport, uid, payload, sizeof(payload));
    aether_transport_flush(&client.app.transport);

    assert(
        aether_transport_send_message(
            &client.app.transport,
            uid,
            payload,
            sizeof(payload)) ==
        AETHER_ERR_BUSY);

    assert(
        aether_transport_flush(&client.app.transport) ==
        AETHER_ERR_BUSY);

    teardown();
}


static void test_reset_from_building(void) {

    setup_ready();

    uint8_t payload[2] = { 0xAAu, 0xBBu };
    aether_uuid_t uid = { 1u, 2u };

    aether_transport_send_message(
        &client.app.transport, uid, payload, sizeof(payload));
    aether_transport_reset(&client.app.transport);

    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_IDLE);
    assert(flush_done_calls == 0u);

    teardown();
}


static void test_reset_from_flushing(void) {

    setup_ready();

    uint8_t payload[2] = { 0xAAu, 0xBBu };
    aether_uuid_t uid = { 1u, 2u };

    aether_transport_send_message(
        &client.app.transport, uid, payload, sizeof(payload));
    aether_transport_flush(&client.app.transport);
    aether_transport_reset(&client.app.transport);

    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_IDLE);
    assert(flush_done_calls == 0u);

    aether_transport_tick(&client.app.transport);
    assert(flush_done_calls == 0u);

    aether_transport_send_message(
        &client.app.transport, uid, payload, sizeof(payload));

    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_BUILDING);

    teardown();
}


static void test_reset_from_idle_is_harmless(void) {

    setup_ready();

    aether_transport_reset(&client.app.transport);
    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_IDLE);

    teardown();
}


static void test_server_readiness_required(void) {

    reset_fixture();

    assert(aether_init(&client, (aether_uuid_t){0u, 0u}) == AETHER_OK);
    assert(aether_start(&client) == AETHER_OK);
    /* Do NOT mark READY: the core stays in NO_IDENTITY. */

    uint8_t payload[2] = { 0xAAu, 0xBBu };
    aether_uuid_t uid = { 1u, 2u };

    assert(
        aether_transport_send_message(
            &client.app.transport,
            uid,
            payload,
            sizeof(payload)) ==
        AETHER_ERR_STATE);

    assert(
        aether_transport_status(&client.app.transport) ==
        AETHER_TRANSPORT_IDLE);
    assert(flush_done_calls == 0u);

    teardown();
}


int main(void) {

    test_initial_state();
    test_first_command_to_building();
    test_multiple_commands_stay_building();
    test_flush_from_idle_is_noop();
    test_flush_transitions_to_flushing();
    test_tick_completes_batch();
    test_send_message_busy_while_flushing();
    test_reset_from_building();
    test_reset_from_flushing();
    test_reset_from_idle_is_harmless();
    test_server_readiness_required();

    return 0;
}

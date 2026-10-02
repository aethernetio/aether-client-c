
/*
 * Message delivery regression tests.
 *
 * Covers the transport-agnostic producer API, caller-owned slot storage,
 * provider-chain semantics, owner validation, duplicate-provider policy and
 * callback-safe enqueue through the real aether_work() component pass.
 */

#include "aether_delivery.h"
#include "aether.h"
#include "aether_platform.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>


/* ---------- fake provider ---------- */


typedef struct {
    unsigned calls;
    aether_uuid_t last_recipient;
    uint8_t last_payload[64];
    size_t last_length;
    aether_delivery_attempt_t attempt;
} fake_provider_t;


static aether_delivery_attempt_t fake_provider_try(
    void *context,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length) {

    fake_provider_t *fp =
        context;

    assert(fp != NULL);
    assert(length <= sizeof(fp->last_payload));

    fp->calls += 1u;
    fp->last_recipient = recipient;
    fp->last_length = length;

    if (length != 0u) {
        memcpy(
            fp->last_payload,
            payload,
            length);
    }

    return fp->attempt;
}


static aether_t g_fake_client;


static void reset_fake_client(void) {
    memset(
        &g_fake_client,
        0,
        sizeof(g_fake_client));
}


/* ---------- provider semantics ---------- */


static void test_not_applicable_then_fallback(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[2];
    uint8_t payload_storage[2 * 32];

    fake_provider_t provider_a = { 0 };
    provider_a.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_NOT_APPLICABLE, AETHER_OK
    };

    fake_provider_t provider_b = { 0 };
    provider_b.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };

    aether_delivery_provider_t providers[2];

    aether_delivery_init(
        &delivery, &g_fake_client, slots, 2u,
        payload_storage, 32u);

    assert(aether_delivery_set_providers(
        &delivery, providers, 2u) == AETHER_OK);

    assert(aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){ &provider_a, fake_provider_try }) == AETHER_OK);

    assert(aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){ &provider_b, fake_provider_try }) == AETHER_OK);

    const uint8_t payload[] = { 1u, 2u, 3u };

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 0x1111u, 0x2222u },
        payload, sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(&g_fake_client, &delivery) == AETHER_OK);

    assert(provider_a.calls == 1u);
    assert(provider_b.calls == 1u);
    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static void test_retry_later_uses_fallback(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[2];
    uint8_t payload_storage[2 * 32];

    fake_provider_t provider_a = { 0 };
    provider_a.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_RETRY_LATER, AETHER_OK
    };

    fake_provider_t provider_b = { 0 };
    provider_b.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };

    aether_delivery_provider_t providers[2];

    aether_delivery_init(&delivery, &g_fake_client, slots, 2u,
                         payload_storage, 32u);
    aether_delivery_set_providers(&delivery, providers, 2u);

    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider_a, fake_provider_try });
    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider_b, fake_provider_try });

    const uint8_t payload[] = { 9u };

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 1u, 2u },
        payload, sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(&g_fake_client, &delivery) == AETHER_OK);

    assert(provider_a.calls == 1u);
    assert(provider_b.calls == 1u);
    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static void test_accepted_stops_chain(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[32];

    fake_provider_t provider_a = { 0 };
    provider_a.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };

    fake_provider_t provider_b = { 0 };
    provider_b.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };

    aether_delivery_provider_t providers[2];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 32u);
    aether_delivery_set_providers(&delivery, providers, 2u);

    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider_a, fake_provider_try });
    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider_b, fake_provider_try });

    const uint8_t payload[] = { 4u, 5u };

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 1u, 2u },
        payload, sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(&g_fake_client, &delivery) == AETHER_OK);

    assert(provider_a.calls == 1u);
    assert(provider_b.calls == 0u);
    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static void test_route_lost_uses_fallback(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[32];

    fake_provider_t provider_a = { 0 };
    provider_a.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ROUTE_LOST, AETHER_OK
    };

    fake_provider_t provider_b = { 0 };
    provider_b.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };

    aether_delivery_provider_t providers[2];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 32u);
    aether_delivery_set_providers(&delivery, providers, 2u);

    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider_a, fake_provider_try });
    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider_b, fake_provider_try });

    const uint8_t payload[] = { 7u };

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 1u, 2u },
        payload, sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(&g_fake_client, &delivery) == AETHER_OK);

    assert(provider_a.calls == 1u);
    assert(provider_b.calls == 1u);
    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static unsigned g_error_calls;
static aether_uuid_t g_error_recipient;
static aether_status_t g_error_status;


static void error_cb(
    void *ctx,
    aether_uuid_t recipient,
    aether_status_t status) {

    (void)ctx;
    g_error_calls += 1u;
    g_error_recipient = recipient;
    g_error_status = status;
}


static void test_error_no_fallback_exact_status(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[32];

    fake_provider_t provider_a = { 0 };
    provider_a.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ERROR, AETHER_ERR_CRYPTO
    };

    fake_provider_t provider_b = { 0 };
    provider_b.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };

    aether_delivery_provider_t providers[2];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 32u);
    aether_delivery_set_providers(&delivery, providers, 2u);

    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider_a, fake_provider_try });
    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider_b, fake_provider_try });

    g_error_calls = 0u;
    g_error_status = AETHER_OK;

    aether_delivery_set_error_callback(&delivery, error_cb, NULL);

    aether_uuid_t recipient = { 0xAAAu, 0xBBBu };
    const uint8_t payload[] = { 1u };

    assert(aether_delivery_send(&delivery, recipient,
        payload, sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(&g_fake_client, &delivery) == AETHER_OK);

    assert(provider_a.calls == 1u);
    assert(provider_b.calls == 0u);
    assert(g_error_calls == 1u);
    assert(g_error_recipient.msb == recipient.msb);
    assert(g_error_recipient.lsb == recipient.lsb);
    assert(g_error_status == AETHER_ERR_CRYPTO);
    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static void test_unknown_result_is_conservative_error(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[32];

    fake_provider_t provider_a = { 0 };
    provider_a.attempt = (aether_delivery_attempt_t){
        (aether_delivery_result_t)999, AETHER_ERR_STATE
    };

    fake_provider_t provider_b = { 0 };
    provider_b.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };

    aether_delivery_provider_t providers[2];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 32u);
    aether_delivery_set_providers(&delivery, providers, 2u);

    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider_a, fake_provider_try });
    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider_b, fake_provider_try });

    g_error_calls = 0u;
    g_error_status = AETHER_OK;
    aether_delivery_set_error_callback(&delivery, error_cb, NULL);

    const uint8_t payload[] = { 1u };

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 1u, 1u },
        payload, sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(&g_fake_client, &delivery) == AETHER_OK);

    assert(provider_a.calls == 1u);
    assert(provider_b.calls == 0u);
    assert(g_error_calls == 1u);
    assert(g_error_status == AETHER_ERR_STATE);
    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


/* ---------- storage ---------- */


static void test_storage_copy_and_mutation(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[16];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 16u);

    uint8_t source[8] = {
        0x00u, 0x01u, 0x02u, 0x7Fu,
        0x80u, 0xFFu, 0x55u, 0xAAu
    };

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 1u, 2u },
        source, sizeof(source)) == AETHER_OK);

    memset(source, 0xEEu, sizeof(source));

    assert(slots[0].state == AETHER_DELIVERY_SLOT_READY);
    assert(slots[0].payload_length == sizeof(source));

    static const uint8_t expected[] = {
        0x00u, 0x01u, 0x02u, 0x7Fu,
        0x80u, 0xFFu, 0x55u, 0xAAu
    };

    assert(memcmp(delivery.payload_storage,
        expected, sizeof(expected)) == 0);
}


static void test_queue_full_does_not_overwrite(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[8];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 8u);

    const uint8_t first[] = { 0xA1u, 0xA2u };

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 1u, 1u },
        first, sizeof(first)) == AETHER_OK);

    const uint8_t second[] = { 0xB1u, 0xB2u, 0xB3u };

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 2u, 2u },
        second, sizeof(second)) == AETHER_ERR_OVERFLOW);

    assert(slots[0].recipient.msb == 1u);
    assert(slots[0].payload_length == sizeof(first));
    assert(payload_storage[0] == 0xA1u);
    assert(payload_storage[1] == 0xA2u);
}


static void test_oversized_payload_rejected(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[4];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 4u);

    const uint8_t oversized[5] = { 1u, 2u, 3u, 4u, 5u };

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 1u, 2u },
        oversized, sizeof(oversized)) == AETHER_ERR_OVERFLOW);

    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static void test_zero_length_payload(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[8];

    fake_provider_t provider = { 0 };
    provider.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };

    aether_delivery_provider_t providers[1];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 8u);
    aether_delivery_set_providers(&delivery, providers, 1u);

    aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider, fake_provider_try });

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 7u, 8u },
        NULL, 0u) == AETHER_OK);

    assert(slots[0].state == AETHER_DELIVERY_SLOT_READY);
    assert(slots[0].payload_length == 0u);

    assert(aether_delivery_work(&g_fake_client, &delivery) == AETHER_OK);

    assert(provider.calls == 1u);
    assert(provider.last_length == 0u);
    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static void test_multiple_recipients_preserved(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[3];
    uint8_t payload_storage[3 * 8];

    aether_delivery_init(&delivery, &g_fake_client, slots, 3u,
                         payload_storage, 8u);

    const uint8_t p1[] = { 1u };
    const uint8_t p2[] = { 2u };
    const uint8_t p3[] = { 3u };

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 0x1111u, 0xAAAAu },
        p1, 1u) == AETHER_OK);
    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 0x2222u, 0xBBBBu },
        p2, 1u) == AETHER_OK);
    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 0x3333u, 0xCCCCu },
        p3, 1u) == AETHER_OK);

    assert(slots[0].recipient.msb == 0x1111u);
    assert(slots[0].recipient.lsb == 0xAAAAu);
    assert(slots[0].payload_length == 1u);
    assert(delivery.payload_storage[0 * 8] == 1u);

    assert(slots[1].recipient.msb == 0x2222u);
    assert(slots[1].recipient.lsb == 0xBBBBu);
    assert(slots[1].payload_length == 1u);
    assert(delivery.payload_storage[1 * 8] == 2u);

    assert(slots[2].recipient.msb == 0x3333u);
    assert(slots[2].recipient.lsb == 0xCCCCu);
    assert(slots[2].payload_length == 1u);
    assert(delivery.payload_storage[2 * 8] == 3u);
}


/* ---------- scheduler / lifecycle ---------- */


typedef struct {
    unsigned n;
} rr_state_t;


static aether_delivery_attempt_t rr_try(
    void *ctx,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length) {

    (void)recipient;
    (void)payload;
    (void)length;

    rr_state_t *state = ctx;
    state->n += 1u;

    if (state->n == 1u) {
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_RETRY_LATER, AETHER_OK
        };
    }

    return (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };
}


static void test_blocked_slot_does_not_starve(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[2];
    uint8_t payload_storage[2 * 8];

    rr_state_t rr = { 0 };

    aether_delivery_provider_t providers[1];

    aether_delivery_init(&delivery, &g_fake_client, slots, 2u,
                         payload_storage, 8u);
    aether_delivery_set_providers(&delivery, providers, 1u);

    assert(aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &rr, rr_try }) == AETHER_OK);

    const uint8_t payload[] = { 1u };

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 1u, 1u },
        payload, sizeof(payload)) == AETHER_OK);
    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 2u, 2u },
        payload, sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(&g_fake_client, &delivery) == AETHER_OK);

    assert(slots[0].state == AETHER_DELIVERY_SLOT_READY);
    assert(slots[1].state == AETHER_DELIVERY_SLOT_EMPTY);

    assert(aether_delivery_work(&g_fake_client, &delivery) == AETHER_OK);

    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static void test_owner_validation(void) {

    reset_fake_client();

    aether_t other_client;
    memset(&other_client, 0, sizeof(other_client));

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[8];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 8u);

    assert(aether_delivery_work(&g_fake_client, &delivery) == AETHER_OK);
    assert(aether_delivery_work(&other_client, &delivery) == AETHER_ERR_ARGUMENT);
    assert(aether_delivery_work(NULL, &delivery) == AETHER_ERR_ARGUMENT);
    assert(aether_delivery_work(&g_fake_client, NULL) == AETHER_ERR_ARGUMENT);
}


/* ---------- provider registry API ---------- */


static void test_set_providers_argument_validation(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[8];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 8u);

    assert(aether_delivery_set_providers(
        &delivery, NULL, 0u) == AETHER_OK);

    assert(aether_delivery_set_providers(
        &delivery, NULL, 3u) == AETHER_ERR_ARGUMENT);

    assert(aether_delivery_set_providers(
        NULL, NULL, 0u) == AETHER_ERR_ARGUMENT);

    aether_delivery_provider_t providers[2];
    assert(aether_delivery_set_providers(
        &delivery, providers, 2u) == AETHER_OK);
}


static void test_duplicate_provider_rejected(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[8];

    fake_provider_t provider = { 0 };
    provider.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };

    aether_delivery_provider_t providers[3];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 8u);

    assert(aether_delivery_set_providers(
        &delivery, providers, 3u) == AETHER_OK);

    assert(aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider, fake_provider_try }) == AETHER_OK);

    assert(aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &provider, fake_provider_try }) == AETHER_ERR_STATE);

    fake_provider_t other_ctx = { 0 };
    other_ctx.attempt = (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };
    assert(aether_delivery_add_provider(&delivery,
        (aether_delivery_provider_t){ &other_ctx, fake_provider_try }) == AETHER_OK);
}


/* ---------- integration with aether_work() ---------- */


static aether_delivery_t g_int_delivery;
static aether_delivery_slot_t g_int_slots[2];
static uint8_t g_int_payloads[2 * 32];
static aether_delivery_provider_t g_int_user_providers[1];

static aether_component_t g_int_components[2];
static unsigned g_int_provider_calls;
static uint8_t g_int_last_payload[16];
static size_t g_int_last_length;
static aether_uuid_t g_int_last_recipient;


static uint64_t int_fake_now_ms(void *ctx) {
    (void)ctx;
    return 1000u;
}


/*
 * Minimal fake platform: enough for aether_init / aether_start / aether_work
 * to succeed on a stopped identity. No network, no crypto operation is ever
 * reached, but every callback required by ensure_required_callbacks() is
 * provided so the core state machine can transition to NO_IDENTITY.
 */


static uint64_t int_fake_resolve_ok;

static aether_status_t int_fake_resolve(
    void *ctx,
    const char *hostname,
    aether_address_t *address) {

    (void)ctx;
    (void)hostname;

    memset(address, 0, sizeof(*address));
    address->kind = AETHER_ADDR_IPV4;
    address->length = 4u;
    address->bytes[0] = 127;
    address->bytes[3] = 1;
    (void)int_fake_resolve_ok;
    return AETHER_OK;
}


static aether_status_t int_fake_transport_open(
    void *ctx, aether_channel_t channel,
    const aether_endpoint_t *endpoint) {
    (void)ctx; (void)channel; (void)endpoint;
    return AETHER_OK;
}


static void int_fake_transport_close(
    void *ctx, aether_channel_t channel) {
    (void)ctx; (void)channel;
}


static aether_status_t int_fake_transport_send(
    void *ctx, aether_channel_t channel,
    const uint8_t *data, size_t length) {
    (void)ctx; (void)channel; (void)data; (void)length;
    return AETHER_OK;
}


static aether_status_t int_fake_flash_load(
    void *ctx, uint8_t *dst, size_t capacity, size_t *length) {
    (void)ctx; (void)dst; (void)capacity; (void)length;
    return AETHER_ERR_STORAGE;
}


static aether_status_t int_fake_flash_save(
    void *ctx, const uint8_t *src, size_t length) {
    (void)ctx; (void)src; (void)length;
    return AETHER_OK;
}


static aether_status_t int_fake_poll(void *ctx) {
    (void)ctx;
    return AETHER_OK;
}


static aether_status_t int_fake_receive(
    void *ctx, aether_channel_t channel,
    aether_bytes_view_t *frame) {
    (void)ctx; (void)channel;
    if (frame != NULL) { frame->data = NULL; frame->length = 0u; }
    return AETHER_OK;
}


static void int_fake_consume(
    void *ctx, aether_channel_t channel,
    const uint8_t *data, size_t length) {
    (void)ctx; (void)channel; (void)data; (void)length;
}


static aether_status_t int_fake_crypto_stub(void *ctx) { (void)ctx; return AETHER_OK; }

static aether_status_t int_fake_random_symmetric_key(
    void *ctx, uint8_t *out, size_t len) {
    (void)ctx; memset(out, 0xA5, len); return AETHER_OK;
}

static aether_status_t int_fake_verify_signature(
    void *ctx, const uint8_t *pk, size_t pkl,
    const uint8_t *sig, size_t sigl,
    const uint8_t *data, size_t datal) {
    (void)ctx; (void)pk; (void)pkl; (void)sig; (void)sigl;
    (void)data; (void)datal;
    return AETHER_OK;
}

static aether_status_t int_fake_asym_enc(
    void *ctx, const uint8_t *pk, size_t pkl,
    const uint8_t *plain, size_t plainl,
    uint8_t *out, size_t outcap, size_t *outl) {
    (void)ctx; (void)pk; (void)pkl;
    if (outcap < plainl) return AETHER_ERR_OVERFLOW;
    memcpy(out, plain, plainl);
    *outl = plainl;
    return AETHER_OK;
}

static aether_status_t int_fake_sym_enc(
    void *ctx, const uint8_t *key, size_t keyl,
    const uint8_t *plain, size_t plainl,
    uint8_t *out, size_t outcap, size_t *outl) {
    (void)ctx; (void)key; (void)keyl;
    if (outcap < plainl) return AETHER_ERR_OVERFLOW;
    memcpy(out, plain, plainl);
    *outl = plainl;
    return AETHER_OK;
}

static aether_status_t int_fake_sym_dec(
    void *ctx, const uint8_t *key, size_t keyl,
    const uint8_t *cipher, size_t cipherl,
    uint8_t *out, size_t outcap, size_t *outl) {
    (void)ctx; (void)key; (void)keyl;
    if (outcap < cipherl) return AETHER_ERR_OVERFLOW;
    memcpy(out, cipher, cipherl);
    *outl = cipherl;
    return AETHER_OK;
}

static aether_status_t int_fake_derive(
    void *ctx, const uint8_t *mk, size_t mkl,
    int32_t sid, uint32_t kn,
    uint8_t *ctx2s, uint8_t *s2c, size_t dlen) {
    (void)ctx; (void)mk; (void)mkl; (void)sid; (void)kn;
    memset(ctx2s, 0, dlen); memset(s2c, 0, dlen);
    return AETHER_OK;
}

static aether_status_t int_fake_pow_generate(
    void *ctx, const uint8_t *salt, size_t saltl,
    const uint8_t *suffix, size_t suffixl,
    uint8_t pool, int32_t maxh, int32_t *pws,
    size_t cap, size_t *count) {
    (void)ctx; (void)salt; (void)saltl; (void)suffix; (void)suffixl;
    (void)pool; (void)maxh; (void)pws; (void)cap;
    if (count != NULL) *count = 0u;
    return AETHER_OK;
}


static const aether_crypto_vtable_t int_fake_crypto = {
    .profile = {
        .crypto_lib = 0u,
        .symmetric_key_type = 0u,
        .asymmetric_public_key_type = 0u,
        .sign_type = 0u,
        .symmetric_key_bytes = AETHER_KEY_BYTES,
        .asymmetric_public_key_bytes = 32u,
        .sign_public_key_bytes = 32u,
        .signature_bytes = 64u
    },
    .initialize = int_fake_crypto_stub,
    .random_symmetric_key = int_fake_random_symmetric_key,
    .verify_signature = int_fake_verify_signature,
    .asymmetric_encrypt = int_fake_asym_enc,
    .symmetric_encrypt = int_fake_sym_enc,
    .symmetric_decrypt = int_fake_sym_dec,
    .derive_server_keys = int_fake_derive
};


static const uint8_t int_fake_root_key[32] = { 0u };


aether_status_t aether_platform_init(
    void *platform_context,
    size_t platform_context_size,
    uint32_t storage_slot,
    aether_client_platform_t *platform) {

    (void)platform_context;
    (void)platform_context_size;
    (void)storage_slot;

    if (platform == NULL) return AETHER_ERR_ARGUMENT;

    memset(platform, 0, sizeof(*platform));

    platform->ctx = (void *)1;
    platform->now_ms = int_fake_now_ms;
    platform->resolve = int_fake_resolve;
    platform->poll = int_fake_poll;
    platform->receive = int_fake_receive;
    platform->consume = int_fake_consume;

    platform->transport = (aether_transport_vtable_t){
        (void *)1,
        int_fake_transport_open,
        int_fake_transport_close,
        int_fake_transport_send
    };

    platform->flash = (aether_flash_vtable_t){
        (void *)1,
        int_fake_flash_load,
        int_fake_flash_save
    };

    platform->crypto = &int_fake_crypto;
    platform->crypto_ctx = NULL;
    platform->trusted_sign_keys = int_fake_root_key;
    platform->trusted_sign_key_count = 1u;

    platform->pow = (aether_pow_vtable_t){
        (void *)1,
        int_fake_pow_generate
    };

    return AETHER_OK;
}


void aether_platform_deinit(void *platform_context) {
    (void)platform_context;
}




static aether_delivery_attempt_t int_provider_try(
    void *ctx,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length) {

    (void)ctx;

    g_int_provider_calls += 1u;
    g_int_last_recipient = recipient;
    g_int_last_length = length;

    if (length != 0u && length <= sizeof(g_int_last_payload)) {
        memcpy(g_int_last_payload, payload, length);
    }

    return (aether_delivery_attempt_t){
        AETHER_DELIVERY_ACCEPTED, AETHER_OK
    };
}



static void test_registry_and_aether_work(void) {

    reset_fake_client();
    g_int_provider_calls = 0u;
    g_int_last_length = 0u;
    memset(g_int_last_payload, 0, sizeof(g_int_last_payload));

    assert(aether_init(
        &g_fake_client,
        (aether_uuid_t){ 0u, 0u }) == AETHER_OK);

    assert(aether_set_components(
        &g_fake_client, g_int_components, 2u) == AETHER_OK);

    aether_delivery_init(
        &g_int_delivery,
        &g_fake_client,
        g_int_slots,
        2u,
        g_int_payloads,
        32u);

    assert(aether_delivery_set_providers(
        &g_int_delivery, g_int_user_providers, 1u) == AETHER_OK);

    assert(aether_delivery_add_provider(
        &g_int_delivery,
        (aether_delivery_provider_t){ NULL, int_provider_try }) == AETHER_OK);

    assert(aether_register_component(
        &g_fake_client,
        &g_int_delivery,
        aether_delivery_work) == AETHER_OK);

    assert(aether_start(&g_fake_client) == AETHER_OK);

    aether_uuid_t recipient = { 0xDEADu, 0xBEEFu };
    const uint8_t payload[] = {
        0x00u, 0x01u, 0x02u, 0x7Fu,
        0x80u, 0xFFu, 0x55u, 0xAAu
    };

    assert(aether_delivery_send(
        &g_int_delivery, recipient,
        payload, sizeof(payload)) == AETHER_OK);

    assert(g_int_provider_calls == 0u);

    (void)aether_work(&g_fake_client);

    assert(g_int_provider_calls == 1u);
    assert(g_int_last_recipient.msb == recipient.msb);
    assert(g_int_last_recipient.lsb == recipient.lsb);
    assert(g_int_last_length == sizeof(payload));
    assert(memcmp(g_int_last_payload, payload, sizeof(payload)) == 0);

    assert(g_int_slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);

    aether_stop(&g_fake_client);
}



static void test_argument_guards(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[8];

    aether_delivery_init(&delivery, &g_fake_client, slots, 1u,
                         payload_storage, 8u);

    assert(aether_delivery_send(NULL, (aether_uuid_t){ 1u, 2u },
        payload_storage, 0u) == AETHER_ERR_ARGUMENT);

    assert(aether_delivery_send(&delivery, (aether_uuid_t){ 1u, 2u },
        NULL, 1u) == AETHER_ERR_ARGUMENT);

    aether_delivery_t uninit = { 0 };
    assert(aether_delivery_send(&uninit, (aether_uuid_t){ 1u, 2u },
        payload_storage, 0u) == AETHER_ERR_ARGUMENT);
}


int main(void) {

    test_not_applicable_then_fallback();
    test_retry_later_uses_fallback();
    test_accepted_stops_chain();
    test_route_lost_uses_fallback();
    test_error_no_fallback_exact_status();
    test_unknown_result_is_conservative_error();

    test_storage_copy_and_mutation();
    test_queue_full_does_not_overwrite();
    test_oversized_payload_rejected();
    test_zero_length_payload();
    test_multiple_recipients_preserved();

    test_blocked_slot_does_not_starve();
    test_owner_validation();
    test_set_providers_argument_validation();
    test_duplicate_provider_rejected();

    test_registry_and_aether_work();
    test_argument_guards();

    return 0;
}
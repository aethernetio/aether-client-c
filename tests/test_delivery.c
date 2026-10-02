
/*
 * Message delivery regression tests.
 *
 * These tests exercise the transport-agnostic producer API, caller-owned
 * slot storage, provider-chain semantics and callback-safe enqueue.
 *
 * All providers are fakes so the tests do not depend on any real transport.
 * The built-in cloud provider is exercised indirectly: a zero-initialized
 * aether_t means aether_is_ready() is false, so cloud returns RETRY_LATER
 * without touching the network.
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
    aether_delivery_result_t result;
} fake_provider_t;


static aether_delivery_result_t fake_provider_try(
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

    return fp->result;
}


static aether_t g_fake_client;


static void reset_fake_client(void) {
    memset(
        &g_fake_client,
        0,
        sizeof(g_fake_client));
}


static void test_not_applicable_then_fallback(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[2];
    uint8_t payload_storage[2 * 32];

    fake_provider_t provider_a = { 0 };
    provider_a.result = AETHER_DELIVERY_NOT_APPLICABLE;

    fake_provider_t provider_b = { 0 };
    provider_b.result = AETHER_DELIVERY_ACCEPTED;

    aether_delivery_provider_t providers[2];

    aether_delivery_init(
        &delivery,
        &g_fake_client,
        slots,
        2u,
        payload_storage,
        32u);

    aether_delivery_set_providers(
        &delivery,
        providers,
        2u);

    assert(aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &provider_a,
            fake_provider_try
        }) == AETHER_OK);

    assert(aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &provider_b,
            fake_provider_try
        }) == AETHER_OK);

    const uint8_t payload[] = { 1u, 2u, 3u };

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 0x1111u, 0x2222u },
        payload,
        sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(
        &g_fake_client,
        &delivery) == AETHER_OK);

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
    provider_a.result = AETHER_DELIVERY_RETRY_LATER;

    fake_provider_t provider_b = { 0 };
    provider_b.result = AETHER_DELIVERY_ACCEPTED;

    aether_delivery_provider_t providers[2];

    aether_delivery_init(
        &delivery,
        &g_fake_client,
        slots,
        2u,
        payload_storage,
        32u);

    aether_delivery_set_providers(
        &delivery,
        providers,
        2u);

    aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &provider_a,
            fake_provider_try
        });

    aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &provider_b,
            fake_provider_try
        });

    const uint8_t payload[] = { 9u };

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 1u, 2u },
        payload,
        sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(
        &g_fake_client,
        &delivery) == AETHER_OK);

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
    provider_a.result = AETHER_DELIVERY_ACCEPTED;

    fake_provider_t provider_b = { 0 };
    provider_b.result = AETHER_DELIVERY_ACCEPTED;

    aether_delivery_provider_t providers[2];

    aether_delivery_init(
        &delivery,
        &g_fake_client,
        slots,
        1u,
        payload_storage,
        32u);

    aether_delivery_set_providers(
        &delivery,
        providers,
        2u);

    aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &provider_a,
            fake_provider_try
        });

    aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &provider_b,
            fake_provider_try
        });

    const uint8_t payload[] = { 4u, 5u };

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 1u, 2u },
        payload,
        sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(
        &g_fake_client,
        &delivery) == AETHER_OK);

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
    provider_a.result = AETHER_DELIVERY_ROUTE_LOST;

    fake_provider_t provider_b = { 0 };
    provider_b.result = AETHER_DELIVERY_ACCEPTED;

    aether_delivery_provider_t providers[2];

    aether_delivery_init(
        &delivery,
        &g_fake_client,
        slots,
        1u,
        payload_storage,
        32u);

    aether_delivery_set_providers(
        &delivery,
        providers,
        2u);

    aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &provider_a,
            fake_provider_try
        });

    aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &provider_b,
            fake_provider_try
        });

    const uint8_t payload[] = { 7u };

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 1u, 2u },
        payload,
        sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(
        &g_fake_client,
        &delivery) == AETHER_OK);

    assert(provider_a.calls == 1u);
    assert(provider_b.calls == 1u);
    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static unsigned g_error_calls;
static aether_uuid_t g_error_recipient;


static void error_cb(
    void *ctx,
    aether_uuid_t recipient,
    aether_status_t status) {

    (void)ctx;
    (void)status;
    g_error_calls += 1u;
    g_error_recipient = recipient;
}


static void test_error_no_fallback(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[32];

    fake_provider_t provider_a = { 0 };
    provider_a.result = AETHER_DELIVERY_ERROR;

    fake_provider_t provider_b = { 0 };
    provider_b.result = AETHER_DELIVERY_ACCEPTED;

    aether_delivery_provider_t providers[2];

    aether_delivery_init(
        &delivery,
        &g_fake_client,
        slots,
        1u,
        payload_storage,
        32u);

    aether_delivery_set_providers(
        &delivery,
        providers,
        2u);

    aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &provider_a,
            fake_provider_try
        });

    aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &provider_b,
            fake_provider_try
        });

    g_error_calls = 0u;

    aether_delivery_set_error_callback(
        &delivery,
        error_cb,
        NULL);

    aether_uuid_t recipient = { 0xAAAu, 0xBBBu };
    const uint8_t payload[] = { 1u };

    assert(aether_delivery_send(
        &delivery,
        recipient,
        payload,
        sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(
        &g_fake_client,
        &delivery) == AETHER_OK);

    assert(provider_a.calls == 1u);
    assert(provider_b.calls == 0u);
    assert(g_error_calls == 1u);
    assert(g_error_recipient.msb == recipient.msb);
    assert(g_error_recipient.lsb == recipient.lsb);
    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static void test_storage_copy_and_mutation(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[16];

    aether_delivery_init(
        &delivery,
        &g_fake_client,
        slots,
        1u,
        payload_storage,
        16u);

    uint8_t source[8] = {
        0x00u, 0x01u, 0x02u, 0x7Fu,
        0x80u, 0xFFu, 0x55u, 0xAAu
    };

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 1u, 2u },
        source,
        sizeof(source)) == AETHER_OK);

    memset(source, 0xEEu, sizeof(source));

    assert(slots[0].state == AETHER_DELIVERY_SLOT_READY);
    assert(slots[0].payload_length == sizeof(source));

    static const uint8_t expected[] = {
        0x00u, 0x01u, 0x02u, 0x7Fu,
        0x80u, 0xFFu, 0x55u, 0xAAu
    };

    assert(memcmp(
        delivery.payload_storage,
        expected,
        sizeof(expected)) == 0);
}


static void test_queue_full_does_not_overwrite(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[8];

    aether_delivery_init(
        &delivery,
        &g_fake_client,
        slots,
        1u,
        payload_storage,
        8u);

    const uint8_t first[] = { 0xA1u, 0xA2u };

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 1u, 1u },
        first,
        sizeof(first)) == AETHER_OK);

    const uint8_t second[] = { 0xB1u, 0xB2u, 0xB3u };

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 2u, 2u },
        second,
        sizeof(second)) == AETHER_ERR_OVERFLOW);

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

    aether_delivery_init(
        &delivery,
        &g_fake_client,
        slots,
        1u,
        payload_storage,
        4u);

    const uint8_t oversized[5] = { 1u, 2u, 3u, 4u, 5u };

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 1u, 2u },
        oversized,
        sizeof(oversized)) == AETHER_ERR_OVERFLOW);

    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


typedef struct {
    unsigned n;
} rr_state_t;


static aether_delivery_result_t rr_try(
    void *ctx,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length) {

    (void)recipient;
    (void)payload;
    (void)length;

    rr_state_t *state = ctx;
    state->n += 1u;

    return state->n == 1u
        ? AETHER_DELIVERY_RETRY_LATER
        : AETHER_DELIVERY_ACCEPTED;
}


static void test_blocked_slot_does_not_starve(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[2];
    uint8_t payload_storage[2 * 8];

    rr_state_t rr = { 0 };

    aether_delivery_provider_t providers[1];

    aether_delivery_init(
        &delivery,
        &g_fake_client,
        slots,
        2u,
        payload_storage,
        8u);

    aether_delivery_set_providers(
        &delivery,
        providers,
        1u);

    assert(aether_delivery_add_provider(
        &delivery,
        (aether_delivery_provider_t){
            &rr,
            rr_try
        }) == AETHER_OK);

    const uint8_t payload[] = { 1u };

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 1u, 1u },
        payload,
        sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 2u, 2u },
        payload,
        sizeof(payload)) == AETHER_OK);

    assert(aether_delivery_work(
        &g_fake_client,
        &delivery) == AETHER_OK);

    assert(slots[0].state == AETHER_DELIVERY_SLOT_READY);
    assert(slots[1].state == AETHER_DELIVERY_SLOT_EMPTY);

    assert(aether_delivery_work(
        &g_fake_client,
        &delivery) == AETHER_OK);

    assert(slots[0].state == AETHER_DELIVERY_SLOT_EMPTY);
}


static void test_argument_guards(void) {

    reset_fake_client();

    aether_delivery_t delivery;
    aether_delivery_slot_t slots[1];
    uint8_t payload_storage[8];

    aether_delivery_init(
        &delivery,
        &g_fake_client,
        slots,
        1u,
        payload_storage,
        8u);

    assert(aether_delivery_send(
        NULL,
        (aether_uuid_t){ 1u, 2u },
        payload_storage,
        0u) == AETHER_ERR_ARGUMENT);

    assert(aether_delivery_send(
        &delivery,
        (aether_uuid_t){ 1u, 2u },
        NULL,
        1u) == AETHER_ERR_ARGUMENT);

    assert(aether_delivery_work(
        &g_fake_client,
        NULL) == AETHER_OK);
}


int main(void) {

    test_not_applicable_then_fallback();
    test_retry_later_uses_fallback();
    test_accepted_stops_chain();
    test_route_lost_uses_fallback();
    test_error_no_fallback();
    test_storage_copy_and_mutation();
    test_queue_full_does_not_overwrite();
    test_oversized_payload_rejected();
    test_blocked_slot_does_not_starve();
    test_argument_guards();

    return 0;
}

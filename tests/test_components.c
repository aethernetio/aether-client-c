
/*
 * Component registry / aether_work() regression tests.
 *
 * These tests deliberately avoid the "skip on start failure" anti-pattern:
 * aether_start() must succeed on the deterministic fake platform below,
 * otherwise the test is wrong, not the implementation.
 */


#include "aether.h"
#include "aether_platform.h"
#include "aether_registration.h"
#include "aether_hydrogen.h"
#include "aether_test_crypto_hydrogen.h"
#include "hydrogen.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>



/* ---- observed counters ---- */

static int work_a_calls;
static int work_b_calls;
static int work_c_calls;
static int noop_calls;

static int order_a_ticks;
static int order_b_ticks;

static int late_registration_component_calls;
static bool late_component_registered;


static int order_a_observed_b;



static aether_status_t work_c_status = AETHER_OK;


static int order_state_b;



static aether_status_t work_a(
    aether_t *client,
    void *ctx) {
    (void)client;
    (void)ctx;
    work_a_calls += 1;
    return AETHER_OK;
}


static aether_status_t work_b(
    aether_t *client,
    void *ctx) {
    (void)client;
    (void)ctx;
    work_b_calls += 1;
    return AETHER_OK;
}


static aether_status_t work_c(
    aether_t *client,
    void *ctx) {
    (void)client;
    (void)ctx;
    work_c_calls += 1;
    return work_c_status;
}




static aether_status_t work_late_noop(
    aether_t *client,
    void *ctx);


static aether_status_t work_register_late(
    aether_t *client,
    void *ctx) {
    (void)ctx;

    if (!late_component_registered) {

        late_component_registered = true;

        assert(aether_register_component(
            client,
            NULL,
            work_late_noop) == AETHER_OK);
    }

    return AETHER_OK;
}


static aether_status_t work_late_noop(
    aether_t *client,
    void *ctx) {
    (void)client;
    (void)ctx;
    late_registration_component_calls += 1;
    return AETHER_OK;
}



static aether_status_t work_noop(
    aether_t *client,
    void *ctx) {
    (void)client;
    (void)ctx;
    noop_calls += 1;
    return AETHER_OK;
}


/* Order-independence: A waits for B via an external flag, no scheduler help. */

static aether_status_t work_order_a(
    aether_t *client,
    void *ctx) {
    (void)client;
    (void)ctx;
    order_a_ticks += 1;
    if (order_state_b) {
        order_a_observed_b = 1;
    }
    return AETHER_OK;
}


static aether_status_t work_order_b(
    aether_t *client,
    void *ctx) {
    (void)client;
    (void)ctx;
    order_b_ticks += 1;
    order_state_b = 1;
    return AETHER_OK;
}


/* ---- fake platform ---- */

static uint64_t fake_time_ms;


static uint64_t fake_now_ms(void *ctx) {
    (void)ctx;
    fake_time_ms += 1u;
    return fake_time_ms;
}


static aether_status_t fake_resolve(
    void *ctx,
    const char *hostname,
    aether_address_t *address) {

    (void)ctx;
    (void)hostname;

    memset(address, 0, sizeof(*address));
    address->kind = AETHER_ADDR_IPV4;
    address->length = 4u;
    address->bytes[0] = 127;
    address->bytes[1] = 0;
    address->bytes[2] = 0;
    address->bytes[3] = 1;
    return AETHER_OK;
}


static aether_status_t fake_transport_open(
    void *ctx,
    aether_channel_t channel,
    const aether_endpoint_t *endpoint) {
    (void)ctx; (void)channel; (void)endpoint;
    return AETHER_OK;
}


static void fake_transport_close(
    void *ctx,
    aether_channel_t channel) {
    (void)ctx; (void)channel;
}


static aether_status_t fake_transport_send(
    void *ctx,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {
    (void)ctx; (void)channel; (void)data; (void)length;
    return AETHER_OK;
}


static aether_status_t fake_flash_load(
    void *ctx,
    uint8_t *dst,
    size_t capacity,
    size_t *length) {
    (void)ctx; (void)dst; (void)capacity; (void)length;
    /*
     * This test always starts with no persistent identity. Returning
     * AETHER_ERR_STORAGE keeps aether_client_start() on the
     * NO_IDENTITY hand-off path.
     */
    return AETHER_ERR_STORAGE;
}


static aether_status_t fake_flash_save(
    void *ctx,
    const uint8_t *src,
    size_t length) {
    (void)ctx; (void)src; (void)length;
    return AETHER_OK;
}


static aether_status_t fake_platform_poll(
    void *ctx) {
    (void)ctx;
    return AETHER_OK;
}


static aether_status_t fake_platform_receive(
    void *ctx,
    aether_channel_t channel,
    aether_bytes_view_t *frame) {
    (void)ctx; (void)channel;
    if (frame != NULL) {
        frame->data = NULL;
        frame->length = 0u;
    }
    return AETHER_OK;
}


static void fake_platform_consume(
    void *ctx,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {
    (void)ctx; (void)channel; (void)data; (void)length;
}



static hydro_sign_keypair fake_root_signer;
static bool crypto_ready;




static aether_status_t fake_pow_generate(
    void *ctx,
    const uint8_t *salt,
    size_t salt_len,
    const uint8_t *suffix,
    size_t suffix_len,
    uint8_t pool_size,
    int32_t max_hash_value,
    int32_t *passwords,
    size_t capacity,
    size_t *count) {
    (void)ctx; (void)salt; (void)salt_len; (void)suffix; (void)suffix_len;
    (void)pool_size; (void)max_hash_value; (void)passwords; (void)capacity;
    if (count != NULL) {
        *count = 0u;
    }
    return AETHER_OK;
}



aether_status_t aether_platform_init(
    void *platform_context,
    size_t platform_context_size,
    uint32_t storage_slot,
    aether_client_platform_t *platform) {

    (void)storage_slot;

    if (platform_context == NULL ||
        platform_context_size == 0u ||
        platform == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    memset(platform, 0, sizeof(*platform));
    platform->ctx = platform_context;
    platform->now_ms = fake_now_ms;

    platform->transport_io = (aether_transport_io_vtable_t){
        platform_context,
        fake_transport_open,
        fake_transport_close,
        fake_transport_send,
        fake_resolve,
        fake_platform_poll,
        fake_platform_receive,
        fake_platform_consume,
        NULL
    };


    platform->flash = (aether_flash_vtable_t){
        platform_context,
        fake_flash_load,
        fake_flash_save
    };


    platform->crypto = &AETHER_TEST_HYDROGEN_CRYPTO;
    platform->crypto_ctx = NULL;
    platform->trusted_sign_keys = fake_root_signer.pk;
    platform->trusted_sign_key_count = 1u;

    platform->pow = (aether_pow_vtable_t){
        platform_context,
        fake_pow_generate
    };



    return AETHER_OK;
}


void aether_platform_deinit(void *platform_context) {
    (void)platform_context;
}


/* ---- helpers ---- */

static void reset_counters(void) {
    work_a_calls = 0;
    work_b_calls = 0;
    work_c_calls = 0;
    noop_calls = 0;
    work_c_status = AETHER_OK;
}



static void start_client(aether_t *client) {

    if (!crypto_ready) {
        assert(aether_hydrogen_init() == 0);
        hydro_sign_keygen(&fake_root_signer);
        crypto_ready = true;
    }


    assert(aether_init(
        client,
        (aether_uuid_t){0u, 0u}) == AETHER_OK);
}


static void finish_start(aether_t *client) {

    /*
     * The fake platform above is complete: crypto is provided and
     * trusted_sign_keys are set. No caller credentials are installed
     * and no persistent state exists, so aether_start() must reach
     * AETHER_STATE_NO_IDENTITY and return AETHER_OK.
     */
    assert(aether_start(client) == AETHER_OK);
}





/* ---- tests ---- */

static void test_registry_storage(void) {

    aether_t client;
    aether_component_t storage[4];

    int ctx_a = 0;
    int ctx_b = 0;
    int ctx_c = 0;

    assert(aether_init(
        &client,
        (aether_uuid_t){0u, 0u}) == AETHER_OK);

    assert(aether_set_components(
        &client,
        storage,
        4u) == AETHER_OK);

    assert(client.components == storage);
    assert(client.component_capacity == 4u);
    assert(client.component_count == 0u);

    assert(aether_register_component(&client, &ctx_a, work_a) == AETHER_OK);
    assert(client.component_count == 1u);
    assert(client.components[0].work == work_a);

    assert(aether_register_component(&client, &ctx_b, work_b) == AETHER_OK);
    assert(client.component_count == 2u);

    assert(aether_register_component(&client, &ctx_c, work_c) == AETHER_OK);
    assert(client.component_count == 3u);

    /* Same work, different context: allowed. */
    assert(aether_register_component(&client, NULL, work_a) == AETHER_OK);
    assert(client.component_count == 4u);

    /* Full. */
    assert(aether_register_component(&client, &ctx_a, work_b) ==
        AETHER_ERR_OVERFLOW);
    assert(client.component_count == 4u);

    aether_stop(&client);
}


static void test_set_components_replaces_registry(void) {

    aether_t client;
    aether_component_t storage_a[2];
    aether_component_t storage_b[4];

    assert(aether_init(
        &client,
        (aether_uuid_t){0u, 0u}) == AETHER_OK);

    assert(aether_set_components(
        &client,
        storage_a,
        2u) == AETHER_OK);

    assert(aether_register_component(&client, NULL, work_a) == AETHER_OK);
    assert(client.component_count == 1u);

    assert(aether_set_components(
        &client,
        storage_b,
        4u) == AETHER_OK);

    assert(client.components == storage_b);
    assert(client.component_capacity == 4u);
    assert(client.component_count == 0u);

    aether_stop(&client);
}


static void test_null_arguments(void) {

    aether_t client;
    aether_component_t storage[2];

    assert(aether_init(
        &client,
        (aether_uuid_t){0u, 0u}) == AETHER_OK);

    assert(aether_set_components(
        &client,
        storage,
        2u) == AETHER_OK);

    assert(aether_register_component(&client, NULL, NULL) ==
        AETHER_ERR_ARGUMENT);
    assert(aether_register_component(NULL, NULL, work_a) ==
        AETHER_ERR_ARGUMENT);

    assert(aether_work(NULL) == AETHER_ERR_ARGUMENT);

    aether_stop(&client);
}


static void test_zero_capacity(void) {

    aether_t client;

    assert(aether_init(
        &client,
        (aether_uuid_t){0u, 0u}) == AETHER_OK);

    assert(client.components == NULL);
    assert(client.component_capacity == 0u);
    assert(client.component_count == 0u);

    assert(aether_register_component(&client, NULL, work_a) ==
        AETHER_ERR_OVERFLOW);

    aether_stop(&client);
}


static void test_capacity_independence(void) {

    aether_t client;
    aether_component_t storage[32];
    int contexts[32];

    assert(aether_init(
        &client,
        (aether_uuid_t){0u, 0u}) == AETHER_OK);

    assert(aether_set_components(
        &client,
        storage,
        32u) == AETHER_OK);

    assert(client.component_capacity == 32u);

    for (size_t i = 0u; i < 32u; ++i) {
        contexts[i] = (int)i;
        assert(aether_register_component(
            &client,
            &contexts[i],
            work_a) == AETHER_OK);
    }

    assert(client.component_count == 32u);

    assert(aether_register_component(&client, NULL, work_a) ==
        AETHER_ERR_OVERFLOW);

    aether_stop(&client);
}


static void test_work_dispatch(void) {

    aether_t client;
    aether_component_t storage[4];

    reset_counters();

    start_client(&client);

    assert(aether_set_components(
        &client,
        storage,
        4u) == AETHER_OK);

    assert(aether_register_component(&client, NULL, work_a) == AETHER_OK);
    assert(aether_register_component(&client, NULL, work_b) == AETHER_OK);
    assert(aether_register_component(&client, NULL, work_c) == AETHER_OK);

    assert(aether_register_component(&client, NULL, work_noop) == AETHER_OK);

    finish_start(&client);

    assert(aether_work(&client) == AETHER_OK);
    assert(work_a_calls == 1);
    assert(work_b_calls == 1);
    assert(work_c_calls == 1);
    assert(noop_calls == 1);

    assert(aether_work(&client) == AETHER_OK);
    assert(work_a_calls == 2);
    assert(work_b_calls == 2);
    assert(work_c_calls == 2);
    assert(noop_calls == 2);

    /*
     * A component error must not starve the rest of the pass: all
     * components still get one tick, and the first error is returned.
     */
    work_c_status = AETHER_ERR_UNSUPPORTED;

    assert(aether_work(&client) == AETHER_ERR_UNSUPPORTED);
    assert(work_a_calls == 3);
    assert(work_b_calls == 3);
    assert(work_c_calls == 3);
    assert(noop_calls == 3);

    /* Subsequent call resets the pass and returns the new first error. */
    assert(aether_work(&client) == AETHER_ERR_UNSUPPORTED);
    assert(work_a_calls == 4);
    assert(work_b_calls == 4);
    assert(work_c_calls == 4);
    assert(noop_calls == 4);

    aether_stop(&client);
}


static void test_error_does_not_starve_earlier_components(void) {

    aether_t client;
    aether_component_t storage[3];

    reset_counters();

    start_client(&client);

    assert(aether_set_components(
        &client,
        storage,
        3u) == AETHER_OK);

    /*
     * Register work_c FIRST. Its error must still let work_a/work_b
     * run: error collection happens across the whole pass.
     */
    assert(aether_register_component(&client, NULL, work_c) == AETHER_OK);
    assert(aether_register_component(&client, NULL, work_a) == AETHER_OK);

    assert(aether_register_component(&client, NULL, work_b) == AETHER_OK);

    finish_start(&client);

    work_c_status = AETHER_ERR_REMOTE;

    assert(aether_work(&client) == AETHER_ERR_REMOTE);
    assert(work_c_calls == 1);
    assert(work_a_calls == 1);
    assert(work_b_calls == 1);

    aether_stop(&client);
}


static void test_order_independence(void) {

    aether_t client;
    aether_component_t storage[2];

    /*
     * Component B sets an external flag; component A observes it. A
     * is written to just return AETHER_OK while waiting (no special
     * scheduler status). After several aether_work() calls the final
     * external result is identical in both registration orders.
     */

    /* Registration order A,B */
    order_state_b = 0;
    order_a_observed_b = 0;
    order_a_ticks = 0;
    order_b_ticks = 0;

    start_client(&client);
    assert(aether_set_components(&client, storage, 2u) == AETHER_OK);
    assert(aether_register_component(&client, NULL, work_order_a) ==
        AETHER_OK);

    assert(aether_register_component(&client, NULL, work_order_b) ==
        AETHER_OK);


    finish_start(&client);

    for (int step = 0; step < 4; ++step) {
        assert(aether_work(&client) == AETHER_OK);
    }

    int a_ticks_ab = order_a_ticks;
    int b_ticks_ab = order_b_ticks;
    int a_observed_ab = order_a_observed_b;
    int state_b_ab = order_state_b;

    aether_stop(&client);

    /* Registration order B,A */
    order_state_b = 0;
    order_a_observed_b = 0;
    order_a_ticks = 0;
    order_b_ticks = 0;

    start_client(&client);
    assert(aether_set_components(&client, storage, 2u) == AETHER_OK);
    assert(aether_register_component(&client, NULL, work_order_b) ==
        AETHER_OK);

    assert(aether_register_component(&client, NULL, work_order_a) ==
        AETHER_OK);


    finish_start(&client);

    for (int step = 0; step < 4; ++step) {
        assert(aether_work(&client) == AETHER_OK);
    }

    int a_ticks_ba = order_a_ticks;
    int b_ticks_ba = order_b_ticks;
    int a_observed_ba = order_a_observed_b;
    int state_b_ba = order_state_b;

    aether_stop(&client);

    /* Every component still got one tick per call in both orders. */
    assert(a_ticks_ab == 4);
    assert(a_ticks_ba == 4);
    assert(b_ticks_ab == 4);
    assert(b_ticks_ba == 4);

    /* Final external result is identical. */
    assert(state_b_ab == 1);
    assert(state_b_ba == 1);
    assert(a_observed_ab == 1);
    assert(a_observed_ba == 1);
}


static void test_waiting_component_returns_ok(void) {

    aether_t client;
    aether_component_t storage[1];

    reset_counters();

    start_client(&client);
    assert(aether_set_components(&client, storage, 1u) == AETHER_OK);

    assert(aether_register_component(&client, NULL, work_c) == AETHER_OK);

    finish_start(&client);

    /*
     * A component whose prerequisite is not yet ready simply returns
     * AETHER_OK. The scheduler does not need any special "retry"
     * status and aether_work() returns AETHER_OK.
     */
    work_c_status = AETHER_OK;

    for (int step = 0; step < 3; ++step) {
        assert(aether_work(&client) == AETHER_OK);
    }

    assert(work_c_calls == 3);

    aether_stop(&client);
}


static void test_no_components_is_noop(void) {

    aether_t client;

    start_client(&client);

    finish_start(&client);

    /* No registry attached: aether_work() must simply run the core. */
    assert(aether_work(&client) == AETHER_OK);
    assert(aether_work(&client) == AETHER_OK);

    aether_stop(&client);
}



static void test_set_components_after_start_rejected(void) {

    aether_t client;
    aether_component_t storage[2];
    aether_component_t other_storage[2];


    start_client(&client);

    finish_start(&client);

    assert(aether_set_components(
        &client,
        storage,
        2u) == AETHER_ERR_STATE);

    assert(aether_set_components(
        &client,
        other_storage,
        2u) == AETHER_ERR_STATE);

    aether_stop(&client);
}


static void test_duplicate_registration_rejected(void) {

    aether_t client;
    aether_component_t storage[4];


    start_client(&client);

    assert(aether_set_components(&client, storage, 4u) == AETHER_OK);


    assert(aether_register_component(&client, NULL, work_a) == AETHER_OK);
    assert(client.component_count == 1u);

    /* Exact same pair → rejected, count unchanged. */
    assert(aether_register_component(&client, NULL, work_a) ==
        AETHER_ERR_STATE);
    assert(client.component_count == 1u);

    /* Different context, same work → allowed. */
    int extra_ctx = 0;
    assert(aether_register_component(&client, &extra_ctx, work_a) ==
        AETHER_OK);
    assert(client.component_count == 2u);

    /* Same context, different work → allowed. */
    aether_component_work_fn work_a2 = work_b;
    assert(aether_register_component(&client, NULL, work_a2) ==
        AETHER_OK);
    assert(client.component_count == 3u);

    aether_stop(&client);
}


static void test_registration_during_pass_deferred(void) {

    aether_t client;
    aether_component_t storage[4];

    reset_counters();

    late_registration_component_calls = 0;
    late_component_registered = false;

    start_client(&client);
    assert(aether_set_components(&client, storage, 4u) == AETHER_OK);

    assert(aether_register_component(
        &client,
        NULL,
        work_register_late) == AETHER_OK);


    finish_start(&client);


    /*
     * First aether_work(): work_register_late runs, registers
     * work_late_noop. Snapshot semantics say the new component
     * does NOT run in the same pass.
     */
    assert(aether_work(&client) == AETHER_OK);
    assert(late_component_registered == true);
    assert(late_registration_component_calls == 0);
    assert(client.component_count == 2u);

    /*
     * Second aether_work(): the newly registered component now runs.
     */
    assert(aether_work(&client) == AETHER_OK);
    assert(late_registration_component_calls == 1);

    assert(aether_work(&client) == AETHER_OK);
    assert(late_registration_component_calls == 2);

    aether_stop(&client);
}






static void test_registration_work_owner_check(void) {

    aether_t owner;
    aether_t other;
    aether_registration_t registration;

    start_client(&owner);
    finish_start(&owner);

    start_client(&other);
    finish_start(&other);

    aether_registration_init(
        &registration,
        &owner.app.core);

    /* Correct owner -> not AETHER_ERR_ARGUMENT. */
    aether_status_t ok_status =
        aether_registration_work(
            &owner,
            &registration);
    assert(ok_status != AETHER_ERR_ARGUMENT);

    /* Wrong owner -> AETHER_ERR_ARGUMENT. */
    aether_status_t wrong_status =
        aether_registration_work(
            &other,
            &registration);
    assert(wrong_status == AETHER_ERR_ARGUMENT);

    /* NULL registration -> AETHER_ERR_ARGUMENT. */
    assert(aether_registration_work(
        &owner,
        NULL) == AETHER_ERR_ARGUMENT);

    assert(aether_registration_work(
        NULL,
        &registration) == AETHER_ERR_ARGUMENT);

    aether_stop(&owner);
    aether_stop(&other);
}


static void test_registration_work_via_component(void) {

    aether_t client;
    aether_component_t storage[1];
    aether_registration_t registration;

    start_client(&client);

    assert(aether_set_components(
        &client,
        storage,
        1u) == AETHER_OK);

    finish_start(&client);

    /*
     * aether_start() calls aether_client_init() which memsets the
     * core, so the registration alias into the core must be set
     * after start. Registration is then attached as a component on
     * the live client.
     */
    aether_registration_init(
        &registration,
        &client.app.core);

    assert(aether_register_component(
        &client,
        &registration,
        aether_registration_work) == AETHER_OK);

    aether_status_t status = aether_work(&client);
    assert(status == AETHER_OK);

    aether_stop(&client);
}



int main(void) {

    test_registry_storage();
    test_set_components_replaces_registry();
    test_null_arguments();
    test_zero_capacity();
    test_capacity_independence();
    test_work_dispatch();
    test_error_does_not_starve_earlier_components();
    test_order_independence();
    test_waiting_component_returns_ok();
    test_no_components_is_noop();

    test_set_components_after_start_rejected();
    test_duplicate_registration_rejected();
    test_registration_during_pass_deferred();

    test_registration_work_owner_check();
    test_registration_work_via_component();



    return 0;
}
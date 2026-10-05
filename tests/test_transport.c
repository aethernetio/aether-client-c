
/*
 * Semantic transport (server implementation) lifecycle tests.
 *
 * Covers batch state machine, borrowed-input lifetime, reset semantics
 * and stale-generation protection. The server transport owns its
 * command descriptors internally; callers do not provide batch storage.
 */

#include "aether_transport.h"
#include "aether_transport_server.h"
#include "aether_client_app.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>


static unsigned flush_done_calls;
static aether_status_t last_flush_status;


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
}


static aether_client_app_t fake_app;


static void init_handle_common(
    aether_transport_server_t *server,
    aether_transport_t *handle,
    struct aether_client_app *app) {

    aether_transport_server_init(
        server,
        app);

    aether_transport_server_bind(
        handle,
        server);
}


static void init_handle(
    aether_transport_server_t *server,
    aether_transport_t *handle) {

    memset(&fake_app, 0, sizeof(fake_app));
    fake_app.started = true;
    fake_app.core.state = AETHER_STATE_READY;

    init_handle_common(
        server,
        handle,
        &fake_app);
}


static void test_initial_state(void) {

    aether_transport_server_t server;
    aether_transport_t handle;

    init_handle(&server, &handle);

    assert(
        aether_transport_status(&handle) ==
        AETHER_TRANSPORT_IDLE);
}


static void test_first_command_to_building(void) {

    aether_transport_server_t server;
    aether_transport_t handle;
    uint8_t payload[4] = { 1u, 2u, 3u, 4u };

    init_handle(&server, &handle);

    aether_uuid_t uid = { 1u, 2u };

    assert(
        aether_transport_send_message(
            &handle, uid, payload, sizeof(payload)) ==
        AETHER_OK);

    assert(
        aether_transport_status(&handle) ==
        AETHER_TRANSPORT_BUILDING);
}


static void test_multiple_commands_in_one_batch(void) {

    aether_transport_server_t server;
    aether_transport_t handle;
    uint8_t payload[2] = { 0xAAu, 0xBBu };

    init_handle(&server, &handle);

    aether_uuid_t uid = { 1u, 2u };

    assert(
        aether_transport_send_message(
            &handle, uid, payload, sizeof(payload)) ==
        AETHER_OK);
    assert(
        aether_transport_send_message(
            &handle, uid, payload, sizeof(payload)) ==
        AETHER_OK);
    assert(
        aether_transport_send_message(
            &handle, uid, payload, sizeof(payload)) ==
        AETHER_OK);

    assert(
        aether_transport_status(&handle) ==
        AETHER_TRANSPORT_BUILDING);
    assert(server.command_count == 3u);
}


static void test_batch_overflow(void) {

    aether_transport_server_t server;
    aether_transport_t handle;
    uint8_t payload[2] = { 0xAAu, 0xBBu };

    init_handle(&server, &handle);

    aether_uuid_t uid = { 1u, 2u };

    for (size_t i = 0u;
         i < AETHER_TRANSPORT_SERVER_MAX_COMMANDS;
         ++i) {

        assert(
            aether_transport_send_message(
                &handle, uid, payload, sizeof(payload)) ==
            AETHER_OK);
    }

    assert(
        aether_transport_send_message(
            &handle, uid, payload, sizeof(payload)) ==
        AETHER_ERR_OVERFLOW);
}


static void test_flush_from_idle_is_noop(void) {

    aether_transport_server_t server;
    aether_transport_t handle;

    reset_fixture();

    init_handle(&server, &handle);

    aether_transport_listener_t listener = {
        NULL,
        on_flush_done
    };

    aether_transport_set_listener(&handle, &listener);

    assert(aether_transport_flush(&handle) == AETHER_OK);

    assert(
        aether_transport_status(&handle) ==
        AETHER_TRANSPORT_IDLE);

    assert(flush_done_calls == 0u);
}


static void test_flush_transitions_to_flushing(void) {

    aether_transport_server_t server;
    aether_transport_t handle;
    uint8_t payload[2] = { 0xAAu, 0xBBu };

    reset_fixture();

    init_handle(&server, &handle);

    aether_transport_listener_t listener = {
        NULL,
        on_flush_done
    };

    aether_transport_set_listener(&handle, &listener);

    aether_uuid_t uid = { 1u, 2u };

    aether_transport_send_message(
        &handle, uid, payload, sizeof(payload));

    assert(aether_transport_flush(&handle) == AETHER_OK);

    assert(
        aether_transport_status(&handle) ==
        AETHER_TRANSPORT_FLUSHING);
}


static void test_send_message_busy_while_flushing(void) {

    aether_transport_server_t server;
    aether_transport_t handle;
    uint8_t payload[2] = { 0xAAu, 0xBBu };

    init_handle(&server, &handle);

    aether_uuid_t uid = { 1u, 2u };

    aether_transport_send_message(
        &handle, uid, payload, sizeof(payload));

    aether_transport_flush(&handle);

    assert(
        aether_transport_send_message(
            &handle, uid, payload, sizeof(payload)) ==
        AETHER_ERR_BUSY);

    assert(aether_transport_flush(&handle) == AETHER_ERR_BUSY);
}


static void test_server_readiness_required(void) {

    aether_transport_server_t server;
    aether_transport_t handle;
    uint8_t payload[2] = { 0xAAu, 0xBBu };

    reset_fixture();

    memset(&fake_app, 0, sizeof(fake_app));
    /* fake_app.started stays false: not ready yet. */

    init_handle_common(&server, &handle, &fake_app);

    aether_transport_listener_t listener = {
        NULL,
        on_flush_done
    };

    aether_transport_set_listener(&handle, &listener);

    aether_uuid_t uid = { 1u, 2u };

    assert(
        aether_transport_send_message(
            &handle, uid, payload, sizeof(payload)) ==
        AETHER_ERR_STATE);

    assert(
        aether_transport_status(&handle) ==
        AETHER_TRANSPORT_IDLE);

    assert(flush_done_calls == 0u);
}


static void test_reset_from_building(void) {

    aether_transport_server_t server;
    aether_transport_t handle;
    uint8_t payload[2] = { 0xAAu, 0xBBu };

    reset_fixture();

    init_handle(&server, &handle);

    aether_transport_listener_t listener = {
        NULL,
        on_flush_done
    };

    aether_transport_set_listener(&handle, &listener);

    aether_uuid_t uid = { 1u, 2u };

    aether_transport_send_message(
        &handle, uid, payload, sizeof(payload));

    aether_transport_reset(&handle);

    assert(
        aether_transport_status(&handle) ==
        AETHER_TRANSPORT_IDLE);

    assert(flush_done_calls == 0u);
    assert(server.command_count == 0u);
}


static void test_reset_from_flushing(void) {

    aether_transport_server_t server;
    aether_transport_t handle;
    uint8_t payload[2] = { 0xAAu, 0xBBu };

    reset_fixture();

    init_handle(&server, &handle);

    aether_transport_listener_t listener = {
        NULL,
        on_flush_done
    };

    aether_transport_set_listener(&handle, &listener);

    aether_uuid_t uid = { 1u, 2u };

    aether_transport_send_message(
        &handle, uid, payload, sizeof(payload));

    aether_transport_flush(&handle);
    aether_transport_reset(&handle);

    assert(
        aether_transport_status(&handle) ==
        AETHER_TRANSPORT_IDLE);

    assert(flush_done_calls == 0u);

    aether_transport_tick(&handle);

    assert(flush_done_calls == 0u);

    aether_transport_send_message(
        &handle, uid, payload, sizeof(payload));

    assert(
        aether_transport_status(&handle) ==
        AETHER_TRANSPORT_BUILDING);
}


static void test_reset_from_idle_is_harmless(void) {

    aether_transport_server_t server;
    aether_transport_t handle;

    init_handle(&server, &handle);

    aether_transport_reset(&handle);

    assert(
        aether_transport_status(&handle) ==
        AETHER_TRANSPORT_IDLE);
}


int main(void) {

    test_initial_state();
    test_first_command_to_building();
    test_multiple_commands_in_one_batch();
    test_batch_overflow();
    test_flush_from_idle_is_noop();
    test_flush_transitions_to_flushing();
    test_send_message_busy_while_flushing();
    test_server_readiness_required();
    test_reset_from_building();
    test_reset_from_flushing();
    test_reset_from_idle_is_harmless();

    return 0;
}


#include "aether_send_future.h"

#include <assert.h>
#include <string.h>


static uint64_t fake_now_ms(
    void *ctx) {

    return
        *(const uint64_t *)ctx;
}


static void test_begin_accept_success(void) {
    aether_send_future_t future;

    aether_send_future_init(
        &future);

    assert(
        aether_send_future_state(
            &future) ==
        AETHER_SEND_FUTURE_IDLE);

    assert(
        aether_send_future_begin(
            &future,
            17u,
            100u,
            50u) ==
        AETHER_OK);

    aether_ingress_t ingress;

    memset(
        &ingress,
        0,
        sizeof(ingress));

    ingress.kind =
        AETHER_INGRESS_REQUEST_RESULT;

    ingress.as.request_result.request_id =
        18u;

    ingress.as.request_result.status =
        AETHER_OK;

    assert(
        !aether_send_future_accept(
            &future,
            &ingress));

    assert(
        aether_send_future_state(
            &future) ==
        AETHER_SEND_FUTURE_PENDING);

    ingress.as.request_result.request_id =
        17u;

    assert(
        aether_send_future_accept(
            &future,
            &ingress));

    assert(
        aether_send_future_state(
            &future) ==
        AETHER_SEND_FUTURE_DONE);

    assert(
        aether_send_future_result(
            &future) ==
        AETHER_OK);

    assert(
        aether_send_future_is_complete(
            &future));
}


static void test_accept_remote_error(void) {
    aether_send_future_t future;

    aether_send_future_init(
        &future);

    assert(
        aether_send_future_begin(
            &future,
            22u,
            1000u,
            500u) ==
        AETHER_OK);

    aether_ingress_t ingress;

    memset(
        &ingress,
        0,
        sizeof(ingress));

    ingress.kind =
        AETHER_INGRESS_REQUEST_RESULT;

    ingress.as.request_result.request_id =
        22u;

    ingress.as.request_result.status =
        AETHER_ERR_REMOTE;

    assert(
        aether_send_future_accept(
            &future,
            &ingress));

    assert(
        aether_send_future_state(
            &future) ==
        AETHER_SEND_FUTURE_ERROR);

    assert(
        aether_send_future_result(
            &future) ==
        AETHER_ERR_REMOTE);
}


static void test_timeout_wraparound(void) {
    aether_send_future_t future;

    aether_send_future_init(
        &future);

    uint64_t start =
        UINT32_MAX - 5u;

    assert(
        aether_send_future_begin(
            &future,
            33u,
            start,
            10u) ==
        AETHER_OK);

    assert(
        !aether_send_future_tick(
            &future,
            start + 9u));

    assert(
        aether_send_future_tick(
            &future,
            start + 10u));

    assert(
        aether_send_future_state(
            &future) ==
        AETHER_SEND_FUTURE_TIMEOUT);

    assert(
        aether_send_future_result(
            &future) ==
        AETHER_ERR_TIMEOUT);
}


static void test_zero_timeout_never_expires(void) {
    aether_send_future_t future;

    aether_send_future_init(
        &future);

    assert(
        aether_send_future_begin(
            &future,
            44u,
            1u,
            0u) ==
        AETHER_OK);

    assert(
        !aether_send_future_tick(
            &future,
            UINT32_MAX));

    assert(
        aether_send_future_state(
            &future) ==
        AETHER_SEND_FUTURE_PENDING);
}


static void test_busy_and_argument_guards(void) {
    aether_send_future_t future;
    aether_client_app_t app;
    uint64_t now =
        123u;

    memset(
        &app,
        0,
        sizeof(app));

    aether_send_future_init(
        &future);

    assert(
        aether_send_future_send(
            &future,
            &app,
            (aether_uuid_t){0u,0u},
            NULL,
            0u) ==
        AETHER_ERR_ARGUMENT);

    app.platform.now_ms =
        fake_now_ms;

    app.platform.ctx =
        &now;

    assert(
        aether_send_future_begin(
            &future,
            55u,
            now,
            100u) ==
        AETHER_OK);

    assert(
        aether_send_future_send(
            &future,
            &app,
            (aether_uuid_t){0u,0u},
            NULL,
            0u) ==
        AETHER_ERR_BUSY);
}


static void test_reset(void) {
    aether_send_future_t future;

    aether_send_future_init(
        &future);

    assert(
        aether_send_future_begin(
            &future,
            66u,
            5u,
            7u) ==
        AETHER_OK);

    aether_send_future_reset(
        &future);

    assert(
        aether_send_future_state(
            &future) ==
        AETHER_SEND_FUTURE_IDLE);

    assert(
        aether_send_future_request_id(
            &future) ==
        0u);

    assert(
        !aether_send_future_is_complete(
            &future));
}


int main(void) {
    test_begin_accept_success();
    test_accept_remote_error();
    test_timeout_wraparound();
    test_zero_timeout_never_expires();
    test_busy_and_argument_guards();
    test_reset();
    return 0;
}

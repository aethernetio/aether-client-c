
#include "result_rpc_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[64];
    size_t length;
    size_t calls;
} capture_t;

static aether_status_t capture_send(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    capture_t *capture =
        ctx;

    assert(capture != NULL);
    assert(data != NULL);
    assert(length <= sizeof(capture->data));

    memcpy(
        capture->data,
        data,
        length);

    capture->length =
        length;

    capture->calls++;

    return AETHER_OK;
}

static result_api_remote_t make_remote(
    capture_t *capture,
    uint8_t *tx,
    size_t tx_capacity) {

    result_api_remote_t remote = {
        capture,
        capture_send,
        tx,
        tx_capacity
    };

    return remote;
}

static void expect_bytes(
    const capture_t *capture,
    const uint8_t *expected,
    size_t expected_length) {

    assert(capture->calls == 1u);
    assert(capture->length == expected_length);

    assert(
        memcmp(
            capture->data,
            expected,
            expected_length) == 0);
}

static void test_fire_and_forget_unchanged(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    result_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    assert(
        result_api_notify(
            &remote,
            INT32_C(0x12345678)) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x78u,
        0x56u,
        0x34u,
        0x12u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_void_result_request_id_is_le4(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    result_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    assert(
        result_api_finish(
            &remote,
            UINT32_C(0x78563412),
            INT32_C(0x01020304)) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x04u,

        /* request id */
        0x12u,
        0x34u,
        0x56u,
        0x78u,

        /* value */
        0x04u,
        0x03u,
        0x02u,
        0x01u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_typed_result_has_same_request_frame(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    result_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    assert(
        result_api_query(
            &remote,
            UINT32_C(0xa1b2c3d4),
            INT16_C(0x1234)) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x05u,

        /* request id */
        0xd4u,
        0xc3u,
        0xb2u,
        0xa1u,

        /* code */
        0x34u,
        0x12u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_result_header_capacity_requires_five_bytes(void) {
    uint8_t tx[4] = {0};
    capture_t capture = {0};

    result_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    assert(
        result_api_finish(
            &remote,
            UINT32_C(1),
            INT32_C(2)) ==
        AETHER_ERR_OVERFLOW);

    assert(capture.calls == 0u);
}

int main(void) {
    test_fire_and_forget_unchanged();
    test_void_result_request_id_is_le4();
    test_typed_result_has_same_request_frame();
    test_result_header_capacity_requires_five_bytes();

    return 0;
}


#include "int_pack_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[32];
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

static void expect_value(
    int64_t value,
    const uint8_t *expected,
    size_t expected_length) {

    uint8_t tx[32] = {0};
    capture_t capture = {0};

    pack_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    assert(
        pack_api_send_value(
            &remote,
            value) ==
        AETHER_OK);

    assert(capture.calls == 1u);
    assert(capture.length == expected_length);

    assert(
        memcmp(
            capture.data,
            expected,
            expected_length) == 0);
}

static void test_pack_boundaries(void) {
    static const uint8_t zero[] = {
        0x03u,
        0x00u
    };

    static const uint8_t max_one_byte[] = {
        0x03u,
        0xfau
    };

    static const uint8_t first_two_byte[] = {
        0x03u,
        0xfbu,
        0x00u
    };

    static const uint8_t last_two_byte[] = {
        0x03u,
        0xffu,
        0xefu
    };

    static const uint8_t first_four_byte[] = {
        0x03u,
        0xffu,
        0xf0u,
        0x00u,
        0x00u
    };

    static const uint8_t last_four_byte[] = {
        0x03u,
        0xffu,
        0xffu,
        0xffu,
        0xfeu
    };

    static const uint8_t first_eight_byte[] = {
        0x03u,
        0xffu,
        0xffu,
        0x00u,
        0xffu,
        0x00u,
        0x00u,
        0x00u,
        0x00u
    };

    static const uint8_t max_eight_byte[] = {
        0x03u,
        0xffu,
        0xffu,
        0xffu,
        0xffu,
        0xffu,
        0xffu,
        0xffu,
        0xffu
    };

    expect_value(
        INT64_C(0),
        zero,
        sizeof(zero));

    expect_value(
        INT64_C(250),
        max_one_byte,
        sizeof(max_one_byte));

    expect_value(
        INT64_C(251),
        first_two_byte,
        sizeof(first_two_byte));

    expect_value(
        INT64_C(1514),
        last_two_byte,
        sizeof(last_two_byte));

    expect_value(
        INT64_C(1515),
        first_four_byte,
        sizeof(first_four_byte));

    expect_value(
        INT64_C(1049834),
        last_four_byte,
        sizeof(last_four_byte));

    expect_value(
        INT64_C(1049835),
        first_eight_byte,
        sizeof(first_eight_byte));

    expect_value(
        INT64_C(1099512677610),
        max_eight_byte,
        sizeof(max_eight_byte));
}

static void test_invalid_domain(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    pack_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    assert(
        pack_api_send_value(
            &remote,
            INT64_C(-1)) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);

    assert(
        pack_api_send_value(
            &remote,
            INT64_C(1099512677611)) ==
        AETHER_ERR_OVERFLOW);

    assert(capture.calls == 0u);
}

static void test_struct_field(void) {
    uint8_t tx[8] = {0};
    capture_t capture = {0};

    pack_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    pack_holder_t holder = {
        INT64_C(251)
    };

    assert(
        pack_api_send_holder(
            &remote,
            &holder) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x04u,
        0xfbu,
        0x00u
    };

    assert(capture.calls == 1u);
    assert(capture.length == sizeof(expected));

    assert(
        memcmp(
            capture.data,
            expected,
            sizeof(expected)) == 0);

    holder.value =
        INT64_C(-1);

    capture.calls =
        0u;

    assert(
        pack_api_send_holder(
            &remote,
            &holder) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}

static void test_buffer_overflow(void) {
    uint8_t tx[1] = {0};
    capture_t capture = {0};

    pack_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    assert(
        pack_api_send_value(
            &remote,
            INT64_C(251)) ==
        AETHER_ERR_OVERFLOW);

    assert(capture.calls == 0u);
}

int main(void) {
    test_pack_boundaries();
    test_invalid_domain();
    test_struct_field();
    test_buffer_overflow();

    return 0;
}
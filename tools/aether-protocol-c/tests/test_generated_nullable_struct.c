
#include "nullable_struct_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[128];
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

static nullable_api_remote_t make_remote(
    capture_t *capture,
    uint8_t *tx,
    size_t tx_capacity) {

    nullable_api_remote_t remote = {
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

static void test_nullable_mixed(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    nullable_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    static const uint8_t name_data[] = {
        0x6fu,
        0x6bu
    };

    aether_bytes_view_t name = {
        name_data,
        sizeof(name_data)
    };

    nullable_mixed_t value = {
        NULL,
        INT8_C(0x2a),
        &name,
        NULL
    };

    assert(
        nullable_api_send_mixed(
            &remote,
            &value) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x05u,
        0x2au,
        0x02u,
        0x6fu,
        0x6bu
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_nullable_present_values(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    nullable_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    int16_t short_value =
        INT16_C(0x1234);

    nullable_inner_t inner = {
        INT16_C(0x4567)
    };

    nullable_mixed_t value = {
        &short_value,
        INT8_C(7),
        NULL,
        &inner
    };

    assert(
        nullable_api_send_mixed(
            &remote,
            &value) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x02u,
        0x34u,
        0x12u,
        0x07u,
        0x67u,
        0x45u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_mask_width_2(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    nullable_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    mask9_t value = {0};

    assert(
        nullable_api_send_mask9(
            &remote,
            &value) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x04u,
        0xffu,
        0x01u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_mask_width_4(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    nullable_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    mask17_t value = {0};

    assert(
        nullable_api_send_mask17(
            &remote,
            &value) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x05u,
        0xffu,
        0xffu,
        0x01u,
        0x00u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_mask_width_8(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    nullable_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    mask33_t value = {0};

    assert(
        nullable_api_send_mask33(
            &remote,
            &value) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x06u,
        0xffu,
        0xffu,
        0xffu,
        0xffu,
        0x01u,
        0x00u,
        0x00u,
        0x00u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_nullable_view_validation(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    nullable_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    aether_bytes_view_t bad_name = {
        NULL,
        1u
    };

    nullable_mixed_t value = {
        NULL,
        INT8_C(1),
        &bad_name,
        NULL
    };

    assert(
        nullable_api_send_mixed(
            &remote,
            &value) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}

int main(void) {
    test_nullable_mixed();
    test_nullable_present_values();
    test_mask_width_2();
    test_mask_width_4();
    test_mask_width_8();
    test_nullable_view_validation();

    return 0;
}

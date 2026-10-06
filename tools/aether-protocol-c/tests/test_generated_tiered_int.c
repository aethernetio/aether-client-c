
#include "tiered_int_api.h"

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

    capture_t *capture = ctx;

    assert(capture != NULL);
    assert(data != NULL);
    assert(length <= sizeof(capture->data));

    memcpy(
        capture->data,
        data,
        length);

    capture->length = length;
    capture->calls++;

    return AETHER_OK;
}

static counter_api_remote_t make_remote(
    capture_t *capture,
    uint8_t *tx,
    size_t tx_capacity) {

    counter_api_remote_t remote = {
        capture,
        capture_send,
        tx,
        tx_capacity
    };

    return remote;
}

static void expect_remote_counter(
    counter_t value,
    const uint8_t *expected,
    size_t expected_length) {

    uint8_t tx[32] = {0};
    capture_t capture = {0};

    counter_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    assert(
        counter_api_set_counter(
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

static void test_remote_canonical_boundaries(void) {
    static const uint8_t zero[] = {
        32u,
        0x00u
    };

    static const uint8_t first_extended[] = {
        32u,
        0xfbu, 0x00u
    };

    static const uint8_t second_extended[] = {
        32u,
        0xffu, 0xf0u, 0x00u, 0x00u
    };

    static const uint8_t third_extended[] = {
        32u,
        0xffu, 0xffu, 0x00u, 0xffu,
        0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8_t max_value[] = {
        32u,
        0xffu, 0xffu, 0xffu, 0xffu,
        0xffu, 0xffu, 0xffu, 0xffu
    };

    expect_remote_counter(
        UINT64_C(0),
        zero,
        sizeof(zero));

    expect_remote_counter(
        UINT64_C(251),
        first_extended,
        sizeof(first_extended));

    expect_remote_counter(
        UINT64_C(1515),
        second_extended,
        sizeof(second_extended));

    expect_remote_counter(
        UINT64_C(1049835),
        third_extended,
        sizeof(third_extended));

    expect_remote_counter(
        UINT64_C(1099512677610),
        max_value,
        sizeof(max_value));
}

static void test_nullable_api_parameter(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    counter_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    counter_t value =
        UINT64_C(1515);

    assert(
        counter_api_set_optional(
            &remote,
            &value) ==
        AETHER_OK);

    static const uint8_t present[] = {
        33u,
        0x00u,
        0xffu, 0xf0u, 0x00u, 0x00u
    };

    assert(capture.calls == 1u);
    assert(capture.length == sizeof(present));
    assert(
        memcmp(
            capture.data,
            present,
            sizeof(present)) == 0);

    memset(&capture, 0, sizeof(capture));

    assert(
        counter_api_set_optional(
            &remote,
            NULL) ==
        AETHER_OK);

    static const uint8_t absent[] = {
        33u,
        0x01u
    };

    assert(capture.calls == 1u);
    assert(capture.length == sizeof(absent));
    assert(
        memcmp(
            capture.data,
            absent,
            sizeof(absent)) == 0);
}

static void test_struct_round_trip(void) {
    counter_t optional =
        UINT64_C(1515);

    counter_holder_t source = {0};
    source.value = UINT64_C(251);
    source.optionalValue = &optional;
    source.pair.data[0] = UINT64_C(250);
    source.pair.data[1] = UINT64_C(1049835);

    uint8_t wire[64] = {0};
    size_t position = 0u;

    assert(
        tiered_int_serialize_counter_holder(
            wire,
            sizeof(wire),
            &position,
            &source) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x00u,
        0xfbu, 0x00u,
        0xffu, 0xf0u, 0x00u, 0x00u,
        0xfau,
        0xffu, 0xffu, 0x00u, 0xffu,
        0x00u, 0x00u, 0x00u, 0x00u
    };

    assert(position == sizeof(expected));
    assert(
        memcmp(
            wire,
            expected,
            sizeof(expected)) == 0);

    uint8_t scratch[64] = {0};

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = position;
    ctx.scratch = scratch;
    ctx.scratch_capacity = sizeof(scratch);

    counter_holder_t decoded = {0};

    assert(
        tiered_int_deserialize_counter_holder(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == position);
    assert(decoded.value == UINT64_C(251));
    assert(decoded.optionalValue != NULL);
    assert(*decoded.optionalValue == UINT64_C(1515));
    assert(
        (const uint8_t *)decoded.optionalValue >=
        scratch);
    assert(
        (const uint8_t *)decoded.optionalValue <
        scratch + sizeof(scratch));
    assert(decoded.pair.data[0] == UINT64_C(250));
    assert(decoded.pair.data[1] == UINT64_C(1049835));
}

static void test_struct_null_optional(void) {
    counter_holder_t source = {0};
    source.value = UINT64_C(250);
    source.optionalValue = NULL;
    source.pair.data[0] = UINT64_C(0);
    source.pair.data[1] = UINT64_C(251);

    uint8_t wire[32] = {0};
    size_t position = 0u;

    assert(
        tiered_int_serialize_counter_holder(
            wire,
            sizeof(wire),
            &position,
            &source) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x01u,
        0xfau,
        0x00u,
        0xfbu, 0x00u
    };

    assert(position == sizeof(expected));
    assert(
        memcmp(
            wire,
            expected,
            sizeof(expected)) == 0);

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = position;

    counter_holder_t decoded = {0};

    assert(
        tiered_int_deserialize_counter_holder(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == position);
    assert(decoded.value == UINT64_C(250));
    assert(decoded.optionalValue == NULL);
    assert(decoded.pair.data[0] == UINT64_C(0));
    assert(decoded.pair.data[1] == UINT64_C(251));
}

int main(void) {
    test_remote_canonical_boundaries();
    test_nullable_api_parameter();
    test_struct_round_trip();
    test_struct_null_optional();
    return 0;
}

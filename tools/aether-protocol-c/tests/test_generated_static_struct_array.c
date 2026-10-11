
#include "aether_api.h"

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

static ssa_static_struct_array_api_remote_t make_remote(
    capture_t *capture,
    uint8_t *tx,
    size_t tx_capacity) {

    ssa_static_struct_array_api_remote_t remote = {
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

static ssa_static_item_array_2_t make_items(void) {
    ssa_static_item_array_2_t items = {
        {
            { INT16_C(0x1122) },
            { INT16_C(-2) }
        }
    };

    return items;
}

static void test_inline_layout(void) {
    ssa_static_holder_t holder = {0};

    /*
     * Static arrays are inline caller-owned storage. There is no pointer,
     * heap allocation, hidden capacity field, or runtime array descriptor.
     */
    assert(
        sizeof(holder.items.data) ==
        sizeof(ssa_static_item_t) * 2u);

    holder.items.data[0].code =
        INT16_C(7);

    holder.items.data[1].code =
        INT16_C(9);

    assert(holder.items.data[0].code == INT16_C(7));
    assert(holder.items.data[1].code == INT16_C(9));
}

static void test_holder_round_trip(void) {
    ssa_static_holder_t source = {
        make_items()
    };

    uint8_t wire[16] = {0};
    size_t position = 0u;

    assert(
        ssa_serialize_static_holder(
            wire,
            sizeof(wire),
            &position,
            &source) ==
        AETHER_OK);

    /*
     * Java-compatible fixed-array wire form has no length prefix:
     * two StaticItem payloads are written directly in declaration order.
     */
    static const uint8_t expected[] = {
        0x22u, 0x11u,
        0xfeu, 0xffu
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

    ssa_static_holder_t decoded = {0};

    assert(
        ssa_deserialize_static_holder(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(expected));
    assert(decoded.items.data[0].code == INT16_C(0x1122));
    assert(decoded.items.data[1].code == INT16_C(-2));

    uint8_t second_wire[16] = {0};
    size_t second_position = 0u;

    assert(
        ssa_serialize_static_holder(
            second_wire,
            sizeof(second_wire),
            &second_position,
            &decoded) ==
        AETHER_OK);

    assert(second_position == sizeof(expected));
    assert(
        memcmp(
            second_wire,
            expected,
            sizeof(expected)) == 0);
}

static void test_holder_api_wire(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    ssa_static_struct_array_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    ssa_static_holder_t holder = {
        make_items()
    };

    assert(
        ssa_static_struct_array_api_send_holder(
            &remote,
            &holder) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x07u,
        0x22u, 0x11u,
        0xfeu, 0xffu
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_api_only_static_array_wire(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    ssa_static_struct_array_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    ssa_static_item_array_2_t items =
        make_items();

    assert(
        ssa_static_struct_array_api_send_items(
            &remote,
            &items) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x08u,
        0x22u, 0x11u,
        0xfeu, 0xffu
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_truncated_decode(void) {
    static const uint8_t wire[] = {
        0x22u, 0x11u,
        0xfeu
    };

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);

    ssa_static_holder_t value = {0};

    assert(
        ssa_deserialize_static_holder(
            &ctx,
            &value) ==
        AETHER_ERR_PROTOCOL);
}

int main(void) {
    test_inline_layout();
    test_holder_round_trip();
    test_holder_api_wire();
    test_api_only_static_array_wire();
    test_truncated_decode();

    return 0;
}
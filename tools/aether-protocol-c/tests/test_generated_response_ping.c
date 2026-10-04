
#include "response_method_ping_response.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


/*
 * returns: void is a response-bearing method. The decoder must read
 * command and request_id and stop, without attempting to decode a
 * result payload. The frame deliberately has no result member.
 */
static void test_void_result(void) {
    static const uint8_t wire[] = {
        0u,
        0x78u, 0x56u, 0x34u, 0x12u
    };

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);

    global_api_ping_response_frame_t frame = {0};

    assert(
        global_api_ping_response_decode(
            &ctx,
            &frame) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(wire));
    assert(frame.kind == GLOBAL_API_PING_RESPONSE_RESULT);
    assert(frame.request_id == UINT32_C(0x12345678));
}


static void test_void_error(void) {
    static const uint8_t wire[] = {
        1u,
        0xaau, 0xbbu, 0xccu, 0xddu
    };

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);

    global_api_ping_response_frame_t frame = {0};

    assert(
        global_api_ping_response_decode(
            &ctx,
            &frame) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(wire));
    assert(frame.kind == GLOBAL_API_PING_RESPONSE_ERROR);
    assert(frame.request_id == UINT32_C(0xddccbbaa));
}


static void test_void_concatenated(void) {
    static const uint8_t wire[] = {
        0u,
        0x01u, 0x00u, 0x00u, 0x00u,
        1u,
        0x02u, 0x00u, 0x00u, 0x00u
    };

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);

    global_api_ping_response_frame_t first = {0};

    assert(
        global_api_ping_response_decode(
            &ctx,
            &first) ==
        AETHER_OK);

    assert(ctx.reader.position == 5u);
    assert(first.kind == GLOBAL_API_PING_RESPONSE_RESULT);
    assert(first.request_id == 1u);

    global_api_ping_response_frame_t second = {0};

    assert(
        global_api_ping_response_decode(
            &ctx,
            &second) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(wire));
    assert(second.kind == GLOBAL_API_PING_RESPONSE_ERROR);
    assert(second.request_id == 2u);
}


static void test_truncated_void(void) {
    static const uint8_t wire[] = { 0u, 0x01u, 0x02u };

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);

    global_api_ping_response_frame_t frame = {0};

    assert(
        global_api_ping_response_decode(
            &ctx,
            &frame) ==
        AETHER_ERR_PROTOCOL);
}


int main(void) {
    test_void_result();
    test_void_error();
    test_void_concatenated();
    test_truncated_void();

    return 0;
}

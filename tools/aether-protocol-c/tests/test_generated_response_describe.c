
#include "response_method_describe_response.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


/*
 * Anonymous result with nullable fields. The generated result struct
 * must use canonical pointer shape for nullable fields (same shape as a
 * normal generated struct), and the decode must go through the generic
 * nullable mask path.
 */
static void test_nullable_anonymous_result(void) {
    /*
     * mask byte: bit0 = maybeCode NULL, bit1 = maybeName present,
     *            bit2 = maybeNested present
     * required = 0x2a
     * maybeCode absent
     * maybeName = "hi"
     * maybeNested = Cloud { data: short[1] = 0x1234 }
     */
    static const uint8_t wire[] = {
        0u,
        0x78u, 0x56u, 0x34u, 0x12u,
        0x01u,
        0x2au,
        0x02u, 0x68u, 0x69u,
        0x01u, 0x34u, 0x12u
    };

    uint8_t scratch[256] = {0};
    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);
    ctx.scratch = scratch;
    ctx.scratch_capacity = sizeof(scratch);

    global_api_describe_response_frame_t frame = {0};

    assert(
        global_api_describe_response_decode(
            &ctx,
            &frame) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(wire));
    assert(frame.kind == GLOBAL_API_DESCRIBE_RESPONSE_RESULT);
    assert(frame.request_id == UINT32_C(0x12345678));

    assert(frame.result.required == INT8_C(0x2a));
    assert(frame.result.maybeCode == NULL);
    assert(frame.result.maybeName != NULL);
    assert(frame.result.maybeName->length == 2u);
    assert(frame.result.maybeName->data[0] == 0x68u);
    assert(frame.result.maybeName->data[1] == 0x69u);
    assert(frame.result.maybeNested != NULL);
    assert(frame.result.maybeNested->data.length == 1u);
    assert(frame.result.maybeNested->data.data[0] == INT16_C(0x1234));
}


static void test_all_nullable(void) {
    static const uint8_t wire[] = {
        0u,
        0x00u, 0x00u, 0x00u, 0x00u,
        0x07u,
        0x11u
    };

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);

    global_api_describe_response_frame_t frame = {0};

    assert(
        global_api_describe_response_decode(
            &ctx,
            &frame) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(wire));
    assert(frame.result.required == INT8_C(0x11));
    assert(frame.result.maybeCode == NULL);
    assert(frame.result.maybeName == NULL);
    assert(frame.result.maybeNested == NULL);
}


int main(void) {
    test_nullable_anonymous_result();
    test_all_nullable();

    return 0;
}
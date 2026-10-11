
#include "aether_api.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


static void write_uuid(
    uint8_t *data,
    uint64_t msb,
    uint64_t lsb) {

    for (size_t i = 0u;
         i < 8u;
         ++i) {

        data[i] =
            (uint8_t)(
                msb >>
                ((7u - i) * 8u));

        data[8u + i] =
            (uint8_t)(
                lsb >>
                ((7u - i) * 8u));
    }
}


static void test_result(void) {
    uint8_t frame[44] = {0};
    uint8_t scratch[512] = {0};

    frame[0] = 0u;
    frame[1] = 0x78u;
    frame[2] = 0x56u;
    frame[3] = 0x34u;
    frame[4] = 0x12u;

    write_uuid(
        frame + 5u,
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718));

    write_uuid(
        frame + 21u,
        UINT64_C(0x2122232425262728),
        UINT64_C(0x3132333435363738));

    frame[37] = 3u;

    frame[38] = 0x34u;
    frame[39] = 0x12u;

    frame[40] = 0xfeu;
    frame[41] = 0xffu;

    frame[42] = 0xffu;
    frame[43] = 0x7fu;

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = frame;
    ctx.reader.length = sizeof(frame);
    ctx.scratch = scratch;
    ctx.scratch_capacity = sizeof(scratch);

    rm_global_api_finish_response_frame_t decoded = {0};

    assert(
        rm_global_api_finish_response_decode(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(frame));
    assert(decoded.kind == RM_GLOBAL_API_FINISH_RESPONSE_RESULT);
    assert(decoded.request_id == UINT32_C(0x12345678));

    assert(
        decoded.result.alias.msb ==
        UINT64_C(0x0102030405060708));

    assert(
        decoded.result.alias.lsb ==
        UINT64_C(0x1112131415161718));

    assert(
        decoded.result.uid.msb ==
        UINT64_C(0x2122232425262728));

    assert(
        decoded.result.uid.lsb ==
        UINT64_C(0x3132333435363738));

    assert(decoded.result.cloud.data.length == 3u);
    assert(
        (const uint8_t *)decoded.result.cloud.data.data >= scratch);
    assert(
        (const uint8_t *)decoded.result.cloud.data.data <
        scratch + sizeof(scratch));
    assert(decoded.result.cloud.data.data[0] == INT16_C(0x1234));
    assert(decoded.result.cloud.data.data[1] == -2);
    assert(decoded.result.cloud.data.data[2] == INT16_C(0x7fff));
}


static void test_error(void) {
    uint8_t frame[] = {
        1u,
        0x04u,
        0x03u,
        0x02u,
        0x01u
    };

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = frame;
    ctx.reader.length = sizeof(frame);

    rm_global_api_finish_response_frame_t decoded = {0};

    assert(
        rm_global_api_finish_response_decode(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(frame));
    assert(decoded.kind == RM_GLOBAL_API_FINISH_RESPONSE_ERROR);
    assert(decoded.request_id == UINT32_C(0x01020304));
}


static void test_pack_251(void) {
    uint8_t frame[
        5u +
        16u +
        16u +
        2u +
        251u * 2u] = {0};
    uint8_t scratch[1024] = {0};

    frame[0] = 0u;
    frame[1] = 1u;

    size_t p =
        5u + 16u + 16u;

    frame[p++] = 251u;
    frame[p++] = 0u;

    for (size_t i = 0u;
         i < 251u;
         ++i) {

        int16_t value =
            (int16_t)((int)i - 125);

        uint16_t raw =
            (uint16_t)value;

        frame[p++] =
            (uint8_t)raw;

        frame[p++] =
            (uint8_t)(raw >> 8u);
    }

    assert(p == sizeof(frame));

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = frame;
    ctx.reader.length = sizeof(frame);
    ctx.scratch = scratch;
    ctx.scratch_capacity = sizeof(scratch);

    rm_global_api_finish_response_frame_t decoded = {0};

    assert(
        rm_global_api_finish_response_decode(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(frame));
    assert(decoded.result.cloud.data.length == 251u);
    assert(decoded.result.cloud.data.data[0] == -125);
    assert(decoded.result.cloud.data.data[125] == 0);
    assert(decoded.result.cloud.data.data[250] == 125);
}


static void test_scratch_overflow(void) {
    uint8_t frame[
        5u +
        16u +
        16u +
        2u +
        3u * 2u] = {0};

    frame[0] = 0u;
    frame[37] = 3u;

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = frame;
    ctx.reader.length = sizeof(frame);

    rm_global_api_finish_response_frame_t decoded = {0};

    assert(
        rm_global_api_finish_response_decode(
            &ctx,
            &decoded) ==
        AETHER_ERR_OVERFLOW);
}


static void test_truncated(void) {
    uint8_t truncated[] = {
        0u,
        1u,
        2u,
        3u
    };

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = truncated;
    ctx.reader.length = sizeof(truncated);

    rm_global_api_finish_response_frame_t decoded = {0};

    assert(
        rm_global_api_finish_response_decode(
            &ctx,
            &decoded) ==
        AETHER_ERR_PROTOCOL);
}


static void test_concatenated(void) {
    uint8_t frames[43] = {0};

    frames[0] = 0u;
    frames[1] = 7u;
    frames[37] = 0u;

    const size_t first_length = 38u;

    frames[first_length] = 1u;
    frames[first_length + 1u] = 9u;

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = frames;
    ctx.reader.length = sizeof(frames);

    rm_global_api_finish_response_frame_t decoded = {0};

    assert(
        rm_global_api_finish_response_decode(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == first_length);
    assert(decoded.kind == RM_GLOBAL_API_FINISH_RESPONSE_RESULT);

    /*
     * The decoder reads one object and leaves the cursor on the next byte,
     * so a second frame in the same buffer decodes cleanly without a
     * separate "consumed" output.
     */
    rm_global_api_finish_response_frame_t second = {0};

    assert(
        rm_global_api_finish_response_decode(
            &ctx,
            &second) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(frames));
    assert(second.kind == RM_GLOBAL_API_FINISH_RESPONSE_ERROR);
    assert(second.request_id == 9u);
}


int main(void) {
    test_result();
    test_error();
    test_pack_251();
    test_scratch_overflow();
    test_truncated();
    test_concatenated();

    return 0;
}
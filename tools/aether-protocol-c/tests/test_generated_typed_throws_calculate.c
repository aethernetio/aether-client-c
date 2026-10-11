
#include "aether_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


static aether_meta_read_context_t make_context(
    const uint8_t *data,
    size_t length) {

    aether_meta_read_context_t ctx = {0};

    ctx.reader.data =
        data;

    ctx.reader.length =
        length;

    return ctx;
}


static size_t make_error_frame(
    uint8_t *wire,
    size_t capacity,
    uint32_t request_id,
    int16_t code,
    const uint8_t *message_data,
    size_t message_length) {

    assert(wire != NULL);
    assert(capacity >= 5u);

    wire[0] = 1u;
    wire[1] = (uint8_t)(request_id & UINT32_C(0xff));
    wire[2] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    wire[3] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    wire[4] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    aether_bytes_view_t message = {
        message_data,
        message_length
    };

    typed_failure_t failure = {
        code,
        message
    };

    size_t position = 5u;

    assert(
        typed_throws_serialize_typed_failure(
            wire,
            capacity,
            &position,
            &failure) ==
        AETHER_OK);

    return position;
}


static void test_result_frame(void) {
    static const uint8_t wire[] = {
        0u,
        0x44u, 0x33u, 0x22u, 0x11u,
        0x66u, 0x55u
    };

    aether_meta_read_context_t ctx =
        make_context(
            wire,
            sizeof(wire));

    typed_throws_api_calculate_response_frame_t frame = {0};

    assert(
        typed_throws_api_calculate_response_decode(
            &ctx,
            &frame) ==
        AETHER_OK);

    assert(
        frame.kind ==
        TYPED_THROWS_API_CALCULATE_RESPONSE_RESULT);

    assert(
        frame.request_id ==
        UINT32_C(0x11223344));

    assert(
        frame.result.value ==
        INT16_C(0x5566));

    assert(
        ctx.reader.position ==
        sizeof(wire));
}


static void test_typed_error_frame(void) {
    static const uint8_t message_data[] = {
        'b', 'a', 'd'
    };

    uint8_t wire[64] = {0};

    size_t length =
        make_error_frame(
            wire,
            sizeof(wire),
            UINT32_C(0xa1b2c3d4),
            INT16_C(-321),
            message_data,
            sizeof(message_data));

    /*
     * Java META_ERROR envelope is command 1 followed by request id.
     * The declared throws value starts immediately at byte 5.
     */
    static const uint8_t expected_prefix[] = {
        1u,
        0xd4u, 0xc3u, 0xb2u, 0xa1u
    };

    assert(
        memcmp(
            wire,
            expected_prefix,
            sizeof(expected_prefix)) == 0);

    aether_meta_read_context_t ctx =
        make_context(
            wire,
            length);

    typed_throws_api_calculate_response_frame_t frame = {0};

    assert(
        typed_throws_api_calculate_response_decode(
            &ctx,
            &frame) ==
        AETHER_OK);

    assert(
        frame.kind ==
        TYPED_THROWS_API_CALCULATE_RESPONSE_ERROR);

    assert(
        frame.request_id ==
        UINT32_C(0xa1b2c3d4));

    assert(
        frame.error.value.code ==
        INT16_C(-321));

    assert(
        frame.error.value.message.length ==
        sizeof(message_data));

    /*
     * String remains a borrowed RX view; typed throws does not allocate.
     */
    assert(
        frame.error.value.message.data >=
        wire);

    assert(
        frame.error.value.message.data <
        wire + length);

    assert(
        memcmp(
            frame.error.value.message.data,
            message_data,
            sizeof(message_data)) == 0);

    assert(
        ctx.reader.position ==
        length);
}


static void test_truncated_error_rejected(void) {
    static const uint8_t message_data[] = {
        'x', 'y'
    };

    uint8_t wire[64] = {0};

    size_t length =
        make_error_frame(
            wire,
            sizeof(wire),
            UINT32_C(7),
            INT16_C(9),
            message_data,
            sizeof(message_data));

    assert(length > 5u);

    aether_meta_read_context_t ctx =
        make_context(
            wire,
            length - 1u);

    typed_throws_api_calculate_response_frame_t frame = {0};

    assert(
        typed_throws_api_calculate_response_decode(
            &ctx,
            &frame) ==
        AETHER_ERR_PROTOCOL);
}


static void test_unknown_meta_command_rejected(void) {
    static const uint8_t wire[] = {
        2u,
        1u, 0u, 0u, 0u
    };

    aether_meta_read_context_t ctx =
        make_context(
            wire,
            sizeof(wire));

    typed_throws_api_calculate_response_frame_t frame = {0};

    assert(
        typed_throws_api_calculate_response_decode(
            &ctx,
            &frame) ==
        AETHER_ERR_PROTOCOL);
}


int main(void) {
    test_result_frame();
    test_typed_error_frame();
    test_truncated_error_rejected();
    test_unknown_meta_command_rejected();

    return 0;
}
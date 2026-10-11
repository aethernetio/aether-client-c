
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
    int16_t code) {

    assert(wire != NULL);
    assert(capacity >= 5u);

    wire[0] = 1u;
    wire[1] = (uint8_t)(request_id & UINT32_C(0xff));
    wire[2] = (uint8_t)((request_id >> 8u) & UINT32_C(0xff));
    wire[3] = (uint8_t)((request_id >> 16u) & UINT32_C(0xff));
    wire[4] = (uint8_t)((request_id >> 24u) & UINT32_C(0xff));

    static const uint8_t message_data[] = {
        'e', 'r', 'r'
    };

    aether_bytes_view_t message = {
        message_data,
        sizeof(message_data)
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


static void test_success_has_no_payload(void) {
    /*
     * A throws-only Java method is still response-bearing. Successful
     * completion is exactly META_RESULT + request_id with no value payload.
     */
    static const uint8_t wire[] = {
        0u,
        0x78u, 0x56u, 0x34u, 0x12u
    };

    aether_meta_read_context_t ctx =
        make_context(
            wire,
            sizeof(wire));

    typed_throws_api_notify_response_frame_t frame = {0};

    assert(
        typed_throws_api_notify_response_decode(
            &ctx,
            &frame) ==
        AETHER_OK);

    assert(
        frame.kind ==
        TYPED_THROWS_API_NOTIFY_RESPONSE_RESULT);

    assert(
        frame.request_id ==
        UINT32_C(0x12345678));

    assert(
        ctx.reader.position ==
        sizeof(wire));
}


static void test_typed_error(void) {
    uint8_t wire[64] = {0};

    size_t length =
        make_error_frame(
            wire,
            sizeof(wire),
            UINT32_C(0x01020304),
            INT16_C(1234));

    aether_meta_read_context_t ctx =
        make_context(
            wire,
            length);

    typed_throws_api_notify_response_frame_t frame = {0};

    assert(
        typed_throws_api_notify_response_decode(
            &ctx,
            &frame) ==
        AETHER_OK);

    assert(
        frame.kind ==
        TYPED_THROWS_API_NOTIFY_RESPONSE_ERROR);

    assert(
        frame.request_id ==
        UINT32_C(0x01020304));

    assert(
        frame.error.value.code ==
        INT16_C(1234));

    assert(
        frame.error.value.message.length ==
        3u);

    assert(
        memcmp(
            frame.error.value.message.data,
            "err",
            3u) == 0);

    assert(
        ctx.reader.position ==
        length);
}


static void test_truncated_request_id_rejected(void) {
    static const uint8_t wire[] = {
        1u,
        0x01u,
        0x02u,
        0x03u
    };

    aether_meta_read_context_t ctx =
        make_context(
            wire,
            sizeof(wire));

    typed_throws_api_notify_response_frame_t frame = {0};

    assert(
        typed_throws_api_notify_response_decode(
            &ctx,
            &frame) ==
        AETHER_ERR_PROTOCOL);
}


int main(void) {
    test_success_has_no_payload();
    test_typed_error();
    test_truncated_request_id_rejected();

    return 0;
}
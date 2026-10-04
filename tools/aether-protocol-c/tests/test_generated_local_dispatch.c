
#include "local_dispatch_local.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


typedef struct {
    size_t enter_calls;
    size_t enter_global_calls;
    size_t send_message_calls;
    size_t send_packet_calls;
    size_t send_inner_calls;
    size_t send_event_calls;
    size_t send_holder_calls;
    size_t noop_calls;
    size_t ping_void_calls;
    size_t query_value_calls;

    const uint8_t *last_stream_data;
    size_t last_stream_length;

    const uint8_t *last_message_data;
    size_t last_message_length;

    aether_uuid_t last_id;
    level_t last_level;
    const uint8_t *last_payload_data;
    size_t last_payload_length;

    int16_t last_inner_code;

    uint8_t last_event_type_id;
    int16_t last_event_value;

    size_t last_holder_bytes_length;
    const uint8_t *last_holder_bytes_data;

    uint32_t last_ping_request_id;
    int16_t last_ping_code;

    uint32_t last_query_request_id;
    int16_t last_query_code;
} capture_t;


static aether_status_t on_enter(
    void *ctx,
    const safe_stream_t *stream) {

    capture_t *capture = ctx;
    assert(capture != NULL);
    assert(stream != NULL);
    capture->enter_calls++;
    capture->last_stream_data = stream->data;
    capture->last_stream_length = stream->length;
    return AETHER_OK;
}


static aether_status_t on_enter_global(
    void *ctx,
    const global_stream_t *stream) {

    capture_t *capture = ctx;
    assert(capture != NULL);
    assert(stream != NULL);
    capture->enter_global_calls++;
    capture->last_stream_data = stream->data;
    capture->last_stream_length = stream->length;
    return AETHER_OK;
}


static aether_status_t on_send_message(
    void *ctx,
    aether_bytes_view_t data) {

    capture_t *capture = ctx;
    assert(capture != NULL);
    capture->send_message_calls++;
    capture->last_message_data = data.data;
    capture->last_message_length = data.length;
    return AETHER_OK;
}


static aether_status_t on_send_packet(
    void *ctx,
    aether_uuid_t id,
    level_t level,
    aether_bytes_view_t payload) {

    capture_t *capture = ctx;
    assert(capture != NULL);
    capture->send_packet_calls++;
    capture->last_id = id;
    capture->last_level = level;
    capture->last_payload_data = payload.data;
    capture->last_payload_length = payload.length;
    return AETHER_OK;
}


static aether_status_t on_send_inner(
    void *ctx,
    const inner_t *inner) {

    capture_t *capture = ctx;
    assert(capture != NULL);
    assert(inner != NULL);
    capture->send_inner_calls++;
    capture->last_inner_code = inner->code;
    return AETHER_OK;
}


static aether_status_t on_send_event(
    void *ctx,
    const event_ref_t *event) {

    capture_t *capture = ctx;
    assert(capture != NULL);
    assert(event != NULL);
    capture->send_event_calls++;
    capture->last_event_type_id = event->type_id;
    if (event->type_id == 7u && event->as.temp_event != NULL) {
        capture->last_event_value = event->as.temp_event->value;
    }
    return AETHER_OK;
}


static aether_status_t on_send_holder(
    void *ctx,
    const array_holder_t *holder) {

    capture_t *capture = ctx;
    assert(capture != NULL);
    assert(holder != NULL);
    capture->send_holder_calls++;
    capture->last_holder_bytes_length = holder->bytes.length;
    capture->last_holder_bytes_data = holder->bytes.data;
    return AETHER_OK;
}


static aether_status_t on_noop(void *ctx) {
    capture_t *capture = ctx;
    assert(capture != NULL);
    capture->noop_calls++;
    return AETHER_OK;
}


static aether_status_t on_ping_void(
    void *ctx,
    uint32_t request_id,
    int16_t code) {

    capture_t *capture = ctx;
    assert(capture != NULL);
    capture->ping_void_calls++;
    capture->last_ping_request_id = request_id;
    capture->last_ping_code = code;
    return AETHER_OK;
}


static aether_status_t on_query_value(
    void *ctx,
    uint32_t request_id,
    int16_t code) {

    capture_t *capture = ctx;
    assert(capture != NULL);
    capture->query_value_calls++;
    capture->last_query_request_id = request_id;
    capture->last_query_code = code;
    return AETHER_OK;
}


static local_api_local_t make_local(capture_t *capture) {
    local_api_local_t local = {
        .ctx = capture,
        .enter = on_enter,
        .enter_global = on_enter_global,
        .send_message = on_send_message,
        .send_packet = on_send_packet,
        .send_inner = on_send_inner,
        .send_event = on_send_event,
        .send_holder = on_send_holder,
        .noop = on_noop,
        .ping_void = on_ping_void,
        .query_value = on_query_value
    };
    return local;
}


static aether_meta_read_context_t make_ctx(
    const uint8_t *data,
    size_t length,
    uint8_t *scratch,
    size_t scratch_capacity) {

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = data;
    ctx.reader.length = length;
    ctx.scratch = scratch;
    ctx.scratch_capacity = scratch_capacity;
    return ctx;
}


static void test_byte_array_zero_copy(void) {
    uint8_t frame[] = {
        5u,
        0x03u, 0xa1u, 0xb2u, 0xc3u
    };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[64] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);

    assert(ctx.reader.position == sizeof(frame));
    assert(capture.send_message_calls == 1u);
    assert(capture.last_message_length == 3u);
    /* Zero-copy: pointer into RX buffer. */
    assert(capture.last_message_data == frame + 2u);
    assert(capture.last_message_data[0] == 0xa1u);
    assert(capture.last_message_data[2] == 0xc3u);
}


static void test_multiple_primitive_params(void) {
    uint8_t frame[] = {
        6u,
        /* UUID msb */ 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u, 0x08u,
        /* UUID lsb */ 0x11u, 0x12u, 0x13u, 0x14u, 0x15u, 0x16u, 0x17u, 0x18u,
        /* level HIGH = 2 */
        0x02u,
        /* payload byte[] */
        0x02u, 0xddu, 0xeeu
    };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[64] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);

    assert(ctx.reader.position == sizeof(frame));
    assert(capture.send_packet_calls == 1u);
    assert(capture.last_id.msb == UINT64_C(0x0102030405060708));
    assert(capture.last_id.lsb == UINT64_C(0x1112131415161718));
    assert(capture.last_level == level_HIGH);
    assert(capture.last_payload_length == 2u);
    assert(capture.last_payload_data == frame + 19u);
}


static void test_nested_struct_param(void) {
    uint8_t frame[] = {
        7u,
        0x34u, 0x12u
    };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[64] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);

    assert(ctx.reader.position == sizeof(frame));
    assert(capture.send_inner_calls == 1u);
    assert(capture.last_inner_code == INT16_C(0x1234));
}


static void test_hierarchy_param(void) {
    uint8_t frame[] = {
        8u,
        0x07u,
        0x2au,
        0x34u, 0x12u
    };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[128] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);

    assert(ctx.reader.position == sizeof(frame));
    assert(capture.send_event_calls == 1u);
    assert(capture.last_event_type_id == 7u);
    assert(capture.last_event_value == INT16_C(0x1234));
}


static void test_array_holder_param(void) {
    uint8_t frame[] = {
        9u,
        /* bytes byte[] */
        0x02u, 0xaau, 0xbbu,
        /* shorts short[] */
        0x01u, 0x34u, 0x12u,
        /* items Item[] */
        0x00u,
        /* fixed byte[4] */
        0x01u, 0x02u, 0x03u, 0x04u
    };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[128] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);

    assert(ctx.reader.position == sizeof(frame));
    assert(capture.send_holder_calls == 1u);
    assert(capture.last_holder_bytes_length == 2u);
    assert(capture.last_holder_bytes_data == frame + 2u);
}


static void test_zero_param_method(void) {
    uint8_t frame[] = { 10u };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[8] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);
    assert(ctx.reader.position == sizeof(frame));
    assert(capture.noop_calls == 1u);
}


static void test_void_response_method(void) {
    uint8_t frame[] = {
        11u,
        0x78u, 0x56u, 0x34u, 0x12u,
        0x34u, 0x12u
    };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[8] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);
    assert(ctx.reader.position == sizeof(frame));
    assert(capture.ping_void_calls == 1u);
    assert(capture.last_ping_request_id == UINT32_C(0x12345678));
    assert(capture.last_ping_code == INT16_C(0x1234));
}


static void test_typed_response_method(void) {
    uint8_t frame[] = {
        12u,
        0xd4u, 0xc3u, 0xb2u, 0xa1u,
        0x34u, 0x12u
    };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[8] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);
    assert(ctx.reader.position == sizeof(frame));
    assert(capture.query_value_calls == 1u);
    assert(capture.last_query_request_id == UINT32_C(0xa1b2c3d4));
    assert(capture.last_query_code == INT16_C(0x1234));
}


static void test_stream_zero_copy(void) {
    uint8_t frame[] = {
        3u,
        0x03u, 0xa1u, 0xb2u, 0xc3u
    };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[8] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);
    assert(ctx.reader.position == sizeof(frame));
    assert(capture.enter_calls == 1u);
    assert(capture.enter_global_calls == 0u);
    assert(capture.last_stream_data == frame + 2u);
    assert(capture.last_stream_length == 3u);
}


static void test_stream_two_byte_pack(void) {
    uint8_t frame[254] = {0};
    frame[0] = 4u;
    frame[1] = 251u;
    frame[2] = 0u;
    for (size_t i = 0u; i < 251u; ++i) {
        frame[3u + i] = (uint8_t)i;
    }

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[8] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);
    assert(ctx.reader.position == sizeof(frame));
    assert(capture.enter_global_calls == 1u);
    assert(capture.last_stream_data == frame + 3u);
    assert(capture.last_stream_length == 251u);
}


static void test_concatenated_commands(void) {
    uint8_t frame[] = {
        10u,
        5u,
        0x01u, 0x11u
    };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[64] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);
    assert(ctx.reader.position == 1u);
    assert(capture.noop_calls == 1u);

    assert(local_api_dispatch(&local, &ctx) == AETHER_OK);
    assert(ctx.reader.position == sizeof(frame));
    assert(capture.send_message_calls == 1u);
    assert(capture.last_message_length == 1u);
    assert(capture.last_message_data == frame + 3u);
}


static void test_truncated_input_rejected(void) {
    uint8_t frame[] = {
        12u,
        0xd4u, 0xc3u
    };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[8] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_ERR_PROTOCOL);
    assert(capture.query_value_calls == 0u);
}


static void test_unknown_command_rejected(void) {
    uint8_t frame[] = { 99u };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[8] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_ERR_PROTOCOL);
}


static void test_missing_callback_rejected(void) {
    uint8_t frame[] = { 10u };

    local_api_local_t local = {0};

    uint8_t scratch[8] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(&local, &ctx) == AETHER_ERR_ARGUMENT);
}


static void test_argument_validation(void) {
    uint8_t frame[] = { 10u };

    capture_t capture = {0};
    local_api_local_t local = make_local(&capture);

    uint8_t scratch[8] = {0};
    aether_meta_read_context_t ctx =
        make_ctx(frame, sizeof(frame), scratch, sizeof(scratch));

    assert(local_api_dispatch(NULL, &ctx) == AETHER_ERR_ARGUMENT);
    assert(local_api_dispatch(&local, NULL) == AETHER_ERR_ARGUMENT);
}


int main(void) {
    test_byte_array_zero_copy();
    test_multiple_primitive_params();
    test_nested_struct_param();
    test_hierarchy_param();
    test_array_holder_param();
    test_zero_param_method();
    test_void_response_method();
    test_typed_response_method();
    test_stream_zero_copy();
    test_stream_two_byte_pack();
    test_concatenated_commands();
    test_truncated_input_rejected();
    test_unknown_command_rejected();
    test_missing_callback_rejected();
    test_argument_validation();

    return 0;
}
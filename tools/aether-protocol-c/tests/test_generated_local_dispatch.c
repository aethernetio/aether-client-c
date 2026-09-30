

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
    size_t global_calls;

    const uint8_t *last_data;
    size_t last_length;
} capture_t;


static aether_status_t on_enter(
    void *ctx,
    const safe_stream_t *stream) {

    capture_t *capture =
        ctx;

    assert(capture != NULL);
    assert(stream != NULL);

    capture->enter_calls++;
    capture->last_data =
        stream->data;
    capture->last_length =
        stream->length;

    return AETHER_OK;
}


static aether_status_t on_enter_global(
    void *ctx,
    const global_stream_t *stream) {

    capture_t *capture =
        ctx;

    assert(capture != NULL);
    assert(stream != NULL);

    capture->global_calls++;
    capture->last_data =
        stream->data;
    capture->last_length =
        stream->length;

    return AETHER_OK;
}


static local_api_local_t make_local(
    capture_t *capture) {

    local_api_local_t local = {
        .ctx = capture,
        .enter = on_enter,
        .enter_global = on_enter_global
    };

    return local;
}


static void test_small_stream_is_zero_copy(void) {
    uint8_t frame[] = {
        3u,
        3u,
        0xa1u,
        0xb2u,
        0xc3u
    };

    capture_t capture = {0};

    local_api_local_t local =
        make_local(
            &capture);

    size_t consumed =
        99u;

    assert(
        local_api_dispatch(
            &local,
            frame,
            sizeof(frame),
            &consumed) ==
        AETHER_OK);

    assert(consumed == sizeof(frame));
    assert(capture.enter_calls == 1u);
    assert(capture.global_calls == 0u);
    assert(capture.last_data == frame + 2u);
    assert(capture.last_length == 3u);

    static const uint8_t expected[] = {
        0xa1u,
        0xb2u,
        0xc3u
    };

    assert(
        memcmp(
            capture.last_data,
            expected,
            sizeof(expected)) == 0);
}


static void test_two_byte_pack_stream(void) {
    uint8_t frame[254] = {0};

    frame[0] =
        4u;

    /*
     * Canonical PackNumber(251):
     * first byte 251, second byte 0.
     */
    frame[1] =
        251u;

    frame[2] =
        0u;

    for (size_t i = 0u;
         i < 251u;
         ++i) {

        frame[3u + i] =
            (uint8_t)i;
    }

    capture_t capture = {0};

    local_api_local_t local =
        make_local(
            &capture);

    size_t consumed =
        0u;

    assert(
        local_api_dispatch(
            &local,
            frame,
            sizeof(frame),
            &consumed) ==
        AETHER_OK);

    assert(consumed == sizeof(frame));
    assert(capture.enter_calls == 0u);
    assert(capture.global_calls == 1u);
    assert(capture.last_data == frame + 3u);
    assert(capture.last_length == 251u);
    assert(capture.last_data[0] == 0u);
    assert(capture.last_data[250] == 250u);
}


static void test_concatenated_frames_use_consumed(void) {
    uint8_t frames[] = {
        3u,
        1u,
        0x11u,

        4u,
        2u,
        0x22u,
        0x33u
    };

    capture_t capture = {0};

    local_api_local_t local =
        make_local(
            &capture);

    size_t first =
        0u;

    assert(
        local_api_dispatch(
            &local,
            frames,
            sizeof(frames),
            &first) ==
        AETHER_OK);

    assert(first == 3u);
    assert(capture.enter_calls == 1u);

    size_t second =
        0u;

    assert(
        local_api_dispatch(
            &local,
            frames + first,
            sizeof(frames) - first,
            &second) ==
        AETHER_OK);

    assert(second == 4u);
    assert(first + second == sizeof(frames));
    assert(capture.global_calls == 1u);
}


static void test_truncated_pack_rejected_without_callback(void) {
    uint8_t frame[] = {
        3u,
        251u
    };

    capture_t capture = {0};

    local_api_local_t local =
        make_local(
            &capture);

    size_t consumed =
        77u;

    assert(
        local_api_dispatch(
            &local,
            frame,
            sizeof(frame),
            &consumed) ==
        AETHER_ERR_PROTOCOL);

    assert(consumed == 0u);
    assert(capture.enter_calls == 0u);
    assert(capture.global_calls == 0u);
}


static void test_truncated_payload_rejected_without_callback(void) {
    uint8_t frame[] = {
        3u,
        3u,
        0xaau,
        0xbbu
    };

    capture_t capture = {0};

    local_api_local_t local =
        make_local(
            &capture);

    size_t consumed =
        88u;

    assert(
        local_api_dispatch(
            &local,
            frame,
            sizeof(frame),
            &consumed) ==
        AETHER_ERR_PROTOCOL);

    assert(consumed == 0u);
    assert(capture.enter_calls == 0u);
    assert(capture.global_calls == 0u);
}


static void test_unknown_method_rejected(void) {
    uint8_t frame[] = {
        9u
    };

    capture_t capture = {0};

    local_api_local_t local =
        make_local(
            &capture);

    size_t consumed =
        1u;

    assert(
        local_api_dispatch(
            &local,
            frame,
            sizeof(frame),
            &consumed) ==
        AETHER_ERR_PROTOCOL);

    assert(consumed == 0u);
}


static void test_missing_callback_rejected(void) {
    uint8_t frame[] = {
        3u,
        0u
    };

    local_api_local_t local = {0};

    size_t consumed =
        1u;

    assert(
        local_api_dispatch(
            &local,
            frame,
            sizeof(frame),
            &consumed) ==
        AETHER_ERR_ARGUMENT);

    assert(consumed == 0u);
}


static void test_argument_validation(void) {
    capture_t capture = {0};

    local_api_local_t local =
        make_local(
            &capture);

    uint8_t frame[] = {
        3u,
        0u
    };

    size_t consumed =
        0u;

    assert(
        local_api_dispatch(
            NULL,
            frame,
            sizeof(frame),
            &consumed) ==
        AETHER_ERR_ARGUMENT);

    assert(
        local_api_dispatch(
            &local,
            NULL,
            1u,
            &consumed) ==
        AETHER_ERR_ARGUMENT);

    assert(
        local_api_dispatch(
            &local,
            frame,
            sizeof(frame),
            NULL) ==
        AETHER_ERR_ARGUMENT);

    assert(
        local_api_dispatch(
            &local,
            NULL,
            0u,
            &consumed) ==
        AETHER_ERR_PROTOCOL);

    assert(consumed == 0u);
}


int main(void) {
    test_small_stream_is_zero_copy();
    test_two_byte_pack_stream();
    test_concatenated_frames_use_consumed();
    test_truncated_pack_rejected_without_callback();
    test_truncated_payload_rejected_without_callback();
    test_unknown_method_rejected();
    test_missing_callback_rejected();
    test_argument_validation();

    return 0;
}
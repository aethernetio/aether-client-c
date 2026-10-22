
#include "aether_reg_unsafe_local.h"

#include "client_server_reg_unsafe_local_local.h"


typedef struct {
    aether_reg_unsafe_frame_t *frame;
} aether_reg_unsafe_capture_t;


static aether_status_t aether_reg_unsafe_capture_enter(
    void *ctx,
    const client_api_reg_safe_stream_t *stream) {

    aether_reg_unsafe_capture_t *capture =
        ctx;

    if (capture == NULL ||
        capture->frame == NULL ||
        stream == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    capture->frame->kind =
        AETHER_REG_UNSAFE_FRAME_ENTER;

    capture->frame->data =
        stream->data;

    capture->frame->length =
        stream->length;

    return AETHER_OK;
}


static aether_status_t aether_reg_unsafe_capture_enter_global(
    void *ctx,
    const global_reg_client_api_stream_t *stream) {

    aether_reg_unsafe_capture_t *capture =
        ctx;

    if (capture == NULL ||
        capture->frame == NULL ||
        stream == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    capture->frame->kind =
        AETHER_REG_UNSAFE_FRAME_ENTER_GLOBAL;

    capture->frame->data =
        stream->data;

    capture->frame->length =
        stream->length;

    return AETHER_OK;
}


aether_status_t aether_generated_reg_unsafe_decode(
    aether_reg_unsafe_frame_t *frame,
    const uint8_t *data,
    size_t length,
    size_t *consumed) {

    if (frame == NULL ||
        consumed == NULL ||
        (length > 0u &&
         data == NULL)) {

        return AETHER_ERR_ARGUMENT;
    }

    frame->kind =
        0;

    frame->data =
        NULL;

    frame->length =
        0u;

    *consumed =
        0u;

    aether_reg_unsafe_capture_t capture = {
        .frame = frame
    };

    client_api_reg_unsafe_local_t local = {
        .ctx = &capture,
        .enter =
            aether_reg_unsafe_capture_enter,
        .enter_global =
            aether_reg_unsafe_capture_enter_global
    };

    return
        client_api_reg_unsafe_dispatch(
            &local,
            data,
            length,
            consumed);
}

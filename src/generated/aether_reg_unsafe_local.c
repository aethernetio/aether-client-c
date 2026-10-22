
#include "aether_reg_unsafe_local.h"

#include "aether_dataio.h"


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

    aether_data_in_t in;
    aether_data_in_init(&in, data, length);

    uint8_t command =
        aether_data_in_read_ubyte(&in);

    uint64_t stream_length_u64 =
        aether_data_in_read_pack(&in);

    if (aether_data_in_error(&in) ||
        stream_length_u64 > (uint64_t)SIZE_MAX) {

        return AETHER_ERR_PROTOCOL;
    }

    size_t stream_length =
        (size_t)stream_length_u64;

    if (stream_length >
        aether_data_in_size_for_read(&in)) {

        return AETHER_ERR_PROTOCOL;
    }

    size_t stream_offset =
        length - aether_data_in_size_for_read(&in);

    switch (command) {
    case 3u:
        frame->kind =
            AETHER_REG_UNSAFE_FRAME_ENTER;
        break;

    case 4u:
        frame->kind =
            AETHER_REG_UNSAFE_FRAME_ENTER_GLOBAL;
        break;

    default:
        return AETHER_ERR_PROTOCOL;
    }

    frame->data =
        data + stream_offset;

    frame->length =
        stream_length;

    *consumed =
        stream_offset + stream_length;

    return AETHER_OK;
}

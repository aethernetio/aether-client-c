#include "client_server_reg_unsafe_local_local.h"

#include <stdint.h>

static bool client_server_reg_unsafe_local_local_read_pack(
    const uint8_t *data,
    size_t length,
    size_t *position,
    uint64_t *value) {

    const uint64_t u8 = UINT64_C(251);
    const uint64_t u16 = UINT64_C(1515);
    const uint64_t u32 = UINT64_C(1049835);
    const uint64_t u64 =
        u32 +
        (UINT64_C(4294967296) * UINT64_C(256));

    if (data == NULL ||
        position == NULL ||
        value == NULL ||
        *position >= length) {
        return false;
    }

    uint64_t current =
        data[(*position)++];

    if (current < u8) {
        *value = current;
        return true;
    }

    if (*position >= length) {
        return false;
    }

    uint64_t next =
        data[(*position)++];

    current =
        ((current - u8) << 8u) +
        u8 +
        next;

    if (current < u16) {
        *value = current;
        return true;
    }

    if (length - *position < 2u) {
        return false;
    }

    uint64_t fraction16 =
        (uint64_t)data[*position] |
        ((uint64_t)data[*position + 1u] << 8u);

    *position += 2u;

    current =
        ((current - u16) << 16u) +
        u16 +
        fraction16;

    if (current < u32) {
        *value = current;
        return true;
    }

    if (length - *position < 4u) {
        return false;
    }

    uint64_t fraction32 =
        (uint64_t)data[*position] |
        ((uint64_t)data[*position + 1u] << 8u) |
        ((uint64_t)data[*position + 2u] << 16u) |
        ((uint64_t)data[*position + 3u] << 24u);

    *position += 4u;

    current =
        ((current - u32) << 32u) +
        u32 +
        fraction32;

    if (current >= u64) {
        return false;
    }

    *value = current;
    return true;
}

aether_status_t client_api_reg_unsafe_dispatch(
    client_api_reg_unsafe_local_t *api,
    const uint8_t *data,
    size_t length,
    size_t *consumed) {

    if (api == NULL ||
        consumed == NULL ||
        (length > 0u && data == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    *consumed = 0u;

    if (length == 0u) {
        return AETHER_ERR_PROTOCOL;
    }

    size_t position = 0u;
    uint8_t command =
        data[position++];

    switch (command) {
    case 3u: {
        if (api->enter == NULL) {
            return AETHER_ERR_ARGUMENT;
        }

        uint64_t packed_length = 0u;

        if (!client_server_reg_unsafe_local_local_read_pack(
                data,
                length,
                &position,
                &packed_length) ||
            packed_length > (uint64_t)SIZE_MAX) {
            return AETHER_ERR_PROTOCOL;
        }

        size_t stream_length =
            (size_t)packed_length;

        if (stream_length >
                length - position) {
            return AETHER_ERR_PROTOCOL;
        }

        client_api_reg_safe_stream_t stream_value = {
            .data =
                (uint8_t *)(data + position),
            .capacity = stream_length,
            .length = stream_length
        };

        position +=
            stream_length;

        aether_status_t status =
            api->enter(
                api->ctx,
                &stream_value);

        if (status != AETHER_OK) {
            return status;
        }

        *consumed = position;
        return AETHER_OK;
    }

    case 4u: {
        if (api->enter_global == NULL) {
            return AETHER_ERR_ARGUMENT;
        }

        uint64_t packed_length = 0u;

        if (!client_server_reg_unsafe_local_local_read_pack(
                data,
                length,
                &position,
                &packed_length) ||
            packed_length > (uint64_t)SIZE_MAX) {
            return AETHER_ERR_PROTOCOL;
        }

        size_t stream_length =
            (size_t)packed_length;

        if (stream_length >
                length - position) {
            return AETHER_ERR_PROTOCOL;
        }

        global_reg_client_api_stream_t stream_value = {
            .data =
                (uint8_t *)(data + position),
            .capacity = stream_length,
            .length = stream_length
        };

        position +=
            stream_length;

        aether_status_t status =
            api->enter_global(
                api->ctx,
                &stream_value);

        if (status != AETHER_OK) {
            return status;
        }

        *consumed = position;
        return AETHER_OK;
    }

    default:
        return AETHER_ERR_PROTOCOL;
    }
}

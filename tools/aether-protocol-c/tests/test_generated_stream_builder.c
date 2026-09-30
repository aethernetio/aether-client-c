
#include "stream_builder_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static void test_multiple_commands_append_in_place(void) {
    uint8_t storage[32] = {0};

    command_stream_t stream = {
        storage,
        sizeof(storage),
        0u
    };

    assert(
        command_stream_stream_api_append_value(
            &stream,
            INT32_C(0x01020304)) ==
        AETHER_OK);

    assert(stream.length == 5u);

    static const uint8_t first_expected[] = {
        0x03u,
        0x04u,
        0x03u,
        0x02u,
        0x01u
    };

    assert(
        memcmp(
            storage,
            first_expected,
            sizeof(first_expected)) == 0);

    assert(
        command_stream_stream_api_query(
            &stream,
            UINT32_C(0xa1b2c3d4),
            INT16_C(0x1234)) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x04u,
        0x03u,
        0x02u,
        0x01u,

        0x04u,
        0xd4u,
        0xc3u,
        0xb2u,
        0xa1u,
        0x34u,
        0x12u
    };

    assert(stream.length == sizeof(expected));

    assert(
        memcmp(
            storage,
            expected,
            sizeof(expected)) == 0);
}

static void test_overflow_does_not_commit_length(void) {
    uint8_t storage[8] = {0};

    command_stream_t stream = {
        storage,
        sizeof(storage),
        0u
    };

    assert(
        command_stream_stream_api_append_value(
            &stream,
            INT32_C(0x01020304)) ==
        AETHER_OK);

    assert(stream.length == 5u);

    assert(
        command_stream_stream_api_query(
            &stream,
            UINT32_C(1),
            INT16_C(2)) ==
        AETHER_ERR_OVERFLOW);

    assert(stream.length == 5u);

    static const uint8_t expected[] = {
        0x03u,
        0x04u,
        0x03u,
        0x02u,
        0x01u
    };

    assert(
        memcmp(
            storage,
            expected,
            sizeof(expected)) == 0);
}

static void test_first_command_overflow_keeps_empty_stream(void) {
    uint8_t storage[4] = {0};

    command_stream_t stream = {
        storage,
        sizeof(storage),
        0u
    };

    assert(
        command_stream_stream_api_append_value(
            &stream,
            INT32_C(1)) ==
        AETHER_ERR_OVERFLOW);

    assert(stream.length == 0u);
}

static void test_stream_arguments(void) {
    assert(
        command_stream_stream_api_append_value(
            NULL,
            INT32_C(1)) ==
        AETHER_ERR_ARGUMENT);

    command_stream_t invalid_length = {
        NULL,
        0u,
        1u
    };

    assert(
        command_stream_stream_api_append_value(
            &invalid_length,
            INT32_C(1)) ==
        AETHER_ERR_ARGUMENT);

    command_stream_t null_storage = {
        NULL,
        8u,
        0u
    };

    assert(
        command_stream_stream_api_append_value(
            &null_storage,
            INT32_C(1)) ==
        AETHER_ERR_ARGUMENT);

    command_stream_t empty = {
        NULL,
        0u,
        0u
    };

    assert(
        command_stream_stream_api_append_value(
            &empty,
            INT32_C(1)) ==
        AETHER_ERR_OVERFLOW);
}

int main(void) {
    test_multiple_commands_append_in_place();
    test_overflow_does_not_commit_length();
    test_first_command_overflow_keeps_empty_stream();
    test_stream_arguments();

    return 0;
}

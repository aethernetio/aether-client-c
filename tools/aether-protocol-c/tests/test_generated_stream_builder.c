
#include "aether_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static void test_multiple_commands_append_in_place(void) {
    uint8_t storage[32] = {0};

    sb_command_stream_t stream = {
        storage,
        sizeof(storage),
        0u
    };

    assert(
        sb_command_stream_stream_api_append_value(
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
        sb_command_stream_stream_api_query(
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

    sb_command_stream_t stream = {
        storage,
        sizeof(storage),
        0u
    };

    assert(
        sb_command_stream_stream_api_append_value(
            &stream,
            INT32_C(0x01020304)) ==
        AETHER_OK);

    assert(stream.length == 5u);

    assert(
        sb_command_stream_stream_api_query(
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

    sb_command_stream_t stream = {
        storage,
        sizeof(storage),
        0u
    };

    assert(
        sb_command_stream_stream_api_append_value(
            &stream,
            INT32_C(1)) ==
        AETHER_ERR_OVERFLOW);

    assert(stream.length == 0u);
}

static void test_stream_arguments(void) {
    assert(
        sb_command_stream_stream_api_append_value(
            NULL,
            INT32_C(1)) ==
        AETHER_ERR_ARGUMENT);

    sb_command_stream_t invalid_length = {
        NULL,
        0u,
        1u
    };

    assert(
        sb_command_stream_stream_api_append_value(
            &invalid_length,
            INT32_C(1)) ==
        AETHER_ERR_ARGUMENT);

    sb_command_stream_t null_storage = {
        NULL,
        8u,
        0u
    };

    assert(
        sb_command_stream_stream_api_append_value(
            &null_storage,
            INT32_C(1)) ==
        AETHER_ERR_ARGUMENT);

    sb_command_stream_t empty = {
        NULL,
        0u,
        0u
    };

    assert(
        sb_command_stream_stream_api_append_value(
            &empty,
            INT32_C(1)) ==
        AETHER_ERR_OVERFLOW);
}


static void test_collapsible_method_is_deferred_until_stream_flush(void) {
    uint8_t storage[32] = {0};

    sb_command_stream_t stream = {
        storage,
        sizeof(storage),
        0u
    };

    assert(
        sb_command_stream_stream_api_append_value(
            &stream,
            INT32_C(0x01020304)) ==
        AETHER_OK);

    sb_stream_api_update_value_entry_t entries[2] = {0};
    sb_stream_api_update_value_context_t collapse = {
        entries,
        2u,

        0u,
        NULL

    };

    assert(
        sb_command_stream_stream_api_update_value(
            &stream,
            &collapse,
            INT32_C(7),
            INT32_C(10)) ==
        AETHER_OK);

    assert(stream.length == 5u);
    assert(collapse.count == 1u);

    assert(
        sb_command_stream_stream_api_update_value(
            &stream,
            &collapse,
            INT32_C(7),
            INT32_C(11)) ==
        AETHER_OK);

    assert(
        sb_command_stream_stream_api_update_value(
            &stream,
            &collapse,
            INT32_C(8),
            INT32_C(12)) ==
        AETHER_OK);

    assert(stream.length == 5u);
    assert(collapse.count == 2u);

    assert(
        sb_command_stream_stream_api_update_value_flush(
            &stream,
            &collapse) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x04u, 0x03u, 0x02u, 0x01u,

        0x05u,
        0x07u, 0x00u, 0x00u, 0x00u,
        0x0bu, 0x00u, 0x00u, 0x00u,

        0x05u,
        0x08u, 0x00u, 0x00u, 0x00u,
        0x0cu, 0x00u, 0x00u, 0x00u
    };

    assert(stream.length == sizeof(expected));
    assert(collapse.count == 0u);

    assert(
        memcmp(
            storage,
            expected,
            sizeof(expected)) == 0);
}


static void test_collapsible_stream_overflow_keeps_pending_table(void) {
    uint8_t storage[8] = {0};

    sb_command_stream_t stream = {
        storage,
        sizeof(storage),
        0u
    };

    sb_stream_api_update_value_entry_t entries[1] = {0};
    sb_stream_api_update_value_context_t collapse = {
        entries,
        1u,

        0u,
        NULL

    };

    /*
     * One METHOD call needs 9 bytes. Enqueue must still succeed because
     * collapse storage is caller-owned and the stream is touched only at flush.
     */
    assert(
        sb_command_stream_stream_api_update_value(
            &stream,
            &collapse,
            INT32_C(1),
            INT32_C(2)) ==
        AETHER_OK);

    assert(stream.length == 0u);
    assert(collapse.count == 1u);

    assert(
        sb_command_stream_stream_api_update_value_flush(
            &stream,
            &collapse) ==
        AETHER_ERR_OVERFLOW);

    assert(stream.length == 0u);
    assert(collapse.count == 1u);
}



int main(void) {
    test_multiple_commands_append_in_place();
    test_overflow_does_not_commit_length();
    test_first_command_overflow_keeps_empty_stream();
    test_stream_arguments();


    test_collapsible_method_is_deferred_until_stream_flush();
    test_collapsible_stream_overflow_keeps_pending_table();

    return 0;
}
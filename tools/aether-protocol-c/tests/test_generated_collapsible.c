
#include "collapsible_fields_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


typedef struct {
    uint8_t data[128];
    size_t length;
    size_t calls;
    aether_status_t result;
} capture_t;


static aether_status_t capture_send(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    capture_t *capture = ctx;

    assert(capture != NULL);
    assert(data != NULL);
    assert(length <= sizeof(capture->data));

    memcpy(capture->data, data, length);
    capture->length = length;
    capture->calls++;

    return capture->result;
}

static void test_collapsible_struct_round_trip(void) {
    static const uint8_t alias_data[] = {
        0x6fu,
        0x6bu
    };

    aether_bytes_view_t alias = {
        alias_data,
        sizeof(alias_data)
    };

    reading_t source = {
        INT64_C(0x0102030405060708),
        1.5f,
        &alias
    };

    uint8_t wire[64] = {0};
    size_t position = 0u;

    assert(
        collapsible_fields_serialize_reading(
            wire,
            sizeof(wire),
            &position,
            &source) ==
        AETHER_OK);

    /*
     * `temperature: float~` has exactly the ordinary float wire form.
     * `alias: String?~` participates in the normal nullable mask.
     */
    assert(position == 1u + 8u + 4u + 1u + sizeof(alias_data));
    assert(wire[0] == 0x00u);

    uint8_t scratch[64] = {0};
    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = position;
    ctx.scratch = scratch;
    ctx.scratch_capacity = sizeof(scratch);

    reading_t decoded = {0};

    assert(
        collapsible_fields_deserialize_reading(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == position);
    assert(decoded.sensorId == source.sensorId);
    assert(decoded.temperature == source.temperature);
    assert(decoded.alias != NULL);
    assert(decoded.alias->length == sizeof(alias_data));
    assert(memcmp(
        decoded.alias->data,
        alias_data,
        sizeof(alias_data)) == 0);
}

static void test_collapsible_nullable_null(void) {
    reading_t source = {
        INT64_C(7),
        2.0f,
        NULL
    };

    uint8_t wire[32] = {0};
    size_t position = 0u;

    assert(
        collapsible_fields_serialize_reading(
            wire,
            sizeof(wire),
            &position,
            &source) ==
        AETHER_OK);

    /* Java-compatible nullable semantics: bit 1 means NULL. */
    assert(wire[0] == 0x01u);
    assert(position == 1u + 8u + 4u);

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = position;

    reading_t decoded = {0};

    assert(
        collapsible_fields_deserialize_reading(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(decoded.sensorId == source.sensorId);
    assert(decoded.temperature == source.temperature);
    assert(decoded.alias == NULL);
}


static void test_collapsible_api_deferred_replace(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    collapsible_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    collapsible_api_update_entry_t entries[2] = {0};
    collapsible_api_update_context_t collapse = {
        entries,
        2u,

        0u,
        NULL

    };

    const int64_t key =
        INT64_C(0x0102030405060708);

    assert(
        collapsible_api_update(
            &remote,
            &collapse,
            key,
            1.0f) ==
        AETHER_OK);

    assert(capture.calls == 0u);
    assert(collapse.count == 1u);

    assert(
        collapsible_api_update(
            &remote,
            &collapse,
            key,
            2.0f) ==
        AETHER_OK);

    assert(capture.calls == 0u);
    assert(collapse.count == 1u);

    assert(
        collapsible_api_update_flush(
            &remote,
            &collapse) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x08u, 0x07u, 0x06u, 0x05u,
        0x04u, 0x03u, 0x02u, 0x01u,
        0x00u, 0x00u, 0x00u, 0x40u
    };

    assert(capture.calls == 1u);
    assert(collapse.count == 0u);
    assert(capture.length == sizeof(expected));
    assert(memcmp(
        capture.data,
        expected,
        sizeof(expected)) == 0);
}


static void test_collapsible_api_preserves_key_order(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    collapsible_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    collapsible_api_update_entry_t entries[2] = {0};
    collapsible_api_update_context_t collapse = {
        entries,
        2u,

        0u,
        NULL

    };

    assert(
        collapsible_api_update(
            &remote,
            &collapse,
            INT64_C(11),
            1.0f) ==
        AETHER_OK);

    assert(
        collapsible_api_update(
            &remote,
            &collapse,
            INT64_C(22),
            2.0f) ==
        AETHER_OK);

    assert(
        collapsible_api_update_flush(
            &remote,
            &collapse) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x0bu, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x80u, 0x3fu,

        0x03u,
        0x16u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x40u
    };

    assert(capture.calls == 1u);
    assert(capture.length == sizeof(expected));
    assert(memcmp(
        capture.data,
        expected,
        sizeof(expected)) == 0);
}


static void test_collapsible_api_capacity_reset_and_retry(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    collapsible_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    collapsible_api_update_entry_t entries[1] = {0};
    collapsible_api_update_context_t collapse = {
        entries,
        1u,

        0u,
        NULL

    };

    assert(
        collapsible_api_update(
            &remote,
            &collapse,
            INT64_C(1),
            1.0f) ==
        AETHER_OK);

    assert(
        collapsible_api_update(
            &remote,
            &collapse,
            INT64_C(2),
            2.0f) ==
        AETHER_ERR_OVERFLOW);

    assert(collapse.count == 1u);

    capture.result =
        AETHER_ERR_BUSY;

    assert(
        collapsible_api_update_flush(
            &remote,
            &collapse) ==
        AETHER_ERR_BUSY);

    assert(collapse.count == 1u);

    capture.result =
        AETHER_OK;

    assert(
        collapsible_api_update_flush(
            &remote,
            &collapse) ==
        AETHER_OK);

    assert(collapse.count == 0u);

    assert(
        collapsible_api_update(
            &remote,
            &collapse,
            INT64_C(3),
            3.0f) ==
        AETHER_OK);

    collapsible_api_update_reset(
        &collapse);

    assert(collapse.count == 0u);
}


static void test_collapsible_string_key_and_value(void) {
    static const uint8_t key_a_data[] = {'k'};
    static const uint8_t key_b_data[] = {'k'};
    static const uint8_t old_data[] = {'o', 'l', 'd'};
    static const uint8_t new_data[] = {'n', 'e', 'w'};

    aether_bytes_view_t key_a = {
        key_a_data,
        sizeof(key_a_data)
    };

    aether_bytes_view_t key_b = {
        key_b_data,
        sizeof(key_b_data)
    };

    aether_bytes_view_t old_value = {
        old_data,
        sizeof(old_data)
    };

    aether_bytes_view_t new_value = {
        new_data,
        sizeof(new_data)
    };

    uint8_t tx[128] = {0};
    capture_t capture = {0};

    collapsible_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    collapsible_api_update_text_entry_t entries[2] = {0};
    collapsible_api_update_text_context_t collapse = {
        entries,
        2u,

        0u,
        NULL

    };

    assert(
        collapsible_api_update_text(
            &remote,
            &collapse,
            key_a,
            old_value) ==
        AETHER_OK);

    assert(
        collapsible_api_update_text(
            &remote,
            &collapse,
            key_b,
            new_value) ==
        AETHER_OK);

    assert(capture.calls == 0u);
    assert(collapse.count == 1u);

    assert(
        collapsible_api_update_text_flush(
            &remote,
            &collapse) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x04u,
        0x01u, 'k',
        0x03u, 'n', 'e', 'w'
    };

    assert(capture.calls == 1u);
    assert(collapse.count == 0u);
    assert(capture.length == sizeof(expected));
    assert(memcmp(
        capture.data,
        expected,
        sizeof(expected)) == 0);
}




static void test_nullable_scalar_snapshot(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    collapsible_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    collapsible_api_update_nullable_entry_t entries[1] = {0};
    collapsible_api_update_nullable_context_t collapse = {
        entries,
        1u,

        0u,
        NULL

    };

    int32_t first_key = INT32_C(7);
    int32_t first_value = INT32_C(10);

    assert(
        collapsible_api_update_nullable(
            &remote,
            &collapse,
            &first_key,
            &first_value) ==
        AETHER_OK);

    assert(collapse.count == 1u);
    assert(capture.calls == 0u);

    /*
     * Mutating caller storage after enqueue must not mutate the retained key or
     * value. Java stores boxed immutable values in the CollapsibleKey/Object[]
     * table, so C must snapshot the fixed-size value rather than its pointer.
     */
    first_key = INT32_C(99);
    first_value = INT32_C(99);

    int32_t equal_key = INT32_C(7);
    int32_t latest_value = INT32_C(12);

    assert(
        collapsible_api_update_nullable(
            &remote,
            &collapse,
            &equal_key,
            &latest_value) ==
        AETHER_OK);

    /*
     * Equal key replaces only the collapsible value and does not consume a
     * second caller entry.
     */
    assert(collapse.count == 1u);

    latest_value = INT32_C(77);

    assert(
        collapsible_api_update_nullable_flush(
            &remote,
            &collapse) ==
        AETHER_OK);

    static const uint8_t expected_present[] = {
        0x05u,
        0x00u,
        0x07u, 0x00u, 0x00u, 0x00u,
        0x0cu, 0x00u, 0x00u, 0x00u
    };

    assert(capture.calls == 1u);
    assert(collapse.count == 0u);
    assert(capture.length == sizeof(expected_present));

    assert(
        memcmp(
            capture.data,
            expected_present,
            sizeof(expected_present)) == 0);

    equal_key = INT32_C(7);

    assert(
        collapsible_api_update_nullable(
            &remote,
            &collapse,
            &equal_key,
            NULL) ==
        AETHER_OK);

    equal_key = INT32_C(88);

    assert(
        collapsible_api_update_nullable_flush(
            &remote,
            &collapse) ==
        AETHER_OK);

    /*
     * Nullable bit 0 is key (present), bit 1 is collapsible value (NULL).
     * The retained key remains 7 despite mutation of equal_key before flush.
     */
    static const uint8_t expected_null[] = {
        0x05u,
        0x02u,
        0x07u, 0x00u, 0x00u, 0x00u
    };

    assert(capture.calls == 2u);
    assert(collapse.count == 0u);
    assert(capture.length == sizeof(expected_null));

    assert(
        memcmp(
            capture.data,
            expected_null,
            sizeof(expected_null)) == 0);
}




static void test_shared_queue_preserves_first_pending_method_order(void) {
    static const uint8_t key_data[] = {'k'};
    static const uint8_t old_data[] = {'a'};
    static const uint8_t new_data[] = {'b'};

    aether_bytes_view_t key = {
        key_data,
        sizeof(key_data)
    };

    aether_bytes_view_t old_value = {
        old_data,
        sizeof(old_data)
    };

    aether_bytes_view_t new_value = {
        new_data,
        sizeof(new_data)
    };

    uint8_t tx[128] = {0};
    capture_t capture = {0};

    collapsible_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    aether_meta_collapse_pending_t pending[2] = {0};
    aether_meta_collapse_queue_t queue = {
        pending,
        2u,
        0u
    };

    collapsible_api_update_text_entry_t text_entries[1] = {0};
    collapsible_api_update_text_context_t text = {
        text_entries,
        1u,
        0u,
        &queue
    };

    collapsible_api_update_entry_t update_entries[1] = {0};
    collapsible_api_update_context_t update = {
        update_entries,
        1u,
        0u,
        &queue
    };

    /*
     * updateText is declared after update, but becomes pending first. Java
     * registers one LazyPacket at this transition, so wire order must be
     * updateText then update regardless of schema declaration order.
     */
    assert(
        collapsible_api_update_text(
            &remote,
            &text,
            key,
            old_value) ==
        AETHER_OK);

    assert(
        collapsible_api_update(
            &remote,
            &update,
            INT64_C(9),
            1.0f) ==
        AETHER_OK);

    /*
     * Replacing the same updateText key must not enqueue the method again.
     */
    assert(
        collapsible_api_update_text(
            &remote,
            &text,
            key,
            new_value) ==
        AETHER_OK);

    assert(queue.count == 2u);
    assert(text.count == 1u);
    assert(update.count == 1u);
    assert(capture.calls == 0u);

    capture.result =
        AETHER_ERR_BUSY;

    assert(
        collapsible_api_update_flush(
            &remote,
            &update) ==
        AETHER_ERR_BUSY);

    /*
     * A failed external send retains the entire FIFO batch and every METHOD
     * table for retry.
     */
    assert(queue.count == 2u);
    assert(text.count == 1u);
    assert(update.count == 1u);
    assert(capture.calls == 1u);

    capture.result =
        AETHER_OK;

    assert(
        collapsible_api_update_flush(
            &remote,
            &update) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x04u,
        0x01u, 'k',
        0x01u, 'b',

        0x03u,
        0x09u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x80u, 0x3fu
    };

    assert(capture.calls == 2u);
    assert(capture.length == sizeof(expected));
    assert(memcmp(
        capture.data,
        expected,
        sizeof(expected)) == 0);

    assert(queue.count == 0u);
    assert(text.count == 0u);
    assert(update.count == 0u);
}



static float float_from_bits(uint32_t bits) {
    float value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

static double double_from_bits(uint64_t bits) {
    double value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}


static void test_float_double_java_key_and_wire_semantics(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    collapsible_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    collapsible_api_update_float_entry_t float_entries[3] = {0};
    collapsible_api_update_float_context_t float_ctx = {
        float_entries,
        3u,
        0u,
        NULL
    };

    float nan_a = float_from_bits(UINT32_C(0x7fa12345));
    float nan_b = float_from_bits(UINT32_C(0xffc54321));
    float plus_zero = float_from_bits(UINT32_C(0x00000000));
    float minus_zero = float_from_bits(UINT32_C(0x80000000));

    assert(
        collapsible_api_update_float(
            &remote,
            &float_ctx,
            nan_a,
            INT32_C(10)) ==
        AETHER_OK);

    assert(
        collapsible_api_update_float(
            &remote,
            &float_ctx,
            nan_b,
            INT32_C(11)) ==
        AETHER_OK);

    /* Java Float.equals considers every NaN equal. */
    assert(float_ctx.count == 1u);

    assert(
        collapsible_api_update_float(
            &remote,
            &float_ctx,
            plus_zero,
            INT32_C(20)) ==
        AETHER_OK);

    assert(
        collapsible_api_update_float(
            &remote,
            &float_ctx,
            minus_zero,
            INT32_C(30)) ==
        AETHER_OK);

    /* Java Float.equals distinguishes +0.0f from -0.0f. */
    assert(float_ctx.count == 3u);

    assert(
        collapsible_api_update_float_flush(
            &remote,
            &float_ctx) ==
        AETHER_OK);

    static const uint8_t expected_float[] = {
        0x06u,
        0x00u, 0x00u, 0xc0u, 0x7fu,
        0x0bu, 0x00u, 0x00u, 0x00u,

        0x06u,
        0x00u, 0x00u, 0x00u, 0x00u,
        0x14u, 0x00u, 0x00u, 0x00u,

        0x06u,
        0x00u, 0x00u, 0x00u, 0x80u,
        0x1eu, 0x00u, 0x00u, 0x00u
    };

    assert(capture.length == sizeof(expected_float));
    assert(
        memcmp(
            capture.data,
            expected_float,
            sizeof(expected_float)) == 0);

    collapsible_api_update_double_entry_t double_entries[3] = {0};
    collapsible_api_update_double_context_t double_ctx = {
        double_entries,
        3u,
        0u,
        NULL
    };

    double dnan_a =
        double_from_bits(UINT64_C(0x7ff0000000000001));
    double dnan_b =
        double_from_bits(UINT64_C(0xfff8123456789abc));
    double dplus_zero =
        double_from_bits(UINT64_C(0x0000000000000000));
    double dminus_zero =
        double_from_bits(UINT64_C(0x8000000000000000));

    assert(
        collapsible_api_update_double(
            &remote,
            &double_ctx,
            dnan_a,
            INT32_C(40)) ==
        AETHER_OK);

    assert(
        collapsible_api_update_double(
            &remote,
            &double_ctx,
            dnan_b,
            INT32_C(41)) ==
        AETHER_OK);

    assert(double_ctx.count == 1u);

    assert(
        collapsible_api_update_double(
            &remote,
            &double_ctx,
            dplus_zero,
            INT32_C(50)) ==
        AETHER_OK);

    assert(
        collapsible_api_update_double(
            &remote,
            &double_ctx,
            dminus_zero,
            INT32_C(60)) ==
        AETHER_OK);

    assert(double_ctx.count == 3u);

    assert(
        collapsible_api_update_double_flush(
            &remote,
            &double_ctx) ==
        AETHER_OK);

    static const uint8_t expected_double[] = {
        0x07u,
        0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0xf8u, 0x7fu,
        0x29u, 0x00u, 0x00u, 0x00u,

        0x07u,
        0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u,
        0x32u, 0x00u, 0x00u, 0x00u,

        0x07u,
        0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x80u,
        0x3cu, 0x00u, 0x00u, 0x00u
    };

    assert(capture.length == sizeof(expected_double));
    assert(
        memcmp(
            capture.data,
            expected_double,
            sizeof(expected_double)) == 0);
}



int main(void) {
    test_collapsible_struct_round_trip();
    test_collapsible_nullable_null();
    test_collapsible_api_deferred_replace();
    test_collapsible_api_preserves_key_order();
    test_collapsible_api_capacity_reset_and_retry();
    test_collapsible_string_key_and_value();


    test_nullable_scalar_snapshot();


    test_shared_queue_preserves_first_pending_method_order();

    test_float_double_java_key_and_wire_semantics();

    return 0;
}
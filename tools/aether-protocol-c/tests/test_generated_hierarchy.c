
#include "aether_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[128];
    size_t length;
    size_t calls;
} capture_t;

static aether_status_t capture_send(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    capture_t *capture =
        ctx;

    assert(capture != NULL);
    assert(data != NULL);
    assert(length <= sizeof(capture->data));

    memcpy(
        capture->data,
        data,
        length);

    capture->length =
        length;

    capture->calls++;

    return AETHER_OK;
}

static hierarchy_hierarchy_api_remote_t make_remote(
    capture_t *capture,
    uint8_t *tx,
    size_t tx_capacity) {

    hierarchy_hierarchy_api_remote_t remote = {
        capture,
        capture_send,
        tx,
        tx_capacity
    };

    return remote;
}

static void expect_bytes(
    const capture_t *capture,
    const uint8_t *expected,
    size_t expected_length) {

    assert(capture->calls == 1u);
    assert(capture->length == expected_length);

    assert(
        memcmp(
            capture->data,
            expected,
            expected_length) == 0);
}

static hierarchy_temperature_event_t make_temperature(void) {
    hierarchy_temperature_event_t value = {
        .source = INT8_C(0x2a),
        .note = NULL,
        .sequence = INT16_C(0x1234),
        .temperature = 21.5f
    };

    return value;
}

static void test_abstract_reference_writes_type_id(void) {
    uint8_t tx[64] = {0};
    capture_t capture = {0};

    hierarchy_hierarchy_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    hierarchy_temperature_event_t temperature =
        make_temperature();

    hierarchy_event_ref_t event = {
        .type_id = UINT8_C(7),
        .as = {
            .hierarchy_temperature_event = &temperature
        }
    };

    assert(
        hierarchy_hierarchy_api_send_event(
            &remote,
            &event) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x07u,

        /* inherited nullable mask: Event.note == NULL */
        0x01u,

        /* Event.source */
        0x2au,

        /* SensorEvent.sequence */
        0x34u,
        0x12u,

        /* TemperatureEvent.temperature == 21.5f */
        0x00u,
        0x00u,
        0xacu,
        0x41u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_nested_polymorphic_field(void) {
    uint8_t tx[64] = {0};
    capture_t capture = {0};

    hierarchy_hierarchy_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    static const uint8_t note_data[] = {
        0x6eu
    };

    static const uint8_t text_data[] = {
        0x6fu,
        0x6bu
    };

    aether_bytes_view_t note = {
        note_data,
        sizeof(note_data)
    };

    aether_bytes_view_t text = {
        text_data,
        sizeof(text_data)
    };

    hierarchy_text_event_t concrete = {
        .source = INT8_C(0x11),
        .note = &note,
        .text = text
    };

    hierarchy_event_ref_t event = {
        .type_id = UINT8_C(9),
        .as = {
            .hierarchy_text_event = &concrete
        }
    };

    hierarchy_event_envelope_t envelope = {
        .event = event
    };

    assert(
        hierarchy_hierarchy_api_send_envelope(
            &remote,
            &envelope) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x04u,
        0x09u,

        /* inherited nullable mask: note present */
        0x00u,

        0x11u,

        /* note */
        0x01u,
        0x6eu,

        /* text */
        0x02u,
        0x6fu,
        0x6bu
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_concrete_leaf_has_no_discriminator(void) {
    uint8_t tx[64] = {0};
    capture_t capture = {0};

    hierarchy_hierarchy_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    hierarchy_temperature_event_t temperature =
        make_temperature();

    assert(
        hierarchy_hierarchy_api_send_temperature(
            &remote,
            &temperature) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x05u,

        /* body begins immediately; no hierarchy type id */
        0x01u,
        0x2au,
        0x34u,
        0x12u,
        0x00u,
        0x00u,
        0xacu,
        0x41u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_unknown_type_id_rejected(void) {
    uint8_t tx[64] = {0};
    capture_t capture = {0};

    hierarchy_hierarchy_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    hierarchy_event_ref_t event = {
        .type_id = UINT8_C(8)
    };

    assert(
        hierarchy_hierarchy_api_send_event(
            &remote,
            &event) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}

static void test_selected_concrete_pointer_required(void) {
    uint8_t tx[64] = {0};
    capture_t capture = {0};

    hierarchy_hierarchy_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    hierarchy_event_ref_t event = {
        .type_id = UINT8_C(7),
        .as = {
            .hierarchy_temperature_event = NULL
        }
    };

    assert(
        hierarchy_hierarchy_api_send_event(
            &remote,
            &event) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}


/*
 * Round-trip decode of a polymorphic hierarchy ref through the generic
 * decoder. Concrete child must land in caller scratch, and the existing
 * expected bytes must survive serialize -> deserialize -> serialize.
 */
static void test_hierarchy_roundtrip(void) {
    static const uint8_t wire[] = {
        0x07u,
        0x01u,
        0x2au,
        0x34u,
        0x12u,
        0x00u,
        0x00u,
        0xacu,
        0x41u
    };

    uint8_t scratch[256] = {0};
    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);
    ctx.scratch = scratch;
    ctx.scratch_capacity = sizeof(scratch);

    hierarchy_event_ref_t decoded = {0};

    assert(
        hierarchy_deserialize_event(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(wire));
    assert(decoded.type_id == UINT8_C(7));
    assert(decoded.as.hierarchy_temperature_event != NULL);
    assert(
        (const uint8_t *)decoded.as.hierarchy_temperature_event >= scratch);
    assert(
        (const uint8_t *)decoded.as.hierarchy_temperature_event <
        scratch + sizeof(scratch));

    assert(decoded.as.hierarchy_temperature_event->source == INT8_C(0x2a));
    assert(decoded.as.hierarchy_temperature_event->note == NULL);
    assert(decoded.as.hierarchy_temperature_event->sequence == INT16_C(0x1234));
    assert(decoded.as.hierarchy_temperature_event->temperature == 21.5f);

    uint8_t tx[64] = {0};
    size_t pos = 0u;

    assert(
        hierarchy_serialize_event(
            tx,
            sizeof(tx),
            &pos,
            &decoded) ==
        AETHER_OK);

    assert(pos == sizeof(wire));
    assert(memcmp(tx, wire, sizeof(wire)) == 0);
}


/*
 * Unknown discriminator must be rejected as AETHER_ERR_PROTOCOL.
 */
static void test_hierarchy_unknown_discriminator(void) {
    static const uint8_t wire[] = {
        0x08u,
        0x00u,
        0x00u,
        0x00u,
        0x00u,
        0x00u,
        0x00u,
        0x00u,
        0x00u
    };

    uint8_t scratch[256] = {0};
    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);
    ctx.scratch = scratch;
    ctx.scratch_capacity = sizeof(scratch);

    hierarchy_event_ref_t decoded = {0};

    assert(
        hierarchy_deserialize_event(
            &ctx,
            &decoded) ==
        AETHER_ERR_PROTOCOL);
}



/*
 * Deliberately misaligned scratch base. The decoder must align the actual
 * returned pointer, not only the offset from the buffer start, so that
 * structurally typed children land on their _Alignof boundary even when
 * the caller passes scratch with an odd base address.
 *
 * Without a test that uses a misaligned base, x86 stack layout tends to
 * hide an offset-only alignment bug.
 */
static void test_hierarchy_misaligned_scratch_base(void) {
    static const uint8_t wire[] = {
        0x07u,
        0x01u,
        0x2au,
        0x34u,
        0x12u,
        0x00u,
        0x00u,
        0xacu,
        0x41u
    };

    uint8_t raw[256];
    memset(raw, 0xaa, sizeof(raw));

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);
    ctx.scratch = raw + 1u;
    ctx.scratch_capacity = sizeof(raw) - 1u;

    hierarchy_event_ref_t decoded = {0};

    assert(
        hierarchy_deserialize_event(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(decoded.type_id == UINT8_C(7));
    assert(decoded.as.hierarchy_temperature_event != NULL);

    assert(
        ((uintptr_t)decoded.as.hierarchy_temperature_event %
         _Alignof(hierarchy_temperature_event_t)) == 0u);

    assert(decoded.as.hierarchy_temperature_event->source == INT8_C(0x2a));
    assert(decoded.as.hierarchy_temperature_event->sequence == INT16_C(0x1234));
    assert(decoded.as.hierarchy_temperature_event->temperature == 21.5f);
}



/*
 * Pathological scratch_position must be rejected before any pointer
 * arithmetic is attempted. The allocator must return NULL rather than
 * forming a bogus returned address.
 */
static void test_scratch_position_overflow(void) {
    uint8_t raw[64] = {0};
    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = raw;
    ctx.reader.length = 0u;
    ctx.scratch = raw;
    ctx.scratch_position = SIZE_MAX;
    ctx.scratch_capacity = SIZE_MAX;

    void *p = aether_meta_scratch_alloc(&ctx, 16u, 8u);
    assert(p == NULL);

    ctx.scratch_position = SIZE_MAX - 3u;
    p = aether_meta_scratch_alloc(&ctx, 16u, 8u);
    assert(p == NULL);

    ctx.scratch_position = 0u;
    ctx.scratch_capacity = 4u;
    p = aether_meta_scratch_alloc(&ctx, 8u, 4u);
    assert(p == NULL);

    ctx.scratch_capacity = 64u;
    p = aether_meta_scratch_alloc(&ctx, 8u, 4u);
    assert(p != NULL);
    assert(((uintptr_t)p % 4u) == 0u);
}




int main(void) {
    test_abstract_reference_writes_type_id();
    test_nested_polymorphic_field();
    test_concrete_leaf_has_no_discriminator();
    test_unknown_type_id_rejected();

    test_selected_concrete_pointer_required();

    test_hierarchy_roundtrip();
    test_hierarchy_unknown_discriminator();
    test_hierarchy_misaligned_scratch_base();
    test_scratch_position_overflow();




    return 0;
}
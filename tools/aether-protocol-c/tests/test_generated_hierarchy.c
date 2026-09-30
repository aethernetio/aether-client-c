
#include "hierarchy_api.h"

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

static hierarchy_api_remote_t make_remote(
    capture_t *capture,
    uint8_t *tx,
    size_t tx_capacity) {

    hierarchy_api_remote_t remote = {
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

static temperature_event_t make_temperature(void) {
    temperature_event_t value = {
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

    hierarchy_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    temperature_event_t temperature =
        make_temperature();

    event_ref_t event = {
        .type_id = UINT8_C(7),
        .as = {
            .temperature_event = &temperature
        }
    };

    assert(
        hierarchy_api_send_event(
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

    hierarchy_api_remote_t remote =
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

    text_event_t concrete = {
        .source = INT8_C(0x11),
        .note = &note,
        .text = text
    };

    event_ref_t event = {
        .type_id = UINT8_C(9),
        .as = {
            .text_event = &concrete
        }
    };

    event_envelope_t envelope = {
        .event = event
    };

    assert(
        hierarchy_api_send_envelope(
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

    hierarchy_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    temperature_event_t temperature =
        make_temperature();

    assert(
        hierarchy_api_send_temperature(
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

    hierarchy_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    event_ref_t event = {
        .type_id = UINT8_C(8)
    };

    assert(
        hierarchy_api_send_event(
            &remote,
            &event) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}

static void test_selected_concrete_pointer_required(void) {
    uint8_t tx[64] = {0};
    capture_t capture = {0};

    hierarchy_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    event_ref_t event = {
        .type_id = UINT8_C(7),
        .as = {
            .temperature_event = NULL
        }
    };

    assert(
        hierarchy_api_send_event(
            &remote,
            &event) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}

int main(void) {
    test_abstract_reference_writes_type_id();
    test_nested_polymorphic_field();
    test_concrete_leaf_has_no_discriminator();
    test_unknown_type_id_rejected();
    test_selected_concrete_pointer_required();

    return 0;
}

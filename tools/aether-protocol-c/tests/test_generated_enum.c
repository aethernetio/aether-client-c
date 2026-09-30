
#include "enum_types_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[32];
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

static void test_enum_parameter_wire(void) {
    uint8_t tx[8] = {0};
    capture_t capture = {0};

    enum_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    assert(
        enum_api_set_mode(
            &remote,
            mode_AUTO) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x02u
    };

    assert(capture.calls == 1u);
    assert(capture.length == sizeof(expected));

    assert(
        memcmp(
            capture.data,
            expected,
            sizeof(expected)) == 0);
}

static void test_enum_struct_field_wire(void) {
    uint8_t tx[8] = {0};
    capture_t capture = {0};

    enum_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    device_state_t state = {
        mode_ON,
        (int16_t)0x1234
    };

    assert(
        enum_api_send_state(
            &remote,
            &state) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x04u,
        0x01u,
        0x34u,
        0x12u
    };

    assert(capture.calls == 1u);
    assert(capture.length == sizeof(expected));

    assert(
        memcmp(
            capture.data,
            expected,
            sizeof(expected)) == 0);
}

int main(void) {
    test_enum_parameter_wire();
    test_enum_struct_field_wire();

    return 0;
}

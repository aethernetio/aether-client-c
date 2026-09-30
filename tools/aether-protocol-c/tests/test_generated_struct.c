
#include "include_root_api.h"

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

static void test_struct_wire(void) {
    uint8_t tx[16] = {0};
    capture_t capture = {0};

    sensor_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    sensor_reading_t reading = {
        21.5f
    };

    assert(
        sensor_api_publish(
            &remote,
            &reading) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x00u,
        0x00u,
        0xacu,
        0x41u
    };

    assert(capture.calls == 1u);
    assert(capture.length == sizeof(expected));

    assert(
        memcmp(
            capture.data,
            expected,
            sizeof(expected)) == 0);
}

static void test_struct_null_argument(void) {
    uint8_t tx[16] = {0};
    capture_t capture = {0};

    sensor_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    assert(
        sensor_api_publish(
            &remote,
            NULL) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}

static void test_struct_overflow(void) {
    uint8_t tx[4] = {0};
    capture_t capture = {0};

    sensor_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    sensor_reading_t reading = {
        1.0f
    };

    assert(
        sensor_api_publish(
            &remote,
            &reading) ==
        AETHER_ERR_OVERFLOW);

    assert(capture.calls == 0u);
}

int main(void) {
    test_struct_wire();
    test_struct_null_argument();
    test_struct_overflow();

    return 0;
}

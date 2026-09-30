
#include "temperature_sensor_api.h"

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

static void test_report_temperature_wire(void) {
    uint8_t tx[16] = {0};
    capture_t capture = {0};

    temperature_sensor_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    float temperature =
        21.5f;

    assert(
        temperature_sensor_api_report_temperature(
            &remote,
            temperature) ==
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

static void test_report_temperature_negative_wire(void) {
    uint8_t tx[16] = {0};
    capture_t capture = {0};

    temperature_sensor_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    float temperature =
        -2.25f;

    assert(
        temperature_sensor_api_report_temperature(
            &remote,
            temperature) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x00u,
        0x00u,
        0x10u,
        0xc0u
    };

    assert(capture.calls == 1u);
    assert(capture.length == sizeof(expected));

    assert(
        memcmp(
            capture.data,
            expected,
            sizeof(expected)) == 0);
}

static void test_report_temperature_overflow(void) {
    uint8_t tx[4] = {0};
    capture_t capture = {0};

    temperature_sensor_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    assert(
        temperature_sensor_api_report_temperature(
            &remote,
            1.0f) ==
        AETHER_ERR_OVERFLOW);

    assert(capture.calls == 0u);
}

static void test_report_temperature_arguments(void) {
    assert(
        temperature_sensor_api_report_temperature(
            NULL,
            1.0f) ==
        AETHER_ERR_ARGUMENT);

    uint8_t tx[16] = {0};

    temperature_sensor_api_remote_t no_send = {
        NULL,
        NULL,
        tx,
        sizeof(tx)
    };

    assert(
        temperature_sensor_api_report_temperature(
            &no_send,
            1.0f) ==
        AETHER_ERR_ARGUMENT);

    capture_t capture = {0};

    temperature_sensor_api_remote_t no_buffer = {
        &capture,
        capture_send,
        NULL,
        16u
    };

    assert(
        temperature_sensor_api_report_temperature(
            &no_buffer,
            1.0f) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}

int main(void) {
    test_report_temperature_wire();
    test_report_temperature_negative_wire();
    test_report_temperature_overflow();
    test_report_temperature_arguments();

    return 0;
}

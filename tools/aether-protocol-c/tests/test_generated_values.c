
#include "value_types_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[2048];
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

static void test_uuid_string_uri_wire(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    value_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    aether_uuid_t id = {
        UINT64_C(0xB1AC52C88D94BD39),
        UINT64_C(0x4C01A631AC594165)
    };

    static const uint8_t name_bytes[] = {
        'a',
        'b',
        'c'
    };

    static const uint8_t endpoint_bytes[] = {
        'x',
        'y'
    };

    aether_bytes_view_t name = {
        name_bytes,
        sizeof(name_bytes)
    };

    aether_bytes_view_t endpoint = {
        endpoint_bytes,
        sizeof(endpoint_bytes)
    };

    assert(
        value_api_send_values(
            &remote,
            id,
            name,
            endpoint) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,

        0xb1u,
        0xacu,
        0x52u,
        0xc8u,
        0x8du,
        0x94u,
        0xbdu,
        0x39u,

        0x4cu,
        0x01u,
        0xa6u,
        0x31u,
        0xacu,
        0x59u,
        0x41u,
        0x65u,

        0x03u,
        0x61u,
        0x62u,
        0x63u,

        0x02u,
        0x78u,
        0x79u
    };

    assert(capture.calls == 1u);
    assert(capture.length == sizeof(expected));

    assert(
        memcmp(
            capture.data,
            expected,
            sizeof(expected)) == 0);
}

static void test_empty_view_allows_null_data(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    value_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    aether_bytes_view_t empty = {
        NULL,
        0u
    };

    assert(
        value_api_send_text(
            &remote,
            empty) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x04u,
        0x00u
    };

    assert(capture.calls == 1u);
    assert(capture.length == sizeof(expected));

    assert(
        memcmp(
            capture.data,
            expected,
            sizeof(expected)) == 0);
}

static void test_nonempty_view_rejects_null_data(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    value_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    aether_bytes_view_t invalid = {
        NULL,
        1u
    };

    assert(
        value_api_send_text(
            &remote,
            invalid) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}

static void test_pack_boundary_251(void) {
    uint8_t tx[512] = {0};
    uint8_t text[251];

    for (size_t i = 0u;
         i < sizeof(text);
         ++i) {

        text[i] =
            (uint8_t)i;
    }

    capture_t capture = {0};

    value_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    aether_bytes_view_t view = {
        text,
        sizeof(text)
    };

    assert(
        value_api_send_text(
            &remote,
            view) ==
        AETHER_OK);

    assert(capture.calls == 1u);
    assert(capture.length == 254u);

    assert(capture.data[0] == 0x04u);
    assert(capture.data[1] == 0xfbu);
    assert(capture.data[2] == 0x00u);

    assert(
        memcmp(
            capture.data + 3u,
            text,
            sizeof(text)) == 0);
}

static void test_pack_boundary_1515(void) {
    uint8_t tx[1600] = {0};
    uint8_t text[1515];

    for (size_t i = 0u;
         i < sizeof(text);
         ++i) {

        text[i] =
            (uint8_t)(i * 3u);
    }

    capture_t capture = {0};

    value_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    aether_bytes_view_t view = {
        text,
        sizeof(text)
    };

    assert(
        value_api_send_text(
            &remote,
            view) ==
        AETHER_OK);

    assert(capture.calls == 1u);
    assert(capture.length == 1520u);

    assert(capture.data[0] == 0x04u);
    assert(capture.data[1] == 0xffu);
    assert(capture.data[2] == 0xf0u);
    assert(capture.data[3] == 0x00u);
    assert(capture.data[4] == 0x00u);

    assert(
        memcmp(
            capture.data + 5u,
            text,
            sizeof(text)) == 0);
}

int main(void) {
    test_uuid_string_uri_wire();
    test_empty_view_allows_null_data();
    test_nonempty_view_rejects_null_data();
    test_pack_boundary_251();
    test_pack_boundary_1515();

    return 0;
}

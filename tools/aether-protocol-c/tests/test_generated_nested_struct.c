
#include "nested_struct_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[64];
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

static void test_nested_wire(void) {
    uint8_t tx[64] = {0};
    capture_t capture = {0};

    nested_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    inner_t inner = {
        INT16_C(0x1234)
    };

    middle_t middle = {
        inner,
        {
            UINT64_C(0x0102030405060708),
            UINT64_C(0x1112131415161718)
        }
    };

    static const uint8_t name_data[] = {
        0x68u,
        0x69u
    };

    outer_t outer = {
        middle,
        {
            name_data,
            sizeof(name_data)
        },
        INT64_C(251)
    };

    assert(
        nested_api_send(
            &remote,
            &outer) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,
        0x34u,
        0x12u,
        0x01u,
        0x02u,
        0x03u,
        0x04u,
        0x05u,
        0x06u,
        0x07u,
        0x08u,
        0x11u,
        0x12u,
        0x13u,
        0x14u,
        0x15u,
        0x16u,
        0x17u,
        0x18u,
        0x02u,
        0x68u,
        0x69u,
        0xfbu,
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

static void test_nested_validation(void) {
    uint8_t tx[64] = {0};
    capture_t capture = {0};

    nested_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    outer_t outer = {0};

    outer.name.data =
        NULL;

    outer.name.length =
        1u;

    assert(
        nested_api_send(
            &remote,
            &outer) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);

    assert(
        nested_api_send(
            &remote,
            NULL) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}

static void test_nested_overflow(void) {
    uint8_t tx[4] = {0};
    capture_t capture = {0};

    nested_api_remote_t remote = {
        &capture,
        capture_send,
        tx,
        sizeof(tx)
    };

    inner_t inner = {
        INT16_C(1)
    };

    middle_t middle = {
        inner,
        {
            UINT64_C(1),
            UINT64_C(2)
        }
    };

    outer_t outer = {
        middle,
        {
            NULL,
            0u
        },
        INT64_C(0)
    };

    assert(
        nested_api_send(
            &remote,
            &outer) ==
        AETHER_ERR_OVERFLOW);

    assert(capture.calls == 0u);
}

int main(void) {
    test_nested_wire();
    test_nested_validation();
    test_nested_overflow();

    return 0;
}

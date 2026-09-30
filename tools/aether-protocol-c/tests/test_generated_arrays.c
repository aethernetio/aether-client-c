
#include "arrays_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[256];
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

static array_api_remote_t make_remote(
    capture_t *capture,
    uint8_t *tx,
    size_t tx_capacity) {

    array_api_remote_t remote = {
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

static void test_struct_arrays_exact_wire(void) {
    uint8_t tx[256] = {0};
    capture_t capture = {0};

    array_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    static const uint8_t bytes_data[] = {
        0xaau,
        0xbbu,
        0xccu
    };

    aether_bytes_view_t bytes = {
        bytes_data,
        sizeof(bytes_data)
    };

    static const int16_t short_data[] = {
        INT16_C(0x1234),
        INT16_C(-2)
    };

    short_array_view_t shorts = {
        short_data,
        2u
    };

    static const aether_uuid_t id_data[] = {
        {
            UINT64_C(0x0102030405060708),
            UINT64_C(0x1112131415161718)
        }
    };

    uuid_array_view_t ids = {
        id_data,
        1u
    };

    static const uint8_t first_name_data[] = {
        0x61u
    };

    static const uint8_t second_name_data[] = {
        0x62u,
        0x63u
    };

    static const aether_bytes_view_t name_data[] = {
        {
            first_name_data,
            sizeof(first_name_data)
        },
        {
            second_name_data,
            sizeof(second_name_data)
        }
    };

    string_array_view_t names = {
        name_data,
        2u
    };

    static const array_item_t item_data[] = {
        {
            INT16_C(7)
        },
        {
            INT16_C(0x1234)
        }
    };

    array_item_array_view_t items = {
        item_data,
        2u
    };

    byte_array_4_t fixed_bytes = {
        {
            INT8_C(1),
            INT8_C(2),
            INT8_C(3),
            INT8_C(4)
        }
    };

    short_array_3_t fixed_shorts = {
        {
            INT16_C(0x1122),
            INT16_C(0x3344),
            INT16_C(0x5566)
        }
    };

    array_holder_t value = {
        bytes,
        shorts,
        ids,
        names,
        items,
        fixed_bytes,
        fixed_shorts,
        NULL
    };

    assert(
        array_api_send_holder(
            &remote,
            &value) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x03u,

        /* one nullable field: maybeBytes == NULL */
        0x01u,

        /* byte[] */
        0x03u,
        0xaau,
        0xbbu,
        0xccu,

        /* short[] */
        0x02u,
        0x34u,
        0x12u,
        0xfeu,
        0xffu,

        /* UUID[] */
        0x01u,
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

        /* String[] */
        0x02u,
        0x01u,
        0x61u,
        0x02u,
        0x62u,
        0x63u,

        /* ArrayItem[] */
        0x02u,
        0x07u,
        0x00u,
        0x34u,
        0x12u,

        /* byte[4], no length prefix */
        0x01u,
        0x02u,
        0x03u,
        0x04u,

        /* short[3], no length prefix */
        0x22u,
        0x11u,
        0x44u,
        0x33u,
        0x66u,
        0x55u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_dynamic_parameter(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    array_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    static const int16_t values_data[] = {
        INT16_C(1),
        INT16_C(0x1234)
    };

    short_array_view_t values = {
        values_data,
        2u
    };

    assert(
        array_api_send_shorts(
            &remote,
            values) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x04u,
        0x02u,
        0x01u,
        0x00u,
        0x34u,
        0x12u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_static_parameter(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    array_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    byte_array_4_t values = {
        {
            INT8_C(9),
            INT8_C(8),
            INT8_C(7),
            INT8_C(6)
        }
    };

    assert(
        array_api_send_fixed(
            &remote,
            &values) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x05u,
        0x09u,
        0x08u,
        0x07u,
        0x06u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_empty_dynamic_null_data(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    array_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    short_array_view_t values = {
        NULL,
        0u
    };

    assert(
        array_api_send_shorts(
            &remote,
            values) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x04u,
        0x00u
    };

    expect_bytes(
        &capture,
        expected,
        sizeof(expected));
}

static void test_invalid_dynamic_null_data(void) {
    uint8_t tx[32] = {0};
    capture_t capture = {0};

    array_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    short_array_view_t values = {
        NULL,
        1u
    };

    assert(
        array_api_send_shorts(
            &remote,
            values) ==
        AETHER_ERR_ARGUMENT);

    assert(capture.calls == 0u);
}

static void test_nullable_array_present(void) {
    uint8_t tx[256] = {0};
    capture_t capture = {0};

    array_api_remote_t remote =
        make_remote(
            &capture,
            tx,
            sizeof(tx));

    aether_bytes_view_t empty = {
        NULL,
        0u
    };

    short_array_view_t shorts = {
        NULL,
        0u
    };

    uuid_array_view_t ids = {
        NULL,
        0u
    };

    string_array_view_t names = {
        NULL,
        0u
    };

    array_item_array_view_t items = {
        NULL,
        0u
    };

    byte_array_4_t fixed_bytes = {
        {
            INT8_C(1),
            INT8_C(2),
            INT8_C(3),
            INT8_C(4)
        }
    };

    short_array_3_t fixed_shorts = {
        {
            INT16_C(1),
            INT16_C(2),
            INT16_C(3)
        }
    };

    static const uint8_t maybe_data[] = {
        0x55u,
        0x66u
    };

    aether_bytes_view_t maybe_bytes = {
        maybe_data,
        sizeof(maybe_data)
    };

    array_holder_t value = {
        empty,
        shorts,
        ids,
        names,
        items,
        fixed_bytes,
        fixed_shorts,
        &maybe_bytes
    };

    assert(
        array_api_send_holder(
            &remote,
            &value) ==
        AETHER_OK);

    /*
     * Only the tail is important here: nullable mask must be 0 and
     * the present byte[] must carry its own dynamic length.
     */
    assert(capture.calls == 1u);
    assert(capture.length >= 4u);
    assert(capture.data[0] == 0x03u);
    assert(capture.data[1] == 0x00u);
    assert(capture.data[capture.length - 3u] == 0x02u);
    assert(capture.data[capture.length - 2u] == 0x55u);
    assert(capture.data[capture.length - 1u] == 0x66u);
}

int main(void) {
    test_struct_arrays_exact_wire();
    test_dynamic_parameter();
    test_static_parameter();
    test_empty_dynamic_null_data();
    test_invalid_dynamic_null_data();
    test_nullable_array_present();

    return 0;
}

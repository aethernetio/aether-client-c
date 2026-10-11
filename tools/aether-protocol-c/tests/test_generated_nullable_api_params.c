
#include "aether_api.h"
#include "nullable_api_params_local.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


typedef struct {
    uint8_t data[128];
    size_t length;
    size_t calls;
} transport_capture_t;


typedef struct {
    size_t send_calls;
    size_t query_calls;

    bool maybe_short_present;
    int16_t maybe_short;

    int8_t required;

    bool maybe_bytes_present;
    const uint8_t *maybe_bytes_data;
    size_t maybe_bytes_length;

    bool maybe_item_present;
    int16_t maybe_item_code;

    uint32_t request_id;
    bool maybe_code_present;
    int16_t maybe_code;
} local_capture_t;


static aether_status_t capture_send(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    transport_capture_t *capture =
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


static aether_status_t on_send(
    void *ctx,
    const int16_t *maybe_short,
    int8_t required,
    const aether_bytes_view_t *maybe_bytes,
    const nullable_param_item_t *maybe_item) {

    local_capture_t *capture =
        ctx;

    assert(capture != NULL);

    capture->send_calls++;

    capture->maybe_short_present =
        maybe_short != NULL;

    capture->maybe_short =
        maybe_short != NULL
            ? *maybe_short
            : INT16_C(0);

    capture->required =
        required;

    capture->maybe_bytes_present =
        maybe_bytes != NULL;

    capture->maybe_bytes_data =
        maybe_bytes != NULL
            ? maybe_bytes->data
            : NULL;

    capture->maybe_bytes_length =
        maybe_bytes != NULL
            ? maybe_bytes->length
            : 0u;

    capture->maybe_item_present =
        maybe_item != NULL;

    capture->maybe_item_code =
        maybe_item != NULL
            ? maybe_item->code
            : INT16_C(0);

    return AETHER_OK;
}


static aether_status_t on_query(
    void *ctx,
    uint32_t request_id,
    const int16_t *maybe_code,
    int8_t required) {

    local_capture_t *capture =
        ctx;

    assert(capture != NULL);

    capture->query_calls++;
    capture->request_id =
        request_id;

    capture->maybe_code_present =
        maybe_code != NULL;

    capture->maybe_code =
        maybe_code != NULL
            ? *maybe_code
            : INT16_C(0);

    capture->required =
        required;

    return AETHER_OK;
}


static nullable_param_api_remote_t make_remote(
    transport_capture_t *capture,
    uint8_t *tx,
    size_t tx_capacity) {

    nullable_param_api_remote_t remote = {
        capture,
        capture_send,
        tx,
        tx_capacity
    };

    return remote;
}


static nullable_param_api_local_t make_local(
    local_capture_t *capture) {

    nullable_param_api_local_t local = {
        .ctx = capture,
        .send = on_send,
        .query = on_query
    };

    return local;
}


static aether_meta_read_context_t make_read_context(
    const uint8_t *data,
    size_t length,
    uint8_t *scratch,
    size_t scratch_capacity) {

    aether_meta_read_context_t ctx = {0};

    ctx.reader.data =
        data;

    ctx.reader.length =
        length;

    ctx.scratch =
        scratch;

    ctx.scratch_capacity =
        scratch_capacity;

    return ctx;
}


static void expect_wire(
    const transport_capture_t *capture,
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


static void test_remote_mixed_nullable_wire(void) {
    uint8_t tx[64] = {0};
    transport_capture_t transport = {0};

    nullable_param_api_remote_t remote =
        make_remote(
            &transport,
            tx,
            sizeof(tx));

    int16_t maybe_short =
        INT16_C(0x1234);

    nullable_param_item_t maybe_item = {
        INT16_C(0x5678)
    };

    assert(
        nullable_param_api_send(
            &remote,
            &maybe_short,
            INT8_C(0x2a),
            NULL,
            &maybe_item) ==
        AETHER_OK);

    /*
     * Nullable parameters are numbered independently from required fields:
     *
     *   bit 0 maybeShort = present
     *   bit 1 maybeBytes = NULL
     *   bit 2 maybeItem  = present
     *
     * Java wire order is command, shared mask, then declaration-order
     * payloads for values that are present.
     */
    static const uint8_t expected[] = {
        21u,
        0x02u,
        0x34u, 0x12u,
        0x2au,
        0x78u, 0x56u
    };

    expect_wire(
        &transport,
        expected,
        sizeof(expected));
}


static void test_remote_all_nullable_absent(void) {
    uint8_t tx[32] = {0};
    transport_capture_t transport = {0};

    nullable_param_api_remote_t remote =
        make_remote(
            &transport,
            tx,
            sizeof(tx));

    assert(
        nullable_param_api_send(
            &remote,
            NULL,
            INT8_C(0x11),
            NULL,
            NULL) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        21u,
        0x07u,
        0x11u
    };

    expect_wire(
        &transport,
        expected,
        sizeof(expected));
}


static void test_response_request_id_precedes_mask(void) {
    uint8_t tx[32] = {0};
    transport_capture_t transport = {0};

    nullable_param_api_remote_t remote =
        make_remote(
            &transport,
            tx,
            sizeof(tx));

    int16_t maybe_code =
        INT16_C(0x5566);

    assert(
        nullable_param_api_query(
            &remote,
            UINT32_C(0x11223344),
            &maybe_code,
            INT8_C(0x33)) ==
        AETHER_OK);

    /*
     * Response-bearing method:
     *
     *   command
     *   request_id little-endian
     *   nullable mask
     *   present parameter payloads
     */
    static const uint8_t expected[] = {
        22u,
        0x44u, 0x33u, 0x22u, 0x11u,
        0x00u,
        0x66u, 0x55u,
        0x33u
    };

    expect_wire(
        &transport,
        expected,
        sizeof(expected));
}


static void test_remote_to_local_round_trip(void) {
    uint8_t tx[64] = {0};
    transport_capture_t transport = {0};

    nullable_param_api_remote_t remote =
        make_remote(
            &transport,
            tx,
            sizeof(tx));

    int16_t maybe_short =
        INT16_C(-1234);

    static const uint8_t bytes_data[] = {
        0xa1u,
        0xb2u,
        0xc3u
    };

    aether_bytes_view_t maybe_bytes = {
        bytes_data,
        sizeof(bytes_data)
    };

    /*
     * maybeItem is NULL, therefore bit 2 is set. The dynamic byte view remains
     * zero-copy on local decode; only its tiny view descriptor lives in scratch.
     */
    assert(
        nullable_param_api_send(
            &remote,
            &maybe_short,
            INT8_C(7),
            &maybe_bytes,
            NULL) ==
        AETHER_OK);

    local_capture_t local_capture = {0};

    nullable_param_api_local_t local =
        make_local(
            &local_capture);

    uint8_t scratch[128] = {0};

    aether_meta_read_context_t ctx =
        make_read_context(
            transport.data,
            transport.length,
            scratch,
            sizeof(scratch));

    assert(
        nullable_param_api_dispatch(
            &local,
            &ctx) ==
        AETHER_OK);

    assert(ctx.reader.position == transport.length);
    assert(ctx.scratch_position == 0u);

    assert(local_capture.send_calls == 1u);
    assert(local_capture.maybe_short_present);
    assert(local_capture.maybe_short == maybe_short);
    assert(local_capture.required == INT8_C(7));

    assert(local_capture.maybe_bytes_present);
    assert(local_capture.maybe_bytes_length == sizeof(bytes_data));

    /*
     * Decoder borrows dynamic byte payload directly from RX.
     */
    assert(
        local_capture.maybe_bytes_data >=
        transport.data);

    assert(
        local_capture.maybe_bytes_data <
        transport.data + transport.length);

    assert(
        memcmp(
            local_capture.maybe_bytes_data,
            bytes_data,
            sizeof(bytes_data)) == 0);

    assert(!local_capture.maybe_item_present);
}


static void test_local_null_values(void) {
    static const uint8_t frame[] = {
        21u,
        0x07u,
        0x44u
    };

    local_capture_t capture = {0};

    nullable_param_api_local_t local =
        make_local(
            &capture);

    uint8_t scratch[32] = {0};

    aether_meta_read_context_t ctx =
        make_read_context(
            frame,
            sizeof(frame),
            scratch,
            sizeof(scratch));

    assert(
        nullable_param_api_dispatch(
            &local,
            &ctx) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(frame));
    assert(ctx.scratch_position == 0u);

    assert(capture.send_calls == 1u);
    assert(!capture.maybe_short_present);
    assert(!capture.maybe_bytes_present);
    assert(!capture.maybe_item_present);
    assert(capture.required == INT8_C(0x44));
}


static void test_local_response_order(void) {
    static const uint8_t frame[] = {
        22u,
        0x78u, 0x56u, 0x34u, 0x12u,
        0x01u,
        0x5au
    };

    local_capture_t capture = {0};

    nullable_param_api_local_t local =
        make_local(
            &capture);

    uint8_t scratch[32] = {0};

    aether_meta_read_context_t ctx =
        make_read_context(
            frame,
            sizeof(frame),
            scratch,
            sizeof(scratch));

    assert(
        nullable_param_api_dispatch(
            &local,
            &ctx) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(frame));
    assert(ctx.scratch_position == 0u);

    assert(capture.query_calls == 1u);
    assert(capture.request_id == UINT32_C(0x12345678));
    assert(!capture.maybe_code_present);
    assert(capture.required == INT8_C(0x5a));
}


static void test_truncated_nullable_mask_rejected(void) {
    static const uint8_t frame[] = {
        21u
    };

    local_capture_t capture = {0};

    nullable_param_api_local_t local =
        make_local(
            &capture);

    uint8_t scratch[16] = {0};

    aether_meta_read_context_t ctx =
        make_read_context(
            frame,
            sizeof(frame),
            scratch,
            sizeof(scratch));

    assert(
        nullable_param_api_dispatch(
            &local,
            &ctx) ==
        AETHER_ERR_PROTOCOL);

    assert(capture.send_calls == 0u);
}


int main(void) {
    test_remote_mixed_nullable_wire();
    test_remote_all_nullable_absent();
    test_response_request_id_precedes_mask();
    test_remote_to_local_round_trip();
    test_local_null_values();
    test_local_response_order();
    test_truncated_nullable_mask_rejected();

    return 0;
}
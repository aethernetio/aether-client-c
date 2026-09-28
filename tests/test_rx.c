
#include "aether_rx.h"

#include <assert.h>
#include <string.h>


typedef struct {
    size_t calls;
    aether_uuid_t from;
    uint8_t data[16];
    size_t length;
} capture_t;


static void capture_message(
    void *ctx,
    aether_uuid_t from,
    const uint8_t *data,
    size_t length) {

    capture_t *capture =
        ctx;

    ++capture->calls;

    capture->from =
        from;

    assert(
        length <=
        sizeof(capture->data));

    memcpy(
        capture->data,
        data,
        length);

    capture->length =
        length;
}


static aether_ingress_t message_ingress(
    aether_uuid_t from,
    const uint8_t *data,
    size_t length) {

    aether_ingress_t ingress;

    memset(
        &ingress,
        0,
        sizeof(ingress));

    ingress.kind =
        AETHER_INGRESS_MESSAGE;

    ingress.as.message.from =
        from;

    ingress.as.message.payload.data =
        data;

    ingress.as.message.payload.length =
        length;

    return ingress;
}


static void test_first(void) {
    static const uint8_t one[] = {
        1u, 2u, 3u
    };

    static const uint8_t two[] = {
        4u, 5u
    };

    aether_ingress_t first_ingress =
        message_ingress(
            (aether_uuid_t){
                UINT64_C(1),
                UINT64_C(2)
            },
            one,
            sizeof(one));

    aether_ingress_t second_ingress =
        message_ingress(
            (aether_uuid_t){
                UINT64_C(3),
                UINT64_C(4)
            },
            two,
            sizeof(two));

    aether_rx_first_t rx;
    capture_t capture;

    memset(
        &capture,
        0,
        sizeof(capture));

    aether_rx_first_init(
        &rx);

    assert(
        aether_rx_first_accept(
            &rx,
            &first_ingress,
            capture_message,
            &capture));

    assert(capture.calls == 1u);
    assert(capture.from.msb == UINT64_C(1));
    assert(capture.from.lsb == UINT64_C(2));
    assert(capture.length == sizeof(one));
    assert(memcmp(capture.data, one, sizeof(one)) == 0);

    /*
     * FIRST recognizes the second MESSAGE so the caller can consume it,
     * but does not deliver it before reset.
     */
    assert(
        aether_rx_first_accept(
            &rx,
            &second_ingress,
            capture_message,
            &capture));

    assert(capture.calls == 1u);

    aether_rx_first_reset(
        &rx);

    assert(
        aether_rx_first_accept(
            &rx,
            &second_ingress,
            capture_message,
            &capture));

    assert(capture.calls == 2u);
    assert(capture.from.msb == UINT64_C(3));
    assert(capture.from.lsb == UINT64_C(4));
    assert(capture.length == sizeof(two));
    assert(memcmp(capture.data, two, sizeof(two)) == 0);
}


static void test_all(void) {
    static const uint8_t payload[] = {
        9u
    };

    aether_ingress_t ingress =
        message_ingress(
            (aether_uuid_t){
                UINT64_C(10),
                UINT64_C(11)
            },
            payload,
            sizeof(payload));

    aether_rx_all_t rx;
    capture_t capture;

    memset(
        &capture,
        0,
        sizeof(capture));

    aether_rx_all_init(
        &rx);

    assert(
        aether_rx_all_accept(
            &rx,
            &ingress,
            capture_message,
            &capture));

    assert(
        aether_rx_all_accept(
            &rx,
            &ingress,
            capture_message,
            &capture));

    assert(capture.calls == 2u);
}


static void test_adaptive(void) {
    static const uint8_t payload[] = {
        7u, 8u
    };

    aether_ingress_t ingress =
        message_ingress(
            (aether_uuid_t){
                UINT64_C(20),
                UINT64_C(21)
            },
            payload,
            sizeof(payload));

    aether_rx_adaptive_t rx;
    capture_t capture;

    memset(
        &capture,
        0,
        sizeof(capture));

    aether_rx_adaptive_init(
        &rx,
        false);

    assert(
        aether_rx_adaptive_accept(
            &rx,
            &ingress,
            capture_message,
            &capture));

    assert(
        aether_rx_adaptive_accept(
            &rx,
            &ingress,
            capture_message,
            &capture));

    assert(capture.calls == 1u);

    aether_rx_adaptive_set_all(
        &rx,
        true);

    assert(
        aether_rx_adaptive_accept(
            &rx,
            &ingress,
            capture_message,
            &capture));

    assert(
        aether_rx_adaptive_accept(
            &rx,
            &ingress,
            capture_message,
            &capture));

    assert(capture.calls == 3u);

    aether_rx_adaptive_set_all(
        &rx,
        false);

    /*
     * FIRST state survives a temporary ALL period.
     */
    assert(
        aether_rx_adaptive_accept(
            &rx,
            &ingress,
            capture_message,
            &capture));

    assert(capture.calls == 3u);

    aether_rx_adaptive_reset(
        &rx);

    assert(
        aether_rx_adaptive_accept(
            &rx,
            &ingress,
            capture_message,
            &capture));

    assert(capture.calls == 4u);
}


static void test_non_message_not_accepted(void) {
    aether_ingress_t ingress;

    memset(
        &ingress,
        0,
        sizeof(ingress));

    ingress.kind =
        AETHER_INGRESS_NONE;

    aether_rx_first_t first;
    aether_rx_all_t all;
    aether_rx_adaptive_t adaptive;

    aether_rx_first_init(
        &first);

    aether_rx_all_init(
        &all);

    aether_rx_adaptive_init(
        &adaptive,
        true);

    assert(
        !aether_rx_first_accept(
            &first,
            &ingress,
            NULL,
            NULL));

    assert(
        !aether_rx_all_accept(
            &all,
            &ingress,
            NULL,
            NULL));

    assert(
        !aether_rx_adaptive_accept(
            &adaptive,
            &ingress,
            NULL,
            NULL));
}


int main(void) {
    test_first();
    test_all();
    test_adaptive();
    test_non_message_not_accepted();
    return 0;
}

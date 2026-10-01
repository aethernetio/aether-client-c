
#include "aether.h"
#include "aether_registration.h"
#include "p2p_test_support.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


typedef struct {
    unsigned count;
    aether_uuid_t from;
    uint8_t data[AETHER_MAX_PACKET];
    size_t size;
} received_message_t;


static aether_t client_a;
static aether_t client_b;

static aether_registration_t registration_a;
static aether_registration_t registration_b;


static received_message_t received_a;
static received_message_t received_b;

static unsigned errors;


static bool uuid_equal(
    aether_uuid_t a,
    aether_uuid_t b) {

    return
        a.msb == b.msb &&
        a.lsb == b.lsb;
}


static void on_message(
    aether_t *client,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    received_message_t *received =
        client == &client_a
            ? &received_a
            : &received_b;

    if (size >
        sizeof(received->data)) {

        ++errors;
        return;
    }

    ++received->count;

    received->from =
        from;

    received->size =
        size;

    memcpy(
        received->data,
        data,
        size);
}


static void on_error(
    aether_t *client,
    aether_status_t code,
    aether_error_origin_t origin) {

    ++errors;

    fprintf(
        stderr,
        "%s Aether error code=%d origin=%d\n",
        client == &client_a
            ? "c-client-a"
            : "c-client-b",
        (int)code,
        (int)origin);
}


static int poll_pair(void) {
    if (registration_a.client == NULL) {
        aether_registration_init(
            &registration_a,
            &client_a.app.core);
    }

    if (registration_b.client == NULL) {
        aether_registration_init(
            &registration_b,
            &client_b.app.core);
    }

    if (!aether_is_registered(&client_a)) {
        if (aether_registration_poll(
                &registration_a) !=
            AETHER_OK) {

            return -1;
        }
    }

    if (!aether_is_registered(&client_b)) {
        if (aether_registration_poll(
                &registration_b) !=
            AETHER_OK) {

            return -1;
        }
    }

    if (aether_poll(&client_a) != AETHER_OK) {
        return -1;
    }

    if (aether_poll(&client_b) != AETHER_OK) {
        return -1;
    }

    return
        errors == 0u
            ? 0
            : -1;
}


static int wait_ready(
    uint64_t timeout_ms) {

    uint64_t deadline =
        p2p_test_now_ms() + timeout_ms;

    while (p2p_test_now_ms() <
           deadline) {

        if (aether_is_ready(&client_a) &&
            aether_is_ready(&client_b) &&
            aether_is_registered(&client_a) &&
            aether_is_registered(&client_b)) {

            return 0;
        }

        if (poll_pair() != 0) {
            return -1;
        }
    }

    return -1;
}


static int pump_for(
    uint64_t duration_ms) {

    uint64_t deadline =
        p2p_test_now_ms() + duration_ms;

    while (p2p_test_now_ms() <
           deadline) {

        if (poll_pair() != 0) {
            return -1;
        }
    }

    return 0;
}


static int wait_message(
    received_message_t *received,
    unsigned previous_count,
    uint64_t timeout_ms) {

    uint64_t deadline =
        p2p_test_now_ms() + timeout_ms;

    while (p2p_test_now_ms() <
           deadline) {

        if (received->count >
            previous_count) {

            return 0;
        }

        if (poll_pair() != 0) {
            return -1;
        }
    }

    return -1;
}


static int verify_message(
    const received_message_t *received,
    aether_uuid_t expected_from,
    const uint8_t *expected,
    size_t expected_size) {

    return
        uuid_equal(
            received->from,
            expected_from) &&
        received->size == expected_size &&
        memcmp(
            received->data,
            expected,
            expected_size) == 0
        ? 0
        : -1;
}


int main(void) {
    static const aether_uuid_t parent = {
        UINT64_C(0xB1AC52C88D94BD39),
        UINT64_C(0x4C01A631AC594165)
    };

    /*
     * This is the complete client setup.
     *
     * Client A uses the default persistent slot.
     * Client B uses a second independent identity.
     */
    if (aether_init(
            &client_a,
            parent) != AETHER_OK ||
        aether_init_slot(
            &client_b,
            parent,
            1u) != AETHER_OK) {

        fprintf(
            stderr,
            "Failed to initialize C clients\n");

        return 1;
    }

    aether_on_message(
        &client_a,
        on_message);

    aether_on_message(
        &client_b,
        on_message);

    aether_on_error(
        &client_a,
        on_error);

    aether_on_error(
        &client_b,
        on_error);

    int result = 1;

    if (aether_start(&client_a) != AETHER_OK ||
        aether_start(&client_b) != AETHER_OK) {

        fprintf(
            stderr,
            "Failed to start C clients\n");

        goto cleanup;
    }

    if (wait_ready(20000u) != 0) {
        fprintf(
            stderr,
            "C clients did not become ready\n");

        goto cleanup;
    }

    if (uuid_equal(
            aether_uid(&client_a),
            aether_uid(&client_b))) {

        fprintf(
            stderr,
            "Clients received identical UIDs\n");

        goto cleanup;
    }


    /*
     * The public facade intentionally uses the production activity timing:
     * ping = 6000 ms, rx window = 5000 ms.
     *
     * Keep polling until the first periodic UAP ping has certainly reached
     * the server before testing P2P delivery.
     */
    if (pump_for(6500u) != 0) {
        fprintf(
            stderr,
            "Failed while establishing UAP window\n");

        goto cleanup;
    }


    static const uint8_t message_a_to_b[] = {
        1u, 2u, 3u, 4u
    };

    unsigned previous_b =
        received_b.count;


    aether_status_t send_a =
        aether_send(
            &client_a,
            aether_uid(&client_b),
            message_a_to_b,
            sizeof(message_a_to_b));

    if (send_a != AETHER_OK) {
        fprintf(
            stderr,
            "A -> B send failed: %d\n",
            (int)send_a);

        goto cleanup;
    }



    if (wait_message(
            &received_b,
            previous_b,
            10000u) != 0) {

        fprintf(
            stderr,
            "A -> B message timeout\n");

        goto cleanup;
    }

    if (verify_message(
            &received_b,
            aether_uid(&client_a),
            message_a_to_b,
            sizeof(message_a_to_b)) != 0) {

        fprintf(
            stderr,
            "A -> B message mismatch\n");

        goto cleanup;
    }


    static const uint8_t message_b_to_a[] = {
        4u, 3u, 2u, 1u
    };

    unsigned previous_a =
        received_a.count;

    if (aether_send(
            &client_b,
            aether_uid(&client_a),
            message_b_to_a,
            sizeof(message_b_to_a)) != AETHER_OK) {

        goto cleanup;
    }

    if (wait_message(
            &received_a,
            previous_a,
            10000u) != 0 ||
        verify_message(
            &received_a,
            aether_uid(&client_b),
            message_b_to_a,
            sizeof(message_b_to_a)) != 0) {

        goto cleanup;
    }

    printf(
        "C public API P2P E2E PASS\n");

    result = 0;

cleanup:
    aether_stop(&client_a);
    aether_stop(&client_b);

    return result;
}
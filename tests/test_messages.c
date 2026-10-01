
#include "aether_messages.h"

#include <assert.h>
#include <string.h>


typedef struct {
    unsigned count;
    aether_peer_t *peer;
    uint8_t data[16];
    size_t size;
} capture_t;


static void capture_message(
    aether_peer_t *peer,
    const uint8_t *data,
    size_t size,
    void *ctx) {

    capture_t *capture =
        (capture_t *)ctx;

    assert(capture != NULL);
    assert(size <= sizeof(capture->data));

    ++capture->count;

    capture->peer =
        peer;

    capture->size =
        size;

    memcpy(
        capture->data,
        data,
        size);
}


static void dispatch(
    aether_t *client,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    assert(client != NULL);
    assert(client->on_message != NULL);

    client->on_message(
        client,
        from,
        data,
        size);
}


static void test_peer_routing(void) {
    aether_t client_a;
    aether_t client_b;

    aether_peer_t peer_a;
    aether_peer_t peer_b;
    aether_peer_t same_uid_other_client;

    capture_t capture_a;
    capture_t capture_b;
    capture_t capture_other;

    memset(
        &client_a,
        0,
        sizeof(client_a));

    memset(
        &client_b,
        0,
        sizeof(client_b));

    memset(
        &peer_a,
        0,
        sizeof(peer_a));

    memset(
        &peer_b,
        0,
        sizeof(peer_b));

    memset(
        &same_uid_other_client,
        0,
        sizeof(same_uid_other_client));

    memset(
        &capture_a,
        0,
        sizeof(capture_a));

    memset(
        &capture_b,
        0,
        sizeof(capture_b));

    memset(
        &capture_other,
        0,
        sizeof(capture_other));

    aether_uuid_t uid_a = {
        0x0102030405060708ULL,
        0x1112131415161718ULL
    };

    aether_uuid_t uid_b = {
        0x2122232425262728ULL,
        0x3132333435363738ULL
    };

    aether_uuid_t uid_unknown = {
        0x4142434445464748ULL,
        0x5152535455565758ULL
    };

    assert(
        aether_peer_init(
            &peer_a,
            &client_a,
            uid_a) ==
        AETHER_OK);

    assert(
        aether_peer_init(
            &peer_b,
            &client_a,
            uid_b) ==
        AETHER_OK);

    assert(
        aether_peer_init(
            &same_uid_other_client,
            &client_b,
            uid_a) ==
        AETHER_OK);

    aether_peer_on_message(
        &peer_a,
        capture_message,
        &capture_a);

    aether_peer_on_message(
        &peer_b,
        capture_message,
        &capture_b);

    aether_peer_on_message(
        &same_uid_other_client,
        capture_message,
        &capture_other);

    static const uint8_t first[] = {
        1u,
        2u,
        3u
    };

    dispatch(
        &client_a,
        uid_a,
        first,
        sizeof(first));

    assert(capture_a.count == 1u);
    assert(capture_b.count == 0u);
    assert(capture_other.count == 0u);
    assert(capture_a.peer == &peer_a);
    assert(capture_a.size == sizeof(first));
    assert(
        memcmp(
            capture_a.data,
            first,
            sizeof(first)) ==
        0);

    dispatch(
        &client_b,
        uid_a,
        first,
        sizeof(first));

    assert(capture_a.count == 1u);
    assert(capture_other.count == 1u);
    assert(
        capture_other.peer ==
        &same_uid_other_client);

    static const uint8_t second[] = {
        9u,
        8u
    };

    dispatch(
        &client_a,
        uid_b,
        second,
        sizeof(second));

    assert(capture_b.count == 1u);
    assert(capture_b.peer == &peer_b);

    dispatch(
        &client_a,
        uid_unknown,
        second,
        sizeof(second));

    assert(capture_a.count == 1u);
    assert(capture_b.count == 1u);

    aether_peer_t duplicate;

    memset(
        &duplicate,
        0,
        sizeof(duplicate));

    assert(
        aether_peer_init(
            &duplicate,
            &client_a,
            uid_a) ==
        AETHER_ERR_BUSY);

    assert(
        aether_peer_init(
            &peer_a,
            &client_a,
            uid_a) ==
        AETHER_ERR_BUSY);

    assert(
        aether_peer_uid(
            &peer_a).msb ==
        uid_a.msb);

    aether_peer_close(
        &peer_a);

    dispatch(
        &client_a,
        uid_a,
        first,
        sizeof(first));

    assert(capture_a.count == 1u);

    assert(client_a.on_message != NULL);

    aether_peer_close(
        &peer_b);

    assert(client_a.on_message == NULL);

    assert(client_b.on_message != NULL);

    aether_peer_close(
        &same_uid_other_client);

    assert(client_b.on_message == NULL);
}


static void foreign_message_callback(
    aether_t *client,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    (void)client;
    (void)from;
    (void)data;
    (void)size;
}


static void test_callback_ownership(void) {
    aether_t client;
    aether_peer_t peer;

    memset(
        &client,
        0,
        sizeof(client));

    memset(
        &peer,
        0,
        sizeof(peer));

    aether_on_message(
        &client,
        foreign_message_callback);

    assert(
        aether_peer_init(
            &peer,
            &client,
            (aether_uuid_t){1u, 2u}) ==
        AETHER_ERR_BUSY);

    assert(
        client.on_message ==
        foreign_message_callback);
}


static void test_argument_guards(void) {
    aether_peer_t peer;

    memset(
        &peer,
        0,
        sizeof(peer));

    assert(
        aether_peer_init(
            NULL,
            NULL,
            (aether_uuid_t){0u, 0u}) ==
        AETHER_ERR_ARGUMENT);

    assert(
        aether_peer_send(
            NULL,
            NULL,
            0u) ==
        AETHER_ERR_ARGUMENT);

    assert(
        aether_peer_uid(
            NULL).msb ==
        0u);
}


int main(void) {
    test_peer_routing();
    test_callback_ownership();
    test_argument_guards();
    return 0;
}

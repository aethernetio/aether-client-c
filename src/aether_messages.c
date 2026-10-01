
#include "aether_messages.h"

#include <stdbool.h>
#include <string.h>


/*
 * Optional registry.
 *
 * This storage exists only when this translation unit is pulled into the final
 * link. Firmware using only aether_send_message() never references this object
 * file and therefore pays neither this pointer nor the peer implementation.
 */
static aether_peer_t *peers;


static bool uuid_equal(
    aether_uuid_t a,
    aether_uuid_t b) {

    return
        a.msb == b.msb &&
        a.lsb == b.lsb;
}


static bool peer_is_registered(
    const aether_peer_t *peer) {

    for (const aether_peer_t *current =
             peers;
         current != NULL;
         current = current->next) {

        if (current == peer) {
            return true;
        }
    }

    return false;
}


static aether_peer_t *find_peer(
    const aether_t *client,
    aether_uuid_t uid) {

    for (aether_peer_t *peer =
             peers;
         peer != NULL;
         peer = peer->next) {

        if (peer->client == client &&
            uuid_equal(
                peer->uid,
                uid)) {

            return peer;
        }
    }

    return NULL;
}


static bool client_has_peer(
    const aether_t *client) {

    for (const aether_peer_t *peer =
             peers;
         peer != NULL;
         peer = peer->next) {

        if (peer->client == client) {
            return true;
        }
    }

    return false;
}


static void receive_message(
    aether_t *client,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    aether_peer_t *peer =
        find_peer(
            client,
            from);

    if (peer == NULL ||
        peer->on_message == NULL) {

        return;
    }

    peer->on_message(
        peer,
        data,
        size,
        peer->message_ctx);
}


aether_status_t aether_peer_init(
    aether_peer_t *peer,
    aether_t *client,
    aether_uuid_t uid) {

    if (peer == NULL ||
        client == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (peer_is_registered(
            peer)) {

        return AETHER_ERR_BUSY;
    }

    if (find_peer(
            client,
            uid) != NULL) {

        return AETHER_ERR_BUSY;
    }

    /*
     * The default peer owns the beginner facade's receive callback while at
     * least one peer is attached. Do not silently replace application policy.
     */
    if (client->on_message != NULL &&
        client->on_message !=
            receive_message) {

        return AETHER_ERR_BUSY;
    }

    memset(
        peer,
        0,
        sizeof(*peer));

    peer->client =
        client;

    peer->uid =
        uid;

    peer->next =
        peers;

    peers =
        peer;

    if (client->on_message == NULL) {
        aether_on_message(
            client,
            receive_message);
    }

    return AETHER_OK;
}


void aether_peer_close(
    aether_peer_t *peer) {

    if (peer == NULL) {
        return;
    }

    aether_t *client =
        peer->client;

    aether_peer_t **cursor =
        &peers;

    while (*cursor != NULL) {
        if (*cursor == peer) {
            *cursor =
                peer->next;

            break;
        }

        cursor =
            &(*cursor)->next;
    }

    peer->client =
        NULL;

    peer->next =
        NULL;

    peer->on_message =
        NULL;

    peer->message_ctx =
        NULL;

    if (client != NULL &&
        !client_has_peer(
            client) &&
        client->on_message ==
            receive_message) {

        aether_on_message(
            client,
            NULL);
    }
}


void aether_peer_on_message(
    aether_peer_t *peer,
    aether_peer_message_fn callback,
    void *ctx) {

    if (peer == NULL) {
        return;
    }

    peer->on_message =
        callback;

    peer->message_ctx =
        callback == NULL
            ? NULL
            : ctx;
}


aether_status_t aether_peer_send(
    aether_peer_t *peer,
    const uint8_t *data,
    size_t size) {

    if (peer == NULL ||
        peer->client == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    return
        aether_send_message(
            peer->client,
            peer->uid,
            data,
            size);
}


aether_uuid_t aether_peer_uid(
    const aether_peer_t *peer) {

    return
        peer == NULL
            ? (aether_uuid_t){0u, 0u}
            : peer->uid;
}

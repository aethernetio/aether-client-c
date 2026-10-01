
/*
 * Optional caller-owned bidirectional messaging channel.
 *
 * One aether_peer_t binds one remote Aether UUID to send/receive operations.
 * It is a logical relationship, not a transport connection, and therefore
 * survives work-server reconnect/failover transparently.
 *
 * The default component is intentionally compact:
 * - no heap;
 * - no queue;
 * - no copied payload storage;
 * - no retry/config arrays;
 * - no mandatory storage in aether_t.
 *
 * Applications that only send one-way messages do not need this component;
 * use aether_send_message() directly.
 */
#ifndef AETHER_MESSAGES_H
#define AETHER_MESSAGES_H

#include "aether.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct aether_peer aether_peer_t;


typedef void (*aether_peer_message_fn)(
    aether_peer_t *peer,
    const uint8_t *data,
    size_t size,
    void *ctx);


/*
 * Caller-owned logical channel to one remote UID.
 *
 * `next` belongs to the optional messaging implementation. Applications
 * should initialize the object once and keep it alive until aether_peer_close().
 */
struct aether_peer {
    aether_t *client;
    aether_peer_t *next;

    aether_uuid_t uid;

    aether_peer_message_fn on_message;
    void *message_ctx;
};


/*
 * Bind this peer to client + uid.
 *
 * The default component uses a tiny intrusive registry. There is no separate
 * router object and no configuration required for the common case.
 *
 * A given client/uid pair may be attached only once.
 */
aether_status_t aether_peer_init(
    aether_peer_t *peer,
    aether_t *client,
    aether_uuid_t uid);


/* Detach the caller-owned peer. No memory is allocated or freed. */
void aether_peer_close(
    aether_peer_t *peer);


/*
 * Install the receive callback for this peer.
 *
 * Passing NULL disables receive delivery. Payload bytes are borrowed and valid
 * only during the callback.
 */
void aether_peer_on_message(
    aether_peer_t *peer,
    aether_peer_message_fn callback,
    void *ctx);


/*
 * Send one message to this peer.
 *
 * No aether_is_ready() guard is required. The common send primitive performs
 * readiness/writability validation internally.
 */
aether_status_t aether_peer_send(
    aether_peer_t *peer,
    const uint8_t *data,
    size_t size);


/* Return the remote UID bound to this peer, or zero for NULL. */
aether_uuid_t aether_peer_uid(
    const aether_peer_t *peer);


#ifdef __cplusplus
}
#endif

#endif

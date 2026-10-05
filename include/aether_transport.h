
/*
 * Semantic Aether transport interface.
 *
 * The upper layer (RPC, ACL, application) speaks only aether_transport_t.
 * Concrete implementations own the physical route and the Aether-aware
 * routing policy behind aether_transport_vtable_t:
 *
 *     server transport   - cloud/server path (Phase 2A only)
 *     peer/LAN transport - direct peer path (later)
 *     mux transport      - composes several transports (later)
 *     LoRa / BLE         - alternative physical media (later)
 *
 * The upper layer never sees TCP/UDP/Wi-Fi/LAN/multicast/sockets/server ids
 * or peer routes.
 *
 * Batch lifecycle (any implementation):
 *   IDLE     -> no batch pending; new semantic commands accepted.
 *   BUILDING -> commands accumulated; more commands accepted.
 *   FLUSHING -> flush accepted; new semantic commands return BUSY.

 *
 * Borrowed input: the caller retains ownership of any memory range
 * passed to a semantic command. The transport only borrows it. The
 * range must remain valid and unchanged until the batch completes
 * (on_flush_done) or until aether_transport_reset() returns.
 *
 * aether_transport_reset() drops the current batch and returns the
 * implementation to IDLE. It does not roll back bytes already handed to
 * the physical medium and does not touch server/peer sessions, discovery,
 * registration or credentials.
 *
 * Caller-owned memory only; no heap allocation.
 */

#ifndef AETHER_TRANSPORT_H
#define AETHER_TRANSPORT_H


#include "aether_client.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    AETHER_TRANSPORT_IDLE = 0,
    AETHER_TRANSPORT_BUILDING,
    AETHER_TRANSPORT_FLUSHING
} aether_transport_status_t;


typedef struct {
    void *ctx;

    void (*on_flush_done)(
        void *ctx,
        aether_status_t status);
} aether_transport_listener_t;


struct aether_client_app;


/*
 * Semantic transport interface.
 *
 * Lifecycle operations are mandatory for any real implementation.
 * Feature operations may be NULL when the implementation does not
 * support them; the generic wrappers surface that as
 * AETHER_ERR_UNSUPPORTED (or a harmless no-op for reset()).
 *
 * A mux transport is just another implementation of this same vtable.
 */
typedef struct aether_transport_vtable {

    aether_transport_status_t (*status)(
        void *ctx);

    aether_status_t (*flush)(
        void *ctx);

    void (*reset)(
        void *ctx);


    /*
     * Install a listener. The implementation copies the ctx and
     * callbacks by value during this call and does not retain a
     * pointer to the caller's listener structure.
     */
    void (*set_listener)(
        void *ctx,
        const aether_transport_listener_t *listener);


    aether_status_t (*tick)(
        void *ctx);

    /*
     * Feature: may be NULL if this transport does not route messages.
     */
    aether_status_t (*send_message)(
        void *ctx,
        aether_uuid_t uid,
        const uint8_t *data,
        size_t length);

} aether_transport_vtable_t;


/*
 * Uniform handle used by the upper layer.
 *
 * Can refer to a server transport, a future peer/LAN transport, or a
 * future mux. Lifetime and storage are owned by the caller (typically
 * aether_client_app_t).
 */
typedef struct aether_transport {
    void *ctx;
    const aether_transport_vtable_t *vtable;
} aether_transport_t;


void aether_transport_set_listener(
    aether_transport_t *transport,
    const aether_transport_listener_t *listener);


aether_transport_status_t aether_transport_status(
    const aether_transport_t *transport);


aether_status_t aether_transport_send_message(
    aether_transport_t *transport,
    aether_uuid_t uid,
    const uint8_t *data,
    size_t length);


#ifdef __cplusplus
}
#endif

#endif
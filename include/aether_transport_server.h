
/*
 * Concrete semantic transport implementation for the Aether server path.
 *
 * Phase 2A: only route is the existing server/cloud send path. The
 * implementation captures a batch of send_message commands and executes
 * them through aether_client_app_send_with_id(), preserving the current
 * synchronous behavior of aether_send_message().
 *
 * Caller owns storage: the object itself and the batch descriptor array.
 */

#ifndef AETHER_TRANSPORT_SERVER_H
#define AETHER_TRANSPORT_SERVER_H


#include "aether_transport.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


struct aether_client_app;



/*
 * Concrete semantic transport implementation for the Aether server path.
 *
 * send_message() only appends a semantic command to the current BUILDING
 * batch. Physical transmission happens in tick(), which is driven by
 * aether_work(). Borrowed payload pointers are valid until on_flush_done
 * for the current batch or until reset() returns.
 *
 * Command capacity is an internal property of this concrete transport
 * implementation. It is intentionally small and bounded; broader batch
 * semantics belong to a future mux/peer implementation.
 */

#define AETHER_TRANSPORT_SERVER_MAX_COMMANDS 4u


typedef struct {
    aether_uuid_t uid;
    const uint8_t *data;
    size_t length;
} aether_transport_server_command_t;


/*
 * Caller-owned server transport instance.
 */
typedef struct {
    struct aether_client_app *app;

    aether_transport_server_command_t commands[
        AETHER_TRANSPORT_SERVER_MAX_COMMANDS];
    size_t command_count;

    aether_transport_status_t status;
    uint32_t generation;

    aether_transport_listener_t listener;
} aether_transport_server_t;


void aether_transport_server_init(
    aether_transport_server_t *server,
    struct aether_client_app *app);



/*
 * Access the vtable for this instance and bind it into a generic handle.
 */
const aether_transport_vtable_t *aether_transport_server_vtable(void);


void aether_transport_server_bind(
    aether_transport_t *transport,
    aether_transport_server_t *server);


#ifdef __cplusplus
}
#endif

#endif
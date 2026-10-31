
/*
 * Concrete semantic transport implementation for the Aether server path.
 *
 * Phase 2A: the only route is the existing server/cloud send path. This
 * implementation does NOT keep its own command queue and does NOT store
 * any borrowed {uid, data, size} triples. Each send_message() call goes
 * directly through the existing aether_client_app_send_authorized() path,
 * which uses the existing ADSL/generated serializer.
 *
 * Borrowed lifetime is a public contract: the caller must keep each
 * memory range [data, data + length) valid and unchanged until
 * on_flush_done() for the current batch or until reset() returns. This
 * implementation may in practice stop reading the range immediately
 * after serialization; the external contract is nevertheless the
 * conservative one above.
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
 * Caller-owned server transport instance.
 */
typedef struct {
    struct aether_client_app *app;

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
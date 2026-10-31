
/*
 * Semantic transport server implementation.
 *
 * Phase 2A backend: the existing Aether server/cloud path through
 * aether_client_app_send_authorized(), which uses the existing
 * ADSL/generated serializer.
 *
 * This implementation does NOT keep its own command queue and does NOT
 * store any borrowed {uid, data, size} triples. Each send_message()
 * call goes directly through the generated serializer path.
 *
 * Borrowed lifetime is a public contract: the caller must keep the
 * memory range [data, data + length) valid and unchanged until
 * on_flush_done() for the current batch or until reset() returns.
 * In practice this implementation stops reading the range as soon as
 * the message has been serialized and handed to the platform transport.
 */

#include "aether_transport_server.h"
#include "aether_client_app.h"

#include <string.h>


void aether_transport_server_init(
    aether_transport_server_t *server,
    struct aether_client_app *app) {

    if (server == NULL) {
        return;
    }

    memset(
        server,
        0,
        sizeof(*server));

    server->app = app;
    server->status = AETHER_TRANSPORT_IDLE;
}


static aether_transport_status_t server_status(
    void *ctx) {

    const aether_transport_server_t *server = ctx;

    if (server == NULL) {
        return AETHER_TRANSPORT_IDLE;
    }

    return server->status;
}


static void server_set_listener(
    void *ctx,
    const aether_transport_listener_t *listener) {

    aether_transport_server_t *server = ctx;

    if (server == NULL) {
        return;
    }

    /*
     * The listener structure is copied by value. The caller may pass
     * the address of a stack variable; no pointer to it is retained.
     */
    if (listener == NULL) {

        memset(
            &server->listener,
            0,
            sizeof(server->listener));

        return;
    }

    server->listener = *listener;
}


static aether_status_t server_send_message(
    void *ctx,
    aether_uuid_t uid,
    const uint8_t *data,
    size_t length) {

    aether_transport_server_t *server = ctx;

    if (server == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (data == NULL && length != 0u) {
        return AETHER_ERR_ARGUMENT;
    }

    if (server->status ==
        AETHER_TRANSPORT_FLUSHING) {

        return AETHER_ERR_BUSY;
    }

    /*
     * Server route readiness is a concern of this concrete transport,
     * not of the generic application layer. A future mux may be able to
     * route through a direct peer even when the server route is not
     * READY.
     */
    if (server->app == NULL ||
        !aether_client_app_is_ready(server->app)) {

        return AETHER_ERR_STATE;
    }


    /*
     * Hand the message directly to the existing generated serializer
     * path. There is no intermediate queue and no descriptor storage here.
     */

    aether_status_t status =

        aether_client_app_send_authorized(
            server->app,
            uid,
            data,
            length);


    if (status != AETHER_OK) {
        return status;
    }

    server->status = AETHER_TRANSPORT_BUILDING;

    return AETHER_OK;
}


static aether_status_t server_flush(
    void *ctx) {

    aether_transport_server_t *server = ctx;

    if (server == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (server->status ==
        AETHER_TRANSPORT_FLUSHING) {

        return AETHER_ERR_BUSY;
    }

    if (server->status ==
        AETHER_TRANSPORT_IDLE) {

        return AETHER_OK;
    }

    server->status = AETHER_TRANSPORT_FLUSHING;

    return AETHER_OK;
}


static void server_reset(
    void *ctx) {

    aether_transport_server_t *server = ctx;

    if (server == NULL) {
        return;
    }


    server->status = AETHER_TRANSPORT_IDLE;
}


static aether_status_t server_tick(
    void *ctx) {

    aether_transport_server_t *server = ctx;

    if (server == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (server->status !=
        AETHER_TRANSPORT_FLUSHING) {

        return AETHER_OK;
    }


    /*
     * Server transport is synchronous with respect to the platform
 * transport: send_authorized() has already handed each message to the
     * physical channel. There is nothing further to progress here. The
     * batch simply closes and the listener is invoked.
     */


    server->status = AETHER_TRANSPORT_IDLE;

    if (server->listener.on_flush_done != NULL) {

        server->listener.on_flush_done(
            server->listener.ctx,
            AETHER_OK);
    }

    return AETHER_OK;
}


static const aether_transport_vtable_t SERVER_VTABLE = {
    server_status,
    server_flush,
    server_reset,
    server_set_listener,
    server_tick,
    server_send_message
};


const aether_transport_vtable_t *aether_transport_server_vtable(void) {

    return &SERVER_VTABLE;
}


void aether_transport_server_bind(
    aether_transport_t *transport,
    aether_transport_server_t *server) {

    if (transport == NULL) {
        return;
    }

    transport->ctx = server;
    transport->vtable =
        server != NULL
            ? &SERVER_VTABLE
            : NULL;
}
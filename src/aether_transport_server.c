


/*
 * Semantic transport server implementation.
 *
 * Phase 2A backend: existing Aether server/cloud send path through
 * aether_client_app_send_with_id(). Batch lifecycle matches the generic
 * contract in aether_transport.h.
 *
 * Command descriptors live inline in the transport object. Their count
 * is a private bounded limit of this concrete implementation, not a
 * public batch-storage contract. Borrowed payload pointers are valid
 * until on_flush_done for the current batch or until reset() returns.
 */

#include "aether_transport_server.h"
#include "aether_client_app.h"

#include <string.h>


static void clear_batch(
    aether_transport_server_t *server) {

    server->command_count = 0u;
}


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
    server->generation = 1u;
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


    if (server->command_count >=
        AETHER_TRANSPORT_SERVER_MAX_COMMANDS) {

        return AETHER_ERR_OVERFLOW;
    }

    aether_transport_server_command_t *command =
        &server->commands[server->command_count];

    command->uid = uid;
    command->data = data;
    command->length = length;

    server->command_count += 1u;

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

    /*
     * Invalidate any in-flight internal operation before releasing
     * borrowed input. Stale completions of the previous generation are
     * dropped by tick().
     */
    server->generation += 1u;

    clear_batch(server);
    server->status = AETHER_TRANSPORT_IDLE;
}


static aether_status_t server_execute_batch(
    aether_transport_server_t *server) {

    if (server->app == NULL) {
        return AETHER_ERR_STATE;
    }

    for (size_t i = 0u;

         i < server->command_count;
         ++i) {

        aether_transport_server_command_t *command =
            &server->commands[i];

        aether_status_t status =
            aether_client_app_send_with_id(
                server->app,
                command->uid,
                command->data,
                command->length,
                NULL);

        if (status != AETHER_OK) {
            return status;
        }
    }


    return AETHER_OK;
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

    uint32_t generation =
        server->generation;

    aether_status_t status =
        server_execute_batch(server);

    if (generation != server->generation) {
        return AETHER_OK;
    }

    /*
     * Release borrowed input and return to IDLE before invoking the
     * listener so the callback may begin the next batch.
     */
    clear_batch(server);
    server->status = AETHER_TRANSPORT_IDLE;

    if (server->listener.on_flush_done != NULL) {

        server->listener.on_flush_done(
            server->listener.ctx,
            status);
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
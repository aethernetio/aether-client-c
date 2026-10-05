
/*
 * Generic semantic transport dispatch.
 *
 * Every call goes through the implementation vtable. Nothing here knows
 * about servers, peers or physical media.
 */

#include "aether_transport.h"

#include <string.h>


static bool transport_is_valid(
    const aether_transport_t *transport) {

    return
        transport != NULL &&
        transport->vtable != NULL;
}


void aether_transport_set_listener(
    aether_transport_t *transport,
    const aether_transport_listener_t *listener) {

    if (!transport_is_valid(transport) ||
        transport->vtable->set_listener == NULL) {

        return;
    }

    transport->vtable->set_listener(
        transport->ctx,
        listener);
}


aether_transport_status_t aether_transport_status(
    const aether_transport_t *transport) {

    if (!transport_is_valid(transport) ||
        transport->vtable->status == NULL) {

        return AETHER_TRANSPORT_IDLE;
    }

    return
        transport->vtable->status(
            transport->ctx);
}


aether_status_t aether_transport_send_message(
    aether_transport_t *transport,
    aether_uuid_t uid,
    const uint8_t *data,
    size_t length) {

    if (!transport_is_valid(transport)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (transport->vtable->send_message == NULL) {
        return AETHER_ERR_UNSUPPORTED;
    }

    return
        transport->vtable->send_message(
            transport->ctx,
            uid,
            data,
            length);
}


aether_status_t aether_transport_flush(
    aether_transport_t *transport) {

    if (!transport_is_valid(transport) ||
        transport->vtable->flush == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    return
        transport->vtable->flush(
            transport->ctx);
}


void aether_transport_reset(
    aether_transport_t *transport) {

    if (!transport_is_valid(transport) ||
        transport->vtable->reset == NULL) {

        return;
    }

    transport->vtable->reset(
        transport->ctx);
}


aether_status_t aether_transport_tick(
    aether_transport_t *transport) {

    if (!transport_is_valid(transport) ||
        transport->vtable->tick == NULL) {

        return AETHER_OK;
    }

    return
        transport->vtable->tick(
            transport->ctx);
}


#include "aether_registration.h"
#include "aether.h"
#include "aether_client_internal.h"
#include <string.h>



static const aether_registration_internal_ops_t
REGISTRATION_INTERNAL_OPS = {
    aether_client_registration_on_rx_internal,
    aether_client_registration_on_transport_state_internal,
    aether_client_registration_on_poll_internal
};



void aether_registration_init(
    aether_registration_t *registration,
    aether_client_t *client) {

    if (registration == NULL) {
        return;
    }

    memset(
        registration,
        0,
        sizeof(*registration));

    registration->client =
        client;


    registration->internal =
        &REGISTRATION_INTERNAL_OPS;


    if (client != NULL &&
        !client->registered &&
        client->server_count == 0u) {

        /*
         * registration aliases the unused ServerDescriptor cache inside
         * aether_client_t. The cache becomes live only after identity creation,
         * when registration callbacks are no longer dispatched.
         */
        client->registration =
            registration;
    }
}


aether_status_t aether_registration_poll(
    aether_registration_t *registration) {

    if (registration == NULL ||
        registration->client == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (!registration->client->registered &&
        registration->client->registration !=
            registration) {

        return AETHER_ERR_STATE;
    }

    aether_client_t *client =
        registration->client;

    if (aether_client_is_registered(
            client)) {

        return AETHER_OK;
    }

    aether_state_t state =
        aether_client_state(
            client);

    if (state ==
        AETHER_STATE_NO_IDENTITY) {

        return
            aether_client_begin_registration_internal(
                client);
    }

    if (state ==
            AETHER_STATE_REG_CONNECTING ||
        state ==
            AETHER_STATE_REG_WAIT_SERVER_KEY ||
        state ==
            AETHER_STATE_REG_WAIT_POW ||
        state ==
            AETHER_STATE_REG_WAIT_FINISH) {

        return AETHER_OK;
    }

    return AETHER_ERR_STATE;
}


bool aether_registration_done(
    const aether_registration_t *registration) {

    return
        registration != NULL &&
        registration->client != NULL &&
        aether_client_is_registered(
            registration->client);
}



aether_status_t aether_registration_work(
    struct aether *client,
    void *context) {

    aether_registration_t *registration =
        (aether_registration_t *)context;

    if (registration == NULL ||
        client == NULL) {

        return AETHER_ERR_ARGUMENT;
    }


    if (registration->client !=
        &client->app.core) {

        return AETHER_ERR_ARGUMENT;
    }


    /*
     * Component lifecycle rule: a state machine that is waiting for
     * a prerequisite returns AETHER_OK and re-checks on the next
     * step. aether_registration_poll() already returns AETHER_OK for
     * its own REG_* waiting states, but the underlying
     * begin_registration path can transiently report AETHER_ERR_BUSY
     * while a core callback is active. That is a waiting condition,
     * not a real failure, so it is normalized here.
     *
     * All other non-OK statuses are real errors (missing POW
     * provider, wrong state, wrong client) and are passed through
     * unchanged so the caller sees them.
     */
    aether_status_t status =
        aether_registration_poll(
            registration);

    if (status == AETHER_ERR_BUSY) {
        return AETHER_OK;
    }

    return status;
}

#include "aether_send_future.h"

#include <string.h>


static uint32_t configured_timeout_ms(
    const aether_client_app_t *client) {

    if (client->config.request_timeout_ms !=
        0u) {

        return
            client->config.request_timeout_ms;
    }

    uint32_t base =
        client->config.ping_interval_ms;

    if (client->config.rx_window_ms >
        base) {

        base =
            client->config.rx_window_ms;
    }

    if (base == 0u) {
        return 10000u;
    }

    if (base >
        UINT32_MAX /
            2u) {

        return UINT32_MAX;
    }

    return
        base * 2u;
}


void aether_send_future_init(
    aether_send_future_t *future) {

    if (future == NULL) {
        return;
    }

    memset(
        future,
        0,
        sizeof(*future));

    future->result =
        AETHER_OK;

    future->state =
        AETHER_SEND_FUTURE_IDLE;
}


void aether_send_future_reset(
    aether_send_future_t *future) {

    aether_send_future_init(
        future);
}


aether_status_t aether_send_future_begin(
    aether_send_future_t *future,
    uint32_t request_id,
    uint64_t now_ms,
    uint32_t timeout_ms) {

    if (future == NULL ||
        request_id == 0u) {

        return AETHER_ERR_ARGUMENT;
    }

    if (future->state ==
        AETHER_SEND_FUTURE_PENDING) {

        return AETHER_ERR_BUSY;
    }

    future->request_id =
        request_id;

    future->started_at_ms =
        (uint32_t)now_ms;

    future->timeout_ms =
        timeout_ms;

    future->result =
        AETHER_OK;

    future->state =
        AETHER_SEND_FUTURE_PENDING;

    return AETHER_OK;
}


aether_status_t aether_send_future_send(
    aether_send_future_t *future,
    aether_client_app_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t length) {

    if (future == NULL ||
        client == NULL ||
        client->platform.now_ms == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (future->state ==
        AETHER_SEND_FUTURE_PENDING) {

        return AETHER_ERR_BUSY;
    }

    uint32_t request_id =
        0u;

    aether_status_t status =
        aether_client_app_send_with_id(
            client,
            destination,
            data,
            length,
            &request_id);

    if (status != AETHER_OK) {
        return status;
    }

    return
        aether_send_future_begin(
            future,
            request_id,
            client->platform.now_ms(
                client->platform.ctx),
            configured_timeout_ms(
                client));
}


bool aether_send_future_accept(
    aether_send_future_t *future,
    const aether_ingress_t *ingress) {

    if (future == NULL ||
        ingress == NULL ||
        future->state !=
            AETHER_SEND_FUTURE_PENDING ||
        ingress->kind !=
            AETHER_INGRESS_REQUEST_RESULT ||
        ingress->as.request_result.request_id !=
            future->request_id) {

        return false;
    }

    future->result =
        ingress->as.request_result.status;

    future->state =
        future->result ==
                AETHER_OK
            ? AETHER_SEND_FUTURE_DONE
            : AETHER_SEND_FUTURE_ERROR;

    return true;
}


bool aether_send_future_tick(
    aether_send_future_t *future,
    uint64_t now_ms) {

    if (future == NULL ||
        future->state !=
            AETHER_SEND_FUTURE_PENDING ||
        future->timeout_ms == 0u) {

        return false;
    }

    uint32_t elapsed =
        (uint32_t)now_ms -
        future->started_at_ms;

    if (elapsed <
        future->timeout_ms) {

        return false;
    }

    future->result =
        AETHER_ERR_TIMEOUT;

    future->state =
        AETHER_SEND_FUTURE_TIMEOUT;

    return true;
}


bool aether_send_future_is_complete(
    const aether_send_future_t *future) {

    if (future == NULL) {
        return false;
    }

    return
        future->state ==
            AETHER_SEND_FUTURE_DONE ||
        future->state ==
            AETHER_SEND_FUTURE_ERROR ||
        future->state ==
            AETHER_SEND_FUTURE_TIMEOUT;
}


aether_send_future_state_t aether_send_future_state(
    const aether_send_future_t *future) {

    return
        future == NULL
            ? AETHER_SEND_FUTURE_IDLE
            : future->state;
}


aether_status_t aether_send_future_result(
    const aether_send_future_t *future) {

    return
        future == NULL
            ? AETHER_ERR_ARGUMENT
            : future->result;
}


uint32_t aether_send_future_request_id(
    const aether_send_future_t *future) {

    return
        future == NULL
            ? 0u
            : future->request_id;
}

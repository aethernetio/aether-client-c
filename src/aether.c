
#include "aether.h"
#include "aether_platform.h"

#include <string.h>


#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
aether_status_t aether_platform_init(
    void *platform_context,
    size_t platform_context_size,
    uint32_t storage_slot,
    aether_client_platform_t *platform) {

    (void)platform_context;
    (void)platform_context_size;
    (void)storage_slot;

    if (platform != NULL) {
        memset(
            platform,
            0,
            sizeof(*platform));
    }

    return AETHER_ERR_UNSUPPORTED;
}


#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void aether_platform_deinit(
    void *platform_context) {

    (void)platform_context;
}


static bool facade_uuid_is_zero(
    aether_uuid_t uid) {

    return
        uid.msb == UINT64_C(0) &&
        uid.lsb == UINT64_C(0);
}


static void facade_ready(
    void *ctx,
    aether_uuid_t uid) {

    aether_t *client =
        ctx;

    if (client->on_ready != NULL) {
        client->on_ready(
            client,
            uid);
    }
}


static void facade_message(
    void *ctx,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    aether_t *client =
        ctx;

    if (client->on_message != NULL) {
        client->on_message(
            client,
            from,
            data,
            size);
    }
}


static void facade_error(
    void *ctx,
    aether_status_t code,
    aether_error_origin_t origin) {

    aether_t *client =
        ctx;

    if (client->on_error != NULL) {
        client->on_error(
            client,
            code,
            origin);
    }
}


aether_status_t aether_init(
    aether_t *client,
    aether_uuid_t parent_uid) {

    return
        aether_init_slot(
            client,
            parent_uid,
            0u);
}


aether_status_t aether_init_slot(
    aether_t *client,
    aether_uuid_t parent_uid,
    uint32_t slot) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(
        client,
        0,
        sizeof(*client));

    aether_rx_all_init(
        &client->rx);

    aether_client_platform_t platform;

    memset(
        &platform,
        0,
        sizeof(platform));

    aether_status_t status =
        aether_platform_init(
            client->platform_context.bytes,
            sizeof(client->platform_context.bytes),
            slot,
            &platform);

    client->init_status =
        status;

    if (status != AETHER_OK) {
        return status;
    }

    aether_client_app_config_t config;

    aether_client_app_config_default(
        &config);

    config.parent_uid =
        facade_uuid_is_zero(
            parent_uid)
            ? AETHER_ANONYMOUS_UID
            : parent_uid;

    config.on_ready =
        facade_ready;

    config.on_error =
        facade_error;

    config.callback_ctx =
        client;

    aether_client_app_init(
        &client->app,
        &config,
        &platform);

    client->init_status =
        AETHER_OK;

    return AETHER_OK;
}


void aether_on_message(
    aether_t *client,
    aether_message_fn callback) {

    if (client == NULL) {
        return;
    }

    client->on_message =
        callback;
}


void aether_on_ready(
    aether_t *client,
    aether_ready_fn callback) {

    if (client == NULL) {
        return;
    }

    client->on_ready =
        callback;
}


void aether_on_error(
    aether_t *client,
    aether_error_fn callback) {

    if (client == NULL) {
        return;
    }

    client->on_error =
        callback;
}


aether_status_t aether_start(
    aether_t *client) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (client->init_status !=
        AETHER_OK) {

        return
            client->init_status;
    }

    return
        aether_client_app_start(
            &client->app);
}


void aether_stop(
    aether_t *client) {

    if (client == NULL ||
        client->init_status !=
            AETHER_OK) {

        return;
    }

    aether_client_app_stop(
        &client->app);

    aether_platform_deinit(
        client->platform_context.bytes);
}


aether_status_t aether_poll(
    aether_t *client) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (client->init_status !=
        AETHER_OK) {

        return
            client->init_status;
    }

    aether_status_t status =
        aether_client_app_tick(
            &client->app);

    if (status != AETHER_OK) {
        return status;
    }

    const aether_ingress_t *ingress =
        aether_client_app_ingress(
            &client->app);

    if (ingress == NULL) {
        return AETHER_OK;
    }

    if (ingress->kind ==
        AETHER_INGRESS_MESSAGE) {

        bool accepted =
            aether_rx_all_accept(
                &client->rx,
                ingress,
                facade_message,
                client);

        if (!accepted) {
            return AETHER_ERR_UNSUPPORTED;
        }

    } else if (
        ingress->kind !=
            AETHER_INGRESS_REQUEST_RESULT) {

        return AETHER_ERR_UNSUPPORTED;
    }

    /*
     * Beginner API intentionally owns no send-future state.
     * Application request results are therefore drained here.
     * Mandatory internal results never reach this layer.
     */
    return
        aether_client_app_consume_ingress(
            &client->app);
}


aether_status_t aether_send(
    aether_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t size) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (client->init_status !=
        AETHER_OK) {

        return
            client->init_status;
    }

    return
        aether_client_app_send(
            &client->app,
            destination,
            data,
            size);
}


bool aether_is_ready(
    const aether_t *client) {

    return
        client != NULL &&
        client->init_status ==
            AETHER_OK &&
        aether_client_app_is_ready(
            &client->app);
}


bool aether_is_registered(
    const aether_t *client) {

    return
        client != NULL &&
        client->init_status ==
            AETHER_OK &&
        aether_client_app_is_registered(
            &client->app);
}


aether_state_t aether_state(
    const aether_t *client) {

    if (client == NULL ||
        client->init_status !=
            AETHER_OK) {

        return AETHER_STATE_STOPPED;
    }

    return
        aether_client_app_state(
            &client->app);
}


aether_uuid_t aether_uid(
    const aether_t *client) {

    if (client == NULL ||
        client->init_status !=
            AETHER_OK) {

        return
            (aether_uuid_t){
                UINT64_C(0),
                UINT64_C(0)
            };
    }

    return
        aether_client_app_uid(
            &client->app);
}
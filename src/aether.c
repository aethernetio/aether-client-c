

/*
 * Beginner facade implementation.
 *
 * This file contains policy glue, not protocol implementation:
 *
 *     aether.h -> aether_client_app -> aether_client core
 *
 * Target-specific mechanisms enter only through aether_platform_init() and
 * aether_platform_deinit().
 *
 * The weak platform definitions below are an unsupported fallback for builds
 * that do not select a real target package. Production ports override them with
 * strong definitions. Do not add ESP32/POSIX-specific mechanisms here.
 */
#include "aether.h"
#include "aether_platform.h"
#include "aether_internal.h"

#include "aether_transport.h"


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

    /*
     * Route through the shared internal dispatch helper so the
     * callback_active guard is uniform across transports. Any
     * aether_send_message() from inside on_message now observes
     * AETHER_ERR_BUSY regardless of whether the message arrived
     * via the cloud path or the LAN path.
     */
    (void)aether_dispatch_message_internal(
        client,
        from,
        data,
        size);
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




aether_status_t aether_set_components(
    aether_t *client,
    aether_component_t *components,
    size_t component_capacity) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (components == NULL &&
        component_capacity != 0u) {

        return AETHER_ERR_ARGUMENT;
    }

    if (client->init_status !=
        AETHER_OK) {

        return client->init_status;
    }

    /*
     * Registry attachment is a pre-start operation.
     *
     * Once the client has started, replacing the registry would
     * silently orphan live component contexts. Reject the call
     * instead of allowing an unsafe mutation of a running client.
     */
    if (client->app.started) {
        return AETHER_ERR_STATE;
    }

    /*
     * Replacing the registry pre-start resets the count. This is the
     * documented single-shot setup path and is intentionally not
     * additive.
     */
    client->components = components;
    client->component_capacity = component_capacity;
    client->component_count = 0u;

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



static aether_status_t facade_core_step(
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


    } else {

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



aether_status_t aether_register_component(
    aether_t *client,
    void *context,
    aether_component_work_fn work) {

    if (client == NULL ||
        work == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (client->init_status !=
        AETHER_OK) {

        return client->init_status;
    }

    if (client->components == NULL ||
        client->component_count >=
            client->component_capacity) {

        return AETHER_ERR_OVERFLOW;
    }

    /*
     * Reject an exact duplicate (context, work) pair.
     *
     * The same context with two different work callbacks is a legal
     * (if unusual) shape and stays allowed. The same pair twice would
     * only cause the same state machine to tick twice per
     * aether_work() call, which is never the caller's intent.
     */
    for (size_t i = 0u;
         i < client->component_count;
         ++i) {

        if (client->components[i].context == context &&
            client->components[i].work == work) {

            return AETHER_ERR_STATE;
        }
    }

    aether_component_t *slot =
        &client->components[
            client->component_count];

    slot->context = context;
    slot->work = work;

    client->component_count += 1u;

    return AETHER_OK;
}



aether_status_t aether_work(
    aether_t *client) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (client->init_status !=
        AETHER_OK) {

        return client->init_status;
    }

    /*
     * One bounded core step.
     *
     * Hard-fail core statuses stop the whole aether_work() call:
     * AETHER_ERR_ARGUMENT means the client is unusable,
     * AETHER_ERR_STATE means the lifecycle has entered an unexpected
     * phase. Both make ticking components structurally unsafe.
     *
     * Every other non-OK core status is a temporary protocol/
     * transport/remote condition (per the core state machine) and
     * components must still get their tick so they can make progress
     * on the conditions the core is waiting for.
     */

    aether_status_t core_status =
        facade_core_step(
            client);


    if (core_status == AETHER_ERR_ARGUMENT ||
        core_status == AETHER_ERR_STATE) {

        return core_status;
    }

    /*
     * Snapshot the component count at the start of the pass.
     *
     * A component may register another component during its own tick
     * (for example a driver lazily attaching a reconnect helper). The
     * new entry becomes visible only on the next aether_work() call,
     * so a single pass has deterministic, non-recursive semantics.
     */
    size_t component_count =
        client->component_count;

    /*
     * Every registered component gets exactly one bounded step.
     *
     * Component errors are collected, not propagated mid-pass: the
     * first non-OK status is remembered and returned after the full
     * pass. This keeps component dispatch order-independent and
     * prevents one failing component from starving the rest.
     *
     * Waiting is the component's own business: a component that is
     * not yet able to make progress returns AETHER_OK and re-checks
     * on the next step. There is no special scheduler status for
     * "waiting".
     */
    aether_status_t component_error =
        AETHER_OK;

    for (size_t i = 0u;
         i < component_count;
         ++i) {

        aether_component_t *component =
            &client->components[i];

        if (component->work == NULL) {
            continue;
        }

        aether_status_t component_status =
            component->work(
                client,
                component->context);

        if (component_status != AETHER_OK &&
            component_error == AETHER_OK) {

            component_error = component_status;
        }
    }


    /*
     * Semantic transport batch boundary.
     *
     * Application code and components may have enqueued semantic
     * commands during this pass. aether_work() is the single owner of
     * the batch boundary: at most one flush() and at most one tick()
     * per work call. Commands queued from an on_flush_done callback
     * remain BUILDING until the next aether_work().
     */
    aether_status_t transport_error =
        AETHER_OK;

    aether_transport_status_t transport_status =
        aether_transport_status(
            &client->app.transport);

    if (transport_status ==
        AETHER_TRANSPORT_BUILDING) {

        transport_error =
            aether_transport_flush(
                &client->app.transport);
    }


    /*
     * Exactly one transport progress step, even if flush returned an
     * error. A transport that failed flush may still need to clear its
     * internal state and invoke on_flush_done.
     */
    aether_status_t tick_error =
        aether_transport_tick(
            &client->app.transport);

    if (transport_error == AETHER_OK) {
        transport_error = tick_error;
    }


    /*
     * Priority: core non-OK, then transport orchestration error,
     * then the first component error. on_flush_done(status) is a
     * per-batch result and does not become aether_work() status.
     */
    if (core_status != AETHER_OK) {
        return core_status;
    }

    if (transport_error != AETHER_OK) {
        return transport_error;
    }

    return component_error;
}






aether_status_t aether_send_message(
    aether_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t size) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (client->init_status !=
        AETHER_OK) {

        return client->init_status;
    }

    return
        aether_client_app_send(
            &client->app,
            destination,
            data,
            size);
}



aether_status_t aether_send(
    aether_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t size) {

    return
        aether_send_message(
            client,
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
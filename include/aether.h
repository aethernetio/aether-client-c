

/*
 * Beginner-facing Aether C API.
 *
 * Normal applications should include this header instead of constructing
 * transport, storage, crypto, proof-of-work or other low-level mechanisms.
 *
 * A ready platform package supplies those mechanisms behind
 * aether_platform_init().
 *
 * Scheduling remains explicit: Aether creates no background thread or hidden
 * event loop. Call aether_work() regularly from the application's normal
 * loop/task.
 *
 * Memory is caller-owned. aether_t contains the protocol/application state plus
 * fixed opaque storage reserved for the selected platform package.
 */
#ifndef AETHER_H
#define AETHER_H


#include "aether_client_app.h"
/* peer temporarily disabled */
#include "aether_rx.h"

#include "aether_api.h"
#include "aether_credentials.h"


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef struct aether aether_t;



/*

 * Unified component lifecycle contract.
 *
 * aether_t is the core runtime. Ordinary components (network, transport,
 * reconnect state machines, future RPC, ...) register a context pointer
 * and a single periodic work callback through the caller-owned registry.
 *
 * A component work function performs at most one bounded state-machine
 * step. It must not block the outer loop, must not allocate, must not
 * rely on hidden global state, and must not rely on registration order
 * for correctness.
 *
 * Waiting states are normal, not special. If a component's prerequisite
 * (network up, route known, cloud resolving, queue non-empty, ...) is
 * not yet ready, the component returns AETHER_OK and advances again on
 * the next step. The scheduler does not interpret component-internal
 * waiting semantics; there is no "retry next tick" status.

 */
typedef aether_status_t (*aether_component_work_fn)(
    aether_t *aether,
    void *context);


typedef struct {
    void *context;
    aether_component_work_fn work;
} aether_component_t;


/*
 * Convenience recommendation only. The registry capacity is not bounded
 * by this value: callers may supply any storage size they choose.
 */
#ifndef AETHER_COMPONENT_CAPACITY_DEFAULT
#define AETHER_COMPONENT_CAPACITY_DEFAULT 10
#endif



/*
 * Application callbacks.
 *
 * The aether_t pointer identifies the client that produced the notification.
 *
 * MESSAGE payload is borrowed and remains valid only for the callback
 * invocation. Copy it if the application needs to retain it.
 *
 * Treat callbacks as non-reentrant with respect to the same client: return to
 * the normal application loop before driving that client again.
 */
typedef void (*aether_message_fn)(
    aether_t *client,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size);




typedef void (*aether_ready_fn)(
    aether_t *client,
    aether_uuid_t uid);

typedef void (*aether_error_fn)(
    aether_t *client,
    aether_status_t code,
    aether_error_origin_t origin);




/*
 * Complete caller-owned beginner client object.
 *
 * `app` contains mandatory protocol/application state.
 *
 * `rx` is the beginner facade's receive-all policy. Lower API levels keep this
 * policy outside the core so applications that do not use it pay no core RAM
 * cost for it.
 *
 * Callback pointers contain application policy only. Platform mechanisms remain
 * behind the lower layers.
 *
 * platform_context is opaque storage owned by the selected target integration.
 * Application code must never inspect or initialize its bytes directly.
 */
#ifndef AETHER_PLATFORM_CONTEXT_BYTES
#define AETHER_PLATFORM_CONTEXT_BYTES 1u
#endif


struct aether {
    aether_client_app_t app;


    aether_rx_all_t rx;

    aether_message_fn on_message;

    aether_ready_fn on_ready;
    aether_error_fn on_error;

    aether_status_t init_status;

    union {
        max_align_t alignment;
        uint8_t bytes[AETHER_PLATFORM_CONTEXT_BYTES];
    } platform_context;


    /*
     * Caller-owned component registry.
     *
     * components points to caller-supplied storage of at least
     * component_capacity entries. A caller that does not use the
     * registry leaves components == NULL and component_capacity == 0.
     *
     * No heap allocation, no hidden dynamic list, no fixed global
     * limit.
     */
    aether_component_t *components;
    size_t component_capacity;
    size_t component_count;

};




/*
 * Initialize the normal persistent Aether identity.
 *
 * No network connection is started here.
 *
 * A zero parent UID selects AETHER_ANONYMOUS_UID. Slot 0 is used for persistent
 * identity storage. Platform initialization errors are returned directly.
 */
aether_status_t aether_init(
    aether_t *client,
    aether_uuid_t parent_uid);


/*
 * Initialize another independent persistent Aether identity.
 *
 * slot is a logical Aether storage namespace, not a physical flash address.
 * Platform implementations decide how logical slots map to persistent storage.
 *
 * Most applications should use aether_init().
 */
aether_status_t aether_init_slot(
    aether_t *client,
    aether_uuid_t parent_uid,
    uint32_t slot);




/*
 * Attach a caller-owned component registry to an already-initialized
 * client.
 *
 * Deliberately orthogonal to aether_init() / aether_init_slot(): the
 * storage slot selects an identity namespace, while the component
 * registry is a separate axis. Keeping them separate avoids a
 * combinatorial init API such as aether_init_slot_with_components().

 * Typical sequence:
 *
 *     aether_init(&aether, parent_uid);
 *     aether_set_components(&aether, storage, N);   // pre-start only
 *     ...component-specific lifecycle for each component...
 *     aether_start(&aether);
 *
 * aether_set_components() must be called before aether_start(). Once
 * the client is started, replacing the registry would orphan live
 * component contexts and is rejected with AETHER_ERR_STATE.
 *
 * How a component's context is initialised is component-specific. Most
 * components are bound to the client before aether_start(); the
 * registration component is a special case because aether_start()
 * reinitialises the underlying core and it must therefore be bound to
 * the live aether.app.core afterwards. See aether_registration.h.
 *
 * components must point to storage of at least component_capacity
 * entries. Passing components == NULL with component_capacity == 0
 * leaves the registry empty (the default behaviour of aether_init()).
 *
 * No heap allocation, no hidden dynamic list, no fixed global limit.

 */
aether_status_t aether_set_components(
    aether_t *client,
    aether_component_t *components,
    size_t component_capacity);






/*
 * Install or replace application callbacks.
 *
 * Passing NULL disables the corresponding callback. Installing callbacks before
 * aether_start() avoids missing an early ready/error notification.
 */
void aether_on_message(
    aether_t *client,
    aether_message_fn callback);




void aether_on_ready(
    aether_t *client,
    aether_ready_fn callback);

void aether_on_error(
    aether_t *client,
    aether_error_fn callback);




/*
 * Start identity restoration/registration and network activity.
 *
 * AETHER_OK means startup was accepted. Readiness is asynchronous; keep polling
 * and observe aether_is_ready() or the ready callback.
 */
aether_status_t aether_start(
    aether_t *client);

/*
 * Stop protocol/network activity and release platform resources associated with
 * this client. Caller-owned aether_t storage itself is not freed.
 */
void aether_stop(
    aether_t *client);




/*
 * Register a component in the caller-owned registry.
 *
 * work must be non-NULL. Registration fills the registry in call order.
 * That order influences only the dispatch order inside aether_work(); it
 * must not affect correctness. A component that depends on another
 * component should return AETHER_OK while waiting for it and re-check
 * on the next step rather than rely on registration order.
 *
 * Lifetime: the component's context pointer is borrowed. It must remain
 * valid at least until the client is stopped (aether_stop()) or the
 * caller stops calling aether_work() for this client. The registry does
 * not copy or own the context.
 *
 * Returns AETHER_ERR_OVERFLOW when the registry is full.
 */

aether_status_t aether_register_component(
    aether_t *client,
    void *context,
    aether_component_work_fn work);








/*
 * Unified scheduling step for the entire client.
 *
 * This is the single function an application's main loop needs to call:
 *
 *     for (;;) {
 *         aether_work(&aether);
 *     }
 *
 * It executes:
 *
 *   1. one core mechanism step;
 *   2. one bounded work step for every registered component, in
 *      registration order.
 *
 * Core temporary statuses (retryable transport/protocol errors) do not
 * starve components: the dispatch loop still runs and the core status
 * is returned to the caller afterwards. Only AETHER_ERR_ARGUMENT or
 * AETHER_ERR_STATE from the core prevent component work entirely.
 *
 * Every registered component receives one tick per call. A component
 * that is waiting for a prerequisite simply returns AETHER_OK and
 * re-checks on the next step; there is no special "retry next tick"
 * status. If any component reports a non-OK status, the first such
 * status is remembered and returned after the full pass, so one
 * component's failure does not starve the rest.
 *
 * aether_t itself is not a component: it is the runtime that drives
 * them. The core entry point therefore remains aether_work(), not
 * aether_component_work().
 */
aether_status_t aether_work(
    aether_t *client);




/*
 * Send one application message to another Aether UID.
 *
 * Readiness/writability is checked inside the library; applications do not
 * need to guard this call with aether_is_ready().
 *
 * This is an immediate, zero-extra-state operation. data is borrowed only for
 * this call because the transport consumes/copies it before returning.
 *

 * Bidirectional relationships should normally use aether_peer_t from
 * aether_messages.h.

 */
aether_status_t aether_send_message(
    aether_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t size);

/*
 * Compatibility alias for the original API.
 *
 * New code should prefer aether_send_message(), whose name makes the operation
 * explicit.
 */
aether_status_t aether_send(
    aether_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t size);



/* Informational only; normal send/message components check readiness internally. */
bool aether_is_ready(
    const aether_t *client);

/* True after an Aether identity has been restored or registered. */
bool aether_is_registered(
    const aether_t *client);

/* Inspect the current protocol state without advancing the client. */
aether_state_t aether_state(
    const aether_t *client);

/* Return the current Aether UID; it can be zero before identity exists. */
aether_uuid_t aether_uid(
    const aether_t *client);



#ifdef __cplusplus
}
#endif

#endif
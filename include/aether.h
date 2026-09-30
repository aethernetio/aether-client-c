

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
 * event loop. Call aether_poll() regularly from the application's normal
 * loop/task.
 *
 * Memory is caller-owned. aether_t contains the protocol/application state plus
 * fixed opaque storage reserved for the selected platform package.
 */
#ifndef AETHER_H
#define AETHER_H


#include "aether_client_app.h"
#include "aether_rx.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef struct aether aether_t;


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
 * Execute one bounded scheduling step.
 *
 * Call regularly from the application's normal loop/task. One call lets the
 * platform collect available physical input, advances protocol/parser state,
 * handles at most one yielded ingress item through this facade's RX_ALL policy,
 * and advances mandatory timers when RX storage is not pinned.
 *
 * There is no hidden scheduler.
 */
aether_status_t aether_poll(
    aether_t *client);


/*
 * Send one application message to another Aether UID.
 *
 * data is borrowed only for this call. Applications requiring explicit
 * request-result/timeout tracking should use the advanced
 * aether_send_future_t component.
 */
aether_status_t aether_send(
    aether_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t size);


/* True only when an authenticated writable work connection is ready. */
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
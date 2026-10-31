

/*
 * Advanced application/platform facade.
 *
 * This layer connects the low-level protocol state machine to an explicit bundle
 * of platform mechanisms while keeping scheduling and optional ingress policy in
 * caller-owned code.
 *
 * It is useful for custom ports, host tests and applications that need direct
 * access to borrowed ingress items. Ordinary applications should prefer
 * aether.h.
 */
#ifndef AETHER_CLIENT_APP_H
#define AETHER_CLIENT_APP_H


#include "aether_client.h"


#include "aether_transport.h"
#include "aether_transport_server.h"



#ifdef __cplusplus
extern "C" {
#endif


#ifndef AETHER_MAX_HOSTNAME
#define AETHER_MAX_HOSTNAME 128u
#endif

#define AETHER_DEFAULT_REGISTRATION_URI \
    "tcp://registration.aethernet.io:9010"


extern const aether_uuid_t AETHER_ANONYMOUS_UID;


/*
 * Platform mechanisms.
 *
 * The platform owns physical I/O, persistence, crypto and time.
 * Protocol/application scheduling belongs to Aether.
 */

typedef uint64_t (*aether_client_now_ms_fn)(
    void *ctx);





/*
 * Mandatory lifecycle compatibility callbacks.
 *
 * Application MESSAGE policy is deliberately absent here.
 * Messages are exposed as explicit ingress and handled by caller-owned
 * components.
 */
typedef void (*aether_client_ready_fn)(
    void *ctx,
    aether_uuid_t uid);

typedef void (*aether_client_error_fn)(
    void *ctx,
    aether_status_t code,
    aether_error_origin_t origin);


typedef struct {
    void *ctx;

    aether_transport_io_vtable_t transport_io;
    aether_flash_vtable_t flash;

    const aether_crypto_vtable_t *crypto;
    void *crypto_ctx;

    aether_pow_vtable_t pow;

    const uint8_t *trusted_sign_keys;
    size_t trusted_sign_key_count;


    /*
     * Monotonic milliseconds for mandatory protocol timers.
     */
    aether_client_now_ms_fn now_ms;

} aether_client_platform_t;


typedef struct {
    /*
     * Examples:
     *   tcp://registration.aethernet.io:9010
     *   tcp://192.0.2.10:9010
     *   tcp://[2001:db8::1]:9010
     */
    const char *registration_uri;

    /*
     * Zero UUID means AETHER_ANONYMOUS_UID.
     */
    aether_uuid_t parent_uid;

    uint32_t ping_interval_ms;
    uint32_t rx_window_ms;
    uint32_t request_timeout_ms;

    /*
     * Temporary lifecycle compatibility bridge.
     *
     * Optional receive/future/stream component state is never stored here.
     */
    aether_client_ready_fn on_ready;
    aether_client_error_fn on_error;
    void *callback_ctx;

} aether_client_app_config_t;




typedef struct aether_client_app {
    /*
     * Mandatory protocol state only.
     *
     * Optional components are caller-owned objects. If a component is not
     * used, it consumes no RAM inside this object.
     */
    aether_client_t core;

    aether_client_app_config_t config;
    aether_client_platform_t platform;



    /*
     * Semantic Aether transport. See aether_transport.h. The server
     * transport is the only concrete implementation in Phase 2A; later
     * peer/LAN or mux implementations plug into the same handle.
     */
    aether_transport_t transport;
    aether_transport_server_t transport_server;




    bool started;
} aether_client_app_t;


/*
 * Defaults:
 *   registration = tcp://registration.aethernet.io:9010
 *   parent       = AETHER_ANONYMOUS_UID
 *   ping         = 6000 ms
 *   rx window    = 5000 ms
 */
void aether_client_app_config_default(
    aether_client_app_config_t *config);


/*
 * Allocation-free initialization.
 * No network operation occurs before start().
 */
void aether_client_app_init(
    aether_client_app_t *client,
    const aether_client_app_config_t *config,
    const aether_client_platform_t *platform);

aether_status_t aether_client_app_start(
    aether_client_app_t *client);

void aether_client_app_stop(
    aether_client_app_t *client);


/*
 * Execute one bounded mechanism step.
 *
 * One tick may:
 *   1. let the platform collect currently available physical input;
 *   2. advance mandatory protocol/parser state;
 *   3. yield at most one application ingress item;
 *   4. advance mandatory timers only when RX storage is not pinned.
 *
 * If ingress is already pending, tick() does nothing and returns AETHER_OK.
 * The caller chooses a policy component, processes that ingress, and then
 * explicitly calls aether_client_app_consume_ingress().
 *
 * The application therefore remains the scheduler.
 */
aether_status_t aether_client_app_tick(
    aether_client_app_t *client);




/*
 * Inspect the one pending application ingress item.
 *
 * Returned payload memory is borrowed. It stays valid until
 * aether_client_app_consume_ingress().
 */
const aether_ingress_t *aether_client_app_ingress(
    const aether_client_app_t *client);


/*
 * Release the current ingress item.
 *
 * If it was the final item in the current transport frame, this function also
 * releases the borrowed platform frame.
 */
aether_status_t aether_client_app_consume_ingress(
    aether_client_app_t *client);


/*
 * Serialize and send one application message through the READY work channel.
 */
aether_status_t aether_client_app_send(
    aether_client_app_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t length);


aether_status_t aether_client_app_send_authorized(
    aether_client_app_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t length);



bool aether_client_app_is_ready(
    const aether_client_app_t *client);

bool aether_client_app_is_registered(
    const aether_client_app_t *client);

aether_state_t aether_client_app_state(
    const aether_client_app_t *client);

aether_uuid_t aether_client_app_uid(
    const aether_client_app_t *client);


/*
 * Canonical UUID helpers.
 */
aether_status_t aether_uuid_parse(
    const char *text,
    aether_uuid_t *uuid);

bool aether_uuid_equal(
    aether_uuid_t left,
    aether_uuid_t right);


#ifdef __cplusplus
}
#endif

#endif
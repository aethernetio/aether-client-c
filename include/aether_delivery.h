
/*
 * Transport-agnostic message delivery: caller-owned outbox + coordinator.
 *
 * A producer enqueues a logical message (recipient UID + payload bytes) and
 * returns immediately. The actual route is chosen later by a delivery
 * component driven by aether_work(); the producer never sees a transport,
 * a cloud endpoint, a server id, an IP address or a socket.
 *
 * The first built-in provider is the existing Aether cloud path. Future
 * providers (for example a direct LAN transport) can be added via
 * aether_delivery_add_provider() without changing producer code:
 *
 *     aether_delivery_send(&delivery, recipient_uid, payload, size);
 *
 * remains the single producer-facing call.
 *
 * No heap allocation. All storage (delivery object, slots, payload bytes,
 * user provider array) is caller-owned.
 *
 * ACCEPTED means "the provider handed the message off to its transport /
 * path", with the same guarantee the underlying send primitive gives.
 * It is NOT end-to-end delivery.
 */
#ifndef AETHER_DELIVERY_H
#define AETHER_DELIVERY_H


#include "aether.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Slot state.
 *
 * EMPTY: slot is free for the next enqueue.
 * READY: a logical message is pending; the coordinator will attempt it.
 */
typedef enum {
    AETHER_DELIVERY_SLOT_EMPTY = 0,
    AETHER_DELIVERY_SLOT_READY = 1
} aether_delivery_slot_state_t;


/*
 * One pending logical message.
 *
 * Route-independent: no cloud/server/socket/endpoint info is stored here.
 * Payload bytes live in caller-owned payload storage; the slot's index is
 * recovered from the slot pointer and the delivery context.
 */
typedef struct {
    aether_uuid_t recipient;
    uint32_t payload_length;
    uint8_t state;
} aether_delivery_slot_t;


/*
 * Provider attempt result.
 *
 * ACCEPTED:
 *   Provider handed the message off to its transport/path. The guarantee is
 *   exactly the one the underlying primitive already gives; e.g. for the
 *   cloud provider it is identical to aether_send_message() returning
 *   AETHER_OK. Slot is freed and no further provider is tried.
 *
 * NOT_APPLICABLE:
 *   Provider cannot serve this recipient right now. Contract requires zero
 *   send side effects. Next provider may be tried.
 *
 * RETRY_LATER:
 *   Provider is applicable but could not hand off now. Contract requires
 *   that no bytes were handed off to any transport. Next provider may be
 *   tried (v1 fallback policy).
 *
 * ROUTE_LOST:
 *   Provider's route disappeared before handoff. Contract requires zero
 *   send side effects. Next provider may be tried.
 *
 * ERROR:
 *   Permanent local failure. No implicit fallback. The error callback (if
 *   installed) is fired once and the slot is released.
 */
typedef enum {
    AETHER_DELIVERY_ACCEPTED = 0,
    AETHER_DELIVERY_NOT_APPLICABLE,
    AETHER_DELIVERY_RETRY_LATER,
    AETHER_DELIVERY_ROUTE_LOST,
    AETHER_DELIVERY_ERROR
} aether_delivery_result_t;


struct aether_delivery;
typedef struct aether_delivery aether_delivery_t;


typedef aether_delivery_result_t (*aether_delivery_try_fn)(
    void *context,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length);


typedef struct {
    void *context;
    aether_delivery_try_fn try_deliver;
} aether_delivery_provider_t;


typedef void (*aether_delivery_error_fn)(
    void *context,
    aether_uuid_t recipient,
    aether_status_t status);


/*
 * Caller-owned delivery context.
 *
 * The struct is intentionally public so it can be statically allocated.
 * Fields are set by aether_delivery_init() and friends; callers should not
 * touch them directly.
 */
struct aether_delivery {
    aether_t *client;

    aether_delivery_slot_t *slots;
    size_t slot_count;

    uint8_t *payload_storage;
    size_t payload_capacity;

    /* user-provided providers; cloud is implicit-last */
    aether_delivery_provider_t *user_providers;
    size_t user_provider_capacity;
    size_t user_provider_count;

    aether_delivery_error_fn on_error;
    void *error_ctx;
};


/*
 * Initialize caller-owned delivery.
 *
 * All storage must outlive the delivery object:
 *   slots           -> slot_count entries
 *   payload_storage -> slot_count * payload_capacity bytes
 *
 * The built-in cloud provider is always available and is consulted after
 * all user providers, so future user providers (e.g. LAN) get to run first.
 */
void aether_delivery_init(
    aether_delivery_t *delivery,
    aether_t *client,
    aether_delivery_slot_t *slots,
    size_t slot_count,
    uint8_t *payload_storage,
    size_t payload_capacity);


/*
 * Attach caller-owned storage for user providers.
 *
 * Optional. Without it, only the built-in cloud provider is used.
 * Must be called before aether_delivery_add_provider().
 */
void aether_delivery_set_providers(
    aether_delivery_t *delivery,
    aether_delivery_provider_t *providers,
    size_t capacity);


/*
 * Insert one user provider. User providers are tried in registration order,
 * then the built-in cloud fallback.
 *
 * Returns:
 *   AETHER_OK           added
 *   AETHER_ERR_ARGUMENT bad argument / NULL try_deliver
 *   AETHER_ERR_OVERFLOW no provider storage or capacity exhausted
 */
aether_status_t aether_delivery_add_provider(
    aether_delivery_t *delivery,
    aether_delivery_provider_t provider);


/*
 * Install an optional error callback.
 *
 * Called once for each permanent delivery failure, before the slot is
 * released. The recipient is passed by value; no internal pointer is
 * exposed.
 */
void aether_delivery_set_error_callback(
    aether_delivery_t *delivery,
    aether_delivery_error_fn callback,
    void *context);


/*
 * Producer API.
 *
 * Transport-agnostic. Copies payload bytes into caller-owned storage and
 * returns. Actual delivery happens later from aether_delivery_work() driven
 * by aether_work().
 *
 * Safe to call from inside a message callback: it performs no transport
 * I/O and no crypto.
 *
 * Returns:
 *   AETHER_OK            enqueued
 *   AETHER_ERR_ARGUMENT  bad arguments or uninitialized delivery
 *   AETHER_ERR_OVERFLOW  queue full, or payload_length > payload_capacity
 */
aether_status_t aether_delivery_send(
    aether_delivery_t *delivery,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length);


/*
 * Component work function.
 *
 * Register with aether_register_component(&aether, &delivery,
 * aether_delivery_work).
 *
 * One bounded scan of READY slots per call. Every READY slot gets at most
 * one provider-chain attempt sequence; a blocked slot does not starve
 * subsequent slots.
 *
 * Ordinary waiting (no READY slot, all providers returned RETRY_LATER) is
 * not an error: the component returns AETHER_OK.
 */
aether_status_t aether_delivery_work(
    aether_t *client,
    void *context);


#ifdef __cplusplus
}
#endif

#endif

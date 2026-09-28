
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
 * Private storage required by the selected platform package.
 *
 * Application code never reads, initializes or configures this memory.
 */
#ifndef AETHER_PLATFORM_CONTEXT_BYTES
#define AETHER_PLATFORM_CONTEXT_BYTES 1u
#endif


struct aether {
    aether_client_app_t app;

    /*
     * Beginner facade policy.
     *
     * The low-level app/core do not contain this optional component.
     */
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
 * Simple application API.
 *
 * A zero UID selects the standard anonymous parent.
 *
 * The selected platform package supplies persistence, networking,
 * crypto, DNS, time and all other platform services.
 */
aether_status_t aether_init(
    aether_t *client,
    aether_uuid_t parent_uid);


/*
 * Optional independent persistent identity.
 *
 * slot is a logical identifier. It is not a flash address.
 * Most applications should simply use aether_init(), which uses slot 0.
 */
aether_status_t aether_init_slot(
    aether_t *client,
    aether_uuid_t parent_uid,
    uint32_t slot);


/*
 * Handlers may be attached before aether_start().
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


aether_status_t aether_start(
    aether_t *client);

void aether_stop(
    aether_t *client);


/*
 * Call regularly from loop().
 *
 * Each call performs one bounded app tick and then applies this facade's
 * caller-owned RX_ALL policy to at most one yielded ingress item.
 */
aether_status_t aether_poll(
    aether_t *client);


aether_status_t aether_send(
    aether_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t size);


bool aether_is_ready(
    const aether_t *client);

bool aether_is_registered(
    const aether_t *client);

aether_state_t aether_state(
    const aether_t *client);

aether_uuid_t aether_uid(
    const aether_t *client);


#ifdef __cplusplus
}
#endif

#endif


#ifndef AETHER_SEND_FUTURE_H
#define AETHER_SEND_FUTURE_H

#include "aether_client_app.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    AETHER_SEND_FUTURE_IDLE = 0,
    AETHER_SEND_FUTURE_PENDING,
    AETHER_SEND_FUTURE_DONE,
    AETHER_SEND_FUTURE_ERROR,
    AETHER_SEND_FUTURE_TIMEOUT
} aether_send_future_state_t;


typedef struct {
    uint32_t request_id;
    uint32_t started_at_ms;
    uint32_t timeout_ms;
    aether_status_t result;
    aether_send_future_state_t state;
} aether_send_future_t;


void aether_send_future_init(
    aether_send_future_t *future);

void aether_send_future_reset(
    aether_send_future_t *future);


/*
 * Begin tracking an already transmitted request.
 *
 * timeout_ms == 0 means that this future never expires locally.
 */
aether_status_t aether_send_future_begin(
    aether_send_future_t *future,
    uint32_t request_id,
    uint64_t now_ms,
    uint32_t timeout_ms);


/*
 * Convenience operation for the advanced app API.
 *
 * The request is transmitted first. Only after a successful send is the
 * caller-owned future armed with the generated request id.
 */
aether_status_t aether_send_future_send(
    aether_send_future_t *future,
    aether_client_app_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t length);


/*
 * Present one bounded application ingress item to this future.
 *
 * Returns true only when REQUEST_RESULT matches this future's request id.
 * The caller still decides when to consume the ingress from the app.
 */
bool aether_send_future_accept(
    aether_send_future_t *future,
    const aether_ingress_t *ingress);


/*
 * Advance only this future's timeout policy.
 *
 * Returns true exactly on the PENDING -> TIMEOUT transition.
 */
bool aether_send_future_tick(
    aether_send_future_t *future,
    uint64_t now_ms);


bool aether_send_future_is_complete(
    const aether_send_future_t *future);

aether_send_future_state_t aether_send_future_state(
    const aether_send_future_t *future);

aether_status_t aether_send_future_result(
    const aether_send_future_t *future);

uint32_t aether_send_future_request_id(
    const aether_send_future_t *future);


#ifdef __cplusplus
}
#endif

#endif

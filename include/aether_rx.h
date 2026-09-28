
#ifndef AETHER_RX_H
#define AETHER_RX_H

#include "aether_client.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Caller-owned receive policy components.
 *
 * The protocol core has already decoded one aether_ingress_t before these
 * functions are called.
 *
 * Components:
 *   - never parse wire bytes;
 *   - never allocate;
 *   - never own transport or payload memory;
 *   - never register themselves in the core;
 *   - never run a scheduler.
 *
 * MESSAGE payload is borrowed and is valid only until the caller consumes the
 * corresponding ingress from aether_client/app.
 */
typedef void (*aether_rx_message_fn)(
    void *ctx,
    aether_uuid_t from,
    const uint8_t *data,
    size_t length);


/*
 * FIRST
 *
 * Deliver the first MESSAGE after init/reset.
 *
 * Later MESSAGE ingress is still recognized by this policy and should be
 * consumed by the caller, but it is not delivered to the callback until reset.
 */
typedef struct {
    bool received;
} aether_rx_first_t;


void aether_rx_first_init(
    aether_rx_first_t *rx);

void aether_rx_first_reset(
    aether_rx_first_t *rx);

bool aether_rx_first_accept(
    aether_rx_first_t *rx,
    const aether_ingress_t *ingress,
    aether_rx_message_fn on_message,
    void *ctx);


/*
 * ALL
 *
 * Deliver every MESSAGE ingress presented by the caller.
 *
 * The one-byte object preserves the uniform caller-owned component model while
 * requiring no dynamic state.
 */
typedef struct {
    uint8_t reserved;
} aether_rx_all_t;


void aether_rx_all_init(
    aether_rx_all_t *rx);

bool aether_rx_all_accept(
    aether_rx_all_t *rx,
    const aether_ingress_t *ingress,
    aether_rx_message_fn on_message,
    void *ctx);


/*
 * ADAPTIVE
 *
 * Caller-controlled switch between FIRST and ALL.
 *
 * The component deliberately does not contain timing/network heuristics:
 * application code owns scheduling and decides when its receive policy should
 * change.
 */
typedef struct {
    aether_rx_first_t first;
    bool all;
} aether_rx_adaptive_t;


void aether_rx_adaptive_init(
    aether_rx_adaptive_t *rx,
    bool receive_all);

void aether_rx_adaptive_set_all(
    aether_rx_adaptive_t *rx,
    bool receive_all);

void aether_rx_adaptive_reset(
    aether_rx_adaptive_t *rx);

bool aether_rx_adaptive_accept(
    aether_rx_adaptive_t *rx,
    const aether_ingress_t *ingress,
    aether_rx_message_fn on_message,
    void *ctx);


#ifdef __cplusplus
}
#endif

#endif

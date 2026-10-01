
#ifndef AETHER_CLIENT_INTERNAL_H
#define AETHER_CLIENT_INTERNAL_H

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Internal boundary used by the caller-owned registration component.
 *
 * Keeping the registration entrypoints behind this narrow bridge avoids
 * exporting the core parser/crypto/transport helpers merely to separate
 * translation units. Application code must not call these functions.
 */

aether_status_t aether_client_begin_registration_internal(
    aether_client_t *client);


struct aether_registration;

aether_status_t aether_client_registration_on_rx_internal(
    struct aether_registration *registration,
    const uint8_t *data,
    size_t length);

void aether_client_registration_on_transport_state_internal(
    struct aether_registration *registration,
    bool writable);

bool aether_client_registration_on_poll_internal(
    struct aether_registration *registration,
    uint64_t now_ms);







#ifdef __cplusplus
}
#endif

#endif
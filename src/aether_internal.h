
/*
 * Shared internal helpers used by multiple transports and components.
 *
 * Not part of the public API. Callers within the library (cloud facade,
 * LAN component, future transports) use these functions to feed
 * already-authenticated logical messages into the same application
 * callback path that cloud ingress uses.
 */

#ifndef AETHER_INTERNAL_H
#define AETHER_INTERNAL_H


#include "aether.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Dispatch one already-authenticated logical message to the application.
 *
 * Used by:
 *   - the cloud ingress path, after the core parser
 *     yielded an AETHER_INGRESS_MESSAGE and the RX-all policy accepted
 *     it;
 *   - the LAN component, after a direct packet has been decrypted,
 *     authenticated and passed replay protection.
 *
 * Semantics:
 *   - callback_active is set around the user on_message call and
 *     restored afterwards, so aether_send_message() called from inside
 *     the callback returns AETHER_ERR_BUSY on every transport path;
 *   - aether_delivery_send() called from inside the callback always
 *     succeeds (it only copies into the delivery outbox);
 *   - the payload is borrowed: it is valid only during the call.
 *
 * Return:
 *   AETHER_OK              message delivered (or no callback installed)
 *   AETHER_ERR_ARGUMENT    bad arguments
 *   AETHER_ERR_BUSY        callback would re-enter the client
 */
aether_status_t aether_dispatch_message_internal(
    aether_t *client,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size);


#ifdef __cplusplus
}
#endif

#endif
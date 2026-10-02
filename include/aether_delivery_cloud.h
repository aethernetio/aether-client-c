
/*
 * Cloud delivery provider for aether_delivery.
 *
 * This is a normal delivery provider: it is not special-cased inside the
 * coordinator. Applications that want the existing Aether cloud path
 * must register it explicitly:
 *
 *     static aether_delivery_provider_t providers[2];
 *
 *     aether_delivery_set_providers(&delivery, providers, 2);
 *     aether_delivery_add_cloud(&delivery, &aether);
 *
 * Registering it as the last provider yields direct-first, cloud-fallback
 * policy (with a LAN provider registered before it). Registering it alone
 * yields cloud-only behavior.
 *
 * The cloud provider has no periodic component of its own: the low-level
 * core state machine already drives the cloud channel. Only LAN/LoRa-like
 * transports need their own component work.
 */
#ifndef AETHER_DELIVERY_CLOUD_H
#define AETHER_DELIVERY_CLOUD_H


#include "aether_delivery.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Register the built-in Aether cloud delivery provider.
 *
 * The aether_t pointer must remain valid for the lifetime of the
 * provider registration.
 *
 * Returns:
 *   AETHER_OK
 *   AETHER_ERR_ARGUMENT   delivery == NULL, client == NULL
 *   AETHER_ERR_OVERFLOW   no caller-owned provider slot available
 *   AETHER_ERR_STATE      registry locked or duplicate (context, fn) pair
 */
aether_status_t aether_delivery_add_cloud(
    aether_delivery_t *delivery,
    aether_t *client);


/*
 * Raw provider adapter used by the convenience helper above.
 *
 * context is aether_t *. Public only so advanced callers can construct
 * their own provider entry if they need a non-standard arrangement.
 */
aether_delivery_attempt_t aether_cloud_delivery_try(
    void *context,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length);


#ifdef __cplusplus
}
#endif

#endif

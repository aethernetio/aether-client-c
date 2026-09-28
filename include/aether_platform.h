
#ifndef AETHER_PLATFORM_H
#define AETHER_PLATFORM_H

#include "aether.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Internal porting boundary.
 *
 * Normal applications include only aether.h.
 *
 * A ready platform integration fills:
 *   transport
 *   crypto
 *   PoW
 *   trusted root signing keys
 *   DNS resolver
 *   monotonic clock
 *   network pump/event hook
 *   bind hook
 *
 * Persistence is also supplied by the ready platform integration.
 * storage_slot is a logical Aether identity slot, never a physical
 * flash address.
 */
aether_status_t aether_platform_init(
    void *platform_context,
    size_t platform_context_size,
    uint32_t storage_slot,
    aether_client_platform_t *platform);

void aether_platform_deinit(
    void *platform_context);

#ifdef __cplusplus
}
#endif

#endif

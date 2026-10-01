
/*
 * Optional caller-owned Aether identity registration component.
 *

 * Registration is policy, not mandatory client state. Applications that
 * provision an Aether identity externally do not need this component.

 *
 * Scheduling is explicit: applications that want self-registration call
 * aether_registration_poll() from their normal loop alongside the client.
 */
#ifndef AETHER_REGISTRATION_H
#define AETHER_REGISTRATION_H

#include "aether_client.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif



struct aether_registration;

typedef struct {
    aether_status_t (*on_rx)(
        struct aether_registration *registration,
        const uint8_t *data,
        size_t length);

    void (*on_transport_state)(
        struct aether_registration *registration,
        bool writable);

    bool (*on_poll)(
        struct aether_registration *registration,
        uint64_t now_ms);
} aether_registration_internal_ops_t;



typedef struct aether_registration {
    aether_client_t *client;


    /*
     * Immutable internal dispatch table.
     *
     * One pointer is stored per optional component; the function table itself
     * lives in optional code/rodata. Mandatory provisioned firmware never
     * references that table directly.
     */
    const aether_registration_internal_ops_t *internal;



    aether_signed_key_t global_key;

    uint32_t req_pow;
    uint32_t req_finish;

    uint8_t pow_salt[AETHER_MAX_POW_TEXT];
    size_t pow_salt_len;

    uint8_t pow_suffix[AETHER_MAX_POW_TEXT];
    size_t pow_suffix_len;

    uint8_t pow_pool_size;
    int32_t pow_max_hash;

    int32_t pow_passwords[AETHER_MAX_POW_PASSWORDS];
    size_t pow_password_count;
} aether_registration_t;


void aether_registration_init(
    aether_registration_t *registration,
    aether_client_t *client);

aether_status_t aether_registration_poll(
    aether_registration_t *registration);

bool aether_registration_done(
    const aether_registration_t *registration);

#ifdef __cplusplus
}
#endif

#endif
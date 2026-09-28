
#include "p2p_test_support.h"

#include "aether_platform.h"
#include "aether_test_crypto_hydrogen.h"
#include "host_posix_transport.h"

#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>


#define P2P_TEST_STORAGE_SLOTS 2u


typedef struct {
    void *owner;

    aether_host_transport_t transport;

    uint8_t flash[AETHER_MAX_PACKET * 2u];
    size_t flash_len;
} p2p_platform_context_t;


static p2p_platform_context_t contexts[
    P2P_TEST_STORAGE_SLOTS
];


static const uint8_t HYDROGEN_ROOT_SIGN_KEY[32] = {
    0x88,0x3B,0x4D,0x7E,0x0F,0xB0,0x4A,0x38,
    0xCA,0x12,0xB3,0xA4,0x51,0xB0,0x09,0x42,
    0x04,0x88,0x58,0x26,0x3E,0xE6,0xE6,0xD6,
    0x11,0x50,0xF2,0xEF,0x15,0xF4,0x03,0x43
};


uint64_t p2p_test_now_ms(void) {

    struct timeval tv;

    gettimeofday(
        &tv,
        NULL);

    return
        (uint64_t)tv.tv_sec *
            UINT64_C(1000) +
        (uint64_t)tv.tv_usec /
            UINT64_C(1000);
}


static aether_status_t platform_resolve(
    void *ctx,
    const char *hostname,
    aether_address_t *address) {

    (void)ctx;

    if (hostname == NULL ||
        address == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    /*
     * The public facade uses the production registration hostname.
     * The host-only P2P fixture redirects it to the local Java server.
     */
    if (strcmp(
            hostname,
            "registration.aethernet.io") != 0 &&
        strcmp(
            hostname,
            "localhost") != 0) {

        return AETHER_ERR_UNSUPPORTED;
    }

    memset(
        address,
        0,
        sizeof(*address));

    address->kind =
        AETHER_ADDR_IPV4;

    address->length =
        4u;

    address->bytes[0] = 127u;
    address->bytes[1] = 0u;
    address->bytes[2] = 0u;
    address->bytes[3] = 1u;

    return AETHER_OK;
}


static uint64_t platform_now_ms(
    void *ctx) {

    (void)ctx;

    return
        p2p_test_now_ms();
}


/*
 * Physical network collection only.
 *
 * aether_host_transport_pump() never parses an Aether packet.
 */
static aether_status_t platform_poll(
    void *ctx) {

    p2p_platform_context_t *platform =
        ctx;

    if (platform == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    return
        aether_host_transport_pump(
            &platform->transport,
            5) == 0
            ? AETHER_OK
            : AETHER_ERR_TRANSPORT;
}


/*
 * Zero-copy borrowed frame exposed to aether_client_app_tick().
 */
static aether_status_t platform_receive(
    void *ctx,
    aether_channel_t channel,
    aether_bytes_view_t *frame) {

    p2p_platform_context_t *platform =
        ctx;

    if (platform == NULL ||
        frame == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    return
        aether_host_transport_receive(
            &platform->transport,
            channel,
            frame);
}


static void platform_consume(
    void *ctx,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {

    p2p_platform_context_t *platform =
        ctx;

    if (platform == NULL) {
        return;
    }

    aether_host_transport_consume(
        &platform->transport,
        channel,
        data,
        length);
}


static aether_status_t platform_bind(
    void *ctx,
    aether_client_t *core) {

    p2p_platform_context_t *platform =
        ctx;

    if (platform == NULL ||
        core == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    aether_host_transport_init(
        &platform->transport,
        core);

    return AETHER_OK;
}


static aether_status_t flash_load(
    void *ctx,
    uint8_t *dst,
    size_t capacity,
    size_t *length) {

    p2p_platform_context_t *platform =
        ctx;

    if (platform == NULL ||
        dst == NULL ||
        length == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (platform->flash_len ==
        0u) {

        return AETHER_ERR_STORAGE;
    }

    if (platform->flash_len >
        capacity) {

        return AETHER_ERR_OVERFLOW;
    }

    memcpy(
        dst,
        platform->flash,
        platform->flash_len);

    *length =
        platform->flash_len;

    return AETHER_OK;
}


static aether_status_t flash_save(
    void *ctx,
    const uint8_t *src,
    size_t length) {

    p2p_platform_context_t *platform =
        ctx;

    if (platform == NULL ||
        (src == NULL &&
         length != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    if (length >
        sizeof(platform->flash)) {

        return AETHER_ERR_OVERFLOW;
    }

    memcpy(
        platform->flash,
        src,
        length);

    platform->flash_len =
        length;

    return AETHER_OK;
}


/*
 * Unchanged host-test PoW behavior from the working P2P fixture.
 */
static aether_status_t pow_generate(
    void *ctx,
    const uint8_t *salt,
    size_t salt_len,
    const uint8_t *suffix,
    size_t suffix_len,
    uint8_t pool_size,
    int32_t max_hash,
    int32_t *passwords,
    size_t capacity,
    size_t *count) {

    (void)ctx;
    (void)salt;
    (void)salt_len;
    (void)suffix;
    (void)suffix_len;

    if (passwords == NULL ||
        count == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (max_hash !=
        INT32_MAX) {

        fprintf(
            stderr,
            "Unsupported host-test PoW maxHash=%" PRId32 "\n",
            max_hash);

        return AETHER_ERR_UNSUPPORTED;
    }

    if (capacity <
        pool_size) {

        return AETHER_ERR_OVERFLOW;
    }

    for (uint8_t i = 0u;
         i < pool_size;
         ++i) {

        passwords[i] =
            (int32_t)i;
    }

    *count =
        pool_size;

    return AETHER_OK;
}


aether_status_t aether_platform_init(
    void *platform_context,
    size_t platform_context_size,
    uint32_t storage_slot,
    aether_client_platform_t *platform) {

    (void)platform_context_size;

    if (platform_context == NULL ||
        platform == NULL ||
        storage_slot >=
            P2P_TEST_STORAGE_SLOTS) {

        return AETHER_ERR_ARGUMENT;
    }

    p2p_platform_context_t *ctx =
        &contexts[storage_slot];

    if (ctx->owner != NULL &&
        ctx->owner !=
            platform_context) {

        return AETHER_ERR_STORAGE;
    }

    ctx->owner =
        platform_context;

    memset(
        platform,
        0,
        sizeof(*platform));

    platform->ctx =
        ctx;

    platform->transport =
        aether_host_transport_vtable(
            &ctx->transport);

    platform->flash =
        (aether_flash_vtable_t){
            ctx,
            flash_load,
            flash_save
        };

    platform->crypto =
        &AETHER_TEST_HYDROGEN_CRYPTO;

    platform->crypto_ctx =
        ctx;

    platform->pow =
        (aether_pow_vtable_t){
            ctx,
            pow_generate
        };

    platform->trusted_sign_keys =
        HYDROGEN_ROOT_SIGN_KEY;

    platform->trusted_sign_key_count =
        1u;

    platform->resolve =
        platform_resolve;

    platform->now_ms =
        platform_now_ms;

    platform->poll =
        platform_poll;

    platform->receive =
        platform_receive;

    platform->consume =
        platform_consume;

    platform->bind =
        platform_bind;

    return AETHER_OK;
}


void aether_platform_deinit(
    void *platform_context) {

    if (platform_context == NULL) {
        return;
    }

    for (size_t i = 0u;
         i <
             P2P_TEST_STORAGE_SLOTS;
         ++i) {

        p2p_platform_context_t *ctx =
            &contexts[i];

        if (ctx->owner !=
            platform_context) {

            continue;
        }

        aether_host_transport_shutdown(
            &ctx->transport);

        ctx->owner =
            NULL;

        return;
    }
}

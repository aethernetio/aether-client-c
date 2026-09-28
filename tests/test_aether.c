#include "aether.h"
#include "aether_platform.h"

#include <string.h>


static uint8_t persisted[AETHER_MAX_PACKET * 2u];
static size_t persisted_size;
static uint32_t observed_storage_slot;


static aether_status_t platform_load(
    void *ctx,
    uint8_t *data,
    size_t capacity,
    size_t *size) {

    (void)ctx;

    if (persisted_size == 0u) {
        return AETHER_ERR_STORAGE;
    }

    if (persisted_size > capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    memcpy(
        data,
        persisted,
        persisted_size);

    *size =
        persisted_size;

    return AETHER_OK;
}


static aether_status_t platform_save(
    void *ctx,
    const uint8_t *data,
    size_t size) {

    (void)ctx;

    if (size > sizeof(persisted)) {
        return AETHER_ERR_OVERFLOW;
    }

    memcpy(
        persisted,
        data,
        size);

    persisted_size =
        size;

    return AETHER_OK;
}


/*
 * Fake ready platform package.
 *
 * Notice that persistence is configured here, not in main().
 */
aether_status_t aether_platform_init(
    void *platform_context,
    size_t platform_context_size,
    uint32_t storage_slot,
    aether_client_platform_t *platform) {

    if (platform_context == NULL ||
        platform_context_size == 0u ||
        platform == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    observed_storage_slot =
        storage_slot;

    memset(
        platform,
        0,
        sizeof(*platform));

    platform->flash =
        (aether_flash_vtable_t){
            platform_context,
            platform_load,
            platform_save
        };

    return AETHER_OK;
}


void aether_platform_deinit(
    void *platform_context) {

    (void)platform_context;
}


static void on_message(
    aether_t *client,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    (void)client;
    (void)from;
    (void)data;
    (void)size;
}


static void on_ready(
    aether_t *client,
    aether_uuid_t uid) {

    (void)client;
    (void)uid;
}


static void on_error(
    aether_t *client,
    aether_status_t code,
    aether_error_origin_t origin) {

    (void)client;
    (void)code;
    (void)origin;
}


int main(void) {
    aether_t client;

    /*
     * This is the intended beginner-facing construction path.
     */
    if (aether_init(
            &client,
            (aether_uuid_t){0u, 0u}) != AETHER_OK) {

        return 1;
    }

    if (observed_storage_slot != 0u) {
        return 2;
    }

    aether_stop(
        &client);

    if (aether_init_slot(
            &client,
            (aether_uuid_t){0u, 0u},
            7u) != AETHER_OK) {

        return 3;
    }

    if (observed_storage_slot != 7u) {
        return 4;
    }

    aether_on_message(
        &client,
        on_message);

    aether_on_ready(
        &client,
        on_ready);

    aether_on_error(
        &client,
        on_error);

    /*
     * Fake platform intentionally has no clock/network.
     */
    if (aether_start(
            &client) != AETHER_ERR_ARGUMENT) {

        return 5;
    }

    aether_stop(
        &client);

    return 0;
}

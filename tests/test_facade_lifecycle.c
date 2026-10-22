
/*
 * Facade-level lifecycle regression.
 *
 * The public beginner lifecycle is:
 *     aether_init()
 *     aether_set_credentials_typed()
 *     aether_start()
 *
 * aether_client_app_start() historically re-initialized the low-level core
 * through aether_client_init(), which memsets the whole aether_client_t and
 * therefore silently erased caller-provisioned credentials before the core
 * could decide between recovery and NO_IDENTITY. This test pins the correct
 * behavior: after aether_start(), the caller-supplied identity must still be
 * present and the core must be entering the recovery path, never
 * AETHER_STATE_NO_IDENTITY.
 */


#include "aether.h"
#include "aether_platform.h"


#include "aether_hydrogen.h"
#include "aether_test_crypto_hydrogen.h"
#include "hydrogen.h"


#include <assert.h>
#include <string.h>

#include <stdio.h>



static uint32_t observed_storage_slot;
static hydro_sign_keypair fake_root_signer;

static uint64_t fake_now_ms(void *ctx) {
    (void)ctx;
    return 0u;
}

static aether_status_t fake_resolve(
    void *ctx,
    const char *hostname,
    aether_address_t *address) {

    (void)ctx;
    (void)hostname;

    memset(address, 0, sizeof(*address));
    address->kind = AETHER_ADDR_IPV4;
    address->length = 4u;
    address->bytes[0] = 127;
    address->bytes[1] = 0;
    address->bytes[2] = 0;
    address->bytes[3] = 1;
    return AETHER_OK;
}

static aether_status_t fake_transport_open(
    void *ctx,
    aether_channel_t channel,
    const aether_endpoint_t *endpoint) {
    (void)ctx; (void)channel; (void)endpoint;
    return AETHER_OK;
}

static void fake_transport_close(
    void *ctx,
    aether_channel_t channel) {
    (void)ctx; (void)channel;
}

static aether_status_t fake_transport_send(
    void *ctx,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {
    (void)ctx; (void)channel; (void)data; (void)length;
    return AETHER_OK;
}

static aether_status_t fake_flash_load(
    void *ctx,
    uint8_t *dst,
    size_t capacity,
    size_t *length) {
    (void)ctx; (void)dst; (void)capacity; (void)length;
    /* Empty storage: no persisted identity. */
    return AETHER_ERR_STORAGE;
}

static aether_status_t fake_flash_save(
    void *ctx,
    const uint8_t *src,
    size_t length) {
    (void)ctx; (void)src; (void)length;
    return AETHER_OK;
}


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

    observed_storage_slot = storage_slot;

    memset(platform, 0, sizeof(*platform));
    platform->ctx = platform_context;
    platform->now_ms = fake_now_ms;


    platform->transport_io = (aether_transport_io_vtable_t){
        platform_context,
        fake_transport_open,
        fake_transport_close,
        fake_transport_send,
        fake_resolve,
        NULL,
        NULL,
        NULL,
        NULL
    };


    platform->flash = (aether_flash_vtable_t){
        platform_context,
        fake_flash_load,
        fake_flash_save
    };

    platform->crypto = &AETHER_TEST_HYDROGEN_CRYPTO;
    platform->crypto_ctx = NULL;

    platform->trusted_sign_keys = fake_root_signer.pk;
    platform->trusted_sign_key_count = 1u;


    return AETHER_OK;
}

void aether_platform_deinit(void *platform_context) {
    (void)platform_context;
}

static void on_message(
    aether_t *client,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {
    (void)client; (void)from; (void)data; (void)size;
}

static void on_ready(aether_t *client, aether_uuid_t uid) {
    (void)client; (void)uid;
}

static void on_error(
    aether_t *client,
    aether_status_t code,
    aether_error_origin_t origin) {
    (void)client; (void)code; (void)origin;
}

int main(void) {

    aether_t client;

    assert(aether_hydrogen_init() == 0);
    hydro_sign_keygen(&fake_root_signer);


    assert(aether_init(
        &client,
        (aether_uuid_t){0u, 0u}) == AETHER_OK);

    assert(observed_storage_slot == 0u);

    aether_on_message(&client, on_message);
    aether_on_ready(&client, on_ready);
    aether_on_error(&client, on_error);

    aether_uuid_t uid = {
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };

    uint8_t master_key[AETHER_KEY_BYTES];
    for (size_t i = 0u; i < sizeof(master_key); ++i) {
        master_key[i] = (uint8_t)(0x40u + i);
    }

    /* Caller provisions credentials BEFORE start, through the typed path
       used by the physical echo firmware for cross-provider recovery. */
    assert(aether_set_credentials_typed(
        &client.app.core,
        uid,
        master_key,

        COMMON_KEY_SYMMETRIC_TYPE_COMMON_HYDROGEN_SECRET_BOX) == AETHER_OK);



    aether_status_t start_status = aether_start(&client);
    if (start_status != AETHER_OK) {
        fprintf(stderr, "aether_start returned %d (state=%d, init_status=%d)\n",
            (int)start_status,
            (int)client.app.core.state,
            (int)client.init_status);
    }
    assert(start_status == AETHER_OK);


    /* Caller identity survived the facade's destructive reinitialization. */
    assert(client.app.core.uid.msb == uid.msb);
    assert(client.app.core.uid.lsb == uid.lsb);
    assert(memcmp(
        client.app.core.master_key,
        master_key,
        AETHER_KEY_BYTES) == 0);
    assert(client.app.core.master_key_type ==

        COMMON_KEY_SYMMETRIC_TYPE_COMMON_HYDROGEN_SECRET_BOX);


    /* The core must not have fallen back to NO_IDENTITY. */
    assert(client.app.core.state != AETHER_STATE_NO_IDENTITY);

    /* Startup went through mandatory recovery. */
    assert(client.app.core.state ==
        AETHER_STATE_RECOVERY_CONNECTING);

    aether_stop(&client);

    return 0;
}
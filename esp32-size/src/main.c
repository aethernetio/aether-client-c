

#include <stdint.h>
#include <string.h>



#ifdef AETHER_SIZE_WITH_CLIENT
#include "aether_client.h"
#endif

#ifdef AETHER_SIZE_WITH_PUBLIC_API
#include "aether.h"
#endif


#ifdef AETHER_SIZE_WITH_P256
void aether_psa_api_probe_reference(void);
#endif

#ifdef AETHER_SIZE_WITH_P256_BACKEND
#include "aether_crypto_p256_aes_gcm.h"

static const aether_crypto_vtable_t *volatile
    aether_size_crypto_probe;
#endif



/*
 * Keep the same runtime branch in both firmware variants.
 * Being volatile prevents the compiler from proving the branch unreachable.
 * At runtime it remains zero, so the size harness has no side effects.
 */
volatile uint32_t aether_size_probe_enabled;


#ifdef AETHER_SIZE_WITH_PUBLIC_API
static aether_t aether_size_public_client;


#elif defined(AETHER_SIZE_WITH_CLIENT)
static aether_client_t aether_size_client;
static const uint8_t aether_size_byte = 0u;
#endif

#ifdef AETHER_P256_VECTOR_TEST
volatile int aether_p256_vector_result;
int aether_p256_vector_test_run(void);
#endif


void app_main(void) {
#ifdef AETHER_P256_VECTOR_TEST
    aether_p256_vector_result =
        aether_p256_vector_test_run();
#endif


#ifdef AETHER_SIZE_WITH_PUBLIC_API
    if (aether_size_probe_enabled != 0u) {
        (void)aether_init(
            &aether_size_public_client,
            (aether_uuid_t){0u, 0u});

        aether_on_message(
            &aether_size_public_client,
            NULL);

        aether_on_ready(
            &aether_size_public_client,
            NULL);

        aether_on_error(
            &aether_size_public_client,
            NULL);

        /*
         * Keep the optional slot API reachable in the size build too.
         * The branch is never entered at runtime.
         */
        (void)aether_init_slot(
            &aether_size_public_client,
            (aether_uuid_t){0u, 0u},
            1u);

        (void)aether_start(
            &aether_size_public_client);

        (void)aether_poll(
            &aether_size_public_client);

        (void)aether_send(
            &aether_size_public_client,
            (aether_uuid_t){0u, 0u},
            NULL,
            0u);

        (void)aether_uid(
            &aether_size_public_client);

        (void)aether_state(
            &aether_size_public_client);

        (void)aether_is_ready(
            &aether_size_public_client);

        (void)aether_is_registered(
            &aether_size_public_client);

        aether_stop(
            &aether_size_public_client);
    }


#elif defined(AETHER_SIZE_WITH_CLIENT)
    if (aether_size_probe_enabled != 0u) {

#ifdef AETHER_SIZE_WITH_P256
        aether_psa_api_probe_reference();
#endif

#ifdef AETHER_SIZE_WITH_P256_BACKEND
        aether_size_crypto_probe =
            aether_esp32_p256_aes_gcm_crypto();
#endif

        aether_uuid_t destination = {0u, 0u};
        uint32_t request_id = 0u;

        aether_client_init(
            &aether_size_client,
            NULL
        );

        (void)aether_client_start(
            &aether_size_client
        );

        (void)aether_client_retry(
            &aether_size_client
        );

        aether_client_poll(
            &aether_size_client,
            0u
        );

        aether_client_on_transport_state(
            &aether_size_client,
            AETHER_CHANNEL_WORK,
            false
        );

        (void)aether_client_on_rx(
            &aether_size_client,
            AETHER_CHANNEL_WORK,
            &aether_size_byte,
            sizeof(aether_size_byte)
        );

        (void)aether_client_send_message(
            &aether_size_client,
            destination,
            &aether_size_byte,
            sizeof(aether_size_byte),
            &request_id
        );

        (void)aether_client_state(
            &aether_size_client
        );

        (void)aether_client_is_registered(
            &aether_size_client
        );

        aether_client_stop(
            &aether_size_client
        );
    }
#else
    (void)aether_size_probe_enabled;
#endif
}
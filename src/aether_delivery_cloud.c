
/*
 * Cloud delivery provider for aether_delivery.
 *
 * This is the existing Aether cloud path exposed as one provider among
 * many. It is not special-cased inside the coordinator.
 *
 * Mapping of aether_client_app_send_with_id() statuses to the provider
 * attempt contract:
 *
 *   AETHER_OK              -> { ACCEPTED,    AETHER_OK             }
 *   AETHER_ERR_BUSY        -> { RETRY_LATER, AETHER_ERR_BUSY      }
 *   AETHER_ERR_STATE       -> { RETRY_LATER, AETHER_ERR_STATE     }
 *   AETHER_ERR_TRANSPORT   -> { ERROR,       AETHER_ERR_TRANSPORT }
 *   AETHER_ERR_OVERFLOW    -> { ERROR,       AETHER_ERR_OVERFLOW  }
 *   AETHER_ERR_CRYPTO      -> { ERROR,       AETHER_ERR_CRYPTO    }
 *   AETHER_ERR_ARGUMENT    -> { ERROR,       AETHER_ERR_ARGUMENT  }
 *   AETHER_ERR_UNSUPPORTED -> { ERROR,       AETHER_ERR_UNSUPPORTED}
 *   other                  -> { ERROR,       status               }
 *
 * "not ready right now" is a temporary condition, not inapplicability
 * of the route, so it maps to RETRY_LATER with AETHER_ERR_STATE.
 *
 * AETHER_ERR_TRANSPORT is deliberately mapped to ERROR rather than
 * RETRY_LATER. On UDP, send() either delivered a whole datagram or
 * failed with no handoff, but the provider cannot observe the codec
 * from this layer, and a TCP partial write can be ambiguous. Correctness
 * is preferred over retry availability until a stable message ID and
 * receiver deduplication exist.
 */

#include "aether_delivery_cloud.h"
#include "aether_client_app.h"


aether_delivery_attempt_t aether_cloud_delivery_try(
    void *context,
    aether_uuid_t recipient,
    const uint8_t *payload,
    size_t length) {

    aether_t *client =
        context;

    if (client == NULL) {
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_ERROR,
            AETHER_ERR_ARGUMENT
        };
    }

    if (!aether_is_ready(client)) {
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_RETRY_LATER,
            AETHER_ERR_STATE
        };
    }

    aether_status_t status =
        aether_client_app_send_with_id(
            &client->app,
            recipient,
            payload,
            length,
            NULL);

    switch (status) {

    case AETHER_OK:
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_ACCEPTED,
            AETHER_OK
        };

    case AETHER_ERR_BUSY:
    case AETHER_ERR_STATE:
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_RETRY_LATER,
            status
        };

    default:
        return (aether_delivery_attempt_t){
            AETHER_DELIVERY_ERROR,
            status
        };
    }
}


aether_status_t aether_delivery_add_cloud(
    aether_delivery_t *delivery,
    aether_t *client) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    return aether_delivery_add_provider(
        delivery,
        (aether_delivery_provider_t){
            client,
            aether_cloud_delivery_try
        });
}

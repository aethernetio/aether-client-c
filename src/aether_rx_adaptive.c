
#include "aether_rx.h"


void aether_rx_adaptive_init(
    aether_rx_adaptive_t *rx,
    bool receive_all) {

    if (rx == NULL) {
        return;
    }

    aether_rx_first_init(
        &rx->first);

    rx->all =
        receive_all;
}


void aether_rx_adaptive_set_all(
    aether_rx_adaptive_t *rx,
    bool receive_all) {

    if (rx == NULL) {
        return;
    }

    rx->all =
        receive_all;
}


void aether_rx_adaptive_reset(
    aether_rx_adaptive_t *rx) {

    if (rx == NULL) {
        return;
    }

    aether_rx_first_reset(
        &rx->first);
}


bool aether_rx_adaptive_accept(
    aether_rx_adaptive_t *rx,
    const aether_ingress_t *ingress,
    aether_rx_message_fn on_message,
    void *ctx) {

    if (rx == NULL) {
        return false;
    }

    if (rx->all) {
        /*
         * ALL itself has no behavioral state. Avoid embedding another
         * component object only for dispatch.
         */
        if (ingress == NULL ||
            ingress->kind !=
                AETHER_INGRESS_MESSAGE) {

            return false;
        }

        if (on_message != NULL) {
            on_message(
                ctx,
                ingress->as.message.from,
                ingress->as.message.payload.data,
                ingress->as.message.payload.length);
        }

        return true;
    }

    return
        aether_rx_first_accept(
            &rx->first,
            ingress,
            on_message,
            ctx);
}


#include "aether_rx.h"


void aether_rx_all_init(
    aether_rx_all_t *rx) {

    if (rx == NULL) {
        return;
    }

    rx->reserved =
        0u;
}


bool aether_rx_all_accept(
    aether_rx_all_t *rx,
    const aether_ingress_t *ingress,
    aether_rx_message_fn on_message,
    void *ctx) {

    if (rx == NULL ||
        ingress == NULL ||
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

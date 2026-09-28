
#include "aether_rx.h"


void aether_rx_first_init(
    aether_rx_first_t *rx) {

    if (rx == NULL) {
        return;
    }

    rx->received =
        false;
}


void aether_rx_first_reset(
    aether_rx_first_t *rx) {

    aether_rx_first_init(
        rx);
}


bool aether_rx_first_accept(
    aether_rx_first_t *rx,
    const aether_ingress_t *ingress,
    aether_rx_message_fn on_message,
    void *ctx) {

    if (rx == NULL ||
        ingress == NULL ||
        ingress->kind !=
            AETHER_INGRESS_MESSAGE) {

        return false;
    }

    if (rx->received) {
        return true;
    }

    rx->received =
        true;

    if (on_message != NULL) {
        on_message(
            ctx,
            ingress->as.message.from,
            ingress->as.message.payload.data,
            ingress->as.message.payload.length);
    }

    return true;
}

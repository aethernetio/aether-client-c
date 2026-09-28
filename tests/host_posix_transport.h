
#ifndef AETHER_HOST_POSIX_TRANSPORT_H
#define AETHER_HOST_POSIX_TRANSPORT_H

#include "aether_client.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#define AETHER_HOST_TCP_RX_CAPACITY \
    (AETHER_MAX_PACKET * 4u + 32u)


typedef struct {
    int fd;
    aether_codec_t codec;
    bool notify_writable;

    /*
     * Transport-owned receive storage.
     *
     * TCP may contain multiple framed packets already received from the
     * socket. UDP keeps exactly one pending datagram until it is consumed.
     */
    uint8_t rx[AETHER_HOST_TCP_RX_CAPACITY];
    size_t rx_len;
} aether_host_channel_t;


typedef struct {
    /*
     * Used only for transport state notifications.
     * RX protocol parsing is never called by this transport.
     */
    aether_client_t *client;

    aether_host_channel_t registration;
    aether_host_channel_t work;
} aether_host_transport_t;


void aether_host_transport_init(
    aether_host_transport_t *transport,
    aether_client_t *client);


aether_transport_vtable_t aether_host_transport_vtable(
    aether_host_transport_t *transport);


/*
 * Collect physical network input into transport-owned storage and report
 * transport state changes.
 *
 * This function never invokes aether_client_on_rx().
 */
int aether_host_transport_pump(
    aether_host_transport_t *transport,
    int timeout_ms);


/*
 * Borrow one complete frame already stored by the transport.
 *
 * frame->data == NULL means no complete frame is ready.
 * The returned memory remains owned by the transport until consume().
 */
aether_status_t aether_host_transport_receive(
    aether_host_transport_t *transport,
    aether_channel_t channel,
    aether_bytes_view_t *frame);


/*
 * Release exactly the frame previously returned by receive().
 */
void aether_host_transport_consume(
    aether_host_transport_t *transport,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length);


void aether_host_transport_shutdown(
    aether_host_transport_t *transport);


#endif


/*
 * POSIX raw I/O transport.
 *
 * A concrete aether_transport_io_vtable_t implementation for host
 * platforms (Linux/macOS) using TCP/UDP sockets.
 *
 * The transport borrows the aether_t core: it uses the core's trusted
 * sign keys and crypto through the normal aether_client_t state
 * machine, and reports transport state through
 * aether_client_on_transport_state(). The transport itself performs no
 * crypto and no protocol parsing.
 *
 * aether_posix_transport_init() binds the transport to a client and
 * records the registration URI in the client's app config, so that
 * aether_start() will use this transport for the registration channel.
 * The URI is what selects the test/local server vs. the production
 * server; no address is hardcoded in the transport.
 */

#ifndef AETHER_POSIX_TRANSPORT_H
#define AETHER_POSIX_TRANSPORT_H

#include "aether.h"
#include "aether_client.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define AETHER_POSIX_TCP_RX_CAPACITY \
    (AETHER_MAX_PACKET * 4u + 32u)


typedef struct {
    int fd;
    aether_codec_t codec;
    bool notify_writable;

    /*
     * Transport-owned receive storage. TCP may contain multiple framed
     * packets already received from the socket. UDP keeps exactly one
     * pending datagram until it is consumed.
     */
    uint8_t rx[AETHER_POSIX_TCP_RX_CAPACITY];
    size_t rx_len;
} aether_posix_channel_t;


typedef struct {
    /*
     * Used only for transport state notifications. RX protocol parsing
     * is never performed by this transport.
     */
    aether_client_t *client;

    aether_posix_channel_t registration;
    aether_posix_channel_t work;
} aether_posix_transport_t;


/*
 * Bind the POSIX transport to a client and record the registration URI.
 *
 * Must be called after aether_init()/aether_init_slot() and before
 * aether_start(). The URI is stored in the client's app config so that
 * aether_start() parses and resolves it through this transport.
 *
 * The caller keeps ownership of the transport struct and the URI
 * string; both must outlive the client.
 */
void aether_posix_transport_init(
    aether_posix_transport_t *transport,
    aether_t *core,
    const char *registration_uri);


aether_transport_io_vtable_t aether_posix_transport_vtable(
    aether_posix_transport_t *transport);


/*
 * Collect physical network input into transport-owned storage and
 * report transport state changes. Never invokes aether_client_on_rx().
 */
int aether_posix_transport_pump(
    aether_posix_transport_t *transport,
    int timeout_ms);


/*
 * Borrow one complete frame already stored by the transport.
 * frame->data == NULL means no complete frame is ready. The returned
 * memory remains owned by the transport until consume().
 */
aether_status_t aether_posix_transport_receive(
    aether_posix_transport_t *transport,
    aether_channel_t channel,
    aether_bytes_view_t *frame);

/*
 * Bind a transport to a low-level client without touching registration
 * URI or platform vtable. Zeroes the transport, sets fd=-1 on both
 * channels and remembers the client for state notifications.
 *
 * Intended for host tests that build the platform bundle themselves,
 * and for callers that need the raw transport separately from the
 * high-level aether_t facade.
 */
void aether_posix_transport_bind(
    aether_posix_transport_t *transport,
    aether_client_t *core);




void aether_posix_transport_consume(
    aether_posix_transport_t *transport,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length);


void aether_posix_transport_shutdown(
    aether_posix_transport_t *transport);


#ifdef __cplusplus
}
#endif

#endif
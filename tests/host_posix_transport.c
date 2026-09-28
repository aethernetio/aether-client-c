
#include "host_posix_transport.h"

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>


static aether_host_channel_t *channel_for(
    aether_host_transport_t *transport,
    aether_channel_t channel) {

    if (transport == NULL) {
        return NULL;
    }

    if (channel ==
        AETHER_CHANNEL_REGISTRATION) {

        return
            &transport->registration;
    }

    if (channel ==
        AETHER_CHANNEL_WORK) {

        return
            &transport->work;
    }

    return NULL;
}


static void close_channel(
    aether_host_channel_t *channel) {

    if (channel == NULL) {
        return;
    }

    if (channel->fd >= 0) {
        close(
            channel->fd);
    }

    channel->fd = -1;
    channel->notify_writable = false;

    /*
     * Do not destroy rx_len here.
     *
     * A borrowed frame may currently be parsed by aether_client_app_tick().
     * Parsing can synchronously close the transport channel. The borrowed
     * bytes must remain valid until the app layer calls consume().
     *
     * host_open() resets rx_len before a newly opened socket is reused.
     */
}


static size_t encode_pack_number(
    uint8_t out[8],
    uint64_t value) {

    const uint64_t u8 = 251u;
    const uint64_t u16 = 1515u;
    const uint64_t u32 = 1049835u;

    if (value < u8) {
        out[0] =
            (uint8_t)value;

        return 1u;
    }

    if (value < u16) {
        uint64_t x =
            value - u8;

        out[0] =
            (uint8_t)(
                ((x >> 8u) & 0xffu) +
                u8);

        out[1] =
            (uint8_t)(
                x & 0xffu);

        return 2u;
    }

    if (value < u32) {
        uint64_t x =
            value - u16;

        uint16_t low =
            (uint16_t)x;

        out[0] = 255u;

        out[1] =
            (uint8_t)(
                ((x >> 16u) - u8) +
                u16);

        out[2] =
            (uint8_t)(
                low & 0xffu);

        out[3] =
            (uint8_t)(
                low >> 8u);

        return 4u;
    }

    return 0u;
}


static bool decode_pack_number(
    const uint8_t *data,
    size_t length,
    size_t *consumed,
    size_t *value) {

    const uint64_t u8 = 251u;
    const uint64_t u16 = 1515u;
    const uint64_t u32 = 1049835u;

    const uint64_t u64 =
        u32 +
        (UINT64_C(4294967296) * 256u);

    if (data == NULL ||
        consumed == NULL ||
        value == NULL ||
        length < 1u) {

        return false;
    }

    size_t pos = 0u;

    uint64_t val =
        data[pos++];

    if (val < u8) {
        *consumed = pos;
        *value = (size_t)val;

        return true;
    }

    if (length <
        pos + 1u) {

        return false;
    }

    uint64_t v =
        data[pos++];

    val =
        ((val - u8) << 8u) +
        u8 +
        v;

    if (val < u16) {
        *consumed = pos;
        *value = (size_t)val;

        return true;
    }

    if (length <
        pos + 2u) {

        return false;
    }

    uint64_t f =
        (uint64_t)data[pos] |
        ((uint64_t)data[pos + 1u] << 8u);

    pos += 2u;

    val =
        ((val - u16) << 16u) +
        u16 +
        f;

    if (val < u32) {
        *consumed = pos;
        *value = (size_t)val;

        return true;
    }

    if (length <
        pos + 4u) {

        return false;
    }

    uint64_t f1 =
        (uint64_t)data[pos] |
        ((uint64_t)data[pos + 1u] << 8u) |
        ((uint64_t)data[pos + 2u] << 16u) |
        ((uint64_t)data[pos + 3u] << 24u);

    pos += 4u;

    val =
        ((val - u32) << 32u) +
        u32 +
        f1;

    if (val >= u64 ||
        val > SIZE_MAX) {

        return false;
    }

    *consumed = pos;
    *value = (size_t)val;

    return true;
}


static int send_all(
    int fd,
    const uint8_t *data,
    size_t length) {

    size_t pos = 0u;

    while (pos < length) {
        ssize_t n =
            send(
                fd,
                data + pos,
                length - pos,
                0);

        if (n > 0) {
            pos +=
                (size_t)n;

            continue;
        }

        if (n < 0 &&
            errno == EINTR) {

            continue;
        }

        return -1;
    }

    return 0;
}


static aether_status_t host_open(
    void *ctx,
    aether_channel_t channel,
    const aether_endpoint_t *endpoint) {

    aether_host_transport_t *transport =
        ctx;

    aether_host_channel_t *slot =
        channel_for(
            transport,
            channel);

    if (transport == NULL ||
        endpoint == NULL ||
        slot == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    close_channel(
        slot);

    /*
     * New socket means old borrowed/partial transport data is obsolete.
     */
    slot->rx_len = 0u;

    int family;
    socklen_t address_length;

    struct sockaddr_storage address;

    memset(
        &address,
        0,
        sizeof(address));

    if (endpoint->address.kind ==
            AETHER_ADDR_IPV4 &&
        endpoint->address.length ==
            4u) {

        struct sockaddr_in *ipv4 =
            (struct sockaddr_in *)&address;

        family =
            AF_INET;

        ipv4->sin_family =
            AF_INET;

        ipv4->sin_port =
            htons(
                endpoint->port);

        memcpy(
            &ipv4->sin_addr,
            endpoint->address.bytes,
            4u);

        address_length =
            sizeof(*ipv4);

    } else if (
        endpoint->address.kind ==
            AETHER_ADDR_IPV6 &&
        endpoint->address.length ==
            16u) {

        struct sockaddr_in6 *ipv6 =
            (struct sockaddr_in6 *)&address;

        family =
            AF_INET6;

        ipv6->sin6_family =
            AF_INET6;

        ipv6->sin6_port =
            htons(
                endpoint->port);

        memcpy(
            &ipv6->sin6_addr,
            endpoint->address.bytes,
            16u);

        address_length =
            sizeof(*ipv6);

    } else {
        return AETHER_ERR_UNSUPPORTED;
    }

    int socket_type;

    if (endpoint->codec ==
        AETHER_CODEC_TCP) {

        socket_type =
            SOCK_STREAM;

    } else if (
        endpoint->codec ==
        AETHER_CODEC_UDP) {

        socket_type =
            SOCK_DGRAM;

    } else {
        return AETHER_ERR_UNSUPPORTED;
    }

    int fd =
        socket(
            family,
            socket_type,
            0);

    if (fd < 0) {
        perror(
            "aether host socket");

        return AETHER_ERR_TRANSPORT;
    }

    if (connect(
            fd,
            (const struct sockaddr *)&address,
            address_length) != 0) {

        perror(
            "aether host connect");

        close(
            fd);

        return AETHER_ERR_TRANSPORT;
    }

    slot->fd =
        fd;

    slot->codec =
        endpoint->codec;

    slot->notify_writable =
        true;

    return AETHER_OK;
}


static void host_close(
    void *ctx,
    aether_channel_t channel) {

    aether_host_transport_t *transport =
        ctx;

    close_channel(
        channel_for(
            transport,
            channel));
}


static aether_status_t host_send(
    void *ctx,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {

    aether_host_transport_t *transport =
        ctx;

    aether_host_channel_t *slot =
        channel_for(
            transport,
            channel);

    if (slot == NULL ||
        slot->fd < 0 ||
        (data == NULL &&
         length != 0u)) {

        return AETHER_ERR_TRANSPORT;
    }

    if (slot->codec ==
        AETHER_CODEC_UDP) {

        ssize_t n;

        do {
            n =
                send(
                    slot->fd,
                    data,
                    length,
                    0);

        } while (
            n < 0 &&
            errno == EINTR);

        return
            n == (ssize_t)length
                ? AETHER_OK
                : AETHER_ERR_TRANSPORT;
    }

    if (slot->codec ==
        AETHER_CODEC_TCP) {

        uint8_t prefix[8];

        size_t prefix_len =
            encode_pack_number(
                prefix,
                length);

        if (prefix_len == 0u) {
            return AETHER_ERR_OVERFLOW;
        }

        if (send_all(
                slot->fd,
                prefix,
                prefix_len) != 0 ||
            send_all(
                slot->fd,
                data,
                length) != 0) {

            return AETHER_ERR_TRANSPORT;
        }

        return AETHER_OK;
    }

    return AETHER_ERR_UNSUPPORTED;
}


/*
 * Inspect one transport-owned channel without modifying it.
 *
 * consumed is the number of bytes that consume() must remove from rx.
 * For TCP this includes the framing prefix.
 */
static aether_status_t peek_channel(
    aether_host_channel_t *slot,
    aether_bytes_view_t *frame,
    size_t *consumed) {

    if (slot == NULL ||
        frame == NULL ||
        consumed == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    frame->data = NULL;
    frame->length = 0u;
    *consumed = 0u;

    if (slot->rx_len == 0u) {
        return AETHER_OK;
    }

    if (slot->codec ==
        AETHER_CODEC_UDP) {

        if (slot->rx_len >
            AETHER_MAX_PACKET) {

            return AETHER_ERR_OVERFLOW;
        }

        frame->data =
            slot->rx;

        frame->length =
            slot->rx_len;

        *consumed =
            slot->rx_len;

        return AETHER_OK;
    }

    if (slot->codec !=
        AETHER_CODEC_TCP) {

        return AETHER_ERR_UNSUPPORTED;
    }

    size_t prefix_len = 0u;
    size_t payload_len = 0u;

    if (!decode_pack_number(
            slot->rx,
            slot->rx_len,
            &prefix_len,
            &payload_len)) {

        return AETHER_OK;
    }

    if (payload_len >
        AETHER_MAX_PACKET) {

        return AETHER_ERR_OVERFLOW;
    }

    if (slot->rx_len <
        prefix_len + payload_len) {

        return AETHER_OK;
    }

    frame->data =
        slot->rx + prefix_len;

    frame->length =
        payload_len;

    *consumed =
        prefix_len + payload_len;

    return AETHER_OK;
}


aether_status_t aether_host_transport_receive(
    aether_host_transport_t *transport,
    aether_channel_t channel,
    aether_bytes_view_t *frame) {

    if (transport == NULL ||
        frame == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    aether_host_channel_t *slot =
        channel_for(
            transport,
            channel);

    if (slot == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t consumed = 0u;

    return
        peek_channel(
            slot,
            frame,
            &consumed);
}


void aether_host_transport_consume(
    aether_host_transport_t *transport,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {

    if (transport == NULL) {
        return;
    }

    aether_host_channel_t *slot =
        channel_for(
            transport,
            channel);

    if (slot == NULL) {
        return;
    }

    aether_bytes_view_t frame;
    size_t consumed = 0u;

    aether_status_t status =
        peek_channel(
            slot,
            &frame,
            &consumed);

    if (status != AETHER_OK ||
        frame.data == NULL ||
        frame.data != data ||
        frame.length != length ||
        consumed > slot->rx_len) {

        return;
    }

    memmove(
        slot->rx,
        slot->rx + consumed,
        slot->rx_len - consumed);

    slot->rx_len -=
        consumed;
}


/*
 * Read more TCP bytes only while there is no complete frame waiting for the
 * application layer.
 */
static int pump_tcp(
    aether_host_channel_t *slot) {

    if (slot == NULL) {
        return -1;
    }

    aether_bytes_view_t frame;
    size_t consumed = 0u;

    aether_status_t status =
        peek_channel(
            slot,
            &frame,
            &consumed);

    if (status != AETHER_OK) {
        return -1;
    }

    if (frame.data != NULL) {
        return 0;
    }

    if (slot->rx_len >=
        sizeof(slot->rx)) {

        fprintf(
            stderr,
            "TCP receive buffer overflow\n");

        return -1;
    }

    ssize_t n;

    do {
        n =
            recv(
                slot->fd,
                slot->rx + slot->rx_len,
                sizeof(slot->rx) -
                    slot->rx_len,
                0);

    } while (
        n < 0 &&
        errno == EINTR);

    if (n <= 0) {
        return -1;
    }

    slot->rx_len +=
        (size_t)n;

    status =
        peek_channel(
            slot,
            &frame,
            &consumed);

    if (status != AETHER_OK) {
        return -1;
    }

    return 0;
}


/*
 * Keep one UDP datagram borrowed until the application layer consumes it.
 * Later datagrams remain queued by the kernel.
 */
static int pump_udp(
    aether_host_channel_t *slot) {

    if (slot == NULL) {
        return -1;
    }

    if (slot->rx_len != 0u) {
        return 0;
    }

    ssize_t n;

    do {
        n =
            recv(
                slot->fd,
                slot->rx,
                AETHER_MAX_PACKET,
                0);

    } while (
        n < 0 &&
        errno == EINTR);

    if (n <= 0) {
        return -1;
    }

    slot->rx_len =
        (size_t)n;

    return 0;
}


void aether_host_transport_init(
    aether_host_transport_t *transport,
    aether_client_t *client) {

    if (transport == NULL) {
        return;
    }

    memset(
        transport,
        0,
        sizeof(*transport));

    transport->client =
        client;

    transport->registration.fd =
        -1;

    transport->work.fd =
        -1;
}


aether_transport_vtable_t aether_host_transport_vtable(
    aether_host_transport_t *transport) {

    return
        (aether_transport_vtable_t){
            transport,
            host_open,
            host_close,
            host_send
        };
}


static void notify_writable_channels(
    aether_host_transport_t *transport,
    aether_host_channel_t *slots[2],
    const aether_channel_t channels[2]) {

    if (transport == NULL ||
        transport->client == NULL) {

        return;
    }

    for (size_t i = 0u;
         i < 2u;
         ++i) {

        if (!slots[i]->notify_writable) {
            continue;
        }

        slots[i]->notify_writable =
            false;

        aether_client_on_transport_state(
            transport->client,
            channels[i],
            true);
    }
}


int aether_host_transport_pump(
    aether_host_transport_t *transport,
    int timeout_ms) {

    if (transport == NULL) {
        return -1;
    }

    aether_host_channel_t *slots[2] = {
        &transport->registration,
        &transport->work
    };

    const aether_channel_t channels[2] = {
        AETHER_CHANNEL_REGISTRATION,
        AETHER_CHANNEL_WORK
    };

    notify_writable_channels(
        transport,
        slots,
        channels);

    struct pollfd poll_fds[2];
    size_t slot_indices[2];
    size_t poll_count = 0u;

    for (size_t i = 0u;
         i < 2u;
         ++i) {

        if (slots[i]->fd < 0) {
            continue;
        }

        poll_fds[poll_count].fd =
            slots[i]->fd;

        poll_fds[poll_count].events =
            POLLIN |
            POLLERR |
            POLLHUP;

        poll_fds[poll_count].revents =
            0;

        slot_indices[poll_count] =
            i;

        ++poll_count;
    }

    if (poll_count == 0u) {
        if (timeout_ms > 0) {
            (void)poll(
                NULL,
                0,
                timeout_ms);
        }

        return 0;
    }

    int ready;

    do {
        ready =
            poll(
                poll_fds,
                (nfds_t)poll_count,
                timeout_ms);

    } while (
        ready < 0 &&
        errno == EINTR);

    if (ready < 0) {
        perror(
            "aether host poll");

        return -1;
    }

    if (ready == 0) {
        return 0;
    }

    for (size_t pi = 0u;
         pi < poll_count;
         ++pi) {

        if (poll_fds[pi].revents ==
            0) {

            continue;
        }

        size_t index =
            slot_indices[pi];

        aether_host_channel_t *slot =
            slots[index];

        aether_channel_t channel =
            channels[index];

        if ((poll_fds[pi].revents &
             (POLLERR |
              POLLHUP |
              POLLNVAL)) != 0) {

            close_channel(
                slot);

            if (transport->client != NULL) {
                aether_client_on_transport_state(
                    transport->client,
                    channel,
                    false);
            }

            continue;
        }

        if ((poll_fds[pi].revents &
             POLLIN) == 0) {

            continue;
        }

        int result =
            slot->codec ==
                    AETHER_CODEC_UDP
                ? pump_udp(
                      slot)
                : pump_tcp(
                      slot);

        if (result == 0) {
            continue;
        }

        close_channel(
            slot);

        if (transport->client != NULL) {
            aether_client_on_transport_state(
                transport->client,
                channel,
                false);
        }
    }

    return 0;
}


void aether_host_transport_shutdown(
    aether_host_transport_t *transport) {

    if (transport == NULL) {
        return;
    }

    close_channel(
        &transport->registration);

    close_channel(
        &transport->work);

    transport->registration.rx_len =
        0u;

    transport->work.rx_len =
        0u;

    transport->client =
        NULL;
}

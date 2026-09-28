
#include "aether_platform.h"
#include "aether_platform_esp32.h"
#include "aether_crypto_p256_aes_gcm.h"

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_timer.h"
#include "nvs.h"
#include "nvs_flash.h"

#include <lwip/inet.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>


#define AETHER_ESP32_TCP_RX_CAPACITY \
    (AETHER_MAX_PACKET + 8u)


typedef struct {
    int fd;
    aether_codec_t codec;
    bool notify_writable;

    /*
     * Platform-owned RX storage.
     *
     * TCP keeps enough memory for one maximal framed packet.
     * UDP keeps one complete datagram until consume().
     */
    uint8_t rx[AETHER_ESP32_TCP_RX_CAPACITY];
    size_t rx_len;
} aether_esp32_channel_t;


typedef struct {
    /*
     * Used only to report transport state.
     * RX protocol parsing never runs from this adapter.
     */
    aether_client_t *client;

    aether_esp32_channel_t registration;
    aether_esp32_channel_t work;

    nvs_handle_t nvs;
    bool nvs_open;
    char state_key[16];
} aether_esp32_platform_t;


_Static_assert(
    sizeof(aether_esp32_platform_t) <=
        AETHER_PLATFORM_CONTEXT_BYTES,
    "AETHER_PLATFORM_CONTEXT_BYTES is too small for ESP32");


#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
const uint8_t *aether_esp32_trusted_sign_keys(
    size_t *count) {

    if (count != NULL) {
        *count = 0u;
    }

    return NULL;
}


static aether_status_t storage_load(
    void *ctx,
    uint8_t *data,
    size_t capacity,
    size_t *size) {

    aether_esp32_platform_t *platform =
        ctx;

    if (platform == NULL ||
        !platform->nvs_open ||
        data == NULL ||
        size == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    size_t required = 0u;

    esp_err_t error =
        nvs_get_blob(
            platform->nvs,
            platform->state_key,
            NULL,
            &required);

    if (error ==
        ESP_ERR_NVS_NOT_FOUND) {

        return AETHER_ERR_STORAGE;
    }

    if (error != ESP_OK) {
        return AETHER_ERR_STORAGE;
    }

    if (required >
        capacity) {

        return AETHER_ERR_OVERFLOW;
    }

    size_t loaded =
        capacity;

    error =
        nvs_get_blob(
            platform->nvs,
            platform->state_key,
            data,
            &loaded);

    if (error != ESP_OK) {
        return AETHER_ERR_STORAGE;
    }

    *size =
        loaded;

    return AETHER_OK;
}


static aether_status_t storage_save(
    void *ctx,
    const uint8_t *data,
    size_t size) {

    aether_esp32_platform_t *platform =
        ctx;

    if (platform == NULL ||
        !platform->nvs_open ||
        (data == NULL &&
         size != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    esp_err_t error =
        nvs_set_blob(
            platform->nvs,
            platform->state_key,
            data,
            size);

    if (error != ESP_OK) {
        return AETHER_ERR_STORAGE;
    }

    error =
        nvs_commit(
            platform->nvs);

    return
        error == ESP_OK
            ? AETHER_OK
            : AETHER_ERR_STORAGE;
}


static aether_esp32_channel_t *channel_for(
    aether_esp32_platform_t *platform,
    aether_channel_t channel) {

    if (platform == NULL) {
        return NULL;
    }

    if (channel ==
        AETHER_CHANNEL_REGISTRATION) {

        return
            &platform->registration;
    }

    if (channel ==
        AETHER_CHANNEL_WORK) {

        return
            &platform->work;
    }

    return NULL;
}


static void close_channel(
    aether_esp32_channel_t *channel) {

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
     * Preserve rx_len here.
     *
     * aether_client_on_rx() is called later by aether_client_app_tick() with
     * a borrowed view into this buffer. Protocol handling can synchronously
     * close the channel. The borrowed bytes remain valid until consume().
     *
     * transport_open() resets rx_len before socket reuse.
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


static bool endpoint_to_sockaddr(
    const aether_endpoint_t *endpoint,
    struct sockaddr_storage *address,
    socklen_t *address_length,
    int *family) {

    if (endpoint == NULL ||
        address == NULL ||
        address_length == NULL ||
        family == NULL) {

        return false;
    }

    memset(
        address,
        0,
        sizeof(*address));

    if (endpoint->address.kind ==
            AETHER_ADDR_IPV4 &&
        endpoint->address.length ==
            4u) {

        struct sockaddr_in *ipv4 =
            (struct sockaddr_in *)address;

        *family =
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

        *address_length =
            sizeof(*ipv4);

        return true;
    }


#if LWIP_IPV6
    if (endpoint->address.kind ==
            AETHER_ADDR_IPV6 &&
        endpoint->address.length ==
            16u) {

        struct sockaddr_in6 *ipv6 =
            (struct sockaddr_in6 *)address;

        *family =
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

        *address_length =
            sizeof(*ipv6);

        return true;
    }
#endif


    return false;
}


static aether_status_t transport_open(
    void *ctx,
    aether_channel_t channel,
    const aether_endpoint_t *endpoint) {

    aether_esp32_platform_t *platform =
        ctx;

    aether_esp32_channel_t *slot =
        channel_for(
            platform,
            channel);

    if (platform == NULL ||
        endpoint == NULL ||
        slot == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    close_channel(
        slot);

    /*
     * A newly opened socket cannot use bytes from the old connection.
     */
    slot->rx_len = 0u;

    struct sockaddr_storage address;
    socklen_t address_length;
    int family;

    if (!endpoint_to_sockaddr(
            endpoint,
            &address,
            &address_length,
            &family)) {

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
        return AETHER_ERR_TRANSPORT;
    }

    if (connect(
            fd,
            (const struct sockaddr *)&address,
            address_length) != 0) {

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


static void transport_close(
    void *ctx,
    aether_channel_t channel) {

    aether_esp32_platform_t *platform =
        ctx;

    close_channel(
        channel_for(
            platform,
            channel));
}


static aether_status_t transport_send(
    void *ctx,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {

    aether_esp32_platform_t *platform =
        ctx;

    aether_esp32_channel_t *slot =
        channel_for(
            platform,
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
 * Inspect one platform-owned channel without modifying it.
 *
 * For TCP, consumed includes the framing prefix.
 */
static aether_status_t peek_channel(
    aether_esp32_channel_t *slot,
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


/*
 * Borrow one complete network frame.
 *
 * No packet copy is made here.
 */
static aether_status_t platform_receive(
    void *ctx,
    aether_channel_t channel,
    aether_bytes_view_t *frame) {

    aether_esp32_platform_t *platform =
        ctx;

    if (platform == NULL ||
        frame == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    aether_esp32_channel_t *slot =
        channel_for(
            platform,
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


/*
 * Release exactly the borrowed frame returned by platform_receive().
 */
static void platform_consume(
    void *ctx,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {

    aether_esp32_platform_t *platform =
        ctx;

    if (platform == NULL) {
        return;
    }

    aether_esp32_channel_t *slot =
        channel_for(
            platform,
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
 * Read socket bytes until a complete TCP frame is available.
 *
 * Once a complete frame is pending, leave further bytes in lwIP until the
 * application consumes that frame. This bounds platform-side buffering.
 */
static int pump_tcp(
    aether_esp32_channel_t *slot) {

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

    return
        status == AETHER_OK
            ? 0
            : -1;
}


/*
 * Keep one UDP datagram in platform-owned memory until consume().
 */
static int pump_udp(
    aether_esp32_channel_t *slot) {

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


static void notify_writable_channels(
    aether_esp32_platform_t *platform,
    aether_esp32_channel_t *slots[2],
    const aether_channel_t channels[2]) {

    if (platform == NULL ||
        platform->client == NULL) {

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
            platform->client,
            channels[i],
            true);
    }
}


/*
 * Physical network pump only.
 *
 * This function may fill transport-owned buffers and report connection state.
 * It never executes Aether RX parsing or crypto.
 */
static aether_status_t platform_poll(
    void *ctx) {

    aether_esp32_platform_t *platform =
        ctx;

    if (platform == NULL ||
        platform->client == NULL) {

        return AETHER_ERR_STATE;
    }

    aether_esp32_channel_t *slots[2] = {
        &platform->registration,
        &platform->work
    };

    const aether_channel_t channels[2] = {
        AETHER_CHANNEL_REGISTRATION,
        AETHER_CHANNEL_WORK
    };

    notify_writable_channels(
        platform,
        slots,
        channels);

    fd_set read_set;
    fd_set error_set;

    FD_ZERO(
        &read_set);

    FD_ZERO(
        &error_set);

    int max_fd = -1;

    for (size_t i = 0u;
         i < 2u;
         ++i) {

        if (slots[i]->fd < 0) {
            continue;
        }

        FD_SET(
            slots[i]->fd,
            &read_set);

        FD_SET(
            slots[i]->fd,
            &error_set);

        if (slots[i]->fd >
            max_fd) {

            max_fd =
                slots[i]->fd;
        }
    }

    if (max_fd < 0) {
        return AETHER_OK;
    }

    struct timeval timeout = {
        .tv_sec = 0,
        .tv_usec = 0
    };

    int ready =
        select(
            max_fd + 1,
            &read_set,
            NULL,
            &error_set,
            &timeout);

    if (ready < 0) {
        return AETHER_ERR_TRANSPORT;
    }

    if (ready == 0) {
        return AETHER_OK;
    }

    for (size_t i = 0u;
         i < 2u;
         ++i) {

        aether_esp32_channel_t *slot =
            slots[i];

        if (slot->fd < 0) {
            continue;
        }

        if (FD_ISSET(
                slot->fd,
                &error_set)) {

            close_channel(
                slot);

            aether_client_on_transport_state(
                platform->client,
                channels[i],
                false);

            continue;
        }

        if (!FD_ISSET(
                slot->fd,
                &read_set)) {

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

        aether_client_on_transport_state(
            platform->client,
            channels[i],
            false);
    }

    return AETHER_OK;
}


static aether_status_t platform_bind(
    void *ctx,
    aether_client_t *core) {

    aether_esp32_platform_t *platform =
        ctx;

    if (platform == NULL ||
        core == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    platform->client =
        core;

    return AETHER_OK;
}


static uint64_t platform_now_ms(
    void *ctx) {

    (void)ctx;

    int64_t us =
        esp_timer_get_time();

    return
        us > 0
            ? (uint64_t)us /
                UINT64_C(1000)
            : 0u;
}


static aether_status_t platform_resolve(
    void *ctx,
    const char *hostname,
    aether_address_t *address) {

    (void)ctx;

    if (hostname == NULL ||
        address == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    struct addrinfo hints;

    memset(
        &hints,
        0,
        sizeof(hints));

    hints.ai_family =
        AF_UNSPEC;

    hints.ai_socktype =
        SOCK_STREAM;

    struct addrinfo *result =
        NULL;

    if (getaddrinfo(
            hostname,
            NULL,
            &hints,
            &result) != 0 ||
        result == NULL) {

        return AETHER_ERR_TRANSPORT;
    }

    aether_status_t status =
        AETHER_ERR_UNSUPPORTED;

    for (struct addrinfo *it = result;
         it != NULL;
         it = it->ai_next) {

        if (it->ai_family ==
                AF_INET &&
            it->ai_addrlen >=
                sizeof(struct sockaddr_in)) {

            const struct sockaddr_in *ipv4 =
                (const struct sockaddr_in *)
                    it->ai_addr;

            memset(
                address,
                0,
                sizeof(*address));

            address->kind =
                AETHER_ADDR_IPV4;

            address->length =
                4u;

            memcpy(
                address->bytes,
                &ipv4->sin_addr,
                4u);

            status =
                AETHER_OK;

            break;
        }


#if LWIP_IPV6
        if (it->ai_family ==
                AF_INET6 &&
            it->ai_addrlen >=
                sizeof(struct sockaddr_in6)) {

            const struct sockaddr_in6 *ipv6 =
                (const struct sockaddr_in6 *)
                    it->ai_addr;

            memset(
                address,
                0,
                sizeof(*address));

            address->kind =
                AETHER_ADDR_IPV6;

            address->length =
                16u;

            memcpy(
                address->bytes,
                &ipv6->sin6_addr,
                16u);

            status =
                AETHER_OK;

            break;
        }
#endif

    }

    freeaddrinfo(
        result);

    return status;
}


/*
 * This is the currently-supported PoW contract already used by the working
 * C integration: the registration server advertises INT32_MAX.
 */
static aether_status_t pow_generate(
    void *ctx,
    const uint8_t *salt,
    size_t salt_len,
    const uint8_t *suffix,
    size_t suffix_len,
    uint8_t pool_size,
    int32_t max_hash,
    int32_t *passwords,
    size_t capacity,
    size_t *count) {

    (void)ctx;
    (void)salt;
    (void)salt_len;
    (void)suffix;
    (void)suffix_len;

    if (passwords == NULL ||
        count == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (max_hash !=
        INT32_MAX) {

        return AETHER_ERR_UNSUPPORTED;
    }

    if (capacity <
        pool_size) {

        return AETHER_ERR_OVERFLOW;
    }

    for (uint8_t i = 0u;
         i < pool_size;
         ++i) {

        passwords[i] =
            (int32_t)i;
    }

    *count =
        pool_size;

    return AETHER_OK;
}


aether_status_t aether_platform_init(
    void *platform_context,
    size_t platform_context_size,
    uint32_t storage_slot,
    aether_client_platform_t *platform) {

    if (platform_context == NULL ||
        platform == NULL ||
        platform_context_size <
            sizeof(aether_esp32_platform_t)) {

        return AETHER_ERR_ARGUMENT;
    }

    aether_esp32_platform_t *ctx =
        platform_context;

    memset(
        ctx,
        0,
        sizeof(*ctx));

    ctx->registration.fd =
        -1;

    ctx->work.fd =
        -1;

    /*
     * NVS is shared with the application and system.
     * Never erase the partition automatically.
     */
    esp_err_t nvs_status =
        nvs_flash_init();

    if (nvs_status != ESP_OK) {
        return AETHER_ERR_STORAGE;
    }

    int key_length =
        snprintf(
            ctx->state_key,
            sizeof(ctx->state_key),
            "state%lu",
            (unsigned long)storage_slot);

    if (key_length <= 0 ||
        (size_t)key_length >=
            sizeof(ctx->state_key)) {

        return AETHER_ERR_ARGUMENT;
    }

    nvs_status =
        nvs_open(
            "aether",
            NVS_READWRITE,
            &ctx->nvs);

    if (nvs_status != ESP_OK) {
        return AETHER_ERR_STORAGE;
    }

    ctx->nvs_open =
        true;

    memset(
        platform,
        0,
        sizeof(*platform));

    platform->ctx =
        ctx;

    platform->transport =
        (aether_transport_vtable_t){
            ctx,
            transport_open,
            transport_close,
            transport_send
        };

    platform->flash =
        (aether_flash_vtable_t){
            ctx,
            storage_load,
            storage_save
        };

    platform->crypto =
        aether_esp32_p256_aes_gcm_crypto();

    platform->crypto_ctx =
        ctx;

    platform->pow =
        (aether_pow_vtable_t){
            ctx,
            pow_generate
        };

    size_t trusted_sign_key_count =
        0u;

    platform->trusted_sign_keys =
        aether_esp32_trusted_sign_keys(
            &trusted_sign_key_count);

    platform->trusted_sign_key_count =
        trusted_sign_key_count;

    platform->resolve =
        platform_resolve;

    platform->now_ms =
        platform_now_ms;

    platform->poll =
        platform_poll;

    platform->receive =
        platform_receive;

    platform->consume =
        platform_consume;

    platform->bind =
        platform_bind;

    return AETHER_OK;
}


void aether_platform_deinit(
    void *platform_context) {

    if (platform_context == NULL) {
        return;
    }

    aether_esp32_platform_t *ctx =
        platform_context;

    close_channel(
        &ctx->registration);

    close_channel(
        &ctx->work);

    ctx->registration.rx_len =
        0u;

    ctx->work.rx_len =
        0u;

    if (ctx->nvs_open) {
        nvs_close(
            ctx->nvs);

        ctx->nvs_open =
            false;
    }

    ctx->client =
        NULL;
}
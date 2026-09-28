# Aether C Client

Minimal, platform-independent Aether client core written in C11.

The project is intended primarily for microcontrollers and other constrained
embedded systems such as ESP32, STM32, nRF52 and similar devices.

The C client follows the behavior and wire protocol of the canonical Java
Aether client, but it deliberately does **not** port the Java runtime
architecture.

The goal is a small deterministic state machine that contains only Aether
business logic and protocol handling.

Platform-specific functionality is supplied by the application through small
callback interfaces.

## Goals

The client is designed to provide the minimum functionality required by an
embedded Aether device:

- register a new Aether client;
- persist registration data in flash;
- restore an already registered client after reboot;
- connect to an Aether work server;
- authenticate using the stored client identity;
- periodically maintain the receive window;
- send messages to another Aether client;
- receive messages from another Aether client;
- operate without threads;
- operate without an OS;
- operate without sockets implemented inside the library;
- operate without mandatory dynamic memory allocation.

The application drives the client explicitly.

There is no background thread and no hidden event loop.

Typical execution is:

```text
application
    |
    | aether_client_start()
    v
Aether FSM
    |
    | transport.open(...)
    v
platform network adapter
    |
    | socket becomes writable
    v
aether_client_on_transport_state(...)
    |
    | received packet
    v
aether_client_on_rx(...)

The application periodically calls:
aether_client_poll(&client, now_ms);

This advances timers such as Aether ping / receive-window maintenance.
## Architecture
The library is divided conceptually into two parts.
### Aether core
The core implements:
- registration state machine;
- work-server login;
- Aether/FastMeta serialization required by the client;
- encryption flow orchestration;
- server selection;
- persistent client state;
- message send/receive logic;
- request tracking;
- ping and receive-window logic.
The core knows nothing about:
- FreeRTOS;
- ESP-IDF;
- POSIX;
- lwIP;
- BSD sockets;
- Wi-Fi;
- Ethernet;
- filesystems;
- NVS;
- hardware RNG;
- a particular crypto library.
Those functions are provided by the platform.
### Platform adapter
A platform integration supplies implementations for:
network transport
crypto
persistent storage
proof of work
time

For example, an ESP32 application can implement the network callbacks using
ESP-IDF/lwIP and the flash callbacks using NVS.
## Memory model
The library currently uses no malloc() or free().
All persistent runtime state belongs to:
aether_client_t

The caller creates the object:
static aether_client_t client;

or places it inside another application-owned structure.
Packet buffers are currently fixed-size arrays whose limits can be changed at
compile time.
For example:
#define AETHER_MAX_PACKET 1024
#define AETHER_MAX_SERVERS 4
#define AETHER_MAX_PENDING 4

before including/building the client.
This makes RAM use deterministic.
The long-term design goal is to keep the core usable with zero dynamic
allocation.
A platform adapter is free to use dynamic memory internally if its operating
system or network stack requires it, but the Aether core itself must not depend
on heap allocation.

With the default compile-time limits, the current host C11 build uses 4352
bytes for aether_client_t. The original three-buffer implementation used
6600 bytes.
The core now keeps two AETHER_MAX_PACKET work buffers. No packet buffer is
allocated dynamically.

## Supported network transports
The embedded C client intentionally supports only:
TCP
UDP

WebSocket and WebSocket Secure are not part of the embedded client target.
They add significant implementation size and dependencies while providing no
benefit for the intended MCU use case.
The Aether server descriptors may contain endpoints using other codecs. The
embedded client should ignore endpoints that it cannot use and select a TCP or
UDP endpoint.
## Transport interface
The Aether core does not create sockets itself.
The application provides a transport implementation through:
typedef struct {
    void *ctx;

    aether_status_t (*open)(
        void *ctx,
        aether_channel_t channel,
        const aether_endpoint_t *endpoint);

    void (*close)(
        void *ctx,
        aether_channel_t channel);

    aether_status_t (*send)(
        void *ctx,
        aether_channel_t channel,
        const uint8_t *data,
        size_t length);
} aether_transport_vtable_t;

It is passed through:
aether_client_config_t config;
config.transport = transport;

The transport is intentionally callback-based so that the same Aether core can
run on:
- ESP-IDF/lwIP;
- bare-metal Ethernet;
- Zephyr;
- FreeRTOS+TCP;
- POSIX;
- a modem;
- a custom radio bridge.
The core never needs to know which implementation is being used.
## Transport channels
There are currently two logical Aether connections:
typedef enum {
    AETHER_CHANNEL_REGISTRATION = 0,
    AETHER_CHANNEL_WORK = 1
} aether_channel_t;

AETHER_CHANNEL_REGISTRATION is used while a new client is registering.
AETHER_CHANNEL_WORK is the normal connection used after registration.
The platform should therefore be prepared to keep transport state separately
for these channels.
A simple implementation may keep two socket descriptors:
typedef struct {
    int registration_socket;
    int work_socket;
} my_transport_t;

On a small system they do not necessarily need to exist simultaneously.
## Opening a socket
The core calls:
transport.open(
    transport.ctx,
    channel,
    endpoint);

The endpoint contains the information required by the platform:
typedef struct {
    aether_codec_t codec;
    aether_address_t address;
    uint16_t port;
} aether_endpoint_t;

For the embedded client the relevant codec values are:
AETHER_CODEC_TCP
AETHER_CODEC_UDP

The public embedded endpoint API supports IPv4 and IPv6 addresses only.
An open() implementation should inspect:
endpoint->codec
endpoint->address
endpoint->port

and create the appropriate platform socket.
open() may be asynchronous.
It is not required to wait until TCP has completed its connection handshake.
For example:
static aether_status_t esp_transport_open(
    void *ctx,
    aether_channel_t channel,
    const aether_endpoint_t *endpoint)
{
    esp_aether_transport_t *transport = ctx;

    switch (endpoint->codec) {
    case AETHER_CODEC_TCP:
        return esp_open_tcp(transport, channel, endpoint);

    case AETHER_CODEC_UDP:
        return esp_open_udp(transport, channel, endpoint);

    default:
        return AETHER_ERR_UNSUPPORTED;
    }
}

The actual esp_open_tcp() / esp_open_udp() functions belong to the ESP32
application, not to aether-client-c.
## Reporting socket state to the core
The platform reports connection availability with:
void aether_client_on_transport_state(
    aether_client_t *client,
    aether_channel_t channel,
    bool writable);

When a TCP connection has completed successfully:
aether_client_on_transport_state(
    &client,
    AETHER_CHANNEL_WORK,
    true);

When it disconnects:
aether_client_on_transport_state(
    &client,
    AETHER_CHANNEL_WORK,
    false);

For UDP there is no TCP-style connected session.
The adapter should report the channel writable when the UDP socket is
configured and capable of transmitting to the selected endpoint.
For example:
socket(..., SOCK_DGRAM, ...);
connect(...);

aether_client_on_transport_state(
    &client,
    channel,
    true);

Using connect() on a UDP socket here does not create a TCP-like connection;
it simply binds the socket to a default remote address and is convenient for
embedded implementations.
## Sending data
The core calls:
transport.send(
    transport.ctx,
    channel,
    data,
    length);

Important ownership rule:
send() must consume or copy the supplied bytes before returning.

The memory pointed to by data belongs to the Aether client and will be reused
immediately after send() returns.
Therefore this is valid:
send(fd, data, length, 0);
return AETHER_OK;

This is also valid:
memcpy(driver_tx_buffer, data, length);
queue_driver_tx(...);
return AETHER_OK;

This is not valid:
saved_pointer = data;
return AETHER_OK;

unless the adapter copies the data before the Aether core reuses the buffer.
## Receiving data
Receive is intentionally not represented as a blocking recv() callback.
Network stacks differ greatly, and embedded applications often already have an
event loop or network task.
Instead, the platform pushes received Aether packets into the core:
aether_status_t aether_client_on_rx(
    aether_client_t *client,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length);

Example:
void on_udp_datagram(
    const uint8_t *data,
    size_t length)
{
    aether_client_on_rx(
        &client,
        AETHER_CHANNEL_WORK,
        data,
        length);
}

This design works equally well with:
- an ISR-to-task queue;
- an ESP-IDF socket task;
- select();
- poll();
- lwIP callbacks;
- a custom cooperative event loop.
The Aether core itself never blocks waiting for network data.
## UDP
UDP is the simplest transport for an MCU.
A received UDP datagram already provides packet boundaries.
Conceptually:
UDP datagram
    |
    v
aether_client_on_rx(...)

The UDP adapter therefore only needs to:
1. create a UDP socket;
2. associate it with the selected server endpoint;
3. send each outgoing Aether packet as one datagram;
4. pass each incoming datagram to aether_client_on_rx().
No stream reassembly is required.
This is generally the smallest network implementation for ESP32.
## TCP
TCP does not preserve packet boundaries.
A single recv() can contain:
half of one Aether packet

or:
packet A + packet B + part of packet C

Therefore the TCP platform adapter must implement the transport framing used by
the Aether TCP transport.
The responsibility boundary is:
TCP byte stream
        |
        v
platform TCP framing / reassembly
        |
        | one complete Aether transport packet
        v
aether_client_on_rx(...)

Similarly, outgoing data supplied through transport.send() must be wrapped
with the TCP transport framing required by the Aether server before being sent
to the TCP socket.
Transport framing belongs outside the business-logic FSM because UDP does not
need it and because different platforms may implement stream buffering very
differently.
The application should not pass arbitrary TCP recv() chunks directly to
aether_client_on_rx().
## Suggested ESP32 transport structure
A practical ESP-IDF integration can look like this:
typedef struct {
    aether_client_t *client;

    int registration_fd;
    int work_fd;

    /* TCP receive framing buffers, if TCP is used. */
    uint8_t registration_rx_buffer[...];
    size_t registration_rx_length;

    uint8_t work_rx_buffer[...];
    size_t work_rx_length;
} esp_aether_transport_t;

Callbacks:
static aether_status_t esp_transport_open(
    void *ctx,
    aether_channel_t channel,
    const aether_endpoint_t *endpoint);

static void esp_transport_close(
    void *ctx,
    aether_channel_t channel);

static aether_status_t esp_transport_send(
    void *ctx,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length);

Configuration:
static esp_aether_transport_t transport_ctx;

static aether_client_t client;

void app_main(void)
{
    aether_client_config_t config = {0};

    transport_ctx.client = &client;

    config.transport.ctx = &transport_ctx;
    config.transport.open = esp_transport_open;
    config.transport.close = esp_transport_close;
    config.transport.send = esp_transport_send;

    /*
     * Configure crypto, flash, registration endpoint,
     * parent UID and event callback here.
     */

    aether_client_init(&client, &config);
    aether_client_start(&client);
}

The ESP-IDF socket task then feeds network events back to the core.
Connection:
aether_client_on_transport_state(
    transport->client,
    channel,
    true);

Disconnect:
aether_client_on_transport_state(
    transport->client,
    channel,
    false);

Incoming complete packet:
aether_client_on_rx(
    transport->client,
    channel,
    packet,
    packet_length);

This is the entire dependency between the Aether business logic and ESP-IDF.
Why the core does not expose recv()
A conventional desktop library might define:
socket_open()
socket_send()
socket_recv()
socket_close()

That is deliberately avoided here.
A blocking recv() would force assumptions about:
- threads;
- blocking behavior;
- timeouts;
- scheduler availability;
- socket ownership;
- event-loop architecture.
Those assumptions are undesirable on a microcontroller.
The C client instead uses an event-driven boundary:
Core -> open
Core -> send
Core -> close

Platform -> connection state
Platform -> received packet

That makes the Aether FSM usable in both bare-metal and RTOS systems.
## Flash interface
Registration state must survive reboot.
The core therefore exposes a small persistent-storage abstraction:
typedef struct {
    void *ctx;

    aether_status_t (*load)(
        void *ctx,
        uint8_t *dst,
        size_t capacity,
        size_t *length);

    aether_status_t (*save)(
        void *ctx,
        const uint8_t *src,
        size_t length);
} aether_flash_vtable_t;

On ESP32 this can be implemented using:
- NVS;
- a dedicated flash partition;
- wear-levelled storage.
The core serializes its own state.
The platform only stores and restores opaque bytes.
Example conceptual NVS adapter:
static aether_status_t esp_flash_save(
    void *ctx,
    const uint8_t *src,
    size_t length)
{
    /*
     * nvs_set_blob(...)
     * nvs_commit(...)
     */
    return AETHER_OK;
}

The platform should treat this blob as private Aether client state.
## Crypto interface
Cryptography is also supplied externally.
The core defines the sequence of cryptographic operations required by the
Aether protocol but does not force a particular crypto implementation.
The platform provides callbacks for:
- random symmetric-key generation;
- signed-key verification;
- asymmetric encryption;
- symmetric encryption;
- symmetric decryption;
- per-server key derivation;
- registration proof of work.
This allows different MCU builds to use the implementation most appropriate for
the device.
For example:
ESP32 hardware RNG
        +
libhydrogen / libsodium / hardware accelerator
        |
        v
aether_crypto_vtable_t

The business-logic state machine does not change.
## Application events
The application receives client events through:
typedef void (*aether_event_callback_t)(
    void *ctx,
    const aether_event_t *event);

Current events include:
state changed
registration completed
message received
request completed
error

Example:
static void on_aether_event(
    void *ctx,
    const aether_event_t *event)
{
    switch (event->type) {
    case AETHER_EVENT_MESSAGE:
        handle_message(
            event->as.message.from,
            event->as.message.payload.data,
            event->as.message.payload.length);
        break;

    case AETHER_EVENT_ERROR:
        handle_error(event->as.error.code);
        break;

    default:
        break;
    }
}

Message payload memory is owned by the core and is valid only during the
callback.
Copy it if the application needs to retain the message.

Callback reentrancy
The Aether core is intentionally non-reentrant.
on_event may inspect the event, read the message payload and copy data into
application-owned memory, but it must not recursively drive the Aether state
machine before the callback returns.
Do not call these functions directly from on_event:
aether_client_start(...);
aether_client_stop(...);
aether_client_poll(...);
aether_client_on_transport_state(...);
aether_client_on_rx(...);
aether_client_send_message(...);

Status-returning functions reject recursive entry with AETHER_ERR_BUSY.
Void lifecycle/event-entry functions ignore recursive entry.
On ESP32, the recommended pattern is to enqueue application work from
on_event and process that work after the callback returns, for example from
the normal FreeRTOS task or application event loop.
This restriction lets receive decryption and transmit crypto share one scratch
buffer and removes one full AETHER_MAX_PACKET buffer from the client object.

## Basic lifecycle
A typical application performs:
static aether_client_t client;

int main(void)
{
    aether_client_config_t config = {0};

    /*
     * Fill:
     *
     * config.transport
     * config.flash
     * config.crypto
     * config.registration_endpoint
     * config.parent_uid
     * config.on_event
     */

    aether_client_init(&client, &config);

    if (aether_client_start(&client) != AETHER_OK) {
        /* handle startup error */
    }

    for (;;) {
        uint64_t now_ms = platform_time_ms();

        platform_network_poll();
        aether_client_poll(&client, now_ms);
    }
}

If persistent registration state exists, the client proceeds directly toward a
work server.
Otherwise it automatically enters the registration state machine.
## Design constraints
The embedded client intentionally avoids features that increase complexity
without helping the target devices.
Not goals:
- Java-compatible object model;
- TypeScript-compatible class hierarchy;
- general-purpose FastMeta runtime;
- generic ADSL runtime;
- WebSocket;
- WSS/TLS web stack;
- dynamic plugin loading;
- filesystem dependency;
- mandatory threads;
- mandatory heap.
The implementation should remain a small, explicit C state machine.
The Java client remains the canonical behavioral reference and the generated
Java APIs/ADSL definitions remain the canonical wire-format reference.
## Current status
The first vertical slice implements:
registration
    ->
persistent client state
    ->
work connection
    ->
login by alias
    ->
ping / receive window
    ->
send message
    ->
receive message


The project builds as C11 and has a host-side regression test covering the
basic state-machine flow.

With the current default limits the host build reports:

```text
aether_address_t   24 bytes
aether_endpoint_t  32 bytes
aether_server_t    36 bytes
aether_client_t  4352 bytes

The first implementation used 6600 bytes for aether_client_t. The current
two-packet-buffer design saves 2248 bytes, about 34%, while keeping memory
usage deterministic and heap-free.
Current work continues in two directions:
1. reduce static RAM further where it does not complicate the MCU API;
2. verify crypto and transport interoperability byte-for-byte against the
   canonical Java implementation and real Aether server stack.

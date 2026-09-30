
# Aether C Client

Small, allocation-free Aether client for C11 and constrained embedded systems.

The library implements the Aether protocol as an explicit state machine. Networking,
persistent storage, cryptography, proof of work, DNS, and the monotonic clock are
provided by a platform integration rather than being hard-wired into the protocol
core.

The normal application API is intentionally small:

```c
#include "aether.h"

static aether_t client;

static void on_message(
    aether_t *client,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    (void)client;
    (void)from;

    /* data is borrowed; copy it if it must outlive the callback. */
}

int main(void) {
    aether_uuid_t parent = {0};

    if (aether_init(&client, parent) != AETHER_OK) {
        return 1;
    }

    aether_on_message(&client, on_message);

    if (aether_start(&client) != AETHER_OK) {
        return 1;
    }

    for (;;) {
        if (aether_poll(&client) != AETHER_OK) {
            /* Application-specific error handling. */
        }

        /* Application work. */
    }
}

A ready platform integration supplies the physical mechanisms behind this API.
Design goals
The C client is designed for devices where RAM, flash, and runtime dependencies
matter.
The core aims to provide:
- no mandatory heap allocation;
- deterministic caller-owned state;
- no background thread;
- no hidden event loop;
- no socket implementation inside the protocol core;
- explicit platform ownership of physical I/O;
- TCP and UDP transport support;
- persistent registration and reconnect after reboot;
- Aether registration and work-server authentication;
- message send and receive;
- compact generated protocol bindings;
- wire compatibility with the canonical Aether ADSL definitions.
The library deliberately does not reproduce the Java object model or runtime
architecture. C code is organized around bounded storage, explicit state, and
small platform contracts.
API layers
There are three useful API levels.
aether.h
This is the normal application-facing API.
Use it when a ready platform port exists for the target.
The application deals with concepts such as:
- Aether client identity;
- parent UID;
- message callbacks;
- ready/error callbacks;
- polling;
- sending;
- optional logical storage slots.
The application does not construct transport, crypto, DNS, proof-of-work, or
clock vtables.
Typical functions include:
aether_init(...)
aether_init_slot(...)
aether_on_message(...)
aether_on_ready(...)
aether_on_error(...)
aether_start(...)
aether_poll(...)
aether_send(...)
aether_stop(...)
aether_uid(...)
aether_is_ready(...)

aether_init() uses the default logical Aether identity slot.
aether_init_slot() exists for applications that intentionally maintain more
than one independent Aether identity.
aether_client_app.h
This is the advanced integration layer.
It exposes the complete platform mechanism bundle and the explicit zero-copy
pull-RX boundary. It is useful for:
- custom operating systems;
- custom network adapters;
- host test environments;
- unusual storage implementations;
- advanced scheduling;
- applications that want direct control over ingress policy.
The application remains the scheduler. aether_client_app_tick() performs one
bounded step and can expose at most one application ingress item.
aether_client.h
This is the low-level protocol core.
It contains the registration/work state machine and the protocol-facing
transport, storage, crypto, and proof-of-work contracts.
Most applications should not need to use it directly.
Platform boundary
Normal application code includes aether.h.
A target port implements the internal contract declared in
include/aether_platform.h:
aether_platform_init(...)
aether_platform_deinit(...)

A platform port owns mechanisms such as:
- TCP/UDP sockets and framing;
- DNS resolution;
- monotonic time;
- network pumping or event integration;
- persistent Aether identity storage;
- cryptographic backend;
- proof of work;
- trusted signing roots.
The protocol core remains independent of ESP-IDF, FreeRTOS, POSIX, lwIP, NVS,
or any particular crypto library.
Memory model
The protocol runtime is designed to operate without malloc()/free().
Persistent runtime state belongs to caller-owned objects.
The low-level client owns bounded packet scratch buffers inside
aether_client_t. Optional receive policies and send futures are separate
caller-owned components, so unused optional mechanisms do not consume RAM inside
the core client object.
Platform implementations may use their operating system's facilities internally,
but the Aether protocol core must not require heap allocation.
Receive model
The advanced API uses a zero-copy pull model.
The platform accumulates physical network input in platform-owned storage.
receive() exposes one complete borrowed transport frame to Aether.
The frame remains owned by the platform until Aether calls the matching
consume() operation.
Protocol parsing and crypto therefore run from the normal application scheduling
path, not from an ISR or raw socket callback.
The low-level parser can stop after yielding one application-visible ingress
item. The caller processes that item, consumes it, and then allows parsing to
continue.
This keeps application scheduling explicit and avoids a hidden message queue.
Receive policies
include/aether_rx.h contains small caller-owned policies that operate on
already-decoded ingress:
- aether_rx_first_t — deliver the first message after init/reset;
- aether_rx_all_t — deliver every message;
- aether_rx_adaptive_t — application-controlled switch between FIRST and ALL.
These policies:
- do not parse wire data;
- do not allocate memory;
- do not own transport buffers;
- do not run a scheduler.
Send futures
include/aether_send_future.h provides an optional caller-owned completion
tracker for requests.
A future is independent of the core client object. If the application does not
need request completion tracking, it pays no RAM cost for it.
The future can:
- track an already transmitted request ID;
- send and arm itself;
- accept a matching request-result ingress;
- enforce a caller-selected local timeout.
Registration
If valid persistent registration state is available, the client restores the
identity and proceeds toward a work server.
Otherwise it automatically enters the registration state machine.
Registration is intentionally split into protocol/trust stages rather than
implemented as one opaque function. The production flow includes global/root
bootstrap, safe registration operations, direct registration, response
decoding, server resolution, and transition to the authorized work connection.
Protocol generation
The repository contains a C compiler for Aether ADSL:
tools/aether-protocol-c/

Its job is to generate C protocol bindings from the same ADSL source of truth
used by the rest of Aether.
Important generator code:
tools/aether-protocol-c/include/adslc.h
tools/aether-protocol-c/src/adsl.c
tools/aether-protocol-c/src/emitter.c
tools/aether-protocol-c/src/main.c
tools/aether-protocol-c/tests/




Production ADSL compiler output is stored under `src/generated/`.

The same directory currently also contains maintained `aether_reg_*`
registration adapter/build-helper sources. Those files consume generated
bindings but are not direct output of the current ADSL emitter.

Generated `client_server_*` protocol files should not be manually patched to fix
generator behavior. Fix the parser/emitter, add a focused generator regression
test, and regenerate the production output.

The generator is a development tool. It is not part of the MCU runtime.


Shared wire runtime
Generated bindings use a small handwritten wire runtime:
include/aether_meta_runtime.h
src/aether_meta_runtime.c

It provides bounded serialization/deserialization primitives for:
- fixed-width integers;
- Aether packed integers;
- byte/string views;
- UUIDs.
No reflection or runtime schema registry is required.
Writers are shared out-of-line functions because production send paths reuse
them heavily.
Reader primitives are handwritten static inline functions so the compiler can
inline the single-use response-decoding path where that produces smaller
firmware.
Building
A normal host build uses CMake:
cmake -S . -B build
cmake --build build

For the host regression build used during development:
cmake --build build-test -j2
ctest --test-dir build-test --output-on-failure

Generator tests
Configure/build the generator test tree and run:
cmake --build build-adsl-generator-release -j2
ctest --test-dir build-adsl-generator-release --output-on-failure

The generator tests include exact wire-format fixtures for primitives, structs,
arrays, nullable fields, enums, inheritance/hierarchy types, streams, intpack,
local dispatch, result RPCs, and generated response methods.
Integration test
The strongest registration/wire compatibility test is the Java↔C public P2P
test in the main Aether repository:
io.aether.cloudClient.CClientP2PTest.twoCClientsRegisterAndExchangeMessages

It registers C clients through the real server path and verifies message
exchange.
ESP32 footprint harness
esp32-size/ contains a PlatformIO/ESP-IDF harness for measuring the linked MCU
footprint.
The authoritative environment is:
cd esp32-size
pio run -e esp32-port

When evaluating footprint, use the linked ELF and final firmware image.
Do not treat:
- generated source line count;
- static-library size;
- individual .o size;
as the actual firmware cost.
The build uses function/data sections and linker garbage collection so unused
generated protocol functions are discarded.
Repository layout
include/
    aether.h                 normal application API
    aether_client_app.h      advanced application/platform API
    aether_client.h          low-level protocol core
    aether_platform.h        internal target-port boundary
    aether_rx.h              optional receive policies
    aether_send_future.h     optional send completion tracker
    aether_meta_runtime.h    shared wire primitives

src/
    aether.c                 normal application facade
    aether_client_app.c      advanced application facade
    aether_client.c          low-level protocol state machine
    aether_meta_runtime.c    shared wire writer runtime
    aether_rx_*.c            receive policies
    aether_send_future.c     optional completion tracking
    generated/               production protocol bindings/adapters

tools/aether-protocol-c/
    C ADSL compiler and tests

esp32-size/
    ESP32 linked-footprint harness

tests/
    host/client tests

Contributor rules
When changing protocol or generator code:
1. preserve the no-mandatory-heap runtime;
2. preserve wire compatibility;
3. do not hand-edit generated bindings as the primary fix;
4. add focused generator tests for generator behavior;
5. run the host tests;
6. run generator tests;
7. run the Java↔C P2P test after registration/wire changes;
8. measure linked ESP32 firmware after size-sensitive changes;
9. evaluate linked size rather than object/source size;
10. keep platform-specific mechanisms out of the protocol core.
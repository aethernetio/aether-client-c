
# Aether Thermometer

This is a minimal real-board ESP32 example for the Aether C client.

The first milestone intentionally does not use a temperature sensor yet.
First we verify the complete board and network path:

1. ESP-IDF boots.
2. NVS initializes.
3. The board connects to Wi-Fi.
4. The Aether ESP32 platform package initializes.
5. Aether restores or registers its persistent identity.
6. The client establishes its Aether connection.
7. The application continuously calls aether_poll.
8. READY, messages and errors are printed to the serial console.

After this works on the physical board we can add the actual temperature
sensor and application message format.

Wi-Fi credentials
-----------------

Copy:

    include/thermometer_secrets.example.h

to:

    include/thermometer_secrets.h

and enter the Wi-Fi credentials.

The real secrets file is ignored by Git.


Peer UUID
---------

The UID of the peer this board talks to is supplied at build time in canonical
UUID form, not hand-written as bytes:

    THERMOMETER_PEER_UUID=01020304-0506-0708-1112-131415161718 pio run -e esp32

CMake parses it at configure time and the firmware uses the generated
THERMOMETER_PEER_UID symbol directly; nothing UUID-related is parsed on the
device. When the variable is not set, the peer UUID is all zeros. See the main
README section "Compile-time UUID and secrets" for details.


PlatformIO environments
-----------------------

The exact physical board has not been identified yet.

Initial environments:

    esp32
    esp32s3
    esp32c3

Do not select the target merely from the fact that the connector is Type-C.
We will select the real environment after identifying the chip.

Build
-----

Compile-only validation for a generic classic ESP32:

    pio run -e esp32

After the actual board is identified:

    pio run -e ENVIRONMENT -t upload

Serial monitor:

    pio device monitor -b 115200
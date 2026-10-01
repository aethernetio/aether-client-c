
#ifndef THERMOMETER_SECRETS_EXAMPLE_H
#define THERMOMETER_SECRETS_EXAMPLE_H

/*
 * Copy this file to thermometer_secrets.h and enter the credentials
 * of the Wi-Fi network that the physical board should use.
 *
 * thermometer_secrets.h is ignored by Git.
 */

#define THERMOMETER_WIFI_SSID "your-wifi-name"
#define THERMOMETER_WIFI_PASSWORD "your-wifi-password"



/*
 * The peer UUID is NOT defined here.
 *
 * Provide it at build time in canonical form, e.g.
 *
 *     THERMOMETER_PEER_UUID=01020304-0506-0708-1112-131415161718 pio run -e esp32
 *
 * CMake parses it at configure time and passes the two uint64 halves to the
 * firmware, so the device never parses UUID text at runtime.
 * If unset, the peer UUID is all zeros.
 */




#endif
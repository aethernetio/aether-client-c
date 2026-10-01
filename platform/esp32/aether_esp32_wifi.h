
#ifndef AETHER_ESP32_WIFI_H
#define AETHER_ESP32_WIFI_H

#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Optional ESP32 Wi-Fi convenience layer.
 *
 * These helpers hide the standard ESP-IDF Wi-Fi boilerplate
 * (esp_netif, default event loop, esp_wifi_init/config/start and, for the
 * station helper, waiting for an IPv4 address).
 *
 * This is a platform utility, not part of the generic aether.h API. It is
 * deliberately NOT tied to aether_start(): a device may already own its
 * network lifecycle (Ethernet, PPP, a custom Wi-Fi task, or a station that is
 * already connected). Calling these helpers is an application policy
 * decision.
 *
 * Only one station/AP bring-up is supported per process because the helpers
 * keep the small amount of ESP-IDF bookkeeping they need in static state.
 */

#define AETHER_ESP32_WIFI_WAIT_FOREVER UINT32_MAX

/*
 * Connect to an existing access point in station mode and wait until an IPv4
 * address is acquired.
 *
 * timeout_ms is the maximum time to wait for a connection; use
 * AETHER_ESP32_WIFI_WAIT_FOREVER to wait indefinitely. The helper
 * automatically reconnects on transient disconnects while it is waiting.
 *
 * Returns ESP_OK on success, ESP_ERR_INVALID_ARG on bad input,
 * ESP_ERR_TIMEOUT if no IPv4 address arrives in time, or an ESP-IDF error.
 */
esp_err_t aether_esp32_wifi_connect(
    const char *ssid,
    const char *password,
    uint32_t timeout_ms);

/*
 * Start a SoftAP.
 *
 * password may be NULL or empty to start an open network; otherwise WPA2-PSK
 * is used. channel may be 0 to use the default channel; max_connections may be
 * 0 to use the default connection limit.
 *
 * Returns ESP_OK on success, ESP_ERR_INVALID_ARG on bad input, or an ESP-IDF
 * error.
 */
esp_err_t aether_esp32_wifi_start_ap(
    const char *ssid,
    const char *password,
    uint8_t channel,
    uint8_t max_connections);

#ifdef __cplusplus
}
#endif

#endif

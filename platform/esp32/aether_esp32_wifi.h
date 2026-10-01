
#ifndef AETHER_ESP32_WIFI_H
#define AETHER_ESP32_WIFI_H

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Optional ESP32 Wi-Fi convenience layer.
 *
 * These helpers hide the standard ESP-IDF Wi-Fi boilerplate (esp_netif, the
 * default event loop, esp_wifi_init/config/start and the STA reconnect
 * policy). They are a platform utility, not part of the generic aether.h API.
 *
 * They are deliberately NOT tied to aether_start()/aether_poll(): a device may
 * already own its network lifecycle (Ethernet, PPP, a custom Wi-Fi task, or a
 * station that is already connected). Calling these helpers is an application
 * policy decision.
 *
 * A single process-wide STA/AP bring-up is supported because the helpers keep
 * their ESP-IDF bookkeeping in static state. Transitions STA -> AP -> STA are
 * supported.
 */

/*
 * Ensure the station is (or becomes) connected to the given access point.
 *
 * This is an idempotent, NON-BLOCKING "ensure connected" call. Typical usage
 * is to invoke it once per application loop iteration:
 *
 *     for (;;) {
 *         aether_esp32_wifi_connect(WIFI_SSID, WIFI_PASSWORD);
 *         aether_poll(&client);
 *         vTaskDelay(...);
 *     }
 *
 * First call: initializes ESP-IDF Wi-Fi as needed, creates the STA netif once,
 * applies ssid/password, starts Wi-Fi and calls esp_wifi_connect().
 *
 * Later calls: do NOT recreate netif/event group/handlers. If an IPv4 address
 * is already held, returns ESP_OK immediately. If a connection is still in
 * progress, returns ESP_ERR_WIFI_NOT_CONNECT. If the connection was lost,
 * reconnect continues to be driven by the event handler (and this call nudges
 * it if needed).
 *
 * Connection recovery is automatic via the WIFI_EVENT_STA_DISCONNECTED
 * handler while a STA connection is desired; it does not depend on this call.
 *
 * Returns ESP_OK when an IPv4 address is currently held,
 * ESP_ERR_WIFI_NOT_CONNECT when not connected yet,
 * ESP_ERR_INVALID_ARG on bad input, or an ESP-IDF error.
 */
esp_err_t aether_esp32_wifi_connect(
    const char *ssid,
    const char *password);

/*
 * Blocking variant of the ensure-connected call: same idempotent setup, but
 * waits until an IPv4 address is held or timeout_ms elapses.
 *
 * timeout_ms == AETHER_ESP32_WIFI_WAIT_FOREVER waits indefinitely.
 */
#define AETHER_ESP32_WIFI_WAIT_FOREVER UINT32_MAX

esp_err_t aether_esp32_wifi_connect_wait(
    const char *ssid,
    const char *password,
    uint32_t timeout_ms);

/*
 * Return whether a working station IPv4 address is currently held.
 * This has no side effects and never blocks.
 */
bool aether_esp32_wifi_connected(void);

/*
 * Start a SoftAP.
 *
 * password may be NULL or empty to start an open network; otherwise WPA2-PSK
 * is used. channel may be 0 to use the default channel; max_connections may be
 * 0 to use the default connection limit.
 *
 * While the AP is active, the STA reconnect policy is suspended so a stale STA
 * disconnect does not fight the AP mode. A later aether_esp32_wifi_connect()
 * switches back to STA and reconnects explicitly.
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

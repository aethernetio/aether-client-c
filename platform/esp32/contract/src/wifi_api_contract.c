
/*
 * Compile-only API contract for the ESP32 Wi-Fi helper.
 *
 * This translation unit only checks that the public helper signatures exist
 * and match their declared types. It deliberately does NOT call, run or fake
 * any runtime Wi-Fi behavior (which would require real hardware). If the
 * aether_esp32_wifi.h declarations drift, this file stops compiling.
 */

#include "aether_esp32_wifi.h"

static esp_err_t (*const contract_connect)(
    const char *,
    const char *) = aether_esp32_wifi_connect;

static esp_err_t (*const contract_connect_wait)(
    const char *,
    const char *,
    uint32_t) = aether_esp32_wifi_connect_wait;

static esp_err_t (*const contract_start_ap)(
    const char *,
    const char *,
    uint8_t,
    uint8_t) = aether_esp32_wifi_start_ap;

static bool (*const contract_connected)(void) =
    aether_esp32_wifi_connected;

void aether_esp32_wifi_api_contract(void) {
    (void)contract_connect;
    (void)contract_connect_wait;
    (void)contract_start_ap;
    (void)contract_connected;
}

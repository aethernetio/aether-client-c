
/*
 * Compile-level API contract for the ESP32 Wi-Fi helper.
 *
 * This translation unit intentionally only checks public signatures. It does
 * NOT exercise or fake runtime Wi-Fi behavior, which requires real hardware.
 * If the declared helper signatures drift, this file stops compiling.
 */

#include "aether_esp32_wifi.h"

static esp_err_t (*const contract_connect)(
    const char *,
    const char *,
    uint32_t) = aether_esp32_wifi_connect;

static esp_err_t (*const contract_start_ap)(
    const char *,
    const char *,
    uint8_t,
    uint8_t) = aether_esp32_wifi_start_ap;

void aether_esp32_wifi_api_contract(void) {
    (void)contract_connect;
    (void)contract_start_ap;
}

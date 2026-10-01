
/*
 * Compile-only ESP-IDF application for the Wi-Fi helper API contract.
 * It never runs on device; its sole purpose is to force the compiler to
 * validate the public helper signatures.
 */

void aether_esp32_wifi_api_contract(void);

void app_main(void) {
    aether_esp32_wifi_api_contract();
}

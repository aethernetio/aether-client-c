
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"

#include "aether.h"
#include "aether_messages.h"

#include "aether_esp32_wifi.h"
#include "thermometer_secrets.h"
#include "aether_uuid_THERMOMETER_PEER_UID.h"



static aether_t client;
static aether_peer_t peer;


static void on_peer_message(
    aether_peer_t *peer,
    const uint8_t *data,
    size_t size,
    void *ctx
) {
    /* handle an incoming message here */
    (void)peer;
    (void)data;
    (void)size;
    (void)ctx;
}



void app_main(void) {
    if (nvs_flash_init() != ESP_OK) {
        return;
    }

    if (aether_init(&client, (aether_uuid_t){0u, 0u}) != AETHER_OK) {
        return;
    }

    if (aether_peer_init(&peer, &client, THERMOMETER_PEER_UID) != AETHER_OK) {
        return;
    }

    aether_peer_on_message(&peer, on_peer_message, NULL);

    if (aether_start(&client) != AETHER_OK) {
        return;
    }

    for (;;) {
        aether_esp32_wifi_connect(
            THERMOMETER_WIFI_SSID,
            THERMOMETER_WIFI_PASSWORD
        );

        aether_poll(&client);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

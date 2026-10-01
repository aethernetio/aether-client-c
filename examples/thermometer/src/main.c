
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "aether.h"
#include "aether_messages.h"
#include "aether_esp32_wifi.h"
#include "thermometer_secrets.h"


static const char *TAG = "thermometer";

static aether_t client;
static aether_peer_t peer;


static void on_peer_message(
    aether_peer_t *peer,
    const uint8_t *data,
    size_t size,
    void *ctx
) {
    (void)data;
    (void)ctx;

    aether_uuid_t from =
        aether_peer_uid(
            peer
        );

    ESP_LOGI(
        TAG,
        "Aether message from=%016" PRIx64 "%016" PRIx64
        " bytes=%u",
        from.msb,
        from.lsb,
        (unsigned)size
    );
}


static aether_status_t initialize_aether(void) {
    aether_status_t status =
        aether_init(
            &client,
            (aether_uuid_t){0u, 0u}
        );

    if (status != AETHER_OK) {
        return status;
    }

    aether_uuid_t peer_uid = {
        THERMOMETER_PEER_UID_MSB,
        THERMOMETER_PEER_UID_LSB
    };

    if (peer_uid.msb == 0u &&
        peer_uid.lsb == 0u) {

        ESP_LOGE(
            TAG,
            "THERMOMETER_PEER_UID is not configured"
        );

        return AETHER_ERR_ARGUMENT;
    }

    status =
        aether_peer_init(
            &peer,
            &client,
            peer_uid
        );

    if (status != AETHER_OK) {
        return status;
    }

    aether_peer_on_message(
        &peer,
        on_peer_message,
        NULL
    );

    return aether_start(
        &client
    );
}


void app_main(void) {
    ESP_LOGI(
        TAG,
        "Aether thermometer boot"
    );

    esp_err_t nvs_status =
        nvs_flash_init();

    if (nvs_status != ESP_OK) {
        ESP_LOGE(
            TAG,
            "NVS initialization failed: %s",
            esp_err_to_name(nvs_status)
        );

        return;
    }

    ESP_LOGI(
        TAG,
        "Connecting to Wi-Fi"
    );

    esp_err_t wifi_status =
        aether_esp32_wifi_connect(
            THERMOMETER_WIFI_SSID,
            THERMOMETER_WIFI_PASSWORD,
            AETHER_ESP32_WIFI_WAIT_FOREVER
        );

    if (wifi_status != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Wi-Fi connection failed: %s",
            esp_err_to_name(wifi_status)
        );

        return;
    }

    ESP_LOGI(
        TAG,
        "Starting Aether client"
    );

    aether_status_t status =
        initialize_aether();

    if (status != AETHER_OK) {
        ESP_LOGE(
            TAG,
            "Aether start failed: %d",
            (int)status
        );

        return;
    }

    uint32_t status_ticks = 0u;

    for (;;) {
        status =
            aether_poll(
                &client
            );

        if (status != AETHER_OK) {
            ESP_LOGW(
                TAG,
                "aether_poll returned %d",
                (int)status
            );
        }

        ++status_ticks;

        if (status_ticks >= 500u) {
            status_ticks = 0u;

            ESP_LOGI(
                TAG,
                "Aether state=%d registered=%d ready=%d",
                (int)aether_state(&client),
                aether_is_registered(&client) ? 1 : 0,
                aether_is_ready(&client) ? 1 : 0
            );
        }

        vTaskDelay(
            pdMS_TO_TICKS(10)
        );
    }
}

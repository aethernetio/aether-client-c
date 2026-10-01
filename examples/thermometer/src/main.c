
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "aether.h"
#include "aether_messages.h"
#include "thermometer_secrets.h"


static const char *TAG = "thermometer";

#define WIFI_CONNECTED_BIT BIT0

static EventGroupHandle_t wifi_events;
static aether_t client;
static aether_peer_t peer;


static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
) {
    (void)arg;
    (void)event_data;

    if (event_base == WIFI_EVENT
            && event_id == WIFI_EVENT_STA_START) {

        ESP_LOGI(
            TAG,
            "Wi-Fi station started"
        );

        esp_err_t status =
            esp_wifi_connect();

        if (status != ESP_OK) {
            ESP_LOGE(
                TAG,
                "esp_wifi_connect failed: %s",
                esp_err_to_name(status)
            );
        }

        return;
    }

    if (event_base == WIFI_EVENT
            && event_id == WIFI_EVENT_STA_DISCONNECTED) {

        xEventGroupClearBits(
            wifi_events,
            WIFI_CONNECTED_BIT
        );

        ESP_LOGW(
            TAG,
            "Wi-Fi disconnected, reconnecting"
        );

        esp_err_t status =
            esp_wifi_connect();

        if (status != ESP_OK) {
            ESP_LOGE(
                TAG,
                "Wi-Fi reconnect failed: %s",
                esp_err_to_name(status)
            );
        }

        return;
    }

    if (event_base == IP_EVENT
            && event_id == IP_EVENT_STA_GOT_IP) {

        xEventGroupSetBits(
            wifi_events,
            WIFI_CONNECTED_BIT
        );

        ESP_LOGI(
            TAG,
            "Wi-Fi connected, IPv4 address acquired"
        );
    }
}


static esp_err_t initialize_wifi(void) {
    if (THERMOMETER_WIFI_SSID[0] == '\0') {
        ESP_LOGE(
            TAG,
            "Wi-Fi SSID is empty. Edit "
            "include/thermometer_secrets.h before flashing."
        );

        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t status =
        esp_netif_init();

    if (status != ESP_OK) {
        return status;
    }

    status =
        esp_event_loop_create_default();

    if (status != ESP_OK
            && status != ESP_ERR_INVALID_STATE) {

        return status;
    }

    esp_netif_t *station =
        esp_netif_create_default_wifi_sta();

    if (station == NULL) {
        return ESP_FAIL;
    }

    wifi_init_config_t init =
        WIFI_INIT_CONFIG_DEFAULT();

    status =
        esp_wifi_init(
            &init
        );

    if (status != ESP_OK) {
        return status;
    }

    status =
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            wifi_event_handler,
            NULL
        );

    if (status != ESP_OK) {
        return status;
    }

    status =
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            wifi_event_handler,
            NULL
        );

    if (status != ESP_OK) {
        return status;
    }

    wifi_config_t config = {0};

    int written =
        snprintf(
            (char *)config.sta.ssid,
            sizeof(config.sta.ssid),
            "%s",
            THERMOMETER_WIFI_SSID
        );

    if (written < 0
            || (size_t)written >=
                sizeof(config.sta.ssid)) {

        return ESP_ERR_INVALID_ARG;
    }

    written =
        snprintf(
            (char *)config.sta.password,
            sizeof(config.sta.password),
            "%s",
            THERMOMETER_WIFI_PASSWORD
        );

    if (written < 0
            || (size_t)written >=
                sizeof(config.sta.password)) {

        return ESP_ERR_INVALID_ARG;
    }

    status =
        esp_wifi_set_mode(
            WIFI_MODE_STA
        );

    if (status != ESP_OK) {
        return status;
    }

    status =
        esp_wifi_set_config(
            WIFI_IF_STA,
            &config
        );

    if (status != ESP_OK) {
        return status;
    }

    return esp_wifi_start();
}





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

    wifi_events =
        xEventGroupCreate();

    if (wifi_events == NULL) {
        ESP_LOGE(
            TAG,
            "Cannot create Wi-Fi event group"
        );

        return;
    }

    esp_err_t wifi_status =
        initialize_wifi();

    if (wifi_status != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Wi-Fi initialization failed: %s",
            esp_err_to_name(wifi_status)
        );

        return;
    }

    ESP_LOGI(
        TAG,
        "Waiting for Wi-Fi"
    );

    (void)xEventGroupWaitBits(
        wifi_events,
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdTRUE,
        portMAX_DELAY
    );

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
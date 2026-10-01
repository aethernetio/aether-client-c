
#include "aether_esp32_wifi.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#define AETHER_ESP32_WIFI_CONNECTED_BIT BIT0

static EventGroupHandle_t s_wifi_events;
static bool s_wifi_initialized;
static bool s_handlers_registered;

static void aether_esp32_wifi_event(
    void *arg,
    esp_event_base_t base,
    int32_t id,
    void *data) {

    (void)arg;
    (void)data;

    if (base == WIFI_EVENT &&
        id == WIFI_EVENT_STA_START) {

        (void)esp_wifi_connect();
        return;
    }

    if (base == WIFI_EVENT &&
        id == WIFI_EVENT_STA_DISCONNECTED) {

        if (s_wifi_events != NULL) {
            xEventGroupClearBits(
                s_wifi_events,
                AETHER_ESP32_WIFI_CONNECTED_BIT);
        }

        (void)esp_wifi_connect();
        return;
    }

    if (base == IP_EVENT &&
        id == IP_EVENT_STA_GOT_IP) {

        if (s_wifi_events != NULL) {
            xEventGroupSetBits(
                s_wifi_events,
                AETHER_ESP32_WIFI_CONNECTED_BIT);
        }
    }
}

static esp_err_t aether_esp32_wifi_prepare(void) {

    esp_err_t status =
        esp_netif_init();

    if (status != ESP_OK &&
        status != ESP_ERR_INVALID_STATE) {
        return status;
    }

    status =
        esp_event_loop_create_default();

    if (status != ESP_OK &&
        status != ESP_ERR_INVALID_STATE) {
        return status;
    }

    if (!s_wifi_initialized) {
        wifi_init_config_t init =
            WIFI_INIT_CONFIG_DEFAULT();

        status =
            esp_wifi_init(&init);

        if (status != ESP_OK) {
            return status;
        }

        s_wifi_initialized = true;
    }

    if (!s_handlers_registered) {
        status =
            esp_event_handler_register(
                WIFI_EVENT,
                ESP_EVENT_ANY_ID,
                aether_esp32_wifi_event,
                NULL);

        if (status != ESP_OK) {
            return status;
        }

        status =
            esp_event_handler_register(
                IP_EVENT,
                IP_EVENT_STA_GOT_IP,
                aether_esp32_wifi_event,
                NULL);

        if (status != ESP_OK) {
            return status;
        }

        s_handlers_registered = true;
    }

    return ESP_OK;
}


esp_err_t aether_esp32_wifi_connect(
    const char *ssid,
    const char *password,
    uint32_t timeout_ms) {

    if (ssid == NULL ||
        ssid[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    wifi_config_t config = {0};

    size_t ssid_len = strlen(ssid);
    size_t password_len =
        password != NULL ? strlen(password) : 0u;

    if (ssid_len >= sizeof(config.sta.ssid) ||
        password_len >= sizeof(config.sta.password)) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t status =
        aether_esp32_wifi_prepare();

    if (status != ESP_OK) {
        return status;
    }

    if (esp_netif_create_default_wifi_sta() == NULL) {
        return ESP_FAIL;
    }

    memcpy(config.sta.ssid, ssid, ssid_len);

    if (password_len != 0u) {
        memcpy(config.sta.password, password, password_len);
    }

    status = esp_wifi_set_mode(WIFI_MODE_STA);

    if (status != ESP_OK) {
        return status;
    }

    status = esp_wifi_set_config(WIFI_IF_STA, &config);

    if (status != ESP_OK) {
        return status;
    }

    s_wifi_events = xEventGroupCreate();

    if (s_wifi_events == NULL) {
        return ESP_ERR_NO_MEM;
    }

    status = esp_wifi_start();

    if (status != ESP_OK) {
        return status;
    }

    TickType_t ticks;
    if (timeout_ms == AETHER_ESP32_WIFI_WAIT_FOREVER) {
        ticks = portMAX_DELAY;
    } else {
        ticks = pdMS_TO_TICKS(timeout_ms);
    }

    EventBits_t bits =
        xEventGroupWaitBits(
            s_wifi_events,
            AETHER_ESP32_WIFI_CONNECTED_BIT,
            pdFALSE,
            pdTRUE,
            ticks);

    if ((bits & AETHER_ESP32_WIFI_CONNECTED_BIT) == 0) {
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}


esp_err_t aether_esp32_wifi_start_ap(
    const char *ssid,
    const char *password,
    uint8_t channel,
    uint8_t max_connections) {

    if (ssid == NULL ||
        ssid[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    wifi_config_t config = {0};

    size_t ssid_len = strlen(ssid);
    size_t password_len =
        password != NULL ? strlen(password) : 0u;

    if (ssid_len >= sizeof(config.ap.ssid) ||
        password_len >= sizeof(config.ap.password)) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t status =
        aether_esp32_wifi_prepare();

    if (status != ESP_OK) {
        return status;
    }

    if (esp_netif_create_default_wifi_ap() == NULL) {
        return ESP_FAIL;
    }

    memcpy(config.ap.ssid, ssid, ssid_len);
    config.ap.ssid_len = (uint8_t)ssid_len;

    if (password_len != 0u) {
        memcpy(config.ap.password, password, password_len);
        config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    } else {
        config.ap.authmode = WIFI_AUTH_OPEN;
    }

    config.ap.channel =
        channel != 0u ? channel : 1u;
    config.ap.max_connection =
        max_connections != 0u ? max_connections : 4u;

    status = esp_wifi_set_mode(WIFI_MODE_AP);

    if (status != ESP_OK) {
        return status;
    }

    status = esp_wifi_set_config(WIFI_IF_AP, &config);

    if (status != ESP_OK) {
        return status;
    }

    return esp_wifi_start();
}

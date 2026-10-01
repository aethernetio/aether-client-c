
#include "aether_esp32_wifi.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#define AETHER_ESP32_WIFI_CONNECTED_BIT BIT0

static EventGroupHandle_t s_wifi_events;
static esp_netif_t *s_sta_netif;
static esp_netif_t *s_ap_netif;
static bool s_wifi_initialized;
static bool s_handlers_registered;

/*
 * Minimal reconnect policy state.
 *
 * s_sta_desired : the application wants a STA connection.
 * s_sta_connected: an IPv4 address is currently held.
 * s_mode : currently programmed Wi-Fi mode.
 */
static volatile bool s_sta_desired;
static volatile bool s_sta_connected;
static volatile bool s_sta_config_applied;
static volatile bool s_ap_transition;
static wifi_mode_t s_mode = WIFI_MODE_NULL;

static char s_ssid[33];
static char s_password[65];

static void aether_esp32_wifi_event(
    void *arg,
    esp_event_base_t base,
    int32_t id,
    void *data) {

    (void)arg;
    (void)data;

    if (base == WIFI_EVENT &&
        id == WIFI_EVENT_STA_DISCONNECTED) {

        s_sta_connected = false;

        if (s_wifi_events != NULL) {
            xEventGroupClearBits(
                s_wifi_events,
                AETHER_ESP32_WIFI_CONNECTED_BIT);
        }

        /*
         * Auto-reconnect only while STA is desired, the real ESP-IDF mode is
         * STA, and no STA -> AP transition is in progress. A stale disconnect
         * must not fight an AP bring-up.
         */
        if (s_sta_desired &&
            s_mode == WIFI_MODE_STA &&
            !s_ap_transition) {

            (void)esp_wifi_connect();
        }

        return;
    }

    if (base == IP_EVENT &&
        id == IP_EVENT_STA_GOT_IP) {

        s_sta_connected = true;

        if (s_wifi_events != NULL) {
            xEventGroupSetBits(
                s_wifi_events,
                AETHER_ESP32_WIFI_CONNECTED_BIT);
        }
    }
}

static esp_err_t aether_esp32_wifi_prepare(void) {

    if (s_wifi_events == NULL) {
        s_wifi_events = xEventGroupCreate();

        if (s_wifi_events == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }

    esp_err_t status = esp_netif_init();

    if (status != ESP_OK &&
        status != ESP_ERR_INVALID_STATE) {
        return status;
    }

    status = esp_event_loop_create_default();

    if (status != ESP_OK &&
        status != ESP_ERR_INVALID_STATE) {
        return status;
    }

    if (!s_wifi_initialized) {
        wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();

        status = esp_wifi_init(&init);

        if (status != ESP_OK) {
            return status;
        }

        s_wifi_initialized = true;
    }

    if (!s_handlers_registered) {
        status = esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            aether_esp32_wifi_event,
            NULL);

        if (status != ESP_OK) {
            return status;
        }

        status = esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            aether_esp32_wifi_event,
            NULL);

        if (status != ESP_OK) {
            return status;
        }

        s_handlers_registered = true;
    }

    if (s_sta_netif == NULL) {
        s_sta_netif = esp_netif_create_default_wifi_sta();

        if (s_sta_netif == NULL) {
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}


static esp_err_t aether_esp32_wifi_configure_sta(
    const char *ssid,
    const char *password) {

    wifi_config_t config = {0};

    size_t ssid_len = strlen(ssid);
    size_t password_len =
        password != NULL ? strlen(password) : 0u;

    if (ssid_len >= sizeof(config.sta.ssid) ||
        password_len >= sizeof(config.sta.password)) {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(config.sta.ssid, ssid, ssid_len);

    if (password_len != 0u) {
        memcpy(config.sta.password, password, password_len);
    }

    esp_err_t status = esp_wifi_set_mode(WIFI_MODE_STA);

    if (status != ESP_OK) {
        return status;
    }

    s_mode = WIFI_MODE_STA;

    status = esp_wifi_set_config(WIFI_IF_STA, &config);

    if (status != ESP_OK) {
        return status;
    }

    memcpy(s_ssid, ssid, ssid_len);
    s_ssid[ssid_len] = '\0';

    if (password_len != 0u) {
        memcpy(s_password, password, password_len);
    }
    s_password[password_len] = '\0';


    s_sta_config_applied = true;

    return ESP_OK;

}



esp_err_t aether_esp32_wifi_connect(
    const char *ssid,
    const char *password) {

    if (ssid == NULL ||
        ssid[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t status = aether_esp32_wifi_prepare();

    if (status != ESP_OK) {
        return status;
    }

    bool same_credentials =
        s_sta_config_applied &&
        s_mode == WIFI_MODE_STA &&
        strcmp(s_ssid, ssid) == 0 &&
        strcmp(s_password, password != NULL ? password : "") == 0;

    if (same_credentials &&
        s_sta_connected) {

        s_sta_desired = true;

        return ESP_OK;
    }

    if (same_credentials) {
        /*
         * Credentials are unchanged and a connection is already in progress
         * (or being auto-retried by the disconnect handler). Do not touch
         * set_config()/esp_wifi_connect() again on every loop iteration.
         */
        s_sta_desired = true;

        return ESP_ERR_WIFI_NOT_CONNECT;
    }


    /*
     * Credentials changed, or we are coming back from AP mode. Reset stale
     * state and apply the new STA configuration exactly once.
     */

    status = aether_esp32_wifi_configure_sta(ssid, password);

    if (status != ESP_OK) {
        return status;
    }

    /*
     * Config is applied successfully; only now commit the desired STA state
     * and reset the stale connected marker.
     */
    s_sta_connected = false;
    s_sta_desired = true;
    xEventGroupClearBits(
        s_wifi_events,
        AETHER_ESP32_WIFI_CONNECTED_BIT);

    status = esp_wifi_start();

    if (status != ESP_OK &&
        status != ESP_ERR_WIFI_CONN &&
        status != ESP_ERR_INVALID_STATE) {
        return status;
    }

    /*
     * Explicit first/again connect. The STA_START event is intentionally not
     * used as a connect trigger to avoid a double esp_wifi_connect().
     */
    status = esp_wifi_connect();

    if (status != ESP_OK) {
        return status;
    }

    return ESP_ERR_WIFI_NOT_CONNECT;
}



esp_err_t aether_esp32_wifi_connect_wait(
    const char *ssid,
    const char *password,
    uint32_t timeout_ms) {

    esp_err_t status =
        aether_esp32_wifi_connect(ssid, password);

    if (status == ESP_OK) {
        return ESP_OK;
    }

    if (status != ESP_ERR_WIFI_NOT_CONNECT) {
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


bool aether_esp32_wifi_connected(void) {
    return s_sta_connected;
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

    esp_err_t status = aether_esp32_wifi_prepare();

    if (status != ESP_OK) {
        return status;
    }

    if (s_ap_netif == NULL) {
        s_ap_netif = esp_netif_create_default_wifi_ap();

        if (s_ap_netif == NULL) {
            return ESP_FAIL;
        }
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

    /*
     * Suspend STA auto-reconnect during the STA -> AP transition so a stale
     * STA disconnect does not fight the AP bring-up. The permanent software
     * state (s_sta_desired=false, s_mode=AP) is committed only after the mode
     * switch actually succeeds.
     */
    s_ap_transition = true;

    status = esp_wifi_set_mode(WIFI_MODE_AP);

    if (status != ESP_OK) {
        s_ap_transition = false;
        return status;
    }

    s_mode = WIFI_MODE_AP;
    s_sta_desired = false;
    s_sta_connected = false;
    status = esp_wifi_set_config(WIFI_IF_AP, &config);

    if (status != ESP_OK) {
        s_ap_transition = false;
        return status;
    }

    status = esp_wifi_start();

    if (status != ESP_OK &&
        status != ESP_ERR_WIFI_CONN &&
        status != ESP_ERR_INVALID_STATE) {
        s_ap_transition = false;
        return status;
    }

    s_ap_transition = false;

    return ESP_OK;
}

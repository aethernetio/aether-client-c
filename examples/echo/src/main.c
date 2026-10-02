
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <string.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "esp_wifi.h"


#include "nvs_flash.h"

#include "aether.h"
#include "aether_credentials.h"
#include "client_server_api_types.h"
#include "aether_esp32_wifi.h"
#include "echo_secrets.h"

#include "aether_uuid_ECHO_UID.h"
#include "aether_hex_ECHO_MASTER_KEY.h"




static aether_t client;
static bool credentials_installed;

/*
 * Deferred echo staging.
 *
 * on_message runs inside RX processing, while the core is holding pinned RX
 * buffers and has rx_active == true. aether_send_message() must not be
 * called reentrantly from that context (it would return AETHER_ERR_BUSY).
 *
 * The callback therefore only copies the borrowed payload into this
 * caller-owned staging slot; the main loop performs the actual send after
 * aether_work() has fully returned.
 *
 * Single-slot policy: a second message that arrives before the pending one
 * is sent is dropped with an explicit error, never silently overwritten.
 */
static bool echo_pending;
static aether_uuid_t echo_pending_from;
static uint8_t echo_pending_data[AETHER_MAX_PACKET];
static size_t echo_pending_size;







static void on_message(
    aether_t *aether,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    (void)aether;

    if (size > sizeof(echo_pending_data)) {
        printf(
            "AETHER_ECHO_OVERFLOW size=%u max=%u\n",
            (unsigned)size,
            (unsigned)sizeof(echo_pending_data));
        return;
    }

    if (echo_pending) {
        printf("AETHER_ECHO_QUEUE_FULL\n");
        return;
    }

    echo_pending_from = from;
    echo_pending_size = size;

    if (size != 0u) {
        memcpy(
            echo_pending_data,
            data,
            size);
    }

    echo_pending = true;
}


static void drain_echo(void) {

    if (!echo_pending) {
        return;
    }

    aether_status_t status =
        aether_send_message(
            &client,
            echo_pending_from,
            echo_pending_data,
            echo_pending_size);

    if (status == AETHER_OK) {
        echo_pending = false;
        return;
    }

    if (status == AETHER_ERR_BUSY) {
        /*
         * Core still busy with RX/lifecycle scratch state. Keep the
         * pending slot and retry on the next main-loop iteration.
         */
        return;
    }

    printf(
        "AETHER_ECHO_SEND_ERROR status=%d state=%d\n",
        (int)status,
        (int)client.app.core.state);

    /*
     * Any other status is not retryable for this single-slot staging
     * contract: drop the pending message rather than spinning on it.
     */
    echo_pending = false;
}



static void on_error(
    aether_t *aether,
    aether_status_t code,
    aether_error_origin_t origin) {

    (void)aether;

    printf(
        "AETHER_ECHO_ERROR code=%d origin=%d\n",
        (int)code,
        (int)origin);
}


void app_main(void) {
    esp_err_t nvs_status =
        nvs_flash_init();

    if (nvs_status == ESP_ERR_NVS_NO_FREE_PAGES ||
        nvs_status == ESP_ERR_NVS_NEW_VERSION_FOUND) {

        if (nvs_flash_erase() != ESP_OK ||
            nvs_flash_init() != ESP_OK) {

            printf("AETHER_ECHO_FATAL=nvs\n");
            return;
        }

    } else if (nvs_status != ESP_OK) {
        printf("AETHER_ECHO_FATAL=nvs\n");
        return;
    }


    esp_err_t wifi_status =
        aether_esp32_wifi_connect_wait(
            AETHER_ECHO_WIFI_SSID,
            AETHER_ECHO_WIFI_PASSWORD,
            15000u);

    if (wifi_status != ESP_OK) {
        printf(
            "AETHER_ECHO_FATAL=wifi status=%d\n",
            (int)wifi_status);
        return;
    }

    printf("AETHER_ECHO_WIFI_CONNECTED\n");


    static const aether_uuid_t parent = {
        UINT64_C(0xB1AC52C88D94BD39),
        UINT64_C(0x4C01A631AC594165)
    };

    if (aether_init(
            &client,
            parent) != AETHER_OK) {

        printf("AETHER_ECHO_FATAL=init\n");
        return;
    }

    aether_on_message(
        &client,
        on_message);

    aether_on_error(
        &client,
        on_error);



    if (!credentials_installed) {

        if (aether_set_credentials_typed(
                &client.app.core,
                ECHO_UID,
                ECHO_MASTER_KEY,
                KEY_SYMMETRIC_TYPE_HYDROGEN_SECRET_BOX) != AETHER_OK) {

            printf("AETHER_ECHO_FATAL=credentials\n");
            return;
        }


        credentials_installed = true;
    }




    aether_status_t start_status = aether_start(&client);
    if (start_status != AETHER_OK) {
        printf("AETHER_ECHO_FATAL=start status=%d state=%d\n",
            (int)start_status,
            (int)client.app.core.state);
        return;
    }


    for (;;) {
        esp_err_t wifi_status =
            aether_esp32_wifi_connect(
                AETHER_ECHO_WIFI_SSID,
                AETHER_ECHO_WIFI_PASSWORD);

        if (wifi_status == ESP_OK ||
            wifi_status == ESP_ERR_WIFI_NOT_CONNECT) {





            aether_status_t poll_status =
                aether_work(
                    &client);


            if (poll_status != AETHER_OK) {
                printf(
                    "AETHER_ECHO_WORK_ERROR=%d\n",
                    (int)poll_status);
            }

            drain_echo();

        }



        vTaskDelay(
            pdMS_TO_TICKS(10));
    }
}

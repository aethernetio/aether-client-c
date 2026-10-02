
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

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





static void on_message(
    aether_t *aether,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    aether_status_t status =
        aether_send_message(
            aether,
            from,
            data,
            size);

    if (status != AETHER_OK) {
        printf(
            "AETHER_ECHO_SEND_ERROR=%d\n",
            (int)status);
    }
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
                aether_poll(
                    &client);

            if (poll_status != AETHER_OK) {
                printf(
                    "AETHER_ECHO_POLL_ERROR=%d\n",
                    (int)poll_status);
            }
        }



        vTaskDelay(
            pdMS_TO_TICKS(10));
    }
}
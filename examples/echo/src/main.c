
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
#include "aether_transport.h"
#include "client_server_api_types.h"

#include "aether_esp32_wifi.h"
#include "echo_secrets.h"



#include "aether_uuid_ECHO_UID.h"
#include "aether_hex_ECHO_MASTER_KEY.h"





typedef enum {
    ECHO_STAGE_EMPTY = 0,
    ECHO_STAGE_READY,
    ECHO_STAGE_IN_FLIGHT
} echo_stage_state_t;

typedef struct {
    aether_uuid_t sender;
    uint8_t payload[AETHER_MAX_PACKET];
    size_t length;
    echo_stage_state_t state;
} echo_staging_t;

static aether_t client;
static bool credentials_installed;

static aether_component_t components[1];

static echo_staging_t staging;
static unsigned sent_count;
static unsigned flush_done_count;
static aether_status_t last_flush_status;

static void on_flush_done(
    void *ctx,
    aether_status_t status) {

    (void)ctx;

    flush_done_count += 1u;
    last_flush_status = status;

    /*
     * The transport no longer reads the borrowed staging buffer, so
     * the staging slot is free again.
     */
    if (staging.state == ECHO_STAGE_IN_FLIGHT) {
        staging.state = ECHO_STAGE_EMPTY;
    }

    if (status != AETHER_OK) {
        printf(
            "AETHER_ECHO_FLUSH_DONE status=%d\n",
            (int)status);
    }
}





static void on_message(
    aether_t *aether,
    aether_uuid_t from,
    const uint8_t *data,
    size_t size) {

    (void)aether;

    /*
     * The RX callback owns borrowed payload memory. The semantic
     * transport is zero-copy and keeps the pointer until flush, so
     * staging must be an application-owned copy.
     */
    if (staging.state != ECHO_STAGE_EMPTY) {
        printf(
            "AETHER_ECHO_STAGE_BUSY size=%u\n",
            (unsigned)size);
        return;
    }

    if (size > sizeof(staging.payload)) {
        printf(
            "AETHER_ECHO_STAGE_OVERFLOW size=%u\n",
            (unsigned)size);
        return;
    }

    staging.sender = from;
    staging.length = size;

    if (size != 0u) {
        memcpy(
            staging.payload,
            data,
            size);
    }

    staging.state = ECHO_STAGE_READY;
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





    aether_set_components(
        &client,
        components,
        1u);

    aether_transport_listener_t listener = {
        NULL,
        on_flush_done
    };

    aether_transport_set_listener(
        &client.app.transport,
        &listener);

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




            /*
             * Attempt to enqueue the staged message. Transport will
             * borrow the staging buffer until on_flush_done.
             */
            if (staging.state == ECHO_STAGE_READY) {

                aether_status_t send_status =
                    aether_send_message(
                        &client,
                        staging.sender,
                        staging.payload,
                        staging.length);

                if (send_status == AETHER_OK) {
                    staging.state = ECHO_STAGE_IN_FLIGHT;
                    sent_count += 1u;
                } else if (send_status != AETHER_ERR_BUSY) {
                    printf(
                        "AETHER_ECHO_SEND_ERROR status=%d\n",
                        (int)send_status);
                    staging.state = ECHO_STAGE_EMPTY;
                }
            }

            aether_status_t work_status =
                aether_work(
                    &client);

            if (work_status != AETHER_OK) {
                printf(
                    "AETHER_ECHO_WORK_ERROR=%d\n",
                    (int)work_status);
            }




        vTaskDelay(
            pdMS_TO_TICKS(10));
    }
}
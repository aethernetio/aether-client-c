
/*
 * libhydrogen ESP-IDF shim.
 *
 * Upstream libhydrogen provides an ESP32 entropy backend using
 * esp_random() and delay(10). ESP-IDF does not define delay(); provide
 * the equivalent FreeRTOS implementation locally, then include the
 * upstream source.
 */
#ifdef ESP_PLATFORM
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"

static void aether_hydrogen_delay_ms(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}
#define delay(ms) aether_hydrogen_delay_ms((uint32_t)(ms))
#include "../../third_party/libhydrogen/hydrogen.c"
#undef delay
#endif

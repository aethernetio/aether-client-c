#ifdef AETHER_SIZE_WITH_HYDROGEN

/*
 * Upstream libhydrogen already has an ESP32 entropy backend using esp_random().
 * New ESP-IDF exposes esp_random() through esp_random.h.
 * Its backend also uses Arduino-style delay(), so provide the equivalent
 * FreeRTOS implementation locally for this ESP-IDF size harness.
 */
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

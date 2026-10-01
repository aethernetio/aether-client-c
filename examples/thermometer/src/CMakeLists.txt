


set(
    AETHER_CORE_SOURCES
    "../../../src/aether.c"
    "../../../src/aether_client.c"
    "../../../src/aether_client_app.c"
    "../../../src/aether_meta_runtime.c"
    "../../../src/aether_rx_first.c"
    "../../../src/aether_rx_all.c"
    "../../../src/aether_rx_adaptive.c"
    "../../../src/aether_send_future.c"
    "../../../src/aether_messages.c"
)

file(
    GLOB AETHER_GENERATED_SOURCES
    "../../../src/generated/*.c"
)

idf_component_register(
    SRCS
        "main.c"
        ${AETHER_CORE_SOURCES}
        ${AETHER_GENERATED_SOURCES}
        "../../../platform/esp32/aether_platform_esp32.c"
        "../../../platform/esp32/aether_crypto_p256_aes_gcm.c"

    INCLUDE_DIRS
        "."
        "../include"
        "../../../include"
        "../../../src"
        "../../../platform/esp32"

    REQUIRES
        esp_event
        esp_netif
        esp_timer
        esp_wifi
        lwip
        mbedtls
        nvs_flash
)

target_compile_definitions(
    ${COMPONENT_LIB}
    PRIVATE
        AETHER_PLATFORM_CONTEXT_BYTES=3200u
)

target_compile_options(
    ${COMPONENT_LIB}
    PRIVATE
        -Os
        -ffunction-sections
        -fdata-sections
)

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
    "../../../src/aether_delivery.c"
    "../../../src/aether_delivery_cloud.c"

    "../../../src/aether_credentials.c"

    "../../../src/aether_internal.c"
    "../../../src/aether_lan.c"
    "../../../src/aether_lan_codec.c"

)


include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/aether_uuid.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/aether_hex.cmake)


set(
    AETHER_GENERATED_SOURCES
    "../../../src/generated/client_server_api_api.c"
    "../../../src/generated/client_server_api_types.c"
    "../../../src/generated/client_server_reg_unsafe_local_local.c"
    "../../../src/generated/client_server_reg_direct_response_response.c"
    "../../../src/generated/aether_reg_global.c"
    "../../../src/generated/aether_reg_root_enter.c"
    "../../../src/generated/aether_reg_root_key.c"
    "../../../src/generated/aether_reg_safe.c"
    "../../../src/generated/aether_reg_unsafe_local.c"
)

idf_component_register(
    SRCS
        "main.c"
        ${AETHER_CORE_SOURCES}
        ${AETHER_GENERATED_SOURCES}
        "../../../platform/esp32/aether_platform_esp32.c"
        "../../../platform/esp32/aether_crypto_p256_aes_gcm.c"

        "../../../platform/esp32/aether_esp32_wifi.c"
        "../../../platform/esp32/aether_hydrogen_esp32.c"


    INCLUDE_DIRS
        "."
        "../include"
        "../../../include"
        "../../../src"
        "../../../src/generated"
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


aether_define_required_uuid(
    ${COMPONENT_LIB}
    ECHO_UID
    ECHO_UID
)

if("$ENV{ECHO_MASTER_KEY}" STREQUAL "")
    message(FATAL_ERROR
        "ECHO_MASTER_KEY is required: pass exactly 32 bytes as 64 hex digits")
endif()

aether_define_hex_bytes(
    ${COMPONENT_LIB}
    ECHO_MASTER_KEY
    32
    "$ENV{ECHO_MASTER_KEY}"
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
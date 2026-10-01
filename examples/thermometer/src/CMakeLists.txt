


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




include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/aether_uuid.cmake)


# Generated protocol sources.
#
# The unified generated API (client_server_api_api.c + _types.c) is the single
# source of truth for all registration endpoints. The obsolete selective
# generated API/types units are intentionally NOT listed here: they duplicate
# symbols now provided by the unified API. Only the generated helper units that
# the maintained adapters still depend on are included explicitly.
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



# Peer UUID, parsed at configure time from a canonical string.
# Provides `THERMOMETER_PEER_UID` (aether_uuid_t) via a generated header.
aether_define_uuid(
    ${COMPONENT_LIB}
    THERMOMETER_PEER_UID
    "$ENV{THERMOMETER_PEER_UUID}"
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
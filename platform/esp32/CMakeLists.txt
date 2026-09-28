
idf_component_register(
    SRCS
        "../../src/aether_client.c"
        "../../src/aether_client_app.c"
        "../../src/aether_rx_first.c"
        "../../src/aether_rx_all.c"
        "../../src/aether_rx_adaptive.c"
        "../../src/aether_send_future.c"
        "../../src/aether.c"
        "aether_platform_esp32.c"
        "aether_crypto_p256_aes_gcm.c"
    INCLUDE_DIRS
        "../../include"
        "../../src"
        "."
    REQUIRES
        esp_timer
        lwip
        mbedtls
        nvs_flash
)

target_compile_definitions(
    ${COMPONENT_LIB}
    PUBLIC
        AETHER_PLATFORM_CONTEXT_BYTES=3200u
)

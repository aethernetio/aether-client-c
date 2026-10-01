

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.c)
list(APPEND app_sources "../../aether_esp32_wifi.c")


idf_component_register(
    SRCS ${app_sources}
    INCLUDE_DIRS
        "../.."
    REQUIRES
        esp_event
        esp_netif
        esp_wifi
        esp_timer
)
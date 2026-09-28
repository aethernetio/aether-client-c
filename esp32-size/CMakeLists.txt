cmake_minimum_required(VERSION 3.16.0)

set(
    TF_PSA_CRYPTO_CONFIG_FILE
    "${CMAKE_CURRENT_LIST_DIR}/aether_psa_crypto_config.h"
    CACHE FILEPATH "Aether TF-PSA primary configuration"
    FORCE
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)


project(esp32-size)

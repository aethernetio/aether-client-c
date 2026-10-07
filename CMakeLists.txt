cmake_minimum_required(VERSION 3.16)
project(aether_client_c C)






add_library(aether_hydrogen_vendor STATIC
    third_party/libhydrogen/hydrogen.c
)

target_include_directories(aether_hydrogen_vendor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libhydrogen
)

target_compile_features(
    aether_hydrogen_vendor
    PUBLIC
        c_std_11
)


add_library(aether_client STATIC

    src/aether_client.c
    src/aether_meta_runtime.c
    src/aether_client_app.c

    src/aether_rx_first.c
    src/aether_rx_all.c
    src/aether_rx_adaptive.c

    src/aether_send_future.c
    src/aether_messages.c
    src/aether_transport.c
    src/aether_transport_server.c
    src/aether_peer_frame.c
    src/aether_peer_uap.c
    platform/posix/aether_posix_transport.c


    src/aether_registration.c
    src/aether_credentials.c


    src/generated/client_server_api_api.c
    src/generated/client_server_api_types.c

    src/generated/peer_api_api.c
    src/generated/peer_api_types.c



    src/aether_internal.c




    src/aether_lan.c
    src/aether_lan_codec.c

    src/aether.c
)





target_include_directories(aether_client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/posix
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/generated
)

target_include_directories(aether_client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libhydrogen
)
target_link_libraries(aether_client
    PUBLIC
        aether_hydrogen_vendor
)


target_compile_features(aether_client PUBLIC c_std_11)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_client
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
    )
endif()

option(AETHER_BUILD_TESTS "Build host-side tests" ON)

if(AETHER_BUILD_TESTS)
    enable_testing()


    add_library(aether_hydrogen_test_support STATIC
        src/aether_hydrogen.c
        tests/aether_test_crypto_hydrogen.c
    )

    target_include_directories(aether_hydrogen_test_support
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libhydrogen
    )

    target_link_libraries(
        aether_hydrogen_test_support
        PUBLIC
            aether_hydrogen_vendor
    )

    target_compile_features(
        aether_hydrogen_test_support
        PUBLIC
            c_std_11
    )


    add_executable(
        aether_peer_uap_test
        tests/test_peer_uap.c
    )

    target_link_libraries(
        aether_peer_uap_test
        PRIVATE
            aether_client
    )

    add_test(
        NAME aether_peer_uap_test
        COMMAND aether_peer_uap_test
    )


    add_executable(
        aether_p2p_e2e
        tests/p2p_e2e.c
        tests/p2p_test_support.c
    )

target_link_libraries(
        aether_p2p_e2e
        PRIVATE
            aether_client
            aether_hydrogen_test_support
    )

    target_compile_features(
        aether_p2p_e2e
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_p2p_e2e
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()



    add_executable(
        aether_client_app_test
        tests/test_app.c
    )

    target_link_libraries(
        aether_client_app_test
        PRIVATE
            aether_client
    )

    target_compile_features(
        aether_client_app_test
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_client_app_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()


    add_test(
        NAME aether_client_app_test
        COMMAND aether_client_app_test
    )

    add_executable(
        aether_api_test
        tests/test_aether.c
    )

    target_link_libraries(
        aether_api_test
        PRIVATE
            aether_client
    )

    target_compile_features(
        aether_api_test
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_api_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()

    add_test(
        NAME aether_api_test
        COMMAND aether_api_test

    )



    add_executable(
        aether_components_test
        tests/test_components.c
    )


    target_link_libraries(
        aether_components_test
        PRIVATE
            aether_client
            aether_hydrogen_test_support
    )


    target_compile_features(
        aether_components_test
        PRIVATE
            c_std_11
    )


    target_include_directories(
        aether_components_test
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated
    )


    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_components_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()

    add_test(
        NAME aether_components_test
        COMMAND aether_components_test
    )



    add_executable(
        aether_facade_lifecycle_test
        tests/test_facade_lifecycle.c
    )

    target_link_libraries(
        aether_facade_lifecycle_test
        PRIVATE

            aether_client
            aether_hydrogen_test_support

    )

    target_compile_features(
        aether_facade_lifecycle_test
        PRIVATE
            c_std_11
    )

    target_include_directories(
        aether_facade_lifecycle_test
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_facade_lifecycle_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()

    add_test(
        NAME aether_facade_lifecycle_test
        COMMAND aether_facade_lifecycle_test
    )



    add_executable(
        aether_facade_scheduler_test
        tests/test_facade_scheduler.c
    )


    target_include_directories(
        aether_facade_scheduler_test
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )


    target_link_libraries(
        aether_facade_scheduler_test
        PRIVATE
            aether_client
    )

    target_compile_features(
        aether_facade_scheduler_test
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_facade_scheduler_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()

    add_test(
        NAME aether_facade_scheduler_test
        COMMAND aether_facade_scheduler_test
    )







    add_executable(
        aether_rx_test
        tests/test_rx.c
    )

    target_link_libraries(
        aether_rx_test
        PRIVATE
            aether_client
    )

    target_compile_features(
        aether_rx_test
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_rx_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()

    add_test(
        NAME aether_rx_test
        COMMAND aether_rx_test
    )


    add_executable(
        aether_send_future_test
        tests/test_send_future.c
    )

    target_link_libraries(
        aether_send_future_test
        PRIVATE
            aether_client
    )

    target_compile_features(
        aether_send_future_test
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_send_future_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()

    add_test(
        NAME aether_send_future_test
        COMMAND aether_send_future_test
    )




    add_executable(
        aether_client_test
        tests/test_client.c
    )

    target_link_libraries(
        aether_client_test
        PRIVATE
            aether_client
            aether_hydrogen_test_support
    )

    target_include_directories(
        aether_client_test
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated
    )


    add_executable(
        aether_hydrogen_test
        tests/test_hydrogen.c
    )

    target_link_libraries(
        aether_hydrogen_test
        PRIVATE
            aether_hydrogen_test_support
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_client_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )

        target_compile_options(
            aether_hydrogen_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )

        target_compile_options(
            aether_hydrogen_test_support
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()

    add_test(
        NAME aether_client_test
        COMMAND aether_client_test
    )


    add_executable(
        aether_messages_test
        tests/test_messages.c
    )

    target_link_libraries(
        aether_messages_test
        PRIVATE
            aether_client
    )

    target_compile_features(
        aether_messages_test
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_messages_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()

    add_test(
        NAME aether_messages_test
        COMMAND aether_messages_test
    )


    add_executable(
        aether_transport_test
        tests/test_transport.c
    )


    target_include_directories(
        aether_transport_test
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )


    target_link_libraries(
        aether_transport_test
        PRIVATE
            aether_client
            aether_hydrogen_test_support
    )

    target_compile_features(
        aether_transport_test
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_transport_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    endif()

    add_test(
        NAME aether_transport_test
        COMMAND aether_transport_test
    )



    add_executable(
        aether_lan_codec_test
        tests/test_lan_codec.c
    )

    target_include_directories(
        aether_lan_codec_test
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(
        aether_lan_codec_test
        PRIVATE
            aether_client
            aether_hydrogen_vendor
    )

    target_compile_features(
        aether_lan_codec_test
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_lan_codec_test
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
                -UNDEBUG
        )
    endif()

    add_test(
        NAME aether_lan_codec_test
        COMMAND aether_lan_codec_test
    )


    add_test(
        NAME aether_hydrogen_test
        COMMAND aether_hydrogen_test
    )

    add_test(
        NAME cmake_helpers_check
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake_helpers_check.sh
    )







    find_package(OpenSSL REQUIRED)

    add_executable(
        aether_lan_cert_test
        tests/test_lan_cert.c
    )

    target_include_directories(
        aether_lan_cert_test
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libhydrogen
    )

    target_link_libraries(
        aether_lan_cert_test
        PRIVATE
            aether_client
            OpenSSL::Crypto
    )

    target_compile_features(
        aether_lan_cert_test
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_lan_cert_test
            PRIVATE
                -Wall -Wextra -Wpedantic -Werror -UNDEBUG
        )
    endif()

    add_test(
        NAME aether_lan_cert_test
        COMMAND aether_lan_cert_test
    )


    add_executable(
        aether_lan_component
        tests/test_lan_component.c
    )

    target_include_directories(
        aether_lan_component
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libhydrogen
    )

    target_link_libraries(
        aether_lan_component
        PRIVATE
            aether_client
    )

    target_compile_features(
        aether_lan_component
        PRIVATE
            c_std_11
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            aether_lan_component
            PRIVATE
                -Wall -Wextra -Wpedantic -Werror -UNDEBUG
        )
    endif()

    add_test(
        NAME aether_lan_component
        COMMAND aether_lan_component
    )


endif()


target_sources(aether_client PRIVATE
    src/generated/client_server_reg_global_api.c
    src/generated/aether_reg_global.c
)


target_sources(aether_client PRIVATE
    src/generated/client_server_reg_safe_api.c
    src/generated/client_server_reg_safe_types.c
    src/generated/aether_reg_safe.c
)


target_sources(aether_client PRIVATE
    src/generated/client_server_reg_root_key_api.c
    src/generated/aether_reg_root_key.c
    src/generated/client_server_reg_root_enter_api.c
    src/generated/aether_reg_root_enter.c
)


target_sources(aether_client PRIVATE
    src/generated/client_server_reg_unsafe_local_local.c
    src/generated/aether_reg_unsafe_local.c
)


target_sources(aether_client PRIVATE
    src/generated/client_server_reg_direct_response_response.c
)
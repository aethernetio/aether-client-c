
cmake_minimum_required(VERSION 3.16)
project(aether_protocol_c_generator C)


add_library(aether_adslc_core STATIC
    src/adsl.c
    src/emitter.c
)

add_library(aether_meta_runtime_test STATIC
    ../../src/aether_meta_runtime.c
)

target_include_directories(aether_meta_runtime_test
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_compile_features(aether_meta_runtime_test PUBLIC c_std_11)



target_include_directories(aether_adslc_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


target_compile_features(aether_adslc_core PUBLIC c_std_11)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(aether_adslc_core PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
endif()

add_executable(aether-adslc
    src/main.c
)

target_link_libraries(aether-adslc
    PRIVATE
        aether_adslc_core
)

target_compile_features(aether-adslc PRIVATE c_std_11)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(aether-adslc PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
endif()

enable_testing()

add_executable(aether_adslc_test
    tests/test_adsl.c
)

target_link_libraries(aether_adslc_test
    PRIVATE
        aether_adslc_core
)

target_compile_definitions(aether_adslc_test
    PRIVATE
        ADSL_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
)


file(MAKE_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/test-adsl-output"
)

target_compile_definitions(aether_adslc_test
    PRIVATE
        ADSL_TEST_OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}/test-adsl-output"
)


target_compile_features(aether_adslc_test PRIVATE c_std_11)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(aether_adslc_test PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
endif()

add_test(
    NAME aether_adslc_test
    COMMAND aether_adslc_test
)


set(AETHER_ADSLC_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_GENERATED_DIR}/temperature_sensor_types.h"
        "${AETHER_ADSLC_GENERATED_DIR}/temperature_sensor_types.c"
        "${AETHER_ADSLC_GENERATED_DIR}/temperature_sensor_api.h"
        "${AETHER_ADSLC_GENERATED_DIR}/temperature_sensor_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/TemperatureSensor.adsl.yaml"
        "${AETHER_ADSLC_GENERATED_DIR}"
        TemperatureSensor
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/TemperatureSensor.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_temperature_test
    tests/test_generated_temperature.c
    "${AETHER_ADSLC_GENERATED_DIR}/temperature_sensor_types.c"
    "${AETHER_ADSLC_GENERATED_DIR}/temperature_sensor_api.c"
)

target_include_directories(aether_generated_temperature_test
    PRIVATE
        "${AETHER_ADSLC_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_temperature_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_temperature_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
    )
endif()

add_test(
    NAME aether_generated_temperature_test
    COMMAND aether_generated_temperature_test
)


add_executable(aether_adslc_include_test
    tests/test_include.c
)

target_link_libraries(aether_adslc_include_test
    PRIVATE
        aether_adslc_core
)

target_compile_definitions(aether_adslc_include_test
    PRIVATE
        ADSL_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
)

target_compile_features(
    aether_adslc_include_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_adslc_include_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
    )
endif()

add_test(
    NAME aether_adslc_include_test
    COMMAND aether_adslc_include_test
)


add_executable(aether_adslc_project_test
    tests/test_project.c
)

target_link_libraries(aether_adslc_project_test
    PRIVATE
        aether_adslc_core
)

target_compile_definitions(aether_adslc_project_test
    PRIVATE
        ADSL_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
)

target_compile_features(
    aether_adslc_project_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_adslc_project_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_adslc_project_test
    COMMAND aether_adslc_project_test
)


add_executable(aether_adslc_project_scan_test
    tests/test_project_scan.c
)

target_link_libraries(aether_adslc_project_scan_test
    PRIVATE
        aether_adslc_core
)

target_compile_definitions(aether_adslc_project_scan_test
    PRIVATE
        ADSL_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
)

target_compile_features(
    aether_adslc_project_scan_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_adslc_project_scan_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_adslc_project_scan_test
    COMMAND aether_adslc_project_scan_test
)


add_executable(aether_adslc_unified_merge_test
    tests/test_unified_merge.c
)

target_link_libraries(aether_adslc_unified_merge_test
    PRIVATE
        aether_adslc_core
)

target_compile_definitions(aether_adslc_unified_merge_test
    PRIVATE
        ADSL_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
)

target_compile_features(
    aether_adslc_unified_merge_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_adslc_unified_merge_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_adslc_unified_merge_test
    COMMAND aether_adslc_unified_merge_test
)


add_executable(aether_adslc_unified_types_test
    tests/test_unified_types.c
)

target_link_libraries(aether_adslc_unified_types_test
    PRIVATE
        aether_adslc_core
)

target_compile_definitions(aether_adslc_unified_types_test
    PRIVATE
        ADSL_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
        ADSL_TEST_OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}/test-unified-types-output"
        AETHER_RUNTIME_INC="${CMAKE_CURRENT_SOURCE_DIR}/../../include"
        AETHER_RUNTIME_SRC="${CMAKE_CURRENT_SOURCE_DIR}/../../src/aether_meta_runtime.c"
        ADSL_CC="${CMAKE_C_COMPILER}"
)

target_compile_features(
    aether_adslc_unified_types_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_adslc_unified_types_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_adslc_unified_types_test
    COMMAND aether_adslc_unified_types_test
)





add_executable(aether_adslc_type_registry_test
    tests/test_type_registry.c
)

target_link_libraries(aether_adslc_type_registry_test
    PRIVATE
        aether_adslc_core
)

target_compile_definitions(aether_adslc_type_registry_test
    PRIVATE
        ADSL_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
)

target_compile_features(
    aether_adslc_type_registry_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_adslc_type_registry_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_adslc_type_registry_test
    COMMAND aether_adslc_type_registry_test
)


add_executable(aether_adslc_cross_module_pending_test
    tests/test_cross_module_pending.c
)

target_link_libraries(aether_adslc_cross_module_pending_test
    PRIVATE
        aether_adslc_core
)

target_include_directories(aether_adslc_cross_module_pending_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


target_compile_definitions(aether_adslc_cross_module_pending_test
    PRIVATE
        ADSL_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
        ADSL_TEST_OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output"
)

target_compile_features(
    aether_adslc_cross_module_pending_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_adslc_cross_module_pending_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

file(MAKE_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output"
)


add_test(
    NAME aether_adslc_cross_module_pending_test
    COMMAND aether_adslc_cross_module_pending_test
)

# Smoke-compile the generated cross-module output after the pending test
# wrote it. This checks that the generated headers form a syntactically
# valid translation unit for the supported A -> B and Holder -> Item
# scenarios. Full link and ABI checks belong to a later step.


add_test(
    NAME aether_adslc_cross_module_smoke_compile_test
    COMMAND ${CMAKE_C_COMPILER}
        -fsyntax-only
        -I${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output
        -I${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output/a_uses_item_types.c
        ${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output/a_uses_item_api.c
        ${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output/b_defines_item_types.c
        ${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output/i_holds_external_types.c
        ${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output/j_uses_external_array_types.c

        ${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output/j_uses_external_array_api.c
        ${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output/g_defines_base_types.c
        ${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-pending-output/h_extends_base_types.c
)

set_tests_properties(
    aether_adslc_cross_module_smoke_compile_test
    PROPERTIES
        DEPENDS aether_adslc_cross_module_pending_test
)






add_executable(aether_adslc_cross_module_test
    tests/test_cross_module.c
)

target_link_libraries(aether_adslc_cross_module_test
    PRIVATE
        aether_adslc_core
)

target_compile_definitions(aether_adslc_cross_module_test
    PRIVATE
        ADSL_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
        ADSL_TEST_OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-output"
)

target_compile_features(
    aether_adslc_cross_module_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_adslc_cross_module_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

file(MAKE_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/test-cross-module-output"
)

add_test(
    NAME aether_adslc_cross_module_test
    COMMAND aether_adslc_cross_module_test
)




add_executable(aether_adslc_emitter_hardening_test
    tests/test_emitter_hardening.c
)

target_link_libraries(aether_adslc_emitter_hardening_test
    PRIVATE
        aether_adslc_core
)

target_compile_definitions(aether_adslc_emitter_hardening_test
    PRIVATE
        ADSL_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
        ADSL_TEST_OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}/test-emitter-hardening-output"
)

target_compile_features(
    aether_adslc_emitter_hardening_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_adslc_emitter_hardening_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

file(MAKE_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/test-emitter-hardening-output"
)

add_test(
    NAME aether_adslc_emitter_hardening_test
    COMMAND aether_adslc_emitter_hardening_test
)


# EMIT-A3: generated local dispatcher must compile for a nullable stream
# parameter. This is a shell test because it requires a real C compiler.

add_test(
    NAME aether_emitter_nullable_stream_test
    COMMAND ${CMAKE_COMMAND} -E env
        ADSL_TEST_GENERATOR=$<TARGET_FILE:aether-adslc>
        ADSL_TEST_FIXTURES=${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures
        ADSL_TEST_RUNTIME_INCLUDE=${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ADSL_TEST_CC=${CMAKE_C_COMPILER}
        bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_emitter_nullable_stream.sh
)


set_tests_properties(aether_emitter_nullable_stream_test PROPERTIES TIMEOUT 60)






set(AETHER_ADSLC_STRUCT_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-struct"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_STRUCT_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_STRUCT_GENERATED_DIR}/include_root_types.h"
        "${AETHER_ADSLC_STRUCT_GENERATED_DIR}/include_root_types.c"
        "${AETHER_ADSLC_STRUCT_GENERATED_DIR}/include_root_api.h"
        "${AETHER_ADSLC_STRUCT_GENERATED_DIR}/include_root_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/IncludeRoot.adsl.yaml"
        "${AETHER_ADSLC_STRUCT_GENERATED_DIR}"
        IncludeRoot
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/IncludeRoot.adsl.yaml"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/CommonTypes.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_struct_test
    tests/test_generated_struct.c
    "${AETHER_ADSLC_STRUCT_GENERATED_DIR}/include_root_types.c"
    "${AETHER_ADSLC_STRUCT_GENERATED_DIR}/include_root_api.c"
)

target_include_directories(aether_generated_struct_test
    PRIVATE
        "${AETHER_ADSLC_STRUCT_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_struct_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_struct_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
    )
endif()

add_test(
    NAME aether_generated_struct_test
    COMMAND aether_generated_struct_test
)


set(AETHER_ADSLC_VALUE_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-values"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_VALUE_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_VALUE_GENERATED_DIR}/value_types_types.h"
        "${AETHER_ADSLC_VALUE_GENERATED_DIR}/value_types_types.c"
        "${AETHER_ADSLC_VALUE_GENERATED_DIR}/value_types_api.h"
        "${AETHER_ADSLC_VALUE_GENERATED_DIR}/value_types_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/ValueTypes.adsl.yaml"
        "${AETHER_ADSLC_VALUE_GENERATED_DIR}"
        ValueTypes
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/ValueTypes.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_value_types_test
    tests/test_generated_values.c
    "${AETHER_ADSLC_VALUE_GENERATED_DIR}/value_types_types.c"
    "${AETHER_ADSLC_VALUE_GENERATED_DIR}/value_types_api.c"
)

target_include_directories(aether_generated_value_types_test
    PRIVATE
        "${AETHER_ADSLC_VALUE_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_value_types_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_value_types_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
    )
endif()

add_test(
    NAME aether_generated_value_types_test
    COMMAND aether_generated_value_types_test
)


set(AETHER_ADSLC_ENUM_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-enum"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_ENUM_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_ENUM_GENERATED_DIR}/enum_types_types.h"
        "${AETHER_ADSLC_ENUM_GENERATED_DIR}/enum_types_types.c"
        "${AETHER_ADSLC_ENUM_GENERATED_DIR}/enum_types_api.h"
        "${AETHER_ADSLC_ENUM_GENERATED_DIR}/enum_types_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/EnumTypes.adsl.yaml"
        "${AETHER_ADSLC_ENUM_GENERATED_DIR}"
        EnumTypes
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/EnumTypes.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_enum_test
    tests/test_generated_enum.c
    "${AETHER_ADSLC_ENUM_GENERATED_DIR}/enum_types_types.c"
    "${AETHER_ADSLC_ENUM_GENERATED_DIR}/enum_types_api.c"
)

target_include_directories(aether_generated_enum_test
    PRIVATE
        "${AETHER_ADSLC_ENUM_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_enum_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_enum_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
    )
endif()

add_test(
    NAME aether_generated_enum_test
    COMMAND aether_generated_enum_test
)


# Test assertions must remain active in Release builds.  CMake's Release
# flags define NDEBUG, which otherwise compiles assert()-based regressions
# away completely and makes the tests both ineffective and -Werror-unsafe.
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    foreach(AETHER_ADSLC_TEST_TARGET IN ITEMS
        aether_adslc_test
        aether_adslc_include_test
        aether_generated_temperature_test
        aether_generated_struct_test
        aether_generated_value_types_test
        aether_generated_enum_test
    )
        target_compile_options(
            ${AETHER_ADSLC_TEST_TARGET}
            PRIVATE
                -UNDEBUG
        )
    endforeach()
endif()


set(AETHER_ADSLC_INTPACK_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-intpack"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_INTPACK_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_INTPACK_GENERATED_DIR}/int_pack_types.h"
        "${AETHER_ADSLC_INTPACK_GENERATED_DIR}/int_pack_types.c"
        "${AETHER_ADSLC_INTPACK_GENERATED_DIR}/int_pack_api.h"
        "${AETHER_ADSLC_INTPACK_GENERATED_DIR}/int_pack_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/IntPack.adsl.yaml"
        "${AETHER_ADSLC_INTPACK_GENERATED_DIR}"
        IntPack
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/IntPack.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_intpack_test
    tests/test_generated_intpack.c
    "${AETHER_ADSLC_INTPACK_GENERATED_DIR}/int_pack_types.c"
    "${AETHER_ADSLC_INTPACK_GENERATED_DIR}/int_pack_api.c"
)

target_include_directories(aether_generated_intpack_test
    PRIVATE
        "${AETHER_ADSLC_INTPACK_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_intpack_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_intpack_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_generated_intpack_test
    COMMAND aether_generated_intpack_test
)


set(AETHER_ADSLC_NESTED_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-nested"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_NESTED_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_NESTED_GENERATED_DIR}/nested_struct_types.h"
        "${AETHER_ADSLC_NESTED_GENERATED_DIR}/nested_struct_types.c"
        "${AETHER_ADSLC_NESTED_GENERATED_DIR}/nested_struct_api.h"
        "${AETHER_ADSLC_NESTED_GENERATED_DIR}/nested_struct_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/NestedStruct.adsl.yaml"
        "${AETHER_ADSLC_NESTED_GENERATED_DIR}"
        NestedStruct
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/NestedStruct.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_nested_struct_test
    tests/test_generated_nested_struct.c
    "${AETHER_ADSLC_NESTED_GENERATED_DIR}/nested_struct_types.c"
    "${AETHER_ADSLC_NESTED_GENERATED_DIR}/nested_struct_api.c"
)

target_include_directories(aether_generated_nested_struct_test
    PRIVATE
        "${AETHER_ADSLC_NESTED_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_nested_struct_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_nested_struct_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_generated_nested_struct_test
    COMMAND aether_generated_nested_struct_test
)


set(AETHER_ADSLC_NULLABLE_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-nullable"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_NULLABLE_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_NULLABLE_GENERATED_DIR}/nullable_struct_types.h"
        "${AETHER_ADSLC_NULLABLE_GENERATED_DIR}/nullable_struct_types.c"
        "${AETHER_ADSLC_NULLABLE_GENERATED_DIR}/nullable_struct_api.h"
        "${AETHER_ADSLC_NULLABLE_GENERATED_DIR}/nullable_struct_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/NullableStruct.adsl.yaml"
        "${AETHER_ADSLC_NULLABLE_GENERATED_DIR}"
        NullableStruct
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/NullableStruct.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_nullable_struct_test
    tests/test_generated_nullable_struct.c
    "${AETHER_ADSLC_NULLABLE_GENERATED_DIR}/nullable_struct_types.c"
    "${AETHER_ADSLC_NULLABLE_GENERATED_DIR}/nullable_struct_api.c"
)

target_include_directories(aether_generated_nullable_struct_test
    PRIVATE
        "${AETHER_ADSLC_NULLABLE_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_nullable_struct_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_nullable_struct_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_generated_nullable_struct_test
    COMMAND aether_generated_nullable_struct_test
)


set(AETHER_ADSLC_ARRAYS_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-arrays"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_ARRAYS_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_ARRAYS_GENERATED_DIR}/arrays_types.h"
        "${AETHER_ADSLC_ARRAYS_GENERATED_DIR}/arrays_types.c"
        "${AETHER_ADSLC_ARRAYS_GENERATED_DIR}/arrays_api.h"
        "${AETHER_ADSLC_ARRAYS_GENERATED_DIR}/arrays_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/Arrays.adsl.yaml"
        "${AETHER_ADSLC_ARRAYS_GENERATED_DIR}"
        Arrays
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/Arrays.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_arrays_test
    tests/test_generated_arrays.c
    "${AETHER_ADSLC_ARRAYS_GENERATED_DIR}/arrays_types.c"
    "${AETHER_ADSLC_ARRAYS_GENERATED_DIR}/arrays_api.c"
)

target_include_directories(aether_generated_arrays_test
    PRIVATE
        "${AETHER_ADSLC_ARRAYS_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_arrays_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_arrays_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_generated_arrays_test
    COMMAND aether_generated_arrays_test
)


set(AETHER_ADSLC_HIERARCHY_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-hierarchy"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_HIERARCHY_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_HIERARCHY_GENERATED_DIR}/hierarchy_types.h"
        "${AETHER_ADSLC_HIERARCHY_GENERATED_DIR}/hierarchy_types.c"
        "${AETHER_ADSLC_HIERARCHY_GENERATED_DIR}/hierarchy_api.h"
        "${AETHER_ADSLC_HIERARCHY_GENERATED_DIR}/hierarchy_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/Hierarchy.adsl.yaml"
        "${AETHER_ADSLC_HIERARCHY_GENERATED_DIR}"
        Hierarchy
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/Hierarchy.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_hierarchy_test
    tests/test_generated_hierarchy.c
    "${AETHER_ADSLC_HIERARCHY_GENERATED_DIR}/hierarchy_types.c"
    "${AETHER_ADSLC_HIERARCHY_GENERATED_DIR}/hierarchy_api.c"
)

target_include_directories(aether_generated_hierarchy_test
    PRIVATE
        "${AETHER_ADSLC_HIERARCHY_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_hierarchy_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_hierarchy_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_generated_hierarchy_test
    COMMAND aether_generated_hierarchy_test
)


set(AETHER_ADSLC_RESULT_RPC_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-result-rpc"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_RESULT_RPC_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_RESULT_RPC_GENERATED_DIR}/result_rpc_types.h"
        "${AETHER_ADSLC_RESULT_RPC_GENERATED_DIR}/result_rpc_types.c"
        "${AETHER_ADSLC_RESULT_RPC_GENERATED_DIR}/result_rpc_api.h"
        "${AETHER_ADSLC_RESULT_RPC_GENERATED_DIR}/result_rpc_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/ResultRpc.adsl.yaml"
        "${AETHER_ADSLC_RESULT_RPC_GENERATED_DIR}"
        ResultRpc
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/ResultRpc.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_result_rpc_test
    tests/test_generated_result_rpc.c
    "${AETHER_ADSLC_RESULT_RPC_GENERATED_DIR}/result_rpc_types.c"
    "${AETHER_ADSLC_RESULT_RPC_GENERATED_DIR}/result_rpc_api.c"
)

target_include_directories(aether_generated_result_rpc_test
    PRIVATE
        "${AETHER_ADSLC_RESULT_RPC_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_result_rpc_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_result_rpc_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_generated_result_rpc_test
    COMMAND aether_generated_result_rpc_test
)


set(AETHER_ADSLC_STREAM_BUILDER_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-stream-builder"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_STREAM_BUILDER_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_STREAM_BUILDER_GENERATED_DIR}/stream_builder_types.h"
        "${AETHER_ADSLC_STREAM_BUILDER_GENERATED_DIR}/stream_builder_types.c"
        "${AETHER_ADSLC_STREAM_BUILDER_GENERATED_DIR}/stream_builder_api.h"
        "${AETHER_ADSLC_STREAM_BUILDER_GENERATED_DIR}/stream_builder_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/StreamBuilder.adsl.yaml"
        "${AETHER_ADSLC_STREAM_BUILDER_GENERATED_DIR}"
        StreamBuilder
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/StreamBuilder.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_stream_builder_test
    tests/test_generated_stream_builder.c
    "${AETHER_ADSLC_STREAM_BUILDER_GENERATED_DIR}/stream_builder_types.c"
    "${AETHER_ADSLC_STREAM_BUILDER_GENERATED_DIR}/stream_builder_api.c"
)

target_include_directories(aether_generated_stream_builder_test
    PRIVATE
        "${AETHER_ADSLC_STREAM_BUILDER_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_stream_builder_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_stream_builder_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_generated_stream_builder_test
    COMMAND aether_generated_stream_builder_test
)



set(AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-local-dispatch"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}/local_dispatch_types.h"
        "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}/local_dispatch_types.c"
        "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}/local_dispatch_local.h"
        "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}/local_dispatch_local.c"
    COMMAND
        aether-adslc
        --local-api
        LocalApi
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/LocalStreamDispatch.adsl.yaml"
        "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}"
        LocalDispatch
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/LocalStreamDispatch.adsl.yaml"
    VERBATIM
)


add_library(aether_generated_local_dispatch_obj OBJECT
    "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}/local_dispatch_local.c"
)

target_include_directories(aether_generated_local_dispatch_obj
    PRIVATE
        "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_local_dispatch_obj
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # MCU stack probe: generated local dispatcher must not create a large
    # stack frame even for schemas with large fixed array parameters.
    # This limit is only applied to the generated dispatcher translation
    # unit, not to the whole project.
    target_compile_options(
        aether_generated_local_dispatch_obj
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -O2
            -Wframe-larger-than=256
    )
endif()

add_executable(aether_generated_local_dispatch_test
    tests/test_generated_local_dispatch.c
    "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}/local_dispatch_types.c"
    $<TARGET_OBJECTS:aether_generated_local_dispatch_obj>
)

target_include_directories(aether_generated_local_dispatch_test
    PRIVATE
        "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_local_dispatch_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_local_dispatch_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
    )
endif()


add_test(
    NAME aether_generated_local_dispatch_test
    COMMAND aether_generated_local_dispatch_test
)



set(AETHER_ADSLC_RESPONSE_METHOD_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-response-method"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_RESPONSE_METHOD_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT

        "${AETHER_ADSLC_RESPONSE_METHOD_GENERATED_DIR}/response_method_types.h"
        "${AETHER_ADSLC_RESPONSE_METHOD_GENERATED_DIR}/response_method_types.c"

        "${AETHER_ADSLC_RESPONSE_METHOD_GENERATED_DIR}/response_method_response.h"
        "${AETHER_ADSLC_RESPONSE_METHOD_GENERATED_DIR}/response_method_response.c"
    COMMAND
        aether-adslc
        --response-method
        GlobalApi.finish
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/ResponseMethod.adsl.yaml"
        "${AETHER_ADSLC_RESPONSE_METHOD_GENERATED_DIR}"
        ResponseMethod
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/ResponseMethod.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_response_method_test
    tests/test_generated_response_method.c

    "${AETHER_ADSLC_RESPONSE_METHOD_GENERATED_DIR}/response_method_types.c"

    "${AETHER_ADSLC_RESPONSE_METHOD_GENERATED_DIR}/response_method_response.c"
)

target_include_directories(aether_generated_response_method_test
    PRIVATE
        "${AETHER_ADSLC_RESPONSE_METHOD_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_response_method_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_response_method_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
    )
endif()


add_test(
    NAME aether_generated_response_method_test
    COMMAND aether_generated_response_method_test
)


set(AETHER_ADSLC_RESPONSE_PING_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-response-ping"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_RESPONSE_PING_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT

        "${AETHER_ADSLC_RESPONSE_PING_GENERATED_DIR}/response_method_ping_types.h"
        "${AETHER_ADSLC_RESPONSE_PING_GENERATED_DIR}/response_method_ping_types.c"

        "${AETHER_ADSLC_RESPONSE_PING_GENERATED_DIR}/response_method_ping_response.h"
        "${AETHER_ADSLC_RESPONSE_PING_GENERATED_DIR}/response_method_ping_response.c"
    COMMAND
        aether-adslc
        --response-method
        GlobalApi.ping
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/ResponseMethod.adsl.yaml"
        "${AETHER_ADSLC_RESPONSE_PING_GENERATED_DIR}"
        ResponseMethodPing
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/ResponseMethod.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_response_ping_test
    tests/test_generated_response_ping.c

    "${AETHER_ADSLC_RESPONSE_PING_GENERATED_DIR}/response_method_ping_types.c"

    "${AETHER_ADSLC_RESPONSE_PING_GENERATED_DIR}/response_method_ping_response.c"
)

target_include_directories(aether_generated_response_ping_test
    PRIVATE
        "${AETHER_ADSLC_RESPONSE_PING_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_response_ping_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_response_ping_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_generated_response_ping_test
    COMMAND aether_generated_response_ping_test
)


set(AETHER_ADSLC_RESPONSE_DESCRIBE_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-response-describe"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_RESPONSE_DESCRIBE_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT

        "${AETHER_ADSLC_RESPONSE_DESCRIBE_GENERATED_DIR}/response_method_describe_types.h"
        "${AETHER_ADSLC_RESPONSE_DESCRIBE_GENERATED_DIR}/response_method_describe_types.c"

        "${AETHER_ADSLC_RESPONSE_DESCRIBE_GENERATED_DIR}/response_method_describe_response.h"
        "${AETHER_ADSLC_RESPONSE_DESCRIBE_GENERATED_DIR}/response_method_describe_response.c"
    COMMAND
        aether-adslc
        --response-method
        GlobalApi.describe
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/ResponseMethod.adsl.yaml"
        "${AETHER_ADSLC_RESPONSE_DESCRIBE_GENERATED_DIR}"
        ResponseMethodDescribe
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/ResponseMethod.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_response_describe_test
    tests/test_generated_response_describe.c

    "${AETHER_ADSLC_RESPONSE_DESCRIBE_GENERATED_DIR}/response_method_describe_types.c"

    "${AETHER_ADSLC_RESPONSE_DESCRIBE_GENERATED_DIR}/response_method_describe_response.c"
)

target_include_directories(aether_generated_response_describe_test
    PRIVATE
        "${AETHER_ADSLC_RESPONSE_DESCRIBE_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_response_describe_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_response_describe_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

add_test(
    NAME aether_generated_response_describe_test
    COMMAND aether_generated_response_describe_test
)


foreach(AETHER_GENERATED_TEST_TARGET IN ITEMS
    aether_generated_temperature_test
    aether_generated_struct_test
    aether_generated_value_types_test
    aether_generated_enum_test
    aether_generated_intpack_test
    aether_generated_nested_struct_test
    aether_generated_nullable_struct_test
    aether_generated_arrays_test
    aether_generated_hierarchy_test
    aether_generated_result_rpc_test
    aether_generated_stream_builder_test
    aether_generated_local_dispatch_test

    aether_generated_response_method_test
    aether_generated_response_ping_test
    aether_generated_response_describe_test

)
    target_link_libraries(
        ${AETHER_GENERATED_TEST_TARGET}
        PRIVATE
            aether_meta_runtime_test
    )
endforeach()


# Collapsible (`~`) is a semantic ADSL modifier. It must not introduce a
# different C representation or wire codec. Keep a generated regression
# target so struct fields, nullable `?~`, and API parameters remain covered.
set(AETHER_ADSLC_COLLAPSIBLE_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-collapsible"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_COLLAPSIBLE_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_COLLAPSIBLE_GENERATED_DIR}/collapsible_fields_types.h"
        "${AETHER_ADSLC_COLLAPSIBLE_GENERATED_DIR}/collapsible_fields_types.c"
        "${AETHER_ADSLC_COLLAPSIBLE_GENERATED_DIR}/collapsible_fields_api.h"
        "${AETHER_ADSLC_COLLAPSIBLE_GENERATED_DIR}/collapsible_fields_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/CollapsibleFields.adsl.yaml"
        "${AETHER_ADSLC_COLLAPSIBLE_GENERATED_DIR}"
        CollapsibleFields
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/CollapsibleFields.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_collapsible_test
    tests/test_generated_collapsible.c
    "${AETHER_ADSLC_COLLAPSIBLE_GENERATED_DIR}/collapsible_fields_types.c"
    "${AETHER_ADSLC_COLLAPSIBLE_GENERATED_DIR}/collapsible_fields_api.c"
)

target_include_directories(aether_generated_collapsible_test
    PRIVATE
        "${AETHER_ADSLC_COLLAPSIBLE_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_collapsible_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_collapsible_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

target_link_libraries(
    aether_generated_collapsible_test
    PRIVATE
        aether_meta_runtime_test
)

add_test(
    NAME aether_generated_collapsible_test
    COMMAND aether_generated_collapsible_test
)


# Fixed arrays of generated concrete structs are inline noheap values.
# Generation must order complete element bodies before T[N] typedefs and keep
# the Java wire form: exactly N payloads with no dynamic length prefix.
set(AETHER_ADSLC_STATIC_STRUCT_ARRAY_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-static-struct-array"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_STATIC_STRUCT_ARRAY_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_STATIC_STRUCT_ARRAY_GENERATED_DIR}/static_struct_array_types.h"
        "${AETHER_ADSLC_STATIC_STRUCT_ARRAY_GENERATED_DIR}/static_struct_array_types.c"
        "${AETHER_ADSLC_STATIC_STRUCT_ARRAY_GENERATED_DIR}/static_struct_array_api.h"
        "${AETHER_ADSLC_STATIC_STRUCT_ARRAY_GENERATED_DIR}/static_struct_array_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/StaticStructArray.adsl.yaml"
        "${AETHER_ADSLC_STATIC_STRUCT_ARRAY_GENERATED_DIR}"
        StaticStructArray
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/StaticStructArray.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_static_struct_array_test
    tests/test_generated_static_struct_array.c
    "${AETHER_ADSLC_STATIC_STRUCT_ARRAY_GENERATED_DIR}/static_struct_array_types.c"
    "${AETHER_ADSLC_STATIC_STRUCT_ARRAY_GENERATED_DIR}/static_struct_array_api.c"
)

target_include_directories(aether_generated_static_struct_array_test
    PRIVATE
        "${AETHER_ADSLC_STATIC_STRUCT_ARRAY_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_static_struct_array_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_static_struct_array_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

target_link_libraries(
    aether_generated_static_struct_array_test
    PRIVATE
        aether_meta_runtime_test
)

add_test(
    NAME aether_generated_static_struct_array_test
    COMMAND aether_generated_static_struct_array_test
)



# Nullable API arguments use the same Java wire mask as nullable DTO fields:
# bit 1 means NULL, nullable arguments alone consume mask bits, and a
# response-bearing command places request_id before that mask.
set(AETHER_ADSLC_NULLABLE_API_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-nullable-api"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}/nullable_api_params_types.h"
        "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}/nullable_api_params_types.c"
        "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}/nullable_api_params_api.h"
        "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}/nullable_api_params_api.c"
        "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}/nullable_api_params_local.h"
        "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}/nullable_api_params_local.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/NullableApiParams.adsl.yaml"
        "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}"
        NullableApiParams
    COMMAND
        aether-adslc
        --local-api
        NullableParamApi
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/NullableApiParams.adsl.yaml"
        "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}"
        NullableApiParams
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/NullableApiParams.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_nullable_api_params_test
    tests/test_generated_nullable_api_params.c
    "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}/nullable_api_params_types.c"
    "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}/nullable_api_params_api.c"
    "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}/nullable_api_params_local.c"
)

target_include_directories(aether_generated_nullable_api_params_test
    PRIVATE
        "${AETHER_ADSLC_NULLABLE_API_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_nullable_api_params_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_nullable_api_params_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

target_link_libraries(
    aether_generated_nullable_api_params_test
    PRIVATE
        aether_meta_runtime_test
)

add_test(
    NAME aether_generated_nullable_api_params_test
    COMMAND aether_generated_nullable_api_params_test
)




# Typed throws response parity. Keep the two selected methods in separate
# generated directories because --response-method emits one complete type pair
# plus one method-specific response pair per invocation.

set(AETHER_ADSLC_TYPED_THROWS_CALCULATE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-typed-throws-calculate"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_TYPED_THROWS_CALCULATE_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_TYPED_THROWS_CALCULATE_DIR}/typed_throws_types.h"
        "${AETHER_ADSLC_TYPED_THROWS_CALCULATE_DIR}/typed_throws_types.c"
        "${AETHER_ADSLC_TYPED_THROWS_CALCULATE_DIR}/typed_throws_response.h"
        "${AETHER_ADSLC_TYPED_THROWS_CALCULATE_DIR}/typed_throws_response.c"
    COMMAND
        aether-adslc
        --response-method
        TypedThrowsApi.calculate
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/TypedThrows.adsl.yaml"
        "${AETHER_ADSLC_TYPED_THROWS_CALCULATE_DIR}"
        TypedThrows
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/TypedThrows.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_typed_throws_calculate_test
    tests/test_generated_typed_throws_calculate.c
    "${AETHER_ADSLC_TYPED_THROWS_CALCULATE_DIR}/typed_throws_types.c"
    "${AETHER_ADSLC_TYPED_THROWS_CALCULATE_DIR}/typed_throws_response.c"
)

target_include_directories(aether_generated_typed_throws_calculate_test
    PRIVATE
        "${AETHER_ADSLC_TYPED_THROWS_CALCULATE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_typed_throws_calculate_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_typed_throws_calculate_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

target_link_libraries(
    aether_generated_typed_throws_calculate_test
    PRIVATE
        aether_meta_runtime_test
)

add_test(
    NAME aether_generated_typed_throws_calculate_test
    COMMAND aether_generated_typed_throws_calculate_test
)


set(AETHER_ADSLC_TYPED_THROWS_NOTIFY_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-typed-throws-notify"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_TYPED_THROWS_NOTIFY_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_TYPED_THROWS_NOTIFY_DIR}/typed_throws_types.h"
        "${AETHER_ADSLC_TYPED_THROWS_NOTIFY_DIR}/typed_throws_types.c"
        "${AETHER_ADSLC_TYPED_THROWS_NOTIFY_DIR}/typed_throws_response.h"
        "${AETHER_ADSLC_TYPED_THROWS_NOTIFY_DIR}/typed_throws_response.c"
    COMMAND
        aether-adslc
        --response-method
        TypedThrowsApi.notify
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/TypedThrows.adsl.yaml"
        "${AETHER_ADSLC_TYPED_THROWS_NOTIFY_DIR}"
        TypedThrows
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/TypedThrows.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_typed_throws_notify_test
    tests/test_generated_typed_throws_notify.c
    "${AETHER_ADSLC_TYPED_THROWS_NOTIFY_DIR}/typed_throws_types.c"
    "${AETHER_ADSLC_TYPED_THROWS_NOTIFY_DIR}/typed_throws_response.c"
)

target_include_directories(aether_generated_typed_throws_notify_test
    PRIVATE
        "${AETHER_ADSLC_TYPED_THROWS_NOTIFY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_typed_throws_notify_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_typed_throws_notify_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

target_link_libraries(
    aether_generated_typed_throws_notify_test
    PRIVATE
        aether_meta_runtime_test
)

add_test(
    NAME aether_generated_typed_throws_notify_test
    COMMAND aether_generated_typed_throws_notify_test
)


# TieredInt wire logic is shared runtime code. Generated bindings only provide
# schema constants and call these helpers.
add_executable(aether_tiered_int_runtime_test
    tests/test_tiered_int_runtime.c
)

target_include_directories(aether_tiered_int_runtime_test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_tiered_int_runtime_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_tiered_int_runtime_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

target_link_libraries(
    aether_tiered_int_runtime_test
    PRIVATE
        aether_meta_runtime_test
)

add_test(
    NAME aether_tiered_int_runtime_test
    COMMAND aether_tiered_int_runtime_test
)



# Named TieredInt is a scalar schema type whose wire codec lives once in the
# handwritten runtime. This generated regression protects Java-compatible wire
# boundaries, nullable pointer semantics, fixed arrays, and struct round trips.
set(AETHER_ADSLC_TIERED_INT_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-tiered-int"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_TIERED_INT_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_TIERED_INT_GENERATED_DIR}/tiered_int_types.h"
        "${AETHER_ADSLC_TIERED_INT_GENERATED_DIR}/tiered_int_types.c"
        "${AETHER_ADSLC_TIERED_INT_GENERATED_DIR}/tiered_int_api.h"
        "${AETHER_ADSLC_TIERED_INT_GENERATED_DIR}/tiered_int_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/TieredInt.adsl.yaml"
        "${AETHER_ADSLC_TIERED_INT_GENERATED_DIR}"
        TieredInt
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/TieredInt.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_tiered_int_test
    tests/test_generated_tiered_int.c
    "${AETHER_ADSLC_TIERED_INT_GENERATED_DIR}/tiered_int_types.c"
    "${AETHER_ADSLC_TIERED_INT_GENERATED_DIR}/tiered_int_api.c"
)

target_include_directories(aether_generated_tiered_int_test
    PRIVATE
        "${AETHER_ADSLC_TIERED_INT_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_tiered_int_test
    PRIVATE
        c_std_11
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_tiered_int_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

target_link_libraries(
    aether_generated_tiered_int_test
    PRIVATE
        aether_meta_runtime_test
)

add_test(
    NAME aether_generated_tiered_int_test
    COMMAND aether_generated_tiered_int_test
)


# Recursive named structs use the ordinary nullable-mask wire format. Nullable
# scalar edges and dynamic arrays break the C layout cycle; nested codecs call
# reusable named serializers/deserializers instead of source-expanding forever.
set(AETHER_ADSLC_RECURSIVE_STRUCT_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated-recursive-struct"
)

file(MAKE_DIRECTORY
    "${AETHER_ADSLC_RECURSIVE_STRUCT_GENERATED_DIR}"
)

add_custom_command(
    OUTPUT
        "${AETHER_ADSLC_RECURSIVE_STRUCT_GENERATED_DIR}/recursive_struct_types.h"
        "${AETHER_ADSLC_RECURSIVE_STRUCT_GENERATED_DIR}/recursive_struct_types.c"
        "${AETHER_ADSLC_RECURSIVE_STRUCT_GENERATED_DIR}/recursive_struct_api.h"
        "${AETHER_ADSLC_RECURSIVE_STRUCT_GENERATED_DIR}/recursive_struct_api.c"
    COMMAND
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/RecursiveStruct.adsl.yaml"
        "${AETHER_ADSLC_RECURSIVE_STRUCT_GENERATED_DIR}"
        RecursiveStruct
    DEPENDS
        aether-adslc
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/RecursiveStruct.adsl.yaml"
    VERBATIM
)

add_executable(aether_generated_recursive_struct_test
    tests/test_generated_recursive_struct.c
    "${AETHER_ADSLC_RECURSIVE_STRUCT_GENERATED_DIR}/recursive_struct_types.c"
    "${AETHER_ADSLC_RECURSIVE_STRUCT_GENERATED_DIR}/recursive_struct_api.c"
)

target_include_directories(aether_generated_recursive_struct_test
    PRIVATE
        "${AETHER_ADSLC_RECURSIVE_STRUCT_GENERATED_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_compile_features(
    aether_generated_recursive_struct_test
    PRIVATE
        c_std_11
)


target_compile_definitions(
    aether_generated_recursive_struct_test
    PRIVATE
        AETHER_META_MAX_RECURSION_DEPTH=4u
)


if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        aether_generated_recursive_struct_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -UNDEBUG
    )
endif()

target_link_libraries(
    aether_generated_recursive_struct_test
    PRIVATE
        aether_meta_runtime_test
)

add_test(
    NAME aether_generated_recursive_struct_test
    COMMAND aether_generated_recursive_struct_test
)
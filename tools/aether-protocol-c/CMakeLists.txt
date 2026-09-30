
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

add_executable(aether_generated_local_dispatch_test
    tests/test_generated_local_dispatch.c
    "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}/local_dispatch_types.c"
    "${AETHER_ADSLC_LOCAL_DISPATCH_GENERATED_DIR}/local_dispatch_local.c"
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
)
    target_link_libraries(
        ${AETHER_GENERATED_TEST_TARGET}
        PRIVATE
            aether_meta_runtime_test
    )
endforeach()

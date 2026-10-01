
# aether_define_hex_bytes(<target> <symbol> <byte_count> <hex_value>)
#
# Convert a hex string into a `static const uint8_t <symbol>[<byte_count>]`
# array at CONFIGURE time, written to a generated header in the target's build
# directory. The build directory is added to the target include path.
#
# Use this for SECRETS such as a master key: unlike a compile definition, the
# value never appears in compile_commands.json or verbose build logs. The
# generated header lives under the (git-ignored) build tree.
#
#   aether_define_hex_bytes(my_target MY_MASTER_KEY 32 "0a0b0c...")
#   -> #include "aether_hex_MY_MASTER_KEY.h"
#
# Separators (dashes/colons/spaces) are ignored. An empty value yields an
# all-zero array of <byte_count> bytes.
function(aether_define_hex_bytes target symbol byte_count hex_value)
    string(REGEX REPLACE "[^0-9a-fA-F]" "" _hex "${hex_value}")
    string(TOLOWER "${_hex}" _hex)

    math(EXPR _expected "${byte_count} * 2")

    if(_hex STREQUAL "")
        set(_hex "")
        foreach(_i RANGE 1 ${byte_count})
            string(APPEND _hex "00")
        endforeach()
    endif()

    string(LENGTH "${_hex}" _len)
    if(NOT _len EQUAL _expected)
        message(FATAL_ERROR
            "aether_define_hex_bytes(${symbol}): expected ${_expected} hex "
            "digits (${byte_count} bytes), got ${_len}")
    endif()

    set(_init "")
    math(EXPR _last "${byte_count} - 1")
    foreach(_i RANGE 0 ${_last})
        math(EXPR _off "${_i} * 2")
        string(SUBSTRING "${_hex}" ${_off} 2 _pair)
        if(_init STREQUAL "")
            set(_init "0x${_pair}")
        else()
            string(APPEND _init ", 0x${_pair}")
        endif()
    endforeach()

    set(_header "${CMAKE_CURRENT_BINARY_DIR}/aether_hex_${symbol}.h")

    file(WRITE "${_header}"
"#ifndef AETHER_HEX_${symbol}_H
#define AETHER_HEX_${symbol}_H

#include <stdint.h>

static const uint8_t ${symbol}[${byte_count}] = { ${_init} };

#endif
")

    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()

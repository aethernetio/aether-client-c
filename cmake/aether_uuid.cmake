
# aether_define_uuid(<target> <symbol> <canonical_value>)
#
# Parse a canonical UUID string at CONFIGURE time and emit a generated header
# that defines `static const aether_uuid_t <symbol>`.
#
#   aether_define_uuid(my_target MY_PEER_UID "01020304-0506-0708-1112-131415161718")
#
# Device code then uses MY_PEER_UID directly; there is no runtime UUID parsing
# and no hand-written MSB/LSB pair. The generated header
# (aether_uuid_<symbol>.h) lives in the build directory, which is added to the
# target include path.
#
# One helper covers every UUID a user has: parent UID, own UID and the
# peer/destination UID. Separators (dashes/colons/spaces) are ignored. An empty
# value yields the zero UUID.
function(aether_define_uuid target symbol value)
    string(REGEX REPLACE "[^0-9a-fA-F]" "" _hex "${value}")
    string(TOLOWER "${_hex}" _hex)

    if(_hex STREQUAL "")
        set(_hex "00000000000000000000000000000000")
    endif()

    string(LENGTH "${_hex}" _len)
    if(NOT _len EQUAL 32)
        message(FATAL_ERROR
            "aether_define_uuid(${symbol}): '${value}' is not a canonical "
            "UUID (expected 32 hex digits, e.g. "
            "01020304-0506-0708-1112-131415161718)")
    endif()

    string(SUBSTRING "${_hex}" 0 16 _msb)
    string(SUBSTRING "${_hex}" 16 16 _lsb)

    set(_header "${CMAKE_CURRENT_BINARY_DIR}/aether_uuid_${symbol}.h")

    file(WRITE  "${_header}" "#ifndef AETHER_UUID_${symbol}_H\n")
    file(APPEND "${_header}" "#define AETHER_UUID_${symbol}_H\n\n")
    file(APPEND "${_header}" "#include \"aether_client.h\"\n\n")
    file(APPEND "${_header}" "static const aether_uuid_t ${symbol} = {\n")
    file(APPEND "${_header}" "    0x${_msb}ULL,\n")
    file(APPEND "${_header}" "    0x${_lsb}ULL\n")
    file(APPEND "${_header}" "};\n\n")
    file(APPEND "${_header}" "#endif\n")

    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()

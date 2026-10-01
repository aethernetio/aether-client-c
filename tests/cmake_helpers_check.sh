
#!/usr/bin/env bash
#
# Configure-level contract checks for the CMake helpers
# cmake/aether_uuid.cmake and cmake/aether_hex.cmake.
#
# Each case configures a tiny throwaway project and asserts whether the
# configure step is expected to succeed or fail. This is the only way to test
# configure-time behaviour without real hardware.
set -u

here="$(cd "$(dirname "$0")" && pwd)"
root="$(cd "$here/.." && pwd)"
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT

fail=0

# case <name> <cmake-snippet> <expect: ok|fail>
case_run() {
    local name="$1" snippet="$2" expect="$3"
    local d="$work/$name"
    mkdir -p "$d"

    cat > "$d/CMakeLists.txt" <<EOF
cmake_minimum_required(VERSION 3.16)
project(cmcheck C)
include("$root/cmake/aether_uuid.cmake")
include("$root/cmake/aether_hex.cmake")
add_executable(cmcheck k.c)
target_include_directories(cmcheck PRIVATE "$root/include")
$snippet
EOF
    printf '#include "aether_client.h"\nint main(void){return 0;}\n' > "$d/k.c"

    if cmake -S "$d" -B "$d/b" >/dev/null 2>&1; then
        actual=ok
    else
        actual=fail
    fi

    if [ "$actual" != "$expect" ]; then
        echo "cmake_helpers_check: FAIL $name (expected $expect, got $actual)"
        fail=1
    else
        echo "cmake_helpers_check: ok   $name ($actual)"
    fi
}

# --- UUID ---------------------------------------------------------------
case_run uuid_valid_canonical \
    'aether_define_uuid(cmcheck A "01020304-0506-0708-1112-131415161718")' ok
case_run uuid_colon_separators \
    'aether_define_uuid(cmcheck A "01020304:0506:0708:1112:131415161718")' ok
case_run uuid_space_separators \
    'aether_define_uuid(cmcheck A "01020304 0506 0708 1112 131415161718")' ok
case_run uuid_empty_is_zero \
    'aether_define_uuid(cmcheck A "")' ok
case_run uuid_invalid_char \
    'aether_define_uuid(cmcheck A "01020304-0506-0708-1112-13141516171Z")' fail
case_run uuid_wrong_length \
    'aether_define_uuid(cmcheck A "0102-0506")' fail

# --- hex bytes ----------------------------------------------------------
case_run hex_correct \
    'aether_define_hex_bytes(cmcheck S 4 "0a0b0c0d")' ok
case_run hex_separators \
    'aether_define_hex_bytes(cmcheck S 4 "0a:0b:0c:0d")' ok
case_run hex_empty_is_zero \
    'aether_define_hex_bytes(cmcheck S 4 "")' ok
case_run hex_invalid_char \
    'aether_define_hex_bytes(cmcheck S 4 "0a0b0c0g")' fail
case_run hex_wrong_size \
    'aether_define_hex_bytes(cmcheck S 4 "0a0b0c")' fail

exit "$fail"

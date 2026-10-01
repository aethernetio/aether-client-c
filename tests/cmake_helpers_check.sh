#!/usr/bin/env bash
#
# Configure-level contract checks for the CMake helpers
# cmake/aether_uuid.cmake and cmake/aether_hex.cmake, plus the thermometer
# peer-UUID policy.
set -u

here="$(cd "$(dirname "$0")" && pwd)"
root="$(cd "$here/.." && pwd)"
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT

fail=0

# case_run <name> <snippet> <expect: ok|fail>
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

# case_file_has <name> <snippet> <file> <needle>
case_file_has() {
    local name="$1" snippet="$2" file="$3" needle="$4"
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

    if ! cmake -S "$d" -B "$d/b" >/dev/null 2>&1; then
        echo "cmake_helpers_check: FAIL $name (configure failed)"
        fail=1
        return
    fi

    local target="$d/b/$file"
    if grep -qF "$needle" "$target" 2>/dev/null; then
        echo "cmake_helpers_check: ok   $name"
    else
        echo "cmake_helpers_check: FAIL $name (missing '$needle' in $file)"
        fail=1
    fi
}

# --- UUID: parsing policy ----------------------------------------------
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
case_run uuid_bad_symbol \
    'aether_define_uuid(cmcheck "1BAD" "01020304-0506-0708-1112-131415161718")' fail

# --- hex: parsing policy -----------------------------------------------
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
case_run hex_bad_symbol \
    'aether_define_hex_bytes(cmcheck "1BAD" 4 "0a0b0c0d")' fail
case_run hex_zero_count \
    'aether_define_hex_bytes(cmcheck S 0 "0a")' fail
case_run hex_negative_count \
    'aether_define_hex_bytes(cmcheck S -1 "0a")' fail
case_run hex_garbage_count \
    'aether_define_hex_bytes(cmcheck S "abc" "0a")' fail

# --- generated content -------------------------------------------------
case_file_has uuid_content_ok \
    'aether_define_uuid(cmcheck PEER "01020304-0506-0708-1112-131415161718")' \
    "aether_uuid_PEER.h" "0x0102030405060708ULL"
case_file_has uuid_content_lsb \
    'aether_define_uuid(cmcheck PEER "01020304-0506-0708-1112-131415161718")' \
    "aether_uuid_PEER.h" "0x1112131415161718ULL"
case_file_has uuid_content_zero \
    'aether_define_uuid(cmcheck Z "")' \
    "aether_uuid_Z.h" "0x0000000000000000ULL"
case_file_has hex_content_ok \
    'aether_define_hex_bytes(cmcheck SEC 4 "0a:0b:0c:0d")' \
    "aether_hex_SEC.h" "{ 0x0a, 0x0b, 0x0c, 0x0d }"
case_file_has hex_content_zero \
    'aether_define_hex_bytes(cmcheck SEC 4 "")' \
    "aether_hex_SEC.h" "{ 0x00, 0x00, 0x00, 0x00 }"


# --- required-UUID policy (thermometer) --------------------------------
# Same helper the thermometer uses. A missing env var must fail at configure
# time with the helper's own diagnostic, not just any non-zero exit.
req_dir="$work/req"
mkdir -p "$req_dir"
cat > "$req_dir/CMakeLists.txt" <<EOF
cmake_minimum_required(VERSION 3.16)
project(cmcheck C)
include("$root/cmake/aether_uuid.cmake")
add_executable(cmcheck k.c)
target_include_directories(cmcheck PRIVATE "$root/include")
aether_define_required_uuid(cmcheck REQUIRED_UID MY_REQUIRED_UUID)
EOF
printf '#include "aether_client.h"\nint main(void){return 0;}\n' > "$req_dir/k.c"

req_out="$work/req.out"
if env -u MY_REQUIRED_UUID cmake -S "$req_dir" -B "$req_dir/b" >"$req_out" 2>&1; then
    echo "cmake_helpers_check: FAIL required_uuid_missing (configure succeeded without env)"
    fail=1
elif grep -qF "MY_REQUIRED_UUID is required" "$req_out"; then
    echo "cmake_helpers_check: ok   required_uuid_missing (failed with our message)"
else
    echo "cmake_helpers_check: FAIL required_uuid_missing (failed for the wrong reason)"
    fail=1
fi

# Sanity: with the env var set, the same fixture configures successfully.
if MY_REQUIRED_UUID=01020304-0506-0708-1112-131415161718 \
        cmake -S "$req_dir" -B "$req_dir/b2" >/dev/null 2>&1; then
    echo "cmake_helpers_check: ok   required_uuid_present"
else
    echo "cmake_helpers_check: FAIL required_uuid_present"
    fail=1
fi


exit "$fail"

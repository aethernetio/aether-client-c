#!/usr/bin/env bash
set -euo pipefail

ROOT=/home/angel/projects/aether-client-c
BUILD="$ROOT/build-adsl-generator-release"
GEN="$BUILD/aether-adslc"
CC_BIN="${CC:-cc}"

fail() {
    echo "JAVA/C INHERITED AUX EMITTERS TEST FAILED: $*" >&2
    exit 1
}

compile_dir() {
    local dir="$1"
    local found=0

    for src in "$dir"/*.c; do
        [[ -f "$src" ]] || continue
        found=1

        "$CC_BIN"             -std=c11             -Wall             -Wextra             -Wpedantic             -Werror             -I"$dir"             -I"$ROOT/include"             -c "$src"             -o "$dir/$(basename "${src%.c}").o"
    done

    [[ "$found" -eq 1 ]] ||
        fail "no generated C sources in $dir"
}

cmake --build "$BUILD" -j2 >/dev/null

[[ -x "$GEN" ]] ||
    fail "generator binary not found: $GEN"

TMP="$(mktemp -d /tmp/aether-inherited-aux-emitters.XXXXXX)"
trap 'rm -rf "$TMP"' EXIT

FIXTURE="$TMP/InheritedAux.adsl.yaml"

cat > "$FIXTURE" <<'ADSL'
types:
  ParentArg:
    fields:
      code: short

  CommandStream:
    stream:
      api: ChildApi

api:
  ChildApi:
    parents:
      - ParentApi
    methods:
      rpc:
        id: 99
        returns: String
        params:
          value: byte

      own:
        id: 8

  ParentApi:
    methods:
      rpc:
        id: 7
        returns: short
        params:
          value: ParentArg
ADSL

ALL="$TMP/all"
RESPONSE="$TMP/response"

mkdir -p "$ALL" "$RESPONSE"

"$GEN"     "$FIXTURE"     "$ALL"     InheritedAux

compile_dir "$ALL"

cat > "$TMP/builder_probe.c" <<'C'
#include "inherited_aux_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

int main(void) {
    uint8_t storage[64] = {0};

    command_stream_t stream = {
        .data = storage,
        .capacity = sizeof(storage),
        .length = 0u
    };

    parent_arg_t arg = {
        .code = INT16_C(0x1234)
    };

    assert(
        command_stream_child_api_rpc(
            &stream,
            UINT32_C(0x01020304),
            &arg) ==
        AETHER_OK);

    assert(stream.length >= 7u);
    assert(storage[0] == UINT8_C(7));

    return 0;
}
C

"$CC_BIN"     -std=c11     -Wall     -Wextra     -Wpedantic     -Werror     -I"$ALL"     -I"$ROOT/include"     "$TMP/builder_probe.c"     "$ALL/inherited_aux_types.c"     "$ALL/inherited_aux_api.c"     "$ROOT/src/aether_meta_runtime.c"     -o "$TMP/builder_probe"

"$TMP/builder_probe"

"$GEN"     --response-method ChildApi.rpc     "$FIXTURE"     "$RESPONSE"     InheritedResponse

compile_dir "$RESPONSE"

cat > "$TMP/response_probe.c" <<'C'
#include "inherited_response_response.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

int main(void) {
    static const uint8_t wire[] = {
        0u,
        0x04u, 0x03u, 0x02u, 0x01u,
        0x34u, 0x12u
    };

    aether_meta_read_context_t ctx = {0};

    ctx.reader.data = wire;
    ctx.reader.length = sizeof(wire);

    child_api_rpc_response_frame_t frame = {0};

    assert(
        child_api_rpc_response_decode(
            &ctx,
            &frame) ==
        AETHER_OK);

    assert(ctx.reader.position == sizeof(wire));
    assert(frame.request_id == UINT32_C(0x01020304));
    assert(frame.result.value == INT16_C(0x1234));

    return 0;
}
C

"$CC_BIN"     -std=c11     -Wall     -Wextra     -Wpedantic     -Werror     -I"$RESPONSE"     -I"$ROOT/include"     "$TMP/response_probe.c"     "$RESPONSE/inherited_response_types.c"     "$RESPONSE/inherited_response_response.c"     "$ROOT/src/aether_meta_runtime.c"     -o "$TMP/response_probe"

"$TMP/response_probe"

echo "JAVA/C INHERITED AUX EMITTERS TEST PASSED"

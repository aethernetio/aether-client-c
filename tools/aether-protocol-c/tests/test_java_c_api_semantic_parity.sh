#!/usr/bin/env bash
set -euo pipefail

ROOT=/home/angel/projects/aether-client-c
AETHER=/home/angel/projects/aether
BUILD="$ROOT/build-adsl-generator-release"
GEN="$BUILD/aether-adslc"
CC_BIN="${CC:-cc}"

fail() {
    echo "JAVA/C API SEMANTIC PARITY TEST FAILED: $*" >&2
    exit 1
}

compile_generated_dir() {
    local dir="$1"
    local found=0

    for src in "$dir"/*.c; do
        [[ -f "$src" ]] || continue
        found=1

        "$CC_BIN"             -std=c11             -Wall             -Wextra             -Wpedantic             -Werror             -I"$dir"             -I"$ROOT/include"             -c "$src"             -o "$dir/$(basename "${src%.c}").o"
    done

    [[ "$found" -eq 1 ]] || fail "no generated C sources in $dir"
}

cmake --build "$BUILD" -j2 >/dev/null

[[ -x "$GEN" ]] || fail "generator binary not found: $GEN"

TMP="$(mktemp -d /tmp/aether-java-c-api-semantics.XXXXXX)"
trap 'rm -rf "$TMP"' EXIT

FIXTURE="$TMP/JavaApiSemantics.adsl.yaml"

cat > "$FIXTURE" <<'ADSL'
types:
  DeclaredStream:
    stream:
      api: InnerApi

api:
  ChildApi:
    parents:
      - LeftApi
      - RightApi
    methods:
      register:
        params:
          stream: DeclaredStream

  LeftApi:
    parents:
      - RootApi
    methods:
      duplicate:
        id: 7
        params:
          value: byte

  RightApi:
    parents:
      - RootApi
    methods:
      duplicate:
        id: 99
        params:
          value: byte

  RootApi:
    methods:
      base:

  InnerApi:
    methods:
      ping:
ADSL

REMOTE="$TMP/remote"
LOCAL="$TMP/local"

mkdir -p "$REMOTE" "$LOCAL"

"$GEN"     --api ChildApi     "$FIXTURE"     "$REMOTE"     JavaApiSemantics

"$GEN"     --local-api ChildApi     "$FIXTURE"     "$LOCAL"     JavaApiSemantics

compile_generated_dir "$REMOTE"
compile_generated_dir "$LOCAL"

cat > "$TMP/remote_probe.c" <<'C'
#include "java_api_semantics_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[128];
    size_t length;
} capture_t;

static aether_status_t capture_send(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    capture_t *capture = (capture_t *)ctx;

    assert(capture != NULL);
    assert(length <= sizeof(capture->data));

    memcpy(capture->data, data, length);
    capture->length = length;

    return AETHER_OK;
}

int main(void) {
    uint8_t tx[128] = {0};
    capture_t capture = {0};

    child_api_remote_t api = {
        .send_ctx = &capture,
        .send = capture_send,
        .tx = tx,
        .tx_capacity = sizeof(tx)
    };

    assert(child_api_base(&api) == AETHER_OK);
    assert(capture.length >= 1u);
    assert(capture.data[0] == UINT8_C(3));

    capture.length = 0u;

    assert(
        child_api_duplicate(
            &api,
            INT8_C(42)) ==
        AETHER_OK);

    assert(capture.length >= 1u);
    assert(capture.data[0] == UINT8_C(7));

    uint8_t payload[] = {
        0xa1u,
        0xb2u,
        0xc3u
    };

    declared_stream_t stream = {
        .data = payload,
        .capacity = sizeof(payload),
        .length = sizeof(payload)
    };

    capture.length = 0u;

    assert(
        child_api_register(
            &api,
            &stream) ==
        AETHER_OK);

    assert(capture.length >= 1u);
    assert(capture.data[0] == UINT8_C(5));

    return 0;
}
C

"$CC_BIN"     -std=c11     -Wall     -Wextra     -Wpedantic     -Werror     -I"$REMOTE"     -I"$ROOT/include"     "$TMP/remote_probe.c"     "$REMOTE/java_api_semantics_types.c"     "$REMOTE/java_api_semantics_api.c"     "$ROOT/src/aether_meta_runtime.c"     -o "$TMP/remote_probe"

"$TMP/remote_probe"

cat > "$TMP/local_probe.c" <<'C'
#include "java_api_semantics_local.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    unsigned int base_calls;
    unsigned int duplicate_calls;
    int8_t duplicate_value;
} capture_t;

static aether_status_t on_base(void *ctx) {
    capture_t *capture = (capture_t *)ctx;

    assert(capture != NULL);

    capture->base_calls++;

    return AETHER_OK;
}

static aether_status_t on_duplicate(
    void *ctx,
    int8_t value) {

    capture_t *capture = (capture_t *)ctx;

    assert(capture != NULL);

    capture->duplicate_calls++;
    capture->duplicate_value = value;

    return AETHER_OK;
}

int main(void) {
    capture_t capture = {0};

    child_api_local_t api = {0};

    api.ctx = &capture;
    api.base = on_base;
    api.duplicate = on_duplicate;

    static const uint8_t frame[] = {
        3u,
        7u,
        42u
    };

    aether_meta_read_context_t decode = {0};

    decode.reader.data = frame;
    decode.reader.length = sizeof(frame);

    assert(
        child_api_dispatch(
            &api,
            &decode) ==
        AETHER_OK);

    assert(capture.base_calls == 1u);
    assert(decode.reader.position == 1u);

    assert(
        child_api_dispatch(
            &api,
            &decode) ==
        AETHER_OK);

    assert(capture.duplicate_calls == 1u);
    assert(capture.duplicate_value == INT8_C(42));
    assert(decode.reader.position == sizeof(frame));

    return 0;
}
C

"$CC_BIN"     -std=c11     -Wall     -Wextra     -Wpedantic     -Werror     -I"$LOCAL"     -I"$ROOT/include"     "$TMP/local_probe.c"     "$LOCAL/java_api_semantics_types.c"     "$LOCAL/java_api_semantics_local.c"     "$ROOT/src/aether_meta_runtime.c"     -o "$TMP/local_probe"

"$TMP/local_probe"

AUTH="$AETHER/auth-service/src/main/resources/AuthDsl.adsl.yaml"

for api in LoginPasswordApi GoogleApi; do
    out="$TMP/auth_${api}"

    mkdir -p "$out"

    "$GEN"         --local-api "$api"         "$AUTH"         "$out"         AuthDsl

    compile_generated_dir "$out"
done

echo "JAVA/C API SEMANTIC PARITY TEST PASSED"

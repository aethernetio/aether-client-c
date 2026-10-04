#!/usr/bin/env bash
set -euo pipefail

ROOT=/home/angel/projects/aether-client-c
BUILD="$ROOT/build-adsl-generator-release"
GEN="$BUILD/aether-adslc"
CC_BIN="${CC:-cc}"

fail() {
    echo "JAVA/C PARENT CONTEXT AUTO-ID TEST FAILED: $*" >&2
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

TMP="$(mktemp -d /tmp/aether-parent-context-auto-id.XXXXXX)"
trap 'rm -rf "$TMP"' EXIT

FIXTURE="$TMP/ParentContext.adsl.yaml"

cat > "$FIXTURE" <<'ADSL'
api:
  ChildApi:
    parents:
      - LeftApi
      - RightApi
    methods:
      child:

  LeftApi:
    methods:
      left:

  RightApi:
    methods:
      right:
ADSL

RIGHT="$TMP/right"
CHILD="$TMP/child"
LOCAL="$TMP/local"

mkdir -p "$RIGHT" "$CHILD" "$LOCAL"

"$GEN"     --api RightApi     "$FIXTURE"     "$RIGHT"     ParentContext

"$GEN"     --api ChildApi     "$FIXTURE"     "$CHILD"     ParentContext

"$GEN"     --local-api ChildApi     "$FIXTURE"     "$LOCAL"     ParentContext

compile_dir "$RIGHT"
compile_dir "$CHILD"
compile_dir "$LOCAL"

cat > "$TMP/right_probe.c" <<'C'
#include "parent_context_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[16];
    size_t length;
} capture_t;

static aether_status_t capture_send(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    capture_t *capture =
        (capture_t *)ctx;

    assert(capture != NULL);
    assert(length <= sizeof(capture->data));

    memcpy(
        capture->data,
        data,
        length);

    capture->length =
        length;

    return AETHER_OK;
}

int main(void) {
    uint8_t tx[16] = {0};
    capture_t capture = {0};

    right_api_remote_t api = {
        .send_ctx = &capture,
        .send = capture_send,
        .tx = tx,
        .tx_capacity = sizeof(tx)
    };

    assert(
        right_api_right(&api) ==
        AETHER_OK);

    assert(capture.length >= 1u);
    assert(capture.data[0] == UINT8_C(3));

    return 0;
}
C

"$CC_BIN"     -std=c11     -Wall     -Wextra     -Wpedantic     -Werror     -I"$RIGHT"     -I"$ROOT/include"     "$TMP/right_probe.c"     "$RIGHT/parent_context_types.c"     "$RIGHT/parent_context_api.c"     "$ROOT/src/aether_meta_runtime.c"     -o "$TMP/right_probe"

"$TMP/right_probe"

cat > "$TMP/child_probe.c" <<'C'
#include "parent_context_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t data[16];
    size_t length;
} capture_t;

static aether_status_t capture_send(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    capture_t *capture =
        (capture_t *)ctx;

    assert(capture != NULL);
    assert(length <= sizeof(capture->data));

    memcpy(
        capture->data,
        data,
        length);

    capture->length =
        length;

    return AETHER_OK;
}

static void assert_command(
    capture_t *capture,
    aether_status_t status,
    uint8_t expected) {

    assert(status == AETHER_OK);
    assert(capture->length >= 1u);
    assert(capture->data[0] == expected);
}

int main(void) {
    uint8_t tx[16] = {0};
    capture_t capture = {0};

    child_api_remote_t api = {
        .send_ctx = &capture,
        .send = capture_send,
        .tx = tx,
        .tx_capacity = sizeof(tx)
    };

    capture.length = 0u;

    assert_command(
        &capture,
        child_api_left(&api),
        UINT8_C(3));

    capture.length = 0u;

    assert_command(
        &capture,
        child_api_right(&api),
        UINT8_C(4));

    capture.length = 0u;

    assert_command(
        &capture,
        child_api_child(&api),
        UINT8_C(5));

    return 0;
}
C

"$CC_BIN"     -std=c11     -Wall     -Wextra     -Wpedantic     -Werror     -I"$CHILD"     -I"$ROOT/include"     "$TMP/child_probe.c"     "$CHILD/parent_context_types.c"     "$CHILD/parent_context_api.c"     "$ROOT/src/aether_meta_runtime.c"     -o "$TMP/child_probe"

"$TMP/child_probe"

cat > "$TMP/local_probe.c" <<'C'
#include "parent_context_local.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    unsigned int left_calls;
    unsigned int right_calls;
    unsigned int child_calls;
} capture_t;

static aether_status_t on_left(void *ctx) {
    capture_t *capture =
        (capture_t *)ctx;

    assert(capture != NULL);

    capture->left_calls++;

    return AETHER_OK;
}

static aether_status_t on_right(void *ctx) {
    capture_t *capture =
        (capture_t *)ctx;

    assert(capture != NULL);

    capture->right_calls++;

    return AETHER_OK;
}

static aether_status_t on_child(void *ctx) {
    capture_t *capture =
        (capture_t *)ctx;

    assert(capture != NULL);

    capture->child_calls++;

    return AETHER_OK;
}

int main(void) {
    capture_t capture = {0};

    child_api_local_t api = {0};

    api.ctx = &capture;
    api.left = on_left;
    api.right = on_right;
    api.child = on_child;

    static const uint8_t frame[] = {
        3u,
        4u,
        5u
    };

    aether_meta_read_context_t decode = {0};

    decode.reader.data =
        frame;

    decode.reader.length =
        sizeof(frame);

    assert(
        child_api_dispatch(
            &api,
            &decode) ==
        AETHER_OK);

    assert(capture.left_calls == 1u);
    assert(decode.reader.position == 1u);

    assert(
        child_api_dispatch(
            &api,
            &decode) ==
        AETHER_OK);

    assert(capture.right_calls == 1u);
    assert(decode.reader.position == 2u);

    assert(
        child_api_dispatch(
            &api,
            &decode) ==
        AETHER_OK);

    assert(capture.child_calls == 1u);
    assert(decode.reader.position == 3u);

    return 0;
}
C

"$CC_BIN"     -std=c11     -Wall     -Wextra     -Wpedantic     -Werror     -I"$LOCAL"     -I"$ROOT/include"     "$TMP/local_probe.c"     "$LOCAL/parent_context_types.c"     "$LOCAL/parent_context_local.c"     "$ROOT/src/aether_meta_runtime.c"     -o "$TMP/local_probe"

"$TMP/local_probe"

echo "JAVA/C PARENT CONTEXT AUTO-ID TEST PASSED"

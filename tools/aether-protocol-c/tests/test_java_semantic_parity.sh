#!/usr/bin/env bash
set -euo pipefail

ROOT=/home/angel/projects/aether-client-c
BUILD="$ROOT/build-adsl-generator-release"
GEN="$BUILD/aether-adslc"
CC_BIN="${CC:-cc}"

fail() {
    echo "JAVA SEMANTIC PARITY TEST FAILED: $*" >&2
    exit 1
}

compile_generated_dir() {
    local dir="$1"
    local found=0

    for src in "$dir"/*.c; do
        if [[ ! -f "$src" ]]; then
            continue
        fi

        found=1

        "$CC_BIN"             -std=c11             -Wall             -Wextra             -Wpedantic             -Werror             -I"$dir"             -I"$ROOT/include"             -c "$src"             -o "$dir/$(basename "${src%.c}").o"
    done

    [[ "$found" -eq 1 ]] ||
        fail "no generated C sources in $dir"
}

cmake --build "$BUILD" -j2 >/dev/null

[[ -x "$GEN" ]] ||
    fail "generator binary not found: $GEN"

TMP="$(mktemp -d /tmp/aether-java-semantic-parity.XXXXXX)"
trap 'rm -rf "$TMP"' EXIT


###############################################################################
# Java ApiGenerator semantic reference
#
# This test intentionally does NOT encode the current C generator
# implementation. It encodes externally visible semantics of the Java
# reference generator:
#
#   - API inheritance is parent-first.
#   - Duplicate inherited method names are added once.
#   - Automatic command IDs are based on the effective method list.
#   - API declaration order must not affect those semantics.
#   - Declared stream parameters are stream parameters just like anonymous
#     stream parameters.
#   - C-target identifiers must remain valid C without changing protocol names.
#
# MCU representation may differ from Java, but the effective API, command IDs
# and wire bytes must not.
###############################################################################

FIXTURE="$TMP/SemanticParity.adsl.yaml"

cat > "$FIXTURE" <<'ADSL'
types:
  DeclaredStream:
    stream:
      api: StreamInnerApi

api:
  # Deliberately declared before its parents. A valid schema must not depend
  # on API declaration order.
  ChildApi:
    parents:
      - LeftApi
      - RightApi

    methods:
      # "register" is deliberately a C keyword. The C target must choose a
      # legal C member identifier without changing the ADSL method name or
      # command ID.
      register:
        params:
          stream: DeclaredStream

  LeftApi:
    parents:
      - RootApi

  RightApi:
    parents:
      - RootApi

  RootApi:
    methods:
      base:

  StreamInnerApi:
    methods:
      ping:
ADSL


REMOTE="$TMP/remote"
LOCAL="$TMP/local"
METHOD="$TMP/method"

mkdir -p     "$REMOTE"     "$LOCAL"     "$METHOD"


###############################################################################
# Generation itself is part of the test.
#
# Java getAllMethods() semantics for ChildApi are:
#
#   RootApi.base       -> id 3
#   ChildApi.register  -> id 4
#
# RootApi.base reaches ChildApi through both LeftApi and RightApi, but Java
# deduplicates by method name before assigning the child method's automatic ID.
#
# Therefore register MUST be 4, never 5.
###############################################################################

"$GEN"     --api ChildApi     "$FIXTURE"     "$REMOTE"     SemanticParity

"$GEN"     --local-api ChildApi     "$FIXTURE"     "$LOCAL"     SemanticParity

# Selected inherited methods must be visible through the child API as well.
"$GEN"     --method ChildApi.base     "$FIXTURE"     "$METHOD"     SemanticInherited


###############################################################################
# Generated code must be valid strict C.
#
# This catches, among other things:
#
#   - raw `register` used as a struct member;
#   - declared stream callback ABI disagreement;
#   - malformed inherited declarations;
#   - duplicate emitted inherited declarations.
###############################################################################

compile_generated_dir "$REMOTE"
compile_generated_dir "$LOCAL"
compile_generated_dir "$METHOD"


###############################################################################
# Remote API runtime test.
#
# This checks actual command bytes rather than grepping implementation text.
###############################################################################

cat > "$TMP/remote_probe.c" <<'C'
#include "semantic_parity_api.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


typedef struct {
    uint8_t data[64];
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
    uint8_t tx[64] = {0};

    capture_t capture = {0};

    child_api_remote_t api = {
        .send_ctx = &capture,
        .send = capture_send,
        .tx = tx,
        .tx_capacity = sizeof(tx)
    };


    /*
     * RootApi.base is inherited through a diamond.
     *
     * It must be flattened into ChildApi exactly once and retain command 3.
     */
    assert(
        child_api_base(
            &api) ==
        AETHER_OK);

    assert(capture.length >= 1u);
    assert(capture.data[0] == UINT8_C(3));


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


    /*
     * Java ApiGenerator.getAllMethods():
     *
     *   Left -> Root.base      adds base, result size = 1
     *   Right -> Root.base     duplicate name, does not add
     *   Child.register        automatic id = size + 3 = 4
     *
     * A raw recursive method count would incorrectly produce 5.
     *
     * Calling this method also proves that a declared stream type has the
     * normal stream-pointer remote ABI.
     */
    capture.length = 0u;

    assert(
        child_api_register(
            &api,
            &stream) ==
        AETHER_OK);

    assert(capture.length >= 1u);
    assert(capture.data[0] == UINT8_C(4));


    return 0;
}
C


"$CC_BIN"     -std=c11     -Wall     -Wextra     -Wpedantic     -Werror     -I"$REMOTE"     -I"$ROOT/include"     "$TMP/remote_probe.c"     "$REMOTE/semantic_parity_types.c"     "$REMOTE/semantic_parity_api.c"     "$ROOT/src/aether_meta_runtime.c"     -o "$TMP/remote_probe"

"$TMP/remote_probe"


###############################################################################
# Local API runtime test.
#
# Compiling semantic_parity_local.h/.c is itself important:
#
#   1. the method named `register` must not become an illegal raw C identifier;
#
#   2. DeclaredStream must have one consistent pointer ABI across:
#        callback declaration
#        local decoded value
#        callback invocation
#
# We intentionally do not depend on HOW the keyword is escaped.
###############################################################################

cat > "$TMP/local_probe.c" <<'C'
#include "semantic_parity_local.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>


static aether_status_t on_base(
    void *ctx) {

    unsigned int *calls =
        (unsigned int *)ctx;

    assert(calls != NULL);

    (*calls)++;

    return AETHER_OK;
}


int main(void) {
    unsigned int calls = 0u;

    child_api_local_t api = {0};

    api.ctx =
        &calls;

    api.base =
        on_base;


    static const uint8_t frame[] = {
        3u
    };

    aether_meta_read_context_t decode = {0};

    decode.reader.data =
        frame;

    decode.reader.length =
        sizeof(frame);


    /*
     * Inherited command 3 must dispatch through ChildApi.
     */
    assert(
        child_api_dispatch(
            &api,
            &decode) ==
        AETHER_OK);

    assert(
        decode.reader.position ==
        sizeof(frame));

    assert(calls == 1u);


    return 0;
}
C


"$CC_BIN"     -std=c11     -Wall     -Wextra     -Wpedantic     -Werror     -I"$LOCAL"     -I"$ROOT/include"     "$TMP/local_probe.c"     "$LOCAL/semantic_parity_types.c"     "$LOCAL/semantic_parity_local.c"     "$ROOT/src/aether_meta_runtime.c"     -o "$TMP/local_probe"

"$TMP/local_probe"


###############################################################################
# Full method semantics are required for inheritance deduplication.
#
# Equal method name + id + number of stream entries is NOT sufficient.
# Stream definition itself is protocol-significant.
#
# ParentA.same and ParentB.same deliberately have the same visible method
# metadata but point their stream to different APIs.
#
# They are NOT the same inherited method and Child must be rejected as
# ambiguous/conflicting.
###############################################################################

CONFLICT="$TMP/StreamConflict.adsl.yaml"

cat > "$CONFLICT" <<'ADSL'
api:
  ConflictChild:
    parents:
      - ParentA
      - ParentB

  ParentA:
    methods:
      same:
        id: 3
        params:
          channel:
            name: SharedStream
            stream:
              api: InnerA

  ParentB:
    methods:
      same:
        id: 3
        params:
          channel:
            name: SharedStream
            stream:
              api: InnerB

  InnerA:
    methods:
      ping:
        id: 3

  InnerB:
    methods:
      ping:
        id: 3
ADSL


mkdir -p "$TMP/conflict"


if "$GEN"         --api ConflictChild         "$CONFLICT"         "$TMP/conflict"         Conflict         >"$TMP/conflict.out"         2>"$TMP/conflict.err"; then

    fail         "inherited methods with different stream semantics were incorrectly deduplicated"
fi



# The generator must reject this because of method/inheritance
# semantics, not because it crashed or interpreted the CLI incorrectly.

if ! grep -Eqi         'conflict|inherit|effective|method'         "$TMP/conflict.err"; then

    echo "unexpected conflict diagnostic:" >&2

    cat         "$TMP/conflict.err"         >&2

    fail         "stream semantic conflict was not diagnosed as an inheritance/method conflict"
fi


echo "JAVA SEMANTIC PARITY TEST PASSED"
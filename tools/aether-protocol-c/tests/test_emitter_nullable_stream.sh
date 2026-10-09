
#!/usr/bin/env bash
set -euo pipefail

GEN="${ADSL_TEST_GENERATOR:?generator binary not provided}"
FIXTURES="${ADSL_TEST_FIXTURES:?fixture directory not provided}"
RUNTIME_INCLUDE="${ADSL_TEST_RUNTIME_INCLUDE:?runtime include directory not provided}"
CC="${ADSL_TEST_CC:?C compiler not provided}"

fail() {
    echo "EMITTER NULLABLE-STREAM TEST FAILED: $*" >&2
    exit 1
}

FIXTURE="$FIXTURES/EmitterNullableStream.adsl.yaml"
[[ -f "$FIXTURE" ]] || fail "fixture not found: $FIXTURE"

TMP="$(mktemp -d /tmp/aether-emitter-nullable-stream.XXXXXX)"
trap 'rm -rf "$TMP"' EXIT

OUT="$TMP/out"
mkdir -p "$OUT"

# EMIT-A3, REGRESSION.
#
# A nullable inline stream parameter (`name: TestStream?`) must produce a
# self-consistent C type name: the declaration in the generated types header
# and the local dispatcher must agree on the stream typedef.
"$GEN" --local-api StreamApi "$FIXTURE" "$OUT" EmitterNullableStream \
    >/dev/null 2>"$TMP/gen.err" ||
    fail "generation failed: $(cat "$TMP/gen.err")"

LOCAL_C="$OUT/emitter_nullable_stream_local.c"
[[ -f "$LOCAL_C" ]] || fail "local source not generated"

# Compile the generated dispatcher against the generated headers and the
# shared runtime headers. This is a one-translation-unit compile
# (syntax + type checking), not a link step; it is sufficient to catch the
# name mismatch between 'test_stream__t' and 'test_stream_t'.
"$CC" -fsyntax-only \
    -I"$OUT" \
    -I"$RUNTIME_INCLUDE" \
    "$LOCAL_C" \
    2>"$TMP/cc.err" ||
    fail "generated local dispatcher does not compile: $(cat "$TMP/cc.err")"

exit 0

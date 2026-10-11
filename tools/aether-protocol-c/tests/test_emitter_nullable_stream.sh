
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
# self-consistent C type name: the stream typedef in the generated header
# and every reference in the generated translation unit must agree.
"$GEN" --input "$FIXTURE" "$OUT" \
    >/dev/null 2>"$TMP/gen.err" ||
    fail "generation failed: $(cat "$TMP/gen.err")"

API_C="$OUT/aether_api.c"
[[ -f "$API_C" ]] || fail "generated source not produced"

# Compile the generated translation unit against the shared runtime
# headers. This is a one-translation-unit syntax/type check; it is
# sufficient to catch a mismatch between 'test_stream__t' and
# 'test_stream_t'.
"$CC" -fsyntax-only \
    -I"$OUT" \
    -I"$RUNTIME_INCLUDE" \
    "$API_C" \
    2>"$TMP/cc.err" ||
    fail "generated source does not compile: $(cat "$TMP/cc.err")"


exit 0
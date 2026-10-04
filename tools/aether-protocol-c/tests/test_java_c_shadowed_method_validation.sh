#!/usr/bin/env bash
set -euo pipefail

ROOT=/home/angel/projects/aether-client-c
BUILD="$ROOT/build-adsl-generator-release"
GEN="$BUILD/aether-adslc"

fail() {
    echo "JAVA/C SHADOWED METHOD VALIDATION TEST FAILED: $*" >&2
    exit 1
}

cmake --build "$BUILD" -j2 >/dev/null

[[ -x "$GEN" ]] ||
    fail "generator binary not found: $GEN"

TMP="$(mktemp -d /tmp/aether-shadowed-method-validation.XXXXXX)"
trap 'rm -rf "$TMP"' EXIT

FIXTURE="$TMP/ShadowedMethod.adsl.yaml"
OUT="$TMP/out"

mkdir -p "$OUT"

cat > "$FIXTURE" <<'ADSL'
api:
  ChildApi:
    parents:
      - ParentApi
    methods:
      foo:
        params:
          bad: MissingType

  ParentApi:
    methods:
      foo:
        params:
          ok: byte
ADSL

if "$GEN"         --api ChildApi         "$FIXTURE"         "$OUT"         ShadowedMethod         >"$TMP/stdout"         2>"$TMP/stderr"; then

    fail "generator accepted an invalid shadowed method declaration"
fi

if ! grep -Eqi         'MissingType|unknown type'         "$TMP/stderr"; then

    echo "unexpected diagnostic:" >&2
    cat "$TMP/stderr" >&2

    fail "generator failed, but not because MissingType was rejected"
fi

echo "JAVA/C SHADOWED METHOD VALIDATION TEST PASSED"

#!/bin/sh
set -eu
cd "$(dirname "$0")"

CFLAGS_BASE="-std=c11 -O2 -Wall -Wextra -Wno-incompatible-function-pointer-types"

TARGET_DIR="${1:-.}"
BUILD_DIR="build"

compile_and_run() {
  src="$1"
  base="$(basename "${src%.c}")"
  out_bin="$2/$base.out"
  echo "[BUILD] $src"
  cc $CFLAGS "$src" -o "$out_bin"
  echo "[RUN]   $out_bin"
  "$out_bin" 2>&1 | sed 's/^/  /'
}

mkdir -p "$BUILD_DIR"
if [ -d "$TARGET_DIR" ]; then
  if [ "$TARGET_DIR" = "." ]; then INC="-I../src"; OUT="$BUILD_DIR"; PASS_OUT="$OUT"; else INC="-I../../src"; OUT="$BUILD_DIR/$TARGET_DIR"; PASS_OUT="../$OUT"; fi
  mkdir -p "$OUT"
  (cd "$TARGET_DIR" && CFLAGS="$CFLAGS_BASE $INC" && for f in test_*.c; do
     [ -f "$f" ] || continue
     compile_and_run "$f" "$PASS_OUT"
   done)
else
  mkdir -p "$BUILD_DIR"
  OUT="$BUILD_DIR"
  CFLAGS="$CFLAGS_BASE -I../src" compile_and_run "$TARGET_DIR" "$OUT"
fi

rm -rf "$BUILD_DIR"

echo "All tests executed."



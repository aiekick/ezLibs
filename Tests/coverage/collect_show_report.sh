#!/usr/bin/env bash
set -euo pipefail
[ "${VERBOSE:-0}" != "0" ] && set -x

BUILD_DIR="${1:?BUILD_DIR missing}"
OUT_DIR="${2:?OUT_DIR missing}"

OBJ_ARGS=$(
  { find "$BUILD_DIR" -type f -name '*.so*' -print0
    find "$BUILD_DIR" -type f -name '*.a'    -print0
    find "$BUILD_DIR" -type f -name '*.o'    -print0; } |
  xargs -0 -I{} printf ' -object %q' {}
)

mapfile -d '' EXES < <(find "$BUILD_DIR" -type f -executable -not -name '*.so*' -print0)
echo "[show] executables=${#EXES[@]}"

: > "$OUT_DIR/coverage.txt"
for exe in "${EXES[@]}"; do
  [[ "$exe" == *"/CMakeFiles/"* ]] && continue
  echo "===== \"$exe\" =====" >> "$OUT_DIR/coverage.txt"
  llvm-cov show "$exe" -instr-profile="$BUILD_DIR/default.profdata" $OBJ_ARGS \
    >> "$OUT_DIR/coverage.txt" 2>> "$OUT_DIR/coverage.txt" || true
done

: > "$OUT_DIR/report.txt"
echo "# profraw files: $(find "$BUILD_DIR"/profraw -name '*.profraw' | wc -l)" >> "$OUT_DIR/report.txt"
for exe in "${EXES[@]}"; do
  [[ "$exe" == *"/CMakeFiles/"* ]] && continue
  echo "===== \"$exe\" =====" >> "$OUT_DIR/report.txt"
  llvm-cov report "$exe" -instr-profile="$BUILD_DIR/default.profdata" $OBJ_ARGS \
    >> "$OUT_DIR/report.txt" 2>> "$OUT_DIR/report.txt" || true
  echo "" >> "$OUT_DIR/report.txt"
done

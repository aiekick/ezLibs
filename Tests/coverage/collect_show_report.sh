#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:?usage: collect_show_report.sh BUILD_DIR OUT_DIR}"
OUT_DIR="${2:?usage: collect_show_report.sh BUILD_DIR OUT_DIR}"

COV_TXT="${OUT_DIR}/coverage.txt"
RPT_TXT="${OUT_DIR}/report.txt"
OUT_PROFDATA="${BUILD_DIR}/default.profdata"

# Ne garder que Tests/Test* et ignorer coverage/forceCover/CMakeFiles
mapfile -d '' EXES < <(
  find "${BUILD_DIR}/Tests" \
    \( -path "${BUILD_DIR}/Tests/coverage" -o -path "${BUILD_DIR}/Tests/coverage/*" \
       -o -path "${BUILD_DIR}/Tests/forceCover" -o -path "${BUILD_DIR}/Tests/forceCover/*" \
       -o -path "${BUILD_DIR}/Tests/CMakeFiles" -o -path "${BUILD_DIR}/Tests/CMakeFiles/*" \) -prune -o \
    -type f -executable -not -name '*.so*' \
    -path "${BUILD_DIR}/Tests/Test*" \
    -print0
)
echo "[show] executables=${#EXES[@]}"

# OBJ mapping
OBJ_ARGS=$(
  { find "${BUILD_DIR}" -type f -name '*.so*' -print0
    find "${BUILD_DIR}" -type f -name '*.a'    -print0
    find "${BUILD_DIR}" -type f -name '*.o'    -print0; } |
  xargs -0 -I{} printf ' -object %q' {}
)

# Filtre agressif (LLVM/Clang/STL/3rdparty/ CMakeFiles)
IGNORE_RE='(^/usr/|^/opt/|.*/(llvm|clang)/|.*/include/c\+\+/|.*/libc\+\+/|.*/third_party/|.*/CMakeFiles/)'

: > "${COV_TXT}"
for exe in "${EXES[@]}"; do
  [[ "$exe" == *"/CMakeFiles/"* ]] && continue
  echo "===== \"$exe\" =====" >> "${COV_TXT}"
  llvm-cov show "$exe" -instr-profile="${OUT_PROFDATA}" \
    -ignore-filename-regex="${IGNORE_RE}" ${OBJ_ARGS} \
    >> "${COV_TXT}" 2>> "${COV_TXT}" || true
done

: > "${RPT_TXT}"
echo "# profraw files: $(find "${BUILD_DIR}/profraw" -type f -name '*.profraw' | wc -l)" >> "${RPT_TXT}"
for exe in "${EXES[@]}"; do
  [[ "$exe" == *"/CMakeFiles/"* ]] && continue
  echo "" >> "${RPT_TXT}"
  echo "===== \"$exe\" =====" >> "${RPT_TXT}"
  llvm-cov report "$exe" -instr-profile="${OUT_PROFDATA}" \
    -ignore-filename-regex="${IGNORE_RE}" ${OBJ_ARGS} \
    >> "${RPT_TXT}" 2>> "${RPT_TXT}" || true
done

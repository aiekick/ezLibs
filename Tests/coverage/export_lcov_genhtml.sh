#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:?usage: export_lcov_genhtml.sh BUILD_DIR LCOV_INFO LCOV_HTML_DIR [INCLUDE_ONLY...paths]}"
LCOV_INFO="${2:?usage: export_lcov_genhtml.sh BUILD_DIR LCOV_INFO LCOV_HTML_DIR [INCLUDE_ONLY...]}"
LCOV_HTML="${3:?usage: export_lcov_genhtml.sh BUILD_DIR LCOV_INFO LCOV_HTML_DIR [INCLUDE_ONLY...]}"
shift 3
INCLUDE_ONLY=( "$@" )  # zéro, un ou plusieurs répertoires à extraire (pattern /* ajouté)

OUT_PROFDATA="${BUILD_DIR}/default.profdata"

# Limiter aux exe sous Tests/Test* et ignorer coverage/forceCover/CMakeFiles
mapfile -d '' EXES < <(
  find "${BUILD_DIR}/Tests" \
    \( -path "${BUILD_DIR}/Tests/coverage" -o -path "${BUILD_DIR}/Tests/coverage/*" \
       -o -path "${BUILD_DIR}/Tests/forceCover" -o -path "${BUILD_DIR}/Tests/forceCover/*" \
       -o -path "${BUILD_DIR}/Tests/CMakeFiles" -o -path "${BUILD_DIR}/Tests/CMakeFiles/*" \) -prune -o \
    -type f -executable -not -name '*.so*' \
    -path "${BUILD_DIR}/Tests/Test*" \
    -print0
)
echo "[lcov] executables=${#EXES[@]}"

IGNORE_RE='(^/usr/|^/opt/|.*/(llvm|clang)/|.*/include/c\+\+/|.*/libc\+\+/|.*/third_party/|.*/CMakeFiles/)'

: > "${LCOV_INFO}"
for exe in "${EXES[@]}"; do
  echo "[lcov] export ${exe}"
  llvm-cov export "${exe}" \
    -instr-profile="${OUT_PROFDATA}" \
    -ignore-filename-regex="${IGNORE_RE}" \
    -format=lcov >> "${LCOV_INFO}" 2>/dev/null || true
done

# Filet de sécurité: remove chemins systèmes/outils
EXCLUDE_GLOBS=(
  "/usr/*" "*/lib/*/clang/*" "*/lib/llvm-*/*" "*/include/c++/*"
  "*/llvm/*" "*/clang/*" "*/third_party/*" "*/CMakeFiles/*"
)
lcov --quiet --rc lcov_branch_coverage=1 \
     --remove "${LCOV_INFO}" "${EXCLUDE_GLOBS[@]}" \
     --output-file "${LCOV_INFO}.tmp" || true
if grep -q '^SF:' "${LCOV_INFO}.tmp"; then
  mv "${LCOV_INFO}.tmp" "${LCOV_INFO}"
else
  rm -f "${LCOV_INFO}.tmp"
fi

# Optionnel: extraire uniquement tes sources publiques
if ((${#INCLUDE_ONLY[@]})); then
  PATTERNS=()
  for p in "${INCLUDE_ONLY[@]}"; do
    PATTERNS+=( "${p%/}/*" )
  done
  echo "[lcov] extract only: ${PATTERNS[*]}"
  if lcov --quiet --rc lcov_branch_coverage=1 \
          --extract "${LCOV_INFO}" "${PATTERNS[@]}" \
          --output-file "${LCOV_INFO}.filtered" && \
     grep -q '^SF:' "${LCOV_INFO}.filtered"; then
    mv "${LCOV_INFO}.filtered" "${LCOV_INFO}"
  else
    rm -f "${LCOV_INFO}.filtered"
    echo "[lcov] extract a échoué, on continue avec le tracefile complet (pas filtré)."
  fi
fi

rm -rf "${LCOV_HTML}"
genhtml "${LCOV_INFO}" \
  --output-directory "${LCOV_HTML}" \
  --legend --branch-coverage --function-coverage --demangle-cpp \
  --title "Coverage (llvm-cov → lcov → genhtml)"
echo "[lcov] HTML : ${LCOV_HTML}/index.html"

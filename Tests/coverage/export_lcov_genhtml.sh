#!/usr/bin/env bash
set -euo pipefail
[ "${VERBOSE:-0}" != "0" ] && set -x

# Usage: export_lcov_genhtml.sh <BUILD_DIR> <OUT_DIR> [INCLUDE_ONLY1] ...
BUILD_DIR="${1:?BUILD_DIR missing}"
OUT_DIR="${2:?OUT_DIR missing}"
shift 2
INCLUDE_ONLY=("$@")

LCOV_INFO="${OUT_DIR}/lcov.info"
LCOV_HTML="${OUT_DIR}/coverage_lcov"

command -v llvm-cov >/dev/null || { echo "llvm-cov introuvable"; exit 1; }
command -v llvm-profdata >/dev/null || { echo "llvm-profdata introuvable"; exit 1; }
command -v genhtml >/dev/null || { echo "genhtml (lcov) introuvable"; exit 1; }
command -v lcov >/dev/null || { echo "lcov introuvable"; exit 1; }
[ -f "${BUILD_DIR}/default.profdata" ] || { echo "default.profdata manquant dans ${BUILD_DIR}"; exit 1; }

# Détection support --branch-coverage
if llvm-cov export --help 2>&1 | grep -q -- '--branch-coverage'; then
  BRANCH_FLAG="--branch-coverage"
else
  BRANCH_FLAG=""
  echo "[lcov] NOTE: llvm-cov ne supporte pas --branch-coverage ici; on exporte sans."
fi

# Construit -object …
OBJ_ARGS=$(
  { find "${BUILD_DIR}" -type f -name '*.so*' -print0
    find "${BUILD_DIR}" -type f -name '*.a'    -print0
    find "${BUILD_DIR}" -type f -name '*.o'    -print0; } |
  xargs -0 -I{} printf ' -object %q' {}
)

mapfile -d '' EXES < <(find "${BUILD_DIR}" -type f -executable -not -name '*.so*' -print0)
: > "${LCOV_INFO}"
tmpdir="$(mktemp -d)"
kept=0
skipped=0

for exe in "${EXES[@]}"; do
  echo "[lcov] export ${exe}"
  tmp="${tmpdir}/$(basename "${exe}").info"
  if llvm-cov export "${exe}" \
       -instr-profile="${BUILD_DIR}/default.profdata" \
       ${OBJ_ARGS} \
       --format=lcov \
       ${BRANCH_FLAG} \
       > "${tmp}" 2>&1; then
    :
  else
    echo "[lcov] ERROR: export a échoué pour ${exe}"
    skipped=$((skipped+1))
    continue
  fi
  if grep -q '^SF:' "${tmp}"; then
    cat "${tmp}" >> "${LCOV_INFO}"
    kept=$((kept+1))
  else
    echo "[lcov] WARN: aucun SF dans ${exe}"
    skipped=$((skipped+1))
  fi
done
rm -rf "${tmpdir}"
echo "[lcov] exports gardés: ${kept}, ignorés: ${skipped}"

if ! grep -q '^SF:' "${LCOV_INFO}"; then
  echo "[lcov] ERREUR: lcov.info est vide (aucun SF)."; exit 1
fi

# Remove ce qu'on ne veut pas
EXCLUDE_GLOBS=( "*/Tests/*" "*/CMakeFiles/*" )
lcov --quiet --rc --branch-coverage --mcdc-coverage --ignore-errors mismatch,format \
     --remove "${LCOV_INFO}" "${EXCLUDE_GLOBS[@]}" \
     --output-file "${LCOV_INFO}.tmp" || true
if grep -q '^SF:' "${LCOV_INFO}.tmp"; then
  mv "${LCOV_INFO}.tmp" "${LCOV_INFO}"
else
  rm -f "${LCOV_INFO}.tmp"
fi

# Extract: ne garder que include/ (si fourni)
if ((${#INCLUDE_ONLY[@]})); then
  PATTERNS=()
  for p in "${INCLUDE_ONLY[@]}"; do PATTERNS+=( "${p%/}/*" ); done
  echo "[lcov] extract only: ${PATTERNS[*]}"
  if lcov --quiet --rc --branch-coverage --mcdc-coverage --ignore-errors mismatch,format \
          --extract "${LCOV_INFO}" "${PATTERNS[@]}" \
          --output-file "${LCOV_INFO}.filtered" && \
     grep -q '^SF:' "${LCOV_INFO}.filtered"; then
    mv "${LCOV_INFO}.filtered" "${LCOV_INFO}"
  else
    echo "[lcov] WARN: extract vide; on conserve le tracefile post-remove."
    rm -f "${LCOV_INFO}.filtered"
  fi
fi

# HTML LCOV (branches+fonctions si dispo) 
rm -rf "${LCOV_HTML}"
genhtml "${LCOV_INFO}" \
  --output-directory "${LCOV_HTML}" \
  --legend \
  --branch-coverage \
  --function-coverage \
  --demangle-cpp \
  --title "Coverage (llvm-cov → genhtml, filtered)"
echo "[lcov] HTML : ${LCOV_HTML}/index.html"

#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:?usage: merge_profraw.sh BUILD_DIR OUT_PROFDATA}"
OUT_PROFDATA="${2:?usage: merge_profraw.sh BUILD_DIR OUT_PROFDATA}"
PROFRAW_DIR="${BUILD_DIR}/profraw"

mapfile -d '' FILES < <(find "${PROFRAW_DIR}" -type f -name '*.profraw' -print0 || true)
echo "[merge_profraw] DIR=${PROFRAW_DIR} files=${#FILES[@]}"
if ((${#FILES[@]}==0)); then
  echo "[merge_profraw] Aucun .profraw"
  exit 1
fi

llvm-profdata merge -o "${OUT_PROFDATA}" "${FILES[@]}"
echo "[merge_profraw] OK -> ${OUT_PROFDATA} (${#FILES[@]} fichiers)"

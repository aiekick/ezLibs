#!/usr/bin/env bash
set -euo pipefail
[ "${VERBOSE:-0}" != "0" ] && set -x

BUILD_DIR="${1:?BUILD_DIR missing}"
OUT_PROFDATA="${2:?OUT_PROFDATA missing}"
PROFRAW_DIR="${BUILD_DIR}/profraw"

mapfile -d '' FILES < <(find "${PROFRAW_DIR}" -type f -name '*.profraw' -print0)
echo "[merge_profraw] DIR=${PROFRAW_DIR} files=${#FILES[@]}"

if ((${#FILES[@]}==0)); then
  echo "[merge_profraw] Aucun .profraw"; exit 1
fi

llvm-profdata merge -sparse "${FILES[@]}" -o "${OUT_PROFDATA}"
echo "[merge_profraw] OK -> ${OUT_PROFDATA} (${#FILES[@]} fichiers

llvm-profdata diagnose "${OUT_PROFDATA}"


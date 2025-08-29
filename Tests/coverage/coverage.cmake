# LLVM source-based coverage (Linux + Clang)
# À inclure depuis Tests/CMakeLists.txt quand USE_CODE_COVERAGE=ON

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "coverage: nécessite Clang (LLVM source-based coverage).")
endif()

# Verbosité pipeline
option(COVERAGE_VERBOSE "Verbosity des scripts de coverage" ON)

# --- Dossiers & scripts
set(COV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/coverage")
set(FC_MIRROR_DIR "${CMAKE_BINARY_DIR}/fc_headers")
set(OUT_DIR "${CMAKE_BINARY_DIR}/Tests")

file(MAKE_DIRECTORY "${OUT_DIR}")
include_directories(BEFORE "${FC_MIRROR_DIR}" "${CMAKE_SOURCE_DIR}/include")

set(SH_MERGE_PROFRAW        "${COV_DIR}/merge_profraw.sh")
set(SH_COLLECT_SHOW_REPORT  "${COV_DIR}/collect_show_report.sh")
set(SH_EXPORT_LCOV_GENHTML  "${COV_DIR}/export_lcov_genhtml.sh")
set(CM_RUN_FC_HEADERS       "${COV_DIR}/run_force_cover_headers.cmake")
set(PY_UTF8                 "${COV_DIR}/convert_to_utf8.py")
set(PY_PATCH_INLINE         "${COV_DIR}/patch_genhtml_forcecover_inline.py") # optionnel

# --- Entrées force_cover (défauts si non passés par -D)
if(NOT DEFINED FORCE_COVER_EXECUTABLE)
  set(FORCE_COVER_EXECUTABLE "${CMAKE_BINARY_DIR}/Tests/coverage/forceCover/forceCover")
endif()
if(NOT DEFINED FIX_COVERAGE_PY)
  set(FIX_COVERAGE_PY "${CMAKE_SOURCE_DIR}/Tests/coverage/forceCover/fixCoverage.py")
endif()
if(NOT DEFINED FORCE_COVER_HEADER_DIRS)
  set(FORCE_COVER_HEADER_DIRS "${CMAKE_SOURCE_DIR}/include")
endif()
if(NOT DEFINED FORCE_COVER_STD)
  set(FORCE_COVER_STD "c++17")
endif()
if(NOT DEFINED FORCE_COVER_INCLUDE_DIRS)
  set(FORCE_COVER_INCLUDE_DIRS "")
endif()
if(NOT DEFINED FORCE_COVER_EXTRA_FLAGS)
  set(FORCE_COVER_EXTRA_FLAGS "")
endif()

# --- Fichiers de sortie
set(COVERAGE_TXT       "${OUT_DIR}/coverage.txt")
set(COVERAGE_TXT_UTF8  "${OUT_DIR}/coverage.utf8.txt")
set(REPORT_TXT         "${OUT_DIR}/report.txt")
set(LCOV_INFO          "${OUT_DIR}/lcov.info")
set(LCOV_HTML_DIR      "${OUT_DIR}/coverage_lcov")
set(COVERAGE_TAR       "${OUT_DIR}/coverage_bundle.tar.gz")

# --- Cible: prétraitement headers avec force_cover (miroir fc_headers)
add_custom_target(forcecover_prep
  COMMAND ${CMAKE_COMMAND} -E echo "[force-cover] prétraitement → ${FC_MIRROR_DIR}"
  COMMAND ${CMAKE_COMMAND}
          -DFORCE_COVER_EXE=${FORCE_COVER_EXECUTABLE}
          -DFC_MIRROR_DIR=${FC_MIRROR_DIR}
          -DFORCE_COVER_HEADER_DIRS=${FORCE_COVER_HEADER_DIRS}
          -DFORCE_COVER_STD=${FORCE_COVER_STD}
          -DFORCE_COVER_INCLUDE_DIRS=${FORCE_COVER_INCLUDE_DIRS}
          -DFORCE_COVER_EXTRA_FLAGS=${FORCE_COVER_EXTRA_FLAGS}
          -DFORCE_COVER_VERBOSE=$<IF:$<BOOL:${COVERAGE_VERBOSE}>,2,0>
          -P ${CM_RUN_FC_HEADERS}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  USES_TERMINAL
  VERBATIM
)

# --- Cible: cov (tests instrumentés -> merge -> show/report -> UTF8/fix -> lcov/html -> patch -> tar)
add_custom_target(cov
  COMMAND ${CMAKE_COMMAND} -E echo "== ctest instrumenté =="
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/profraw
  COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/profraw/%p_%m.profraw
          ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure

  COMMAND ${CMAKE_COMMAND} -E echo "== merge profraw =="
  COMMAND bash ${SH_MERGE_PROFRAW} ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/default.profdata

  COMMAND ${CMAKE_COMMAND} -E echo "== llvm-cov show + report =="
  COMMAND bash ${SH_COLLECT_SHOW_REPORT} ${CMAKE_BINARY_DIR} ${OUT_DIR}

  COMMAND ${CMAKE_COMMAND} -E echo "== UTF8 + fix_coverage.py =="
  COMMAND python3 ${PY_UTF8} ${COVERAGE_TXT} ${COVERAGE_TXT_UTF8}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${COVERAGE_TXT}
  COMMAND python3 ${FIX_COVERAGE_PY} ${COVERAGE_TXT_UTF8}

  COMMAND ${CMAKE_COMMAND} -E echo "== export LCOV + genhtml =="
  COMMAND bash ${SH_EXPORT_LCOV_GENHTML} ${CMAKE_BINARY_DIR} ${LCOV_INFO} ${LCOV_HTML_DIR} ${CMAKE_SOURCE_DIR}/include

  # Optionnel: patch visuel inline (cosmétique, n’affecte pas les stats)
  COMMAND ${CMAKE_COMMAND} -E echo "== patch genhtml (inline, optionnel) =="
  COMMAND python3 ${PY_PATCH_INLINE} ${COVERAGE_TXT_UTF8} ${LCOV_HTML_DIR} || true

  COMMAND ${CMAKE_COMMAND} -E echo "== bundle TAR =="
  COMMAND ${CMAKE_COMMAND} -E tar cfvz ${COVERAGE_TAR}
          ${LCOV_INFO} 
          ${COVERAGE_TXT_UTF8} 
          ${REPORT_TXT} 
          ${LCOV_HTML_DIR}
          ${CMAKE_BINARY_DIR}/fc_headers

  COMMENT "LLVM coverage: tests → merge → show/report → UTF8/fix → LCOV+genhtml → patch → TAR"
  USES_TERMINAL
  VERBATIM
)
# NOTE: pas de add_dependencies(cov forcecover_prep) ici, car tu gères déjà preBuild dans ton Makefile.

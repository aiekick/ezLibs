# Linux + Clang uniquement
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "coverage: Linux uniquement.")
endif()
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "coverage: nécessite Clang (LLVM source-based coverage).")
endif()

# Verbosité globale (ON pour afficher des logs détaillés)
option(COVERAGE_VERBOSE "Logs verbeux pour coverage + force_cover" OFF)
if(COVERAGE_VERBOSE)
  set(_VERBOSE_ENV "VERBOSE=1")
else()
  set(_VERBOSE_ENV "VERBOSE=0")
endif()

# Flags d’instrumentation LLVM (NE PAS mélanger avec gcov/gcovr)
add_compile_options(-fprofile-instr-generate -fcoverage-mapping -O0 -fno-inline -fno-elide-constructors)
add_link_options(-fprofile-instr-generate -fcoverage-mapping)

# --- Options force-cover ---
set(FORCE_COVER_EXECUTABLE "${CMAKE_SOURCE_DIR}/third_party/force_cover/force_cover" CACHE FILEPATH "")
set(FIX_COVERAGE_PY        "${CMAKE_SOURCE_DIR}/third_party/force_cover/fix_coverage.py" CACHE FILEPATH "")
set(FORCE_COVER_STD        "c++17" CACHE STRING "")
# Répertoires contenant des templates à traiter (séparés par ';')
set(FORCE_COVER_HEADER_DIRS "${CMAKE_SOURCE_DIR}/include" CACHE STRING "")
# -I supplémentaires pour le parser libtooling (séparés par ';')
set(FORCE_COVER_INCLUDE_DIRS "" CACHE STRING "")
# Flags supplémentaires pour le parser (ex: -DXXX=1)
set(FORCE_COVER_EXTRA_FLAGS "" CACHE STRING "")

# Dossier miroir (headers réécrits)
set(FC_MIRROR_DIR "${CMAKE_BINARY_DIR}/fc_headers")
# On le crée dès la config pour pouvoir l'inclure immédiatement
file(MAKE_DIRECTORY "${FC_MIRROR_DIR}")
include_directories(BEFORE "${FC_MIRROR_DIR}")
# Et on garde aussi les includes “réels”
foreach(_root IN LISTS FORCE_COVER_HEADER_DIRS)
  if(EXISTS "${_root}")
    include_directories("${_root}")
  endif()
endforeach()

# --- Scripts & outils locaux ---
set(COV_DIR "${CMAKE_SOURCE_DIR}/Tests/coverage")
set(SH_RUN_FC_HEADERS_CM   "${COV_DIR}/run_force_cover_headers.cmake")
set(SH_MERGE_PROFRAW       "${COV_DIR}/merge_profraw.sh")
set(SH_SHOW_REPORT         "${COV_DIR}/collect_show_report.sh")
set(SH_LCOV_GENHTML        "${COV_DIR}/export_lcov_genhtml.sh")
set(PY_UTF8                "${COV_DIR}/convert_to_utf8.py")
set(PY_PATCH_INLINE        "${COV_DIR}/patch_genhtml_forcecover_inline.py")

foreach(_req
  "${SH_RUN_FC_HEADERS_CM}"
  "${SH_MERGE_PROFRAW}"
  "${SH_SHOW_REPORT}"
  "${SH_LCOV_GENHTML}"
  "${PY_UTF8}"
  "${PY_PATCH_INLINE}"
  "${FIX_COVERAGE_PY}"
)
  if(NOT EXISTS "${_req}")
    message(FATAL_ERROR "coverage: script manquant -> ${_req}")
  endif()
endforeach()

# --- Sorties ---
set(COVERAGE_TXT       "${CMAKE_CURRENT_BINARY_DIR}/coverage.txt")
set(COVERAGE_TXT_UTF8  "${CMAKE_CURRENT_BINARY_DIR}/coverage.utf8.txt")
set(REPORT_TXT         "${CMAKE_CURRENT_BINARY_DIR}/report.txt")
set(LCOV_INFO          "${CMAKE_CURRENT_BINARY_DIR}/lcov.info")
set(LCOV_HTML_DIR      "${CMAKE_CURRENT_BINARY_DIR}/coverage_lcov")
set(COVERAGE_TAR       "${CMAKE_CURRENT_BINARY_DIR}/coverage_bundle.tar.gz")

# --- Cible: prétraitement force_cover (à la demande / avant cov) ---
add_custom_target(forcecover_prep
  COMMAND ${CMAKE_COMMAND}
          -DFORCE_COVER_EXE=${FORCE_COVER_EXECUTABLE}
          -DFC_MIRROR_DIR=${FC_MIRROR_DIR}
          -DFORCE_COVER_HEADER_DIRS=${FORCE_COVER_HEADER_DIRS}
          -DFORCE_COVER_STD=${FORCE_COVER_STD}
          -DFORCE_COVER_INCLUDE_DIRS=${FORCE_COVER_INCLUDE_DIRS}
          -DFORCE_COVER_EXTRA_FLAGS=${FORCE_COVER_EXTRA_FLAGS}
          -DFORCE_COVER_VERBOSE=$<IF:$<BOOL:${COVERAGE_VERBOSE}>,2,0>
          -P ${SH_RUN_FC_HEADERS_CM}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "force-cover: prétraitement des headers → ${FC_MIRROR_DIR}"
  VERBATIM
)

# --- Cible: cov (exécute tests + collecte + lcov + patch inline) ---
add_custom_target(cov
  # 1) Tests instrumentés → .profraw
  COMMAND ${CMAKE_COMMAND} -E echo "== ctest instrumenté =="
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/profraw
  COMMAND ${CMAKE_COMMAND} -E env ${_VERBOSE_ENV} LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/profraw/%p_%m.profraw
          ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure

  # 2) Merge .profraw → default.profdata
  COMMAND ${CMAKE_COMMAND} -E echo "== merge profraw =="
  COMMAND ${CMAKE_COMMAND} -E env ${_VERBOSE_ENV}
          /bin/bash ${SH_MERGE_PROFRAW} ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/default.profdata

  # 3) llvm-cov show/report → coverage.txt + report.txt
  COMMAND ${CMAKE_COMMAND} -E echo "== llvm-cov show/report =="
  COMMAND ${CMAKE_COMMAND} -E env ${_VERBOSE_ENV}
          /bin/bash ${SH_SHOW_REPORT} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}

  # 4) UTF-8 + fix_coverage (force-cover)
  COMMAND ${CMAKE_COMMAND} -E echo "== UTF8 + fix_coverage.py =="
  COMMAND ${CMAKE_COMMAND} -E env ${_VERBOSE_ENV} python3 ${PY_UTF8} ${COVERAGE_TXT} ${COVERAGE_TXT_UTF8}
  COMMAND ${CMAKE_COMMAND} -E env ${_VERBOSE_ENV} python3 ${FIX_COVERAGE_PY} ${COVERAGE_TXT_UTF8}

  # 5) Export LCOV + genhtml (filtré sur include/)
  COMMAND ${CMAKE_COMMAND} -E echo "== export LCOV + genhtml =="
  COMMAND ${CMAKE_COMMAND} -E env ${_VERBOSE_ENV}
          /bin/bash ${SH_LCOV_GENHTML} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${FORCE_COVER_HEADER_DIRS}

  # 6) Patch inline genhtml (injecte 0 sur lignes ‘forcées’)
  COMMAND ${CMAKE_COMMAND} -E echo "== patch genhtml (force-cover zeros) =="
  COMMAND ${CMAKE_COMMAND} -E env PYTHONUNBUFFERED=1 ${_VERBOSE_ENV}
          python3 ${PY_PATCH_INLINE}
          --genhtml-dir ${LCOV_HTML_DIR}
          --fixed-txt   ${COVERAGE_TXT_UTF8}
          --repo-root   ${CMAKE_SOURCE_DIR}
          --lcov-info   ${LCOV_INFO}

  # 7) Bundle tar.gz (utile pour artefacts CI)
  COMMAND ${CMAKE_COMMAND} -E echo "== bundle tar.gz =="
  COMMAND ${CMAKE_COMMAND} -E rm -f ${COVERAGE_TAR}
  COMMAND ${CMAKE_COMMAND} -E tar cfvz ${COVERAGE_TAR}
          ${LCOV_HTML_DIR}
          ${LCOV_INFO}
          ${COVERAGE_TXT}
          ${COVERAGE_TXT_UTF8}
          ${REPORT_TXT}
          ${CMAKE_BINARY_DIR}/fc_headers/.force_cover_logs

  COMMENT "LLVM coverage: force_cover → tests → merge → show/report → UTF8+fix → LCOV+genhtml → patch inline → TAR"
  USES_TERMINAL
  VERBATIM
)
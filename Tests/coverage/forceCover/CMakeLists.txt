cmake_minimum_required(VERSION 3.10)

project(forceCoverProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option to enable coverage build
option(FORCE_COVER_BUILD_COVERAGE "Build forceCoverCoverage target with coverage instrumentation" OFF)

add_definitions(${LLVM_DEFINITIONS})

# Shared list of Clang + LLVM libs (with linker group)
set(CLANG_LLVM_LIBS
    "-Wl,--start-group"
    clangAST
    clangASTMatchers
    clangAnalysis
    clangBasic
    clangDriver
    clangEdit
    clangFrontend
    clangFrontendTool
    clangLex
    clangParse
    clangSema
    clangEdit
    clangRewrite
    clangRewriteFrontend
    clangStaticAnalyzerFrontend
    clangStaticAnalyzerCheckers
    clangStaticAnalyzerCore
    clangSerialization
    clangToolingCore
    clangTooling
    clangFormat
    clangToolingSyntax
    clangAPINotes
    "-Wl,--end-group"
    ${LLVM_LIBRARIES}
)

# ---------- forceCover (normal build) ----------
add_executable(forceCover
    forceCover.cpp
)

if (NOT MSVC)
    target_compile_options(forceCover PRIVATE -O0 -g)
endif()

target_include_directories(forceCover PRIVATE
    ${LLVM_INCLUDE_DIRS}
    ${Clang_INCLUDE_DIRS}
)

target_link_libraries(forceCover PRIVATE ${CLANG_LLVM_LIBS})

# ---------- forceCoverCoverage (only if FORCE_COVER_BUILD_COVERAGE=ON) ----------
if (FORCE_COVER_BUILD_COVERAGE)
    add_executable(forceCoverCoverage
        forceCover.cpp
    )

    if (NOT MSVC)
        target_compile_options(forceCoverCoverage PRIVATE
            -O0
            -fprofile-instr-generate
            -fcoverage-mapping
        )
        target_link_options(forceCoverCoverage PRIVATE
            -fprofile-instr-generate
            -fcoverage-mapping
        )
    endif()

    target_include_directories(forceCoverCoverage PRIVATE
        ${LLVM_INCLUDE_DIRS}
        ${Clang_INCLUDE_DIRS}
    )

    target_link_libraries(forceCoverCoverage PRIVATE ${CLANG_LLVM_LIBS})
endif()

# ---------- format ----------
find_program(CLANG_FORMAT_EXE NAMES clang-format)
if (CLANG_FORMAT_EXE)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -style=file -i
                ${CMAKE_CURRENT_SOURCE_DIR}/forceCover.cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running clang-format"
        VERBATIM
    )
endif()

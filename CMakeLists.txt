cmake_minimum_required(VERSION 3.20)

# --- Force Clang on Linux ---
if(UNIX AND NOT APPLE)
  if(NOT DEFINED CMAKE_C_COMPILER AND NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(_CLANG NAMES clang)
    find_program(_CLANGXX NAMES clang++)
    if(_CLANG AND _CLANGXX)
      set(CMAKE_C_COMPILER "${_CLANG}" CACHE FILEPATH "" FORCE)
      set(CMAKE_CXX_COMPILER "${_CLANGXX}" CACHE FILEPATH "" FORCE)
      message(STATUS "Using Clang: C=${_CLANG} CXX=${_CLANGXX}")
    else()
      message(FATAL_ERROR "Clang not found on Linux. Please install clang/clang++.")
    endif()
  endif()
endif()

project(ezLibs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE STRING "" FORCE)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE STRING "" FORCE)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE STRING "" FORCE)

option(USE_EZ_LIBS_TESTING "Enable ezLibs testing" OFF)

set(EZ_LIBS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# if there is a parent project
if(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  set(EZ_LIBS_INCLUDE_DIR ${EZ_LIBS_INCLUDE_DIR} PARENT_SCOPE)
else()
  set(USE_EZ_LIBS_TESTING ON CACHE BOOL "" FORCE)
endif()

include_directories(${EZ_LIBS_INCLUDE_DIR})

if(NOT TARGET ezBuildInc)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Tests/ezBuildInc)
	set_target_properties(ezBuildInc PROPERTIES FOLDER 3rdparty/tools)
endif()

if(USE_EZ_LIBS_TESTING)
  enable_testing()
  set(TEST_RESULTS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/TestResults/ezLibs)
  set(TEST_SAMPLES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/samples)
  file(MAKE_DIRECTORY ${TEST_RESULTS_PATH})
  add_definitions(-DRESULTS_PATH="${TEST_RESULTS_PATH}/")
  add_definitions(-DSAMPLES_PATH="${TEST_SAMPLES_PATH}/")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Tests)
endif()

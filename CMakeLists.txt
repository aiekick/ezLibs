cmake_minimum_required(VERSION 3.20)
project(ezLibs)
set(CMAKE_CXX_STANDARD 11)

option(USE_EZ_LIBS_TESTING "Enable ezLibs testing" OFF)

set(EZ_LIBS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# if there is a parent project
if (NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	set(EZ_LIBS_INCLUDE_DIR ${EZ_LIBS_INCLUDE_DIR} PARENT_SCOPE)
else()
	set(USE_EZ_LIBS_TESTING ON CACHE BOOL "" FORCE)
endif()

include_directories(${EZ_LIBS_INCLUDE_DIR})

if(USE_EZ_LIBS_TESTING)
	enable_testing()
	set(TEST_RESULTS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/TestResults/ezLibs)
	set(TEST_SAMPLES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/samples)
	file(MAKE_DIRECTORY ${TEST_RESULTS_PATH})
	add_definitions(-DRESULTS_PATH="${TEST_RESULTS_PATH}/")
	add_definitions(-DSAMPLES_PATH="${TEST_SAMPLES_PATH}/")
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Tests)
endif()

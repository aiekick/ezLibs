cmake_minimum_required(VERSION 3.10)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CmakeTargets")

option(USE_LEAK_SANITIZER "Enable ezLibs leak sanitizer" OFF)
option(USE_CODE_COVERAGE "Enable ezLibs code coverage" OFF)
option(TESTING_WIP "Enable testing of Work in progress libs" OFF)

option(TESTING_APP "Enable testing of Time related libs" ON)
option(TESTING_COMMUNICATION "Enable testing of Communication related libs" ON)
option(TESTING_COMPRESSION "Enable testing of Compression related libs" ON)
option(TESTING_FILE "Enable testing of File related libs" ON)
option(TESTING_MATH "Enable testing of Math related libs" ON)
option(TESTING_MISC "Enable testing of Misc related libs" ON)
option(TESTING_OPENGL "Enable testing of Opengl related libs" ON)
option(TESTING_TIME "Enable testing of Time related libs" ON)
option(TESTING_GEO "Enable testing of Geo related libs" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (USE_LEAK_SANITIZER)
	add_compile_options(-fexceptions -pthread -ldl -lGL -Wall -g -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-cast-overflow -fsanitize=leak -fsanitize=address -fsanitize-address-use-after-scope -fstack-protector -fstack-protector-all)
	add_link_options(-fexceptions -pthread -ldl -lGL -Wall -g -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-cast-overflow -fsanitize=leak -fsanitize=address -fsanitize-address-use-after-scope -fstack-protector -fstack-protector-all)
endif()

if (USE_CODE_COVERAGE)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # LLVM source-based coverage (no gcov flags here)
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping -O0 -fno-inline -fno-elide-constructors)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
  else()
    # GCC / others: gcov-style coverage
    add_compile_options(-fprofile-arcs -ftest-coverage --coverage -fno-inline -O0)
    add_link_options(-fprofile-arcs -ftest-coverage --coverage -fno-inline -O0)
  endif()
endif()

function(AddTest arg)
  	add_test("${arg}" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}" "${arg}")
endfunction(AddTest)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/BuildInc)
set_target_properties(BuildInc PROPERTIES FOLDER 3rdparty)

function(StageBuildInc)
	add_custom_command(
		TARGET ${PROJECT}
		COMMAND BuildInc ezLibs $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/include/ezlibs.hpp> -ff ${CMAKE_SOURCE_DIR}/Tests/res/big.flf
	)
endfunction(StageBuildInc)

if (USE_CODE_COVERAGE)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/coverage)
endif()

if (TESTING_WIP)
	add_definitions(-DTESTING_WIP)
endif()

if (TESTING_APP)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TestApp)
endif()

if (TESTING_COMMUNICATION)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TestCom)
endif()

if (TESTING_COMPRESSION)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TestComp)
endif()

if (TESTING_FILE)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TestFile)
endif()

if (TESTING_MATH)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TestMath)
endif()

if (TESTING_MISC)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TestMisc)
endif()

if (TESTING_OPENGL)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TestOpengl)
endif()

if (TESTING_TIME)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TestTime)
endif()

if (TESTING_GEO)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TestGeo)
endif()

if (USE_CODE_COVERAGE)
	include(${CMAKE_CURRENT_SOURCE_DIR}/coverage/coverage.cmake)
endif()

cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0135 OLD) ## for ExternalProject_Add

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CmakeTargets")

option(USE_LEAK_SANITIZER "Enable ezLibs leak sanitizer" OFF)
option(USE_CODE_COVERAGE "Enable ezLibs code coverage" OFF)

option(TESTING_APP "Enable testing of Time related libs" ON)
option(TESTING_COMPRESSION "Enable testing of Compression related libs" ON)
option(TESTING_FILE "Enable testing of File related libs" ON)
option(TESTING_MATH "Enable testing of Math related libs" ON)
option(TESTING_MISC "Enable testing of Misc related libs" ON)
option(TESTING_OPENGL "Enable testing of Opengl related libs" ON)
option(TESTING_TIME "Enable testing of Time related libs" ON)

if (USE_LEAK_SANITIZER)
	add_compile_options(-fexceptions -pthread -ldl -lGL -Wall -g -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-cast-overflow -fsanitize=leak -fsanitize=address -fsanitize-address-use-after-scope -fstack-protector -fstack-protector-all)
	add_link_options(-fexceptions -pthread -ldl -lGL -Wall -g -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-cast-overflow -fsanitize=leak -fsanitize=address -fsanitize-address-use-after-scope -fstack-protector -fstack-protector-all)
endif()

if (USE_CODE_COVERAGE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		add_compile_options(-fprofile-instr-generate -fcoverage-mapping -fprofile-arcs -ftest-coverage --coverage -fno-inline -O0)
		add_link_options(-fprofile-instr-generate -fcoverage-mapping -fprofile-arcs -ftest-coverage --coverage -fno-inline -O0)
	else()
		add_compile_options(-fprofile-arcs -ftest-coverage --coverage -fno-inline -O0)
		add_link_options(-fprofile-arcs -ftest-coverage --coverage -fno-inline -O0)
	endif()
endif()

function(AddTest arg)
  add_test("${arg}" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}" "${arg}")
endfunction(AddTest)

if (TESTING_APP)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/TestApp)
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

if (USE_CODE_COVERAGE)
	set(COVERAGE_WEB_PAGE "coverage.html")
	set(COVERAGE_XML "coverage.xml") # for codecov
	add_custom_target(cov DEPENDS ${COVERAGE_WEB_PAGE} ${COVERAGE_XML})
	add_custom_command(
		OUTPUT ${COVERAGE_WEB_PAGE}
		COMMAND "gcovr -r ../.. . -f '${EZ_LIBS_INCLUDE_DIR}' "
			"--exclude-unreachable-branches "
			"--exclude-throw-branches "
			"--exclude-function-lines "
			"--exclude-noncode-lines "
			"--html-single-page "
			"--html-details ${COVERAGE_WEB_PAGE}"
	)
	add_custom_command(
		OUTPUT ${COVERAGE_XML}
		COMMAND "gcovr -r ../.. . -f '${EZ_LIBS_INCLUDE_DIR}' "
			"--exclude-unreachable-branches "
			"--exclude-throw-branches "
			"--exclude-function-lines "
			"--exclude-noncode-lines "
			"--cobertura > ${COVERAGE_XML}"
	)
	set_directory_properties(PROPERTIES
		ADDITIONAL_CLEAN_FILES ${COVERAGE_WEB_PAGE}
		ADDITIONAL_CLEAN_FILES ${COVERAGE_XML}
	)
endif()

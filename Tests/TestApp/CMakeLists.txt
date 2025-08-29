cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestApp)
enable_language(C CXX)
project(${PROJECT} CXX)

file(GLOB_RECURSE PROJECT_TEST_SRC_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC_RECURSE})

file(GLOB SRC_RECURSE 
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezApp.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezArgs.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezBuildInc.hpp)
source_group(TREE ${EZ_LIBS_INCLUDE_DIR}/ezlibs PREFIX Libs FILES ${SRC_RECURSE})

add_executable(${PROJECT} 
	${SRC_RECURSE}
	${PROJECT_TEST_SRC_RECURSE})

set_target_properties(${PROJECT} PROPERTIES FOLDER Tests)

target_include_directories(${PROJECT} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
)

##########################################################
##### TESTS ##############################################
##########################################################

##########################################################
##### EzArgs #############################################
##########################################################

AddTest("TestEzArgs_parsing")
AddTest("TestEzArgs_optional_delimiters")
AddTest("TestEzArgs_optional_delimiters_empties")
AddTest("TestEzArgs_optional_groupeds")
AddTest("TestEzArgs_optional_required")
AddTest("TestEzArgs_positional")
AddTest("TestEzArgs_positional_optional")

##########################################################
##### EzBuildInc #########################################
##########################################################

AddTest("TestEzBuildInc_base")

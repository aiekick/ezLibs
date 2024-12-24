cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0110 OLD) ## for use [] in test names like "ezTools.double.Vec2.[]"

set(PROJECT TestApp)
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
AddTest("TestEzArgs_delimiters")
AddTest("TestEzArgs_delimiters_empties")
AddTest("TestEzArgs_groupeds")

##########################################################
##### EzBuildInc #########################################
##########################################################

AddTest("TestEzBuildInc_base")

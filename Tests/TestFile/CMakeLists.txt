cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0110 OLD) ## for use [] in test names like "ezTools.double.Vec2.[]"

set(PROJECT EzLibs_TestFile)
enable_language(C CXX)
project(${PROJECT} CXX)

file(GLOB_RECURSE PROJECT_TEST_SRC_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC_RECURSE})

file(GLOB SRC_RECURSE 
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezIni.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezLog.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezBmp.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/wip/ezGif.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/wip/ezPng.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/wip/ezSvg.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/wip/ezJson.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezFile.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezVdbWriter.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezVoxWriter.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezXmlConfig.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezFileWatcher.hpp)
source_group(TREE ${EZ_LIBS_INCLUDE_DIR}/ezlibs PREFIX Libs FILES ${SRC_RECURSE})

add_executable(${PROJECT} 
	${SRC_RECURSE}
	${PROJECT_TEST_SRC_RECURSE}
)

set_target_properties(${PROJECT} PROPERTIES FOLDER Tests)

target_include_directories(${PROJECT} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
)

##########################################################
##### TESTS ##############################################
##########################################################

##########################################################
##### TESTS EzBmp ########################################
##########################################################

AddTest("TestEzBmp_Writer")

##########################################################
##### TESTS EzFileWatcher ################################
##########################################################

AddTest("TestEzFileWatcher_0")

##########################################################
##### TESTS EzGif ########################################
##########################################################

if (TESTING_WIP)
	AddTest("TestEzGif_Writer")
endif()

##########################################################
##### TESTS EzPng ########################################
##########################################################

if (TESTING_WIP)
	AddTest("TestEzPng_Writer")
endif()

##########################################################
##### TESTS EzSvg ########################################
##########################################################

if (TESTING_WIP)
	AddTest("TestEzSvg_Writer")
endif()

##########################################################
##### TESTS EzJson #######################################
##########################################################

AddTest("TestEzJson_Read")

##########################################################
##### TESTS EzVoxWriter ##################################
##########################################################

AddTest("TestEzVoxWriter_Writer")

##########################################################
##### TESTS EzVdbWriter ##################################
##########################################################

AddTest("TestEzVdbWriter_Writer")

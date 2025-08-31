cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestFile)
enable_language(C CXX)
project(${PROJECT} CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PROJECT_TEST_SRC_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC_RECURSE})

file(GLOB SRC_RECURSE 
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezIni.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezLog.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezBmp.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezFile.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezBinBuf.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/wip/ezGif.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/wip/ezPng.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/wip/ezSvg.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/wip/ezJson.hpp
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

StageBuildInc()

##########################################################
##### TESTS ##############################################
##########################################################

##########################################################
##### TESTS EzBmp ########################################
##########################################################

AddTest("TestEzBmp_Writer")

##########################################################
##### TESTS EzFile #######################################
##########################################################

AddTest("TestEzFile_saveStringToFile")
AddTest("TestEzFile_saveBinToFile")
AddTest("TestEzFile_parsePathFileName")
AddTest("TestEzFile_simplifyFilePath")
AddTest("TestEzFile_composePath")
AddTest("TestEzFile_correctSlashTypeForFilePathName")
AddTest("TestEzFile_createDirectoryIfNotExist")

##########################################################
##### TESTS EzBinBuf #####################################
##########################################################

AddTest("TestEzBinBuf_WriteReadValueLE")
AddTest("TestEzBinBuf_WriteReadValueBE")
AddTest("TestEzBinBuf_WriteReadArrayLE")
AddTest("TestEzBinBuf_WriteReadArrayBE")
AddTest("TestEzBinBuf_SetDatasGetDatas")
AddTest("TestEzBinBuf_ClearSize")
AddTest("TestEzBinBuf_Reserve")
AddTest("TestEzBinBuf_OperatorAt")
AddTest("TestEzBinBuf_OutOfRange")

##########################################################
##### TESTS EzFileWatcher ################################
##########################################################

AddTest("TestEzFileWatcher_start_stop")
AddTest("TestEzFileWatcher_Dir")
AddTest("TestEzFileWatcher_File_PATH")
AddTest("TestEzFileWatcher_File_GLOB")
AddTest("TestEzFileWatcher_File_REGEX")

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

if (TESTING_WIP)
	AddTest("TestEzJson_Parse_1")
endif()

##########################################################
##### TESTS EzVoxWriter ##################################
##########################################################

AddTest("TestEzVoxWriter_Writer")

##########################################################
##### TESTS EzVdbWriter ##################################
##########################################################

AddTest("TestEzVdbWriter_Writer")

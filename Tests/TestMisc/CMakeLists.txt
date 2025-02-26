cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestMisc)
enable_language(C CXX)
project(${PROJECT} CXX)

file(GLOB_RECURSE PROJECT_TEST_SRC_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC_RECURSE})

file(GLOB EZ_LIBS_SOURCE
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezCmdProcessor.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezGraph.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezStr.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezCnt.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezXml.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezFigFont.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/derived/ezQrCode.hpp)
source_group(TREE ${EZ_LIBS_INCLUDE_DIR}/ezlibs PREFIX Libs FILES ${EZ_LIBS_SOURCE})

file(GLOB_RECURSE PROJECT_TEST_RES_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/res/*.*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/res PREFIX Res FILES ${PROJECT_TEST_RES_RECURSE})

add_executable(${PROJECT} 
	${EZ_LIBS_SOURCE}
	${PROJECT_TEST_RES_RECURSE}
	${PROJECT_TEST_SRC_RECURSE}
)

set_target_properties(${PROJECT} PROPERTIES FOLDER Tests)

target_include_directories(${PROJECT} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
)

##########################################################
##### TESTS EzCmdProcessor ###############################
##########################################################

AddTest("TestEzCmdProcessor_Basis")

##########################################################
##### TESTS EzGraph ######################################
##########################################################

AddTest("TestEzGraph_Building")
AddTest("TestEzGraph_Evaluation")

##########################################################
##### TESTS EzStr ########################################
##########################################################

AddTest("TestEzStrGetDigitsCountOfAIntegralNumber")
AddTest("TestEzStrSearchForPatternWithWildcards")

##########################################################
##### TESTS EzCnt ########################################
##########################################################

AddTest("TestEzCntGeneric")
AddTest("TestEzCntOneTypename")
AddTest("TestEzCntTwoTypename")

##########################################################
##### TESTS EzXml ########################################
##########################################################

AddTest("TestEzXmlParsingOK")
AddTest("TestEzXmlParsingNOK_0")
AddTest("TestEzXmlParsingNOK_1")

##########################################################
##### TESTS EzFigFont ####################################
##########################################################

AddTest("TestEzFigFontLoadAndPrint")

##########################################################
##### TESTS EzQrCode #####################################
##########################################################

AddTest("TestEzQrCodeBase")

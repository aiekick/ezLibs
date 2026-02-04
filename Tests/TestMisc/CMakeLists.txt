cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestMisc)
enable_language(C CXX)
project(${PROJECT} CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_definitions(-DNOMINMAX)
endif()

file(GLOB_RECURSE PROJECT_TEST_SRC_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC_RECURSE})

file(GLOB EZ_LIBS_SOURCE
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezCmdProcessor.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezGraph.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezStr.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezSqlite.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezStackString.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezCnt.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezXml.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezLog.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezSha.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezTemplater.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezFigFont.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/wip/derived/ezQrCode.hpp)
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

StageBuildInc()

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

AddTest("TestEzStr_SplitStringToList")
AddTest("TestEzStr_SplitStringToVector")
AddTest("TestEzStr_SplitStringToSet")
AddTest("TestEzStr_StringToNumber")
AddTest("TestEzStr_StringToNumberVector")
AddTest("TestEzStr_ToUpper")
AddTest("TestEzStr_ToLower")
AddTest("TestEzStr_ToHex")
AddTest("TestEzStr_ToHexStr")
AddTest("TestEzStr_ToDecStr")
AddTest("TestEzStr_StrContains")
AddTest("TestEzStr_ReplaceString")
AddTest("TestEzStr_GetCountOccurence")
AddTest("TestEzStr_GetCountOccurenceInSection")
AddTest("TestEzStr_GetDigitsCountOfAIntegralNumber")
AddTest("TestEzStr_SearchForPatternWithWildcards")
AddTest("TestEzStr_ExtractWildcardsFromPattern")
AddTest("TestEzStr_ToStr")
AddTest("TestEzStr_ToStrFromArray")
AddTest("TestEzStr_ToStrTemplate")
AddTest("TestEzStr_SplitStringToListInversion")
AddTest("TestEzStr_SplitStringToListEmpty")
AddTest("TestEzStr_SplitStringToVectorEmpty")
AddTest("TestEzStr_SplitStringToSetEmpty")
AddTest("TestEzStr_StringToNumberEdgeCases")
AddTest("TestEzStr_StringToNumberVectorEmpty")
AddTest("TestEzStr_ToHexEdgeCases")
AddTest("TestEzStr_ToHexStrEdgeCases")
AddTest("TestEzStr_ToDecStrEdgeCases")
AddTest("TestEzStr_StrContainsEdgeCases")
AddTest("TestEzStr_ReplaceStringEdgeCases")
AddTest("TestEzStr_GetCountOccurenceEdgeCases")
AddTest("TestEzStr_GetCountOccurenceInSectionEdgeCases")
AddTest("TestEzStr_GetDigitsCountOfAIntegralNumberEdgeCases")
AddTest("TestEzStr_SearchForPatternWithWildcardsEdgeCases")
AddTest("TestEzStr_ExtractWildcardsFromPatternEdgeCases")

##########################################################
##### TESTS EzStackString ################################
##########################################################

AddTest("TestEzStackString_Constructor_CopyMove")
AddTest("TestEzStackString_Append_CStr")
AddTest("TestEzStackString_Append_Char")
AddTest("TestEzStackString_OperatorPlusEqual")
AddTest("TestEzStackString_HeapSwitch")
AddTest("TestEzStackString_Clear")
AddTest("TestEzStackString_Find")
AddTest("TestEzStackString_Substr")
AddTest("TestEzStackString_OperatorAt")
AddTest("TestEzStackString_HeapDisabled")

##########################################################
##### TESTS EzCnt ########################################
##########################################################

AddTest("TestEzCnt_Generic")
AddTest("TestEzCnt_OneTypename")
AddTest("TestEzCnt_TwoTypename")

##########################################################
##### TESTS EzXml ########################################
##########################################################

AddTest("TestEzXml_ParsingOK")
AddTest("TestEzXml_ParsingNOK_0")
AddTest("TestEzXml_ParsingNOK_1")
AddTest("TestEzXml_Writing_1")
AddTest("TestEzXml_EscapeUnescapeXml")
AddTest("TestEzXml_NodeOperations")
AddTest("TestEzXml_GetOrAddChild")
AddTest("TestEzXml_GetChildNull")
AddTest("TestEzXml_AddChilds")
AddTest("TestEzXml_AttributeWithTemplateTypes")
AddTest("TestEzXml_ReplaceAll")

##########################################################
##### TESTS EzFigFont ####################################
##########################################################

AddTest("TestEzFigFont_LoadAndPrint")

##########################################################
##### TESTS EzSha ########################################
##########################################################

AddTest("TestEzSha_0")

##########################################################
##### TESTS EzLog #########################################
##########################################################

AddTest("TestEzLog_LogSimpleString")
AddTest("TestEzLog_LogSimpleStringByType")
AddTest("TestEzLog_LogStringWithFunction")
AddTest("TestEzLog_LogStringByTypeWithFunction")
AddTest("TestEzLog_VerboseMode")
AddTest("TestEzLog_LogMacros")
AddTest("TestEzLog_LogLightMacros")
AddTest("TestEzLog_LogDebugMacros")
AddTest("TestEzLog_CustomLogFunctor")
AddTest("TestEzLog_CloseAndReopen")

##########################################################
##### TESTS EzSqlite #####################################
##########################################################

AddTest("TestEzSqlite_EmptySQL")
AddTest("TestEzSqlite_SimpleSelect")
AddTest("TestEzSqlite_SyntaxError_SelectFrom")
AddTest("TestEzSqlite_SyntaxError_CreateTableMissingParenOrAs")
AddTest("TestEzSqlite_SyntaxError_InsertMissingInto")
AddTest("TestEzSqlite_SyntaxError_InsertValuesNoParens")
AddTest("TestEzSqlite_SyntaxError_UpdateNoSet")
AddTest("TestEzSqlite_SyntaxError_DeleteNoFrom")
AddTest("TestEzSqlite_SyntaxError_OrderWithoutBy")
AddTest("TestEzSqlite_SyntaxError_LimitNoValue")
AddTest("TestEzSqlite_SyntaxError_ParensUnbalancedOpen")
AddTest("TestEzSqlite_SyntaxError_ParensUnbalancedClose")
AddTest("TestEzSqlite_SyntaxError_SelectTrailingCommaBeforeFrom")
AddTest("TestEzSqlite_SyntaxError_MultiLine_ShowLocation")
AddTest("TestEzSqlite_QueryBuilder_InsertQuery")
AddTest("TestEzSqlite_QueryBuilder_UpdateQuery")
AddTest("TestEzSqlite_QueryBuilder_InsertIfNotExist")
AddTest("TestEzSqlite_QueryBuilder_WithSubQuery")
AddTest("TestEzSqlite_QueryBuilder_MultipleWhereConditions")

##########################################################
##### TESTS EzScreen #####################################
##########################################################

AddTest("TestEzScreen_FloorNb_Scalar_Float")
AddTest("TestEzScreen_FloorNb_Scalar_Double")
AddTest("TestEzScreen_FloorNb_Scalar_Int_NoOp")
AddTest("TestEzScreen_FloorNb_Vec4_Float")
AddTest("TestEzScreen_FloorNb_Vec4_Int_NoOp")
AddTest("TestEzScreen_GetScreenRectWithSize_Pillarbox")
AddTest("TestEzScreen_GetScreenRectWithSize_Letterbox")
AddTest("TestEzScreen_GetScreenRectWithRatio_2dot0_Letterbox")
AddTest("TestEzScreen_GetScreenRectWithRatio_0dot5_Pillarbox")
AddTest("TestEzScreen_ScreenToWorld_Centered_YDown_And_YUp")
AddTest("TestEzScreen_CenteredZoom_KeepPointUnderMouse")
AddTest("TestEzScreen_ZoomedTranslation_YDown")
AddTest("TestEzScreen_ComputeFitScalePx")
AddTest("TestEzScreen_ComputeFitToContent")

##########################################################
##### TESTS EzQrCode #####################################
##########################################################

if (TESTING_WIP)
	AddTest("TestEzQrCodeBase")
endif()

##########################################################
##### TESTS EzTemplater ##################################
##########################################################

AddTest("TestEzTemplater_SimpleTag")
AddTest("TestEzTemplater_MissingTagIsIgnored")
AddTest("TestEzTemplater_DisabledTag")
AddTest("TestEzTemplater_MultiLineBlockEnabled")
AddTest("TestEzTemplater_MultiLineBlockDisabled")
AddTest("TestEzTemplater_NestedBlocks")
AddTest("TestEzTemplater_IndentPreservedForMultilineValue")
AddTest("TestEzTemplater_SyntaxError_UnmatchedClose")
AddTest("TestEzTemplater_SyntaxError_UnclosedBlock")
AddTest("TestEzTemplater_GetUsedTagInfos_Basic")
AddTest("TestEzTemplater_GetUsedTagInfos_MultilineValue")
AddTest("TestEzTemplater_SaveToFile")
AddTest("TestEzTemplater_LoadFromFile")
AddTest("TestEzTemplater_Exception_CloseBeforeOpen")
AddTest("TestEzTemplater_Exception_MissingCloseSimpleTag")
AddTest("TestEzTemplater_Exception_UnclosedMultilineBlock")
AddTest("TestEzTemplater_Exception_UnclosedNestedBlock")
AddTest("TestEzTemplater_Exception_StrayClosingTag")

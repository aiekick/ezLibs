cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestApp)
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

StageBuildInc()

##########################################################
##### TESTS ##############################################
##########################################################

##########################################################
##### EzArgs #############################################
##########################################################

AddTest("TestEzArgs_parsing")
AddTest("TestEzArgs_unknown_option")
AddTest("TestEzArgs_default_constructor")
AddTest("TestEzArgs_empty_name")
AddTest("TestEzArgs_custom_help_key")
AddTest("TestEzArgs_help_short")
AddTest("TestEzArgs_help_long")
AddTest("TestEzArgs_header_footer_description")
AddTest("TestEzArgs_optional_alias")
AddTest("TestEzArgs_optional_alias_long_form")
AddTest("TestEzArgs_optional_value_eq_delimiter")
AddTest("TestEzArgs_optional_value_space_delimiter")
AddTest("TestEzArgs_optional_default_value")
AddTest("TestEzArgs_required_optional_missing")
AddTest("TestEzArgs_required_optional_present")
AddTest("TestEzArgs_positional_single")
AddTest("TestEzArgs_positional_multiple")
AddTest("TestEzArgs_positional_missing")
AddTest("TestEzArgs_positional_array_fixed")
AddTest("TestEzArgs_positional_array_range")
AddTest("TestEzArgs_positional_array_too_few")
AddTest("TestEzArgs_positional_array_unlimited")
AddTest("TestEzArgs_optional_array_space")
AddTest("TestEzArgs_optional_array_unlimited")
AddTest("TestEzArgs_getValue_int")
AddTest("TestEzArgs_getValue_float")
AddTest("TestEzArgs_getValue_double")
AddTest("TestEzArgs_getValue_bool_true")
AddTest("TestEzArgs_getValue_bool_one")
AddTest("TestEzArgs_getValue_bool_false")
AddTest("TestEzArgs_getValue_unknown_key")
AddTest("TestEzArgs_getArrayValues_typed")
AddTest("TestEzArgs_combined_short_args")
AddTest("TestEzArgs_combined_short_args_unknown")
AddTest("TestEzArgs_command_basic")
AddTest("TestEzArgs_command_not_active")
AddTest("TestEzArgs_command_with_sub_positional")
AddTest("TestEzArgs_command_with_sub_optional")
AddTest("TestEzArgs_command_missing_required_sub_positional")
AddTest("TestEzArgs_command_missing_required_sub_optional")
AddTest("TestEzArgs_command_with_sub_optional_value")
AddTest("TestEzArgs_mixed_positional_and_optional")
AddTest("TestEzArgs_positional_then_optional")
AddTest("TestEzArgs_global_optional_with_command")
AddTest("TestEzArgs_isPresent_false")
AddTest("TestEzArgs_isArray_false")
AddTest("TestEzArgs_getArrayValues_non_array")
AddTest("TestEzArgs_multiple_errors")
AddTest("TestEzArgs_empty_args_no_required")
AddTest("TestEzArgs_parse_startIdx")
AddTest("TestEzArgs_optional_type_annotation")
AddTest("TestEzArgs_positional_type_annotation")
AddTest("TestEzArgs_positional_dash_error")
AddTest("TestEzArgs_empty_optional_key")
AddTest("TestEzArgs_empty_positional_key")
AddTest("TestEzArgs_empty_command_key")
AddTest("TestEzArgs_printHelp_output")
AddTest("TestEzArgs_chaining")
AddTest("TestEzArgs_optional_array_stops_at_known_arg")
AddTest("TestEzArgs_positional_array_stops_at_dash")
AddTest("TestEzArgs_command_alias")
AddTest("TestEzArgs_hasErrors_no_errors")
AddTest("TestEzArgs_optional_array_eq_delimiter")
AddTest("TestEzArgs_space_delimiter_no_value_if_next_known")
AddTest("TestEzArgs_multi_char_short_option")
AddTest("TestEzArgs_short_and_multi_char_coexist")
AddTest("TestEzArgs_combined_short_still_works")
AddTest("TestEzArgs_ff_not_combined_when_ff_defined")
AddTest("TestEzArgs_custom_prefix_combined")
AddTest("TestEzArgs_custom_prefix_multi_char_not_combined")
AddTest("TestEzArgs_different_prefix_no_combine")
AddTest("TestEzArgs_ff_when_only_f_defined")

##########################################################
##### EzBuildInc #########################################
##########################################################

AddTest("TestEzBuildInc_base")

##########################################################
##### EzApp ###############################################
##########################################################

AddTest("TestEzApp_DefaultConstructor")
AddTest("TestEzApp_SetAppPath")
AddTest("TestEzApp_SetAppPathWithBackslash")
AddTest("TestEzApp_SetAppPathEmpty")
AddTest("TestEzApp_GetCurDirectory")
AddTest("TestEzApp_SetCurDirectory")
AddTest("TestEzApp_MiniFunction")
AddTest("TestEzApp_ConstructorWithArgv")

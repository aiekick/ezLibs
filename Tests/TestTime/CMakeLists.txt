cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0110 OLD) ## for use [] in test names like "ezTools.double.Vec2.[]"

set(PROJECT EzLibs_TestTime)
enable_language(C CXX)
project(${PROJECT} CXX)

file(GLOB_RECURSE PROJECT_TEST_SRC_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC_RECURSE})

file(GLOB SRC_RECURSE 
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezCron.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezTime.hpp
)
source_group(TREE ${EZ_LIBS_INCLUDE_DIR}/ezlibs PREFIX Lib FILES ${SRC_RECURSE})

add_executable(${PROJECT} 
	${SRC_RECURSE}
	${PROJECT_TEST_SRC_RECURSE}
)

set_target_properties(${PROJECT} PROPERTIES FOLDER Tests)

target_include_directories(${PROJECT} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
)

##########################################################
##### EzTime #############################################
##########################################################

##########################################################
##### EzCron #############################################
##########################################################

AddTest("TestEzCron_Format_Mins")
AddTest("TestEzCron_Format_Hours")
AddTest("TestEzCron_Format_MonthDay")
AddTest("TestEzCron_Format_Month")
AddTest("TestEzCron_Format_WeekDay")
AddTest("TestEzCron_Format_ManyErrors_0")
AddTest("TestEzCron_Format_ManyErrors_1")
AddTest("TestEzCron_Format_FieldCount_0")
AddTest("TestEzCron_Format_FieldCount_1")
AddTest("TestEzCron_Format_Type_Value")
AddTest("TestEzCron_Format_Type_Interval")
AddTest("TestEzCron_Format_Type_Range")
AddTest("TestEzCron_Format_Type_Values")
AddTest("TestEzCron_TimeCheck_Type_Value")
AddTest("TestEzCron_TimeCheck_Type_Interval")
AddTest("TestEzCron_TimeCheck_Type_Range")
AddTest("TestEzCron_TimeCheck_Type_Values")

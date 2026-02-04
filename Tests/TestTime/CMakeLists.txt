cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestTime)
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

StageBuildInc()

##########################################################
##### EzTime #############################################
##########################################################

AddTest("TestEzTime_Iso8601ToEpoch")
AddTest("TestEzTime_EpochToISO8601")
AddTest("TestEzTime_DecomposeEpoch")
AddTest("TestEzTime_GetCurrentDate")
AddTest("TestEzTime_GetCurrentDateWithOffset")
AddTest("TestEzTime_GetTicks")
AddTest("TestEzTime_ScopedTimer")
AddTest("TestEzTime_GetTimeInterval")
AddTest("TestEzTime_ActionTime_Basic")
AddTest("TestEzTime_ActionTime_GetTime")
AddTest("TestEzTime_ActionTime_Fix")
AddTest("TestEzTime_ActionTime_SetTime")
AddTest("TestEzTime_ActionTime_FixTime")
AddTest("TestEzTime_ActionTime_PauseResume")
AddTest("TestEzTime_ActionTime_IsTimeToAct")
AddTest("TestEzTime_MeasureOperationUs")
AddTest("TestEzTime_MeasureOperationUs_WithWork")
AddTest("TestEzTime_WaitUntil_Success")
AddTest("TestEzTime_WaitUntil_Timeout")
AddTest("TestEzTime_WaitUntil_ImmediateTrue")

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


##########################################################
##### EzDate #############################################
##########################################################

AddTest("TestEzDate_ParseYmd")
AddTest("TestEzDate_IsLeap_MonthDays")
AddTest("TestEzDate_AddDays_Generic")
AddTest("TestEzDate_DiffDays")
AddTest("TestEzDate_Start_End_Of_Month")
AddTest("TestEzDate_YmKey")
AddTest("TestEzDate_DayOfMonthToDate_PositiveNegative")
AddTest("TestEzDate_DayOfMonthToDate_FebruaryLeap") 
AddTest("TestEzDate_Date_SetYear_NoSideEffect")
AddTest("TestEzDate_Date_SetMonth_Clamped_1_12")
AddTest("TestEzDate_Date_SetDay_Clamped_By_Month")
AddTest("TestEzDate_Date_OffsetYear_Simple")
AddTest("TestEzDate_Date_OffsetMonth_RollYear")
AddTest("TestEzDate_Date_OffsetDay_Generic")
AddTest("TestEzDate_Date_GetDate_Format")
AddTest("TestEzDate_Date_GetDate_AfterMutations")   
AddTest("TestEzDate_Date_OffsetDay_LeapYearSpans")   
AddTest("TestEzDate_Date_OffsetDay_NonLeapYearSpans")   
AddTest("TestEzDate_Date_OffsetDay_LongSpan_1970_2000")   

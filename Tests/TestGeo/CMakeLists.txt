cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestGeo)
enable_language(C CXX)
project(${PROJECT} CXX)

file(GLOB_RECURSE PROJECT_TEST_SRC_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC_RECURSE})

file(GLOB SRC_RECURSE 
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezCron.hpp
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezGeo.hpp
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
##### EzGeo ##############################################
##########################################################

##########################################################
##### TESTS ezGeo ########################################
##########################################################

# Mercator / Degrees
AddTest("TestEzGeo_Mercator_Degrees_RoundTrip_Paris")
AddTest("TestEzGeo_Mercator_Degrees_RoundTrip_NYC")
AddTest("TestEzGeo_Mercator_Degrees_RoundTrip_Sydney")
AddTest("TestEzGeo_MercatorYToUnrolledY")
AddTest("TestEzGeo_UnscaleWGS84Yaxis")

# DEM helpers
AddTest("TestEzGeo_ParseDemCoordinate")
AddTest("TestEzGeo_CheckDemFileName_Valid_Upper")
AddTest("TestEzGeo_CheckDemFileName_Valid_Lower")
AddTest("TestEzGeo_CheckDemFileName_Invalid_Length")
AddTest("TestEzGeo_CheckDemFileName_Invalid_Prefix")

# DMS (valid)
AddTest("TestEzGeo_Dms_Parse_DecimalDegrees")
AddTest("TestEzGeo_Dms_Parse_OnlyDegrees")
AddTest("TestEzGeo_Dms_Parse_Full_With_Symbols")
AddTest("TestEzGeo_Dms_Parse_With_Spaces_And_E")
AddTest("TestEzGeo_Dms_Parse_NoSeconds")
AddTest("TestEzGeo_Dms_ToAngle_With_S")
AddTest("TestEzGeo_Dms_FromAngle_Latitude")
AddTest("TestEzGeo_Dms_FromAngle_Longitude")
AddTest("TestEzGeo_Dms_RoundTrip_Lat")
AddTest("TestEzGeo_Dms_RoundTrip_Lon")
AddTest("TestEzGeo_Dms_IsLat_IsLon")

# DMS (failures)
AddTest("TestEzGeo_Dms_ParseFail_NegativeMinutes")
AddTest("TestEzGeo_Dms_ParseFail_NegativeSeconds")
AddTest("TestEzGeo_Dms_ParseFail_MinutesOverflow")
AddTest("TestEzGeo_Dms_ParseFail_SecondsOverflow")
AddTest("TestEzGeo_Dms_ParseFail_Empty")

# DMS (offsets)
AddTest("TestEzGeo_Dms_OffsetSec_CarriesToMin_Positive")
AddTest("TestEzGeo_Dms_OffsetSec_CarriesToMin_Negative")
AddTest("TestEzGeo_Dms_OffsetMin_CarriesToDeg_AndPushFracToSec")
AddTest("TestEzGeo_Dms_OffsetMin_NegativeWithFrac")
AddTest("TestEzGeo_Dms_OffsetDeg_PushFracToMinAndSec")
AddTest("TestEzGeo_Dms_OffsetDeg_PurgeExistingFracInDeg")
AddTest("TestEzGeo_Dms_OffsetDeg_WrapLatitude")
AddTest("TestEzGeo_Dms_OffsetDeg_WrapLongitude")
AddTest("TestEzGeo_Dms_Offsets_PreserveOnlySecFraction")

##########################################################
##### TESTS ezTile #######################################
##########################################################

AddTest("TestEzTile_Base_NonZeroMin")
AddTest("TestEzTile_Discrete_NonZeroMin")
AddTest("TestEzTile_Continuous_NoWrap_NonZeroMin")
AddTest("TestEzTile_Continuous_Wrap_NonZeroMin")
AddTest("TestEzTile_AllNeg_Discrete")
AddTest("TestEzTile_AllNeg_Continuous_NoWrap")
AddTest("TestEzTile_AllNeg_Continuous_Wrap")
AddTest("TestEzTile_AllPos_Discrete")
AddTest("TestEzTile_AllPos_Continuous_NoWrap")
AddTest("TestEzTile_AllPos_Continuous_Wrap")

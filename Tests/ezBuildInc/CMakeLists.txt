cmake_minimum_required(VERSION 3.20)

set(PROJECT ezBuildInc)

project(${PROJECT} CXX)
enable_language(CXX)

add_executable(${PROJECT} main.cpp)

set_target_properties(${PROJECT} PROPERTIES FOLDER 3rdparty/tools)

if (MSVC)
    add_definitions(-DNOMINMAX)
	set_property(TARGET ${PROJECT} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

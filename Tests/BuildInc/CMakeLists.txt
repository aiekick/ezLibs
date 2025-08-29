cmake_minimum_required(VERSION 3.20)

set(PROJECT BuildInc)

project(${PROJECT} CXX)
enable_language(CXX)

add_executable(${PROJECT} main.cpp)

set_target_properties(${PROJECT} PROPERTIES FOLDER tools)

if (MSVC)
	set_property(TARGET ${PROJECT} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

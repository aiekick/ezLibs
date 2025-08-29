cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestOpengl)
enable_language(C CXX)
project(${PROJECT} CXX)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
  find_package(X11 REQUIRED)
  if (NOT X11_Xi_FOUND)
    message(FATAL_ERROR "X11 Xi library is required")
  endif ()
endif ()

file(GLOB_RECURSE PROJECT_TEST_SRC_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC_RECURSE})

file(GLOB SRC_RECURSE 
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezGL/*.hpp)
source_group(TREE ${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezGL PREFIX Lib FILES ${SRC_RECURSE})

add_executable(${PROJECT} 
	${SRC_RECURSE}
	${PROJECT_TEST_SRC_RECURSE}
)

if(NOT GLAD_LIBRARIES)
	set(GLAD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/include)
	file(GLOB GLAD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/src/glad.c)
	file(GLOB GLAD_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/glad.h)
	add_library(glad STATIC ${GLAD_HEADERS} ${GLAD_SOURCES})
	set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)
	target_include_directories(glad PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/include
	)
	if(WIN32)
		set(GLAD_LIBRARIES glad)
	else()
		set(GLAD_LIBRARIES glad dl)
	endif()
endif() 

target_link_libraries(${PROJECT} PRIVATE ${GLAD_LIBRARIES})

target_include_directories(${PROJECT} PRIVATE 
	${GLAD_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/include
)

set_target_properties(glad PROPERTIES FOLDER Tests/Opengl)
set_target_properties(${PROJECT} PROPERTIES FOLDER Tests/Opengl)

target_compile_definitions(${PROJECT} PRIVATE 
	-DOPENGL_LOADER="${GLAD_INCLUDE_DIR}/glad/glad.h"
)

StageBuildInc()

##########################################################
##########################################################
##########################################################

## ez::gl::Uniforms
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_0")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_1")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_2")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_3")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_4")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_5")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_6")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_7")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_8")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_9")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_10")
AddTest("TestEzGL_Uniforms_Parsing_Good_Syntax_11")
AddTest("TestEzGL_Uniforms_Parsing_Bad_Syntax_0")
AddTest("TestEzGL_Uniforms_Parsing_Bad_Syntax_1")
AddTest("TestEzGL_Uniforms_Parsing_Bad_Syntax_2")
AddTest("TestEzGL_Uniforms_Parsing_Bad_Syntax_3")
AddTest("TestEzGL_Uniforms_Parsing_Bad_Syntax_4")
AddTest("TestEzGL_Uniforms_Parsing_Bad_Syntax_5")
AddTest("TestEzGL_Uniforms_Parsing_Bad_Syntax_6")
AddTest("TestEzGL_Uniforms_Parsing_Bad_Syntax_7")
AddTest("TestEzGL_Uniforms_Parsing_Time_Widget_0")
AddTest("TestEzGL_Uniforms_Parsing_Buffer_Widget_0")
AddTest("TestEzGL_Uniforms_Parsing_Slider_Widget_0")

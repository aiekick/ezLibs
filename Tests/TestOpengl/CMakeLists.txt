cmake_minimum_required(VERSION 3.10)

set(PROJECT EzLibs_TestOpengl)
enable_language(C CXX)
project(${PROJECT} CXX)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_definitions(-DNOMINMAX)
endif()

add_definitions(-D_TESTING_)

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
  find_package(X11 REQUIRED)
  if (NOT X11_Xi_FOUND)
    message(FATAL_ERROR "X11 Xi library is required")
  endif ()
endif ()

file(GLOB PROJECT_TEST_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC})

file(GLOB SRC_RECURSE 
	${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezGL/*.hpp)
source_group(TREE ${EZ_LIBS_INCLUDE_DIR}/ezlibs/ezGL PREFIX Lib FILES ${SRC_RECURSE})

file(GLOB INPUTS_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/inputs/*.*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/inputs PREFIX inputs FILES ${INPUTS_RECURSE})

add_executable(${PROJECT} 
	${SRC_RECURSE}
	${INPUTS_RECURSE}
	${PROJECT_TEST_SRC}
)

target_compile_definitions(${PROJECT} PRIVATE INPUTS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/inputs")

if(NOT TARGET glad)
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

if(NOT TARGET glfw)
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
	# Detect if we're in a CI environment or if X11 is not available
	if(NOT DEFINED GLFW_BUILD_X11)
		set(GLFW_BUILD_X11 ON)
	endif()
	if(NOT DEFINED GLFW_BUILD_WAYLAND)
		set(GLFW_BUILD_WAYLAND OFF)
	endif()
	set(GLFW_BUILD_WAYLAND ${GLFW_BUILD_WAYLAND} CACHE BOOL "" FORCE)
	set(GLFW_BUILD_X11 ${GLFW_BUILD_X11} CACHE BOOL "" FORCE)
	add_subdirectory(3rdparty/glfw)
	if(TARGET update_mappings)
		set_target_properties(glfw PROPERTIES FOLDER 3rdparty/glfw)
		set_target_properties(update_mappings PROPERTIES FOLDER 3rdparty/glfw)
	else()
		set_target_properties(glfw PROPERTIES FOLDER 3rdparty)
	endif()
	set(GLFW_LIBRARIES glfw)
	set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glfw/include)
endif()

target_link_libraries(${PROJECT} PRIVATE ${GLAD_LIBRARIES} ${GLFW_LIBRARIES})

target_include_directories(${PROJECT} PRIVATE 
	${GLAD_INCLUDE_DIR}
	${GLFW_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}	
)

set_target_properties(glad PROPERTIES FOLDER 3rdparty)
set_target_properties(${PROJECT} PROPERTIES FOLDER Tests)

target_compile_definitions(${PROJECT} PRIVATE 
	-DOPENGL_LOADER="${GLAD_INCLUDE_DIR}/glad/glad.h"
)

StageBuildInc()

##########################################################
#### UNIFORMS ############################################
##########################################################

## ez::gl::Uniforms - Original Tests
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

## ez::gl::Core - Extended Uniform Tests
# Widget tests with vec types
AddTest("TestEzGL_Uniforms_Widget_Vec2_0")
AddTest("TestEzGL_Uniforms_Widget_Vec3_0")
AddTest("TestEzGL_Uniforms_Widget_Vec4_0")
AddTest("TestEzGL_Uniforms_Widget_Int_0")
AddTest("TestEzGL_Uniforms_Widget_Mat3_0")
AddTest("TestEzGL_Uniforms_Widget_Mat4_0")
AddTest("TestEzGL_Uniforms_Widget_Sampler2D_0")
AddTest("TestEzGL_Uniforms_Widget_Sampler3D_0")
AddTest("TestEzGL_Uniforms_Widget_SamplerCube_0")

# Complex widget tests
AddTest("TestEzGL_Uniforms_Widget_Multiple_Params_0")
AddTest("TestEzGL_Uniforms_Widget_Numeric_Params_0")

# Comment extraction and code generation
AddTest("TestEzGL_Uniforms_Comment_Extraction_0")
AddTest("TestEzGL_Uniforms_GetFinalCode_0")
AddTest("TestEzGL_Uniforms_GetFinalCode_1")

# Additional GLSL types with widgets
AddTest("TestEzGL_Uniforms_Widget_Double_0")
AddTest("TestEzGL_Uniforms_Widget_Ivec2_0")
AddTest("TestEzGL_Uniforms_Widget_Ivec3_0")
AddTest("TestEzGL_Uniforms_Widget_Ivec4_0")
AddTest("TestEzGL_Uniforms_Widget_Uvec2_0")
AddTest("TestEzGL_Uniforms_Widget_Uvec3_0")
AddTest("TestEzGL_Uniforms_Widget_Uvec4_0")
AddTest("TestEzGL_Uniforms_Widget_Bool_0")
AddTest("TestEzGL_Uniforms_Widget_Bvec2_0")
AddTest("TestEzGL_Uniforms_Widget_Bvec3_0")
AddTest("TestEzGL_Uniforms_Widget_Bvec4_0")
AddTest("TestEzGL_Uniforms_Widget_Mat2_0")
AddTest("TestEzGL_Uniforms_Widget_Sampler2DArray_0")

##########################################################
#### Canvas ##############################################
##########################################################

AddTest("TestEzGL_Canvas_init")

##########################################################
#### QUAD VFX ############################################
##########################################################

AddTest("TestEzGL_QuadVfx_init")

##########################################################
#### QUAD VFX Auto #######################################
##########################################################

AddTest("TestEzGL_QuadVfxAuto_init")

##########################################################
#### COMP VFX ############################################
##########################################################

AddTest("TestEzGL_CompVfx_init")

##########################################################
#### MESH VFX ############################################
##########################################################

AddTest("TestEzGL_MeshVfx_init")

##########################################################
#### BUFFER BLOCK #########################################
##########################################################

AddTest("TestEzGL_BufferBlock_Create")
AddTest("TestEzGL_BufferBlock_Upload")
AddTest("TestEzGL_BufferBlock_Bind")
AddTest("TestEzGL_BufferBlockAuto_Create")
AddTest("TestEzGL_BufferBlockAuto_PushData")
AddTest("TestEzGL_BufferBlock_SSBO_Create")
AddTest("TestEzGL_BufferBlock_UBOAuto_Create")
AddTest("TestEzGL_BufferBlock_UBOAuto_RegisterVar_Float")
AddTest("TestEzGL_BufferBlock_UBOAuto_RegisterVar_Int")
AddTest("TestEzGL_BufferBlock_UBOAuto_SetVar")
AddTest("TestEzGL_BufferBlock_UBOAuto_SetAddVar")
AddTest("TestEzGL_BufferBlock_UBOAuto_UploadIfDirty")
AddTest("TestEzGL_BufferBlock_UBOAuto_RegisterArray")
AddTest("TestEzGL_BufferBlock_UBOAuto_Bind")
AddTest("TestEzGL_BufferBlock_UBOAuto_Clear")
AddTest("TestEzGL_BufferBlock_UBOAuto_Recreate")

##########################################################
#### PROC MESH ############################################
##########################################################

AddTest("TestEzGL_ProcMesh_CreateUVSphere")
AddTest("TestEzGL_ProcMesh_CreateUVSphere_SmallSubdivs")
AddTest("TestEzGL_ProcMesh_CreateUVSphere_LargeSubdivs")
AddTest("TestEzGL_ProcMesh_CreateUVSphere_DifferentRadius")
AddTest("TestEzGL_ProcMesh_ProcDatas_DefaultConstructor")
AddTest("TestEzGL_ProcMesh_ProcDatas_ParameterizedConstructor")

##########################################################
#### TEXTURE ##############################################
##########################################################

AddTest("TestEzGL_Texture_CreateEmpty")
AddTest("TestEzGL_Texture_CreateEmpty_WithMipmap")
AddTest("TestEzGL_Texture_CreateFromBuffer_RGBA")
AddTest("TestEzGL_Texture_CreateFromBuffer_RGB")
AddTest("TestEzGL_Texture_CreateFromBuffer_Grayscale")
AddTest("TestEzGL_Texture_CreateFromBuffer_WithFormat")
AddTest("TestEzGL_Texture_Bind")
AddTest("TestEzGL_Texture_Unbind")
AddTest("TestEzGL_Texture_Resize")
AddTest("TestEzGL_Texture_GetChannelsCount")
AddTest("TestEzGL_Texture_SetPixels")
AddTest("TestEzGL_Texture_SetPixels_PartialUpdate")
AddTest("TestEzGL_Texture_GenerateMipmap")
AddTest("TestEzGL_Texture_Clear")
AddTest("TestEzGL_Texture_Clear_WithColor")

##########################################################
#### SHADER ###############################################
##########################################################

AddTest("TestEzGL_Shader_CreateFromCode_Vertex")
AddTest("TestEzGL_Shader_CreateFromCode_Fragment")
AddTest("TestEzGL_Shader_CreateFromCode_Compute")
AddTest("TestEzGL_Shader_CreateFromCode_Invalid")
AddTest("TestEzGL_Shader_CreateFromFile")
AddTest("TestEzGL_Shader_CreateFromFile_NonExistent")
AddTest("TestEzGL_Shader_Unit")
AddTest("TestEzGL_Shader_MultipleShaders")

##########################################################
#### PROGRAM ##############################################
##########################################################

AddTest("TestEzGL_Program_Create")
AddTest("TestEzGL_Program_AddShader")
AddTest("TestEzGL_Program_Link")
AddTest("TestEzGL_Program_Link_WithoutShaders")
AddTest("TestEzGL_Program_AddUniformFloat")
AddTest("TestEzGL_Program_AddUniformVec2")
AddTest("TestEzGL_Program_AddUniformInt")
AddTest("TestEzGL_Program_AddUniformMat4")
AddTest("TestEzGL_Program_AddUniformSampler2D")
AddTest("TestEzGL_Program_Use")
AddTest("TestEzGL_Program_SetUniformFloatDatas")

#ASSIMP

include_directories(assimp/code/BoostWorkaround)
add_definitions(-DASSIMP_BUILD_BOOST_WORKAROUND)

set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)

set(ASSIMP_OPT_BUILD_PACKAGES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(assimp)
set_property(TARGET assimp PROPERTY CXX_STANDARD 14)

set_target_properties(
	assimp
	IrrXML
	UpdateAssimpLibsDebugSymbolsAndDLLs
	zlib
	zlibstatic
	PROPERTIES FOLDER "external/assimp"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
	
#GLSLANG

set(ENABLE_CTEST OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL OFF CACHE BOOL "" FORCE)
set(ENABLE_HLSL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(glslang)
include_directories(glslang/glslang/Public)

set_target_properties(
	glslang
	OGLCompiler
	OSDependent
	SPIRV
	SPVRemapper
	PROPERTIES FOLDER "external/glslang"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

#RAPIDJSON

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE rapidjson/include/)

#VULKAN

add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE vulkan/)

#MD5

add_library(md5 INTERFACE)
target_include_directories(md5 INTERFACE md5/)

#STB

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb/)

#IMGUI

file(GLOB IMGUI_SOURCES "imgui/*.cpp")
file(GLOB IMGUI_HEADERS "imgui/*.h")

source_group(
	TREE ${CMAKE_CURRENT_SOURCE_DIR}/imgui/
	FILES 
	${IMGUI_HEADERS}
	${IMGUI_SOURCES})
	
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS} )
target_include_directories(imgui PUBLIC imgui/. )
target_link_libraries(imgui vulkan)
target_compile_definitions(imgui PUBLIC ImDrawIdx=unsigned)

set_target_properties(
	imgui
	PROPERTIES FOLDER "external"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

#OTHERS

set_target_properties(
	uninstall
	PROPERTIES FOLDER "CMakePredefinedTargets")
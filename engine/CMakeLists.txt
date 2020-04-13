file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h" "*.hpp")
file(GLOB_RECURSE NATVIS "*.natvis")

source_group(
	TREE ${CMAKE_CURRENT_SOURCE_DIR}/ 
	FILES 
	${HEADERS} 
	${SOURCES} 
	${NATVIS} 
	${ADDITIONAL} 
	cpp.hint 
	Graphics/Vulkan/ListOfVulkanFunctions.inl)

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yustdafx.h /FIstdafx.h")
endif()

add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DVK_NO_PROTOTYPES )

add_library(engine STATIC ${SOURCES} ${HEADERS} ${NATVIS} cpp.hint Graphics/Vulkan/ListOfVulkanFunctions.inl)

target_link_libraries(engine md5 rapidjson vulkan)

target_include_directories(engine PUBLIC . )


set_source_files_properties( stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h" )
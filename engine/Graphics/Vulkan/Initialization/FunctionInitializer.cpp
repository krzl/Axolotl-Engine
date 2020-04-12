// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
// ReSharper disable CppClangTidyBugproneMacroParentheses
#include "stdafx.h"
#include "Initializer.h"
#include "../Vulkan.h"

namespace axlt::vk::init {
	bool LoadGlobalFunctions() {

	#define EXPORTED_VULKAN_FUNCTION( name )											\
		name = (PFN_##name) GetProcAddress( library, #name );							\
		if( nullptr == (name) ) {														\
			printf( "Could not load vulkan function %s\n", #name );						\
			return false;																\
		}

	#define GLOBAL_LEVEL_VULKAN_FUNCTION( name )										\
		name = (PFN_##name) vkGetInstanceProcAddr( nullptr, #name );					\
		if( nullptr == (name) ) {														\
			printf( "Could not load global function %s\n", #name );						\
			return false;																\
		}

	#include "../ListOfVulkanFunctions.inl"

		return true;
	}

	bool LoadInstanceFunctions( const Array<const char*>& enabledExtensions ) {
		
	#define INSTANCE_LEVEL_VULKAN_FUNCTION( name )										\
		name = (PFN_##name) vkGetInstanceProcAddr( instance, #name );					\
		if( nullptr == (name) ) {														\
			printf( "Could not load instance function %s\n", #name );					\
			return false;																\
		}

	#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )			\
		for( const char* ext : enabledExtensions ) {									\
			if( strcmp( ext, extension ) == 0 ) {										\
				name = (PFN_##name) vkGetInstanceProcAddr( instance, #name );			\
				if( nullptr == (name) ) {												\
					printf( "Could not load device function %s\n", #name );				\
					return false;														\
				}	 																	\
				break;																	\
			}																			\
		}

	#include "../ListOfVulkanFunctions.inl"
		

		return true;
	}

	bool LoadDeviceFunctions( const Array<const char*>& enabledExtensions ) {
	#define DEVICE_LEVEL_VULKAN_FUNCTION( name )										\
		name = (PFN_##name) vkGetDeviceProcAddr( device, #name );						\
		if( nullptr == (name) ) {														\
			printf( "Could not load device function %s\n", #name );						\
			return false;																\
		}

	#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )				\
		for( const char* ext : enabledExtensions ) {									\
			if( strcmp( ext, extension ) == 0 ) {										\
				name = (PFN_##name) vkGetDeviceProcAddr( device, #name );				\
				if( nullptr == (name )) {												\
					printf( "Could not load device function %s\n", #name );				\
					return false;														\
				}	 																	\
				break;																	\
			}																			\
		}

	#include "../ListOfVulkanFunctions.inl"

		return true;
	}
}
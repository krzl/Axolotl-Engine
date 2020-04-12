// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "Shaders.h"

namespace axlt::vk {

	bool CreateShaderModule( Array<uint8_t>& source, VkShaderModule& shaderModule ) {
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0,
			source.GetSize(),
			reinterpret_cast<uint32_t*>( source.GetData() )
		};

		const VkResult result = vkCreateShaderModule( device, &shaderModuleCreateInfo, nullptr, &shaderModule );
		if( VK_SUCCESS != result ) {
			printf( "Could not create shader module\n" );
			return false;
		}

		return true;
	}

	void DestroyShaderModule( VkShaderModule& shaderModule ) {
		if( shaderModule != VK_NULL_HANDLE ) {
			vkDestroyShaderModule( device, shaderModule, nullptr );
			shaderModule = VK_NULL_HANDLE;
		}
	}
}
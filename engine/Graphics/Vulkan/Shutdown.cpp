#include "stdafx.h"
#include "Vulkan.h"

namespace axlt::vk {

	void Shutdown() {
		vkDeviceWaitIdle( device );

		vkDestroyRenderPass( device, renderPass, nullptr );

		vkDestroySemaphore( device, imageAvailableSemaphore, nullptr );
		vkDestroySemaphore( device,  renderingFinishedSemaphore, nullptr );

		vkDestroyCommandPool( device, commandPool, nullptr );

		for( uint32_t i = 0; i < swapchainImages.GetSize(); i++ ) {
			vkDestroyBuffer( device, perCameraBuffers[i], nullptr );
			vkFreeMemory( device, perCameraBuffersMemory[i], nullptr );
			vkDestroyFence( device, renderFences[i], nullptr );
			vkDestroyFramebuffer( device, framebuffers[i], nullptr );
			vkDestroyImageView( device, swapchainImageViews[i], nullptr );
		}

		for( auto& material : materialDataArray ) {
			vkDestroyDescriptorPool( device, material.value.descriptorPool, nullptr );
			for( MaterialData::PerCommandBuffer& perCommandBuffer : material.value.perFrameData ) {
				for( uint32_t i = 0; i < perCommandBuffer.uniformBuffers.GetSize(); i++ ) {
					vkFreeMemory( device, perCommandBuffer.uniformBuffersMemory[i], nullptr );
					vkDestroyBuffer( device, perCommandBuffer.uniformBuffers[i], nullptr );
				}
			}
		}
		materialDataArray.Clear();

		for( auto& technique : techniqueDataArray ) {
			for( auto& layout : technique.value.layouts ) {
				vkDestroyDescriptorSetLayout( device, layout, nullptr );
			}
			for( auto& shaderModule : technique.value.shaderModules ) {
				vkDestroyShaderModule( device, shaderModule, nullptr );
			}
			vkDestroyPipelineLayout( device, technique.value.pipelineLayout, nullptr );
			vkDestroyPipeline( device, technique.value.pipeline, nullptr );
		}
		techniqueDataArray.Clear();

		for( auto& mesh : meshBuffers ) {
			for( VkBuffer buffer : mesh.value.buffers ) {
				vkDestroyBuffer( device, buffer, nullptr );
			}
			vkFreeMemory( device, mesh.value.memory, nullptr );
		}
		meshBuffers.Clear();

		for( auto& texture : textureDataArray ) {
			vkFreeMemory( device, texture.value.imageMemory, nullptr );
			vkDestroySampler( device, texture.value.sampler, nullptr );
			vkDestroyImageView( device, texture.value.imageView, nullptr );
			vkDestroyImage( device, texture.value.image, nullptr );
		}
		textureDataArray.Clear();
		
		if( swapchain ) {
			vkDestroySwapchainKHR( device, swapchain, nullptr );
			swapchain = VK_NULL_HANDLE;
		}

		if( presentationSurface ) {
			vkDestroySurfaceKHR( instance, presentationSurface, nullptr );
			presentationSurface = VK_NULL_HANDLE;
		}

		if( device ) {
			vkDestroyDevice( device, nullptr );
			device = VK_NULL_HANDLE;
		}

		if( instance ) {
			vkDestroyInstance( instance, nullptr );
			instance = VK_NULL_HANDLE;
		}

		FreeLibrary( library );
		library = nullptr;
	}
}
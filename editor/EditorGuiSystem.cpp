#include "stdafx.h"
#include "EditorGuiSystem.h"
#include <examples/imgui_impl_win32.h>
#include <Game.h>
#include <examples/imgui_impl_vulkan.h>

#include <Graphics/Vulkan/Vulkan.h>

namespace axlt::editor {

	void EditorGuiSystem::OnInitialize() {
		ImGui_ImplWin32_Init( GameInstance.GetWindow().GetHandle() );

		VkDescriptorPool  descriptorPool;

		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE( pool_sizes );
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE( pool_sizes );
		pool_info.pPoolSizes = pool_sizes;
		vkCreateDescriptorPool( vk::device, &pool_info, nullptr, &descriptorPool );
		
		ImGui_ImplVulkan_InitInfo vulkanInitInfo = {
			vk::instance,
			vk::physicalDevice,
			vk::device,
			vk::queueFamilyIndices[ 0 ],
			vk::queues[ 0 ][ 0 ],
			VK_NULL_HANDLE,
			descriptorPool,
			vk::swapchainImages.GetSize(),
			vk::swapchainImages.GetSize(),
			VK_SAMPLE_COUNT_1_BIT,
			nullptr,
			nullptr
		};

		ImGui_ImplVulkan_Init( &vulkanInitInfo, vk::renderPass );

		VkCommandBuffer commandBuffer;

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			vk::commandPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1
		};
		
		vkAllocateCommandBuffers( vk::device, &commandBufferAllocateInfo, &commandBuffer );

		vkResetCommandBuffer( commandBuffer, 0 );

		VkCommandBufferBeginInfo beginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			nullptr
		};
		vkBeginCommandBuffer( commandBuffer, &beginInfo );

		ImGui_ImplVulkan_CreateFontsTexture( commandBuffer );

		vkEndCommandBuffer( commandBuffer );

		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			0,
			nullptr,
			nullptr,
			1,
			&commandBuffer,
			0,
			nullptr
		};
		vkQueueSubmit( vk::queues[ 0 ][ 0 ], 1, &submitInfo, VK_NULL_HANDLE );

		vkQueueWaitIdle( vk::queues[ 0 ][ 0 ] );

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
	
	void EditorGuiSystem::Update() {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		static bool show_demo_window = true;

		if (show_demo_window) {
			ImGui::ShowDemoWindow( &show_demo_window );
		}
		
		ImGui::Render();
	}
	
	DEFINE_SYSTEM( EditorGuiSystem, 10000 )
}

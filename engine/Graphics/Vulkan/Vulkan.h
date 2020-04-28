// ReSharper disable CppUnusedIncludeDirective
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// ReSharper disable once CppUnusedIncludeDirective
#include "vulkan/vulkan.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "VulkanFunctions.h"
#include "Collections/Array.h"
#include "FileSystem/Guid.h"
#include "Collections/Map.h"
#include "Resources/ModelResource.h"

namespace axlt::vk {

	extern HMODULE library;
	extern VkInstance instance;
	extern VkPhysicalDevice physicalDevice;
	extern VkPhysicalDeviceProperties deviceProperties;
	extern Array<VkQueueFamilyProperties> queueFamilyProperties;
	extern VkDevice device;
	extern Array<Array<VkQueue>> queues;
	extern VkSurfaceKHR presentationSurface;
	extern VkQueue presentationQueue;
	extern VkSwapchainKHR swapchain;
	extern VkExtent2D swapchainExtents;
	extern VkSurfaceFormatKHR surfaceFormat;
	extern Array<VkImage> swapchainImages;
	extern Array<VkImageView> swapchainImageViews;
	extern VkCommandPool commandPool;
	extern Array<VkCommandBuffer> commandBuffers;
	extern VkRenderPass renderPass;
	extern VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	
	extern Array<VkBuffer> perCameraBuffers;
	extern Array<VkDeviceMemory> perCameraBuffersMemory;
	extern VkDescriptorSetLayout sharedSetLayout;

	extern VkSemaphore imageAvailableSemaphore;
	extern VkSemaphore renderingFinishedSemaphore;
	
	inline struct PerCameraUniformBuffer {
		Matrix4 projectionMatrix;
		Matrix4 viewMatrix;
		Matrix4 pvMatrix;
		Vector3 cameraPosition;
	} perCameraUniformBuffer;

	inline struct PerDrawUniformBuffer {
		Matrix4 modelMatrix;
		Matrix4 pvmMatrix;
	} perDrawUniformBuffer;

	struct DrawBuffers {
		// size is 10 times the amount of meshes in model
		// last buffer buffer in each 10 is index buffer
		ExactArray<VkBuffer> buffers;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};
	
	struct TechniqueData {
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
		uint16_t usedBuffers = 0;
	};
	
	struct MaterialData {
		struct PerCommandBuffer {
			Array<VkBuffer> uniformBuffers;
			Array<VkDeviceMemory> uniformBuffersMemory;
			Array<VkSampler> samplers;
		};
		Guid techniqueGuid = Guid::invalidGuid;
		Array<VkDescriptorSet> descriptorSets;

		Array<PerCommandBuffer> perFrameData;
	};


	inline Map<Guid, DrawBuffers> meshBuffers;
	inline Map<Guid, TechniqueData> techniqueDataArray;
	inline Map<Guid, MaterialData> materialDataArray;

	const uint32_t buffersPerMesh = 5 + MAX_UV_CHANNELS + MAX_COLOR_CHANNELS;

	bool Initialize( HINSTANCE hInstance, HWND hWnd, uint32_t width, uint32_t height );
	void Update();
	void BindResources();
	void Draw();
	void Shutdown();
}

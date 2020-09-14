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
#include "Graphics/MaterialResource.h"
#include "Graphics/TextureResource.h"

namespace axlt::vk {

	extern HMODULE library;
	extern VkInstance instance;
	extern VkPhysicalDevice physicalDevice;
	extern VkPhysicalDeviceProperties deviceProperties;
	extern Array<VkQueueFamilyProperties> queueFamilyProperties;
	extern VkDevice device;
	extern Array<Array<VkQueue>> queues;
	extern Array<uint32_t> queueFamilyIndices;
	extern VkSurfaceKHR presentationSurface;
	extern VkQueue presentationQueue;
	extern uint32_t presentationQueueFamilyIndex;
	extern VkSwapchainKHR swapchain;
	extern VkExtent2D swapchainExtents;
	extern VkSurfaceFormatKHR surfaceFormat;
	extern Array<VkImage> swapchainImages;
	extern Array<VkImageView> swapchainImageViews;
	extern Array<VkFramebuffer> framebuffers;
	extern VkFormat depthTextureFormat;
	extern VkImage depthImage;
	extern VkDeviceMemory depthImageMemory;
	extern VkImageView depthImageView;
	extern VkCommandPool commandPool;
	extern VkCommandPool copyCommandPool;
	extern Array<VkCommandBuffer> commandBuffers;
	extern Array<VkFence> renderFences;
	extern VkRenderPass renderPass;
	extern VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	
	extern Array<VkBuffer> perCameraBuffers;
	extern Array<VkDeviceMemory> perCameraBuffersMemory;

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

	const uint32_t buffersPerMesh = 5 + MAX_COLOR_CHANNELS + MAX_UV_CHANNELS;

	bool AllocateMemory( const VkMemoryRequirements& memoryRequirements,
						 VkMemoryPropertyFlagBits memoryProperties,
						 VkDeviceMemory* memoryObject );

	VkFormat GetInputFormat( ShaderType type, ShaderPrecision precision, 
							 uint8_t vectorSize );

	bool Initialize( HINSTANCE hInstance, HWND hWnd, uint32_t width, uint32_t height );
	void ResizeFramebuffer(uint32_t width, uint32_t height);
	void Draw();
	void Shutdown();
}

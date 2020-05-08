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
	extern VkCommandPool commandPool;
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

	struct TextureData {
		VkImage image;
		VkImageView imageView;
		VkSampler sampler;

		VkDeviceMemory imageMemory;
	};

	struct DrawBuffers {
		// size is 10 times the amount of meshes in model
		// last buffer buffer in each 10 is index buffer
		ExactArray<VkBuffer> buffers;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};
	
	struct TechniqueData {
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;
		Array<VkDescriptorSetLayout> layouts;
		uint16_t usedBuffers = 0;
		Array<VkShaderModule> shaderModules;
	};
	
	struct MaterialData {
		struct PerCommandBuffer {
			Array<VkBuffer> uniformBuffers;
			Array<VkDeviceMemory> uniformBuffersMemory;
			Array<VkDescriptorSet> descriptorSets;
			BitArray<> dirtyUniformBuffers;
		};
		Guid techniqueGuid = Guid::invalidGuid;
		
		VkDescriptorPool descriptorPool;

		Array<PerCommandBuffer> perFrameData;
	};

	inline Map<Guid, TextureData> textureDataArray;
	inline Map<Guid, DrawBuffers> meshBuffers;
	inline Map<Guid, TechniqueData> techniqueDataArray;
	inline Map<Guid, MaterialData> materialDataArray;

	const uint32_t buffersPerMesh = 5 + MAX_COLOR_CHANNELS + MAX_UV_CHANNELS;

	bool AllocateMemory( const VkMemoryRequirements& memoryRequirements,
						 VkMemoryPropertyFlagBits memoryProperties,
						 VkDeviceMemory* memoryObject );

	VkFormat GetInputFormat( ShaderType type, ShaderPrecision precision, 
							 uint8_t vectorSize );

	bool Initialize( HINSTANCE hInstance, HWND hWnd, uint32_t width, uint32_t height );


	void Update();
	void BindResources();
	void Draw();

	void CreateTextureBuffer( const ResourceHandle<TextureResource>& texture );
	void CreateDrawBuffers( const ResourceHandle<ModelResource>& model );
	void CreateTechniqueData( const ResourceHandle<TechniqueResource>& technique );
	MaterialData* CreateMaterialData( ResourceHandle<MaterialResource>& material, TechniqueData& techniqueData );

	void CopyAllBuffers();
	void CreateAllPipelines();
	void SetupDescriptorSets();
	
	void ResizeFramebuffer(uint32_t width, uint32_t height);

	void Shutdown();
}

#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	bool CreateImage( VkImageType type, VkFormat format, VkExtent3D size, uint32_t mipLevels, uint32_t layerCount,
					  VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap, VkImage& image );

	bool BindMemoryToImage( VkImage image,
							VkMemoryPropertyFlagBits memoryProperties = (VkMemoryPropertyFlagBits) 0,
							VkDeviceMemory memoryObject = VK_NULL_HANDLE, uint32_t memoryOffset = 0 );

	struct ImageTransition {
		VkImage image{};
		VkImageAspectFlags aspect{};
		VkAccessFlags oldAccess{};
		VkAccessFlags newAccess{};
		VkImageLayout oldLayout{};
		VkImageLayout newLayout{};
		uint32_t oldQueueFamily = VK_QUEUE_FAMILY_IGNORED;
		uint32_t newQueueFamily = VK_QUEUE_FAMILY_IGNORED;
	};

	void SetImageBarriers( Array<ImageTransition>& imageTransitions, VkCommandBuffer commandBuffer,
						   VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages );

	bool CreateImageView( VkImage image, VkImageViewType viewType, VkFormat format,
						  VkImageAspectFlags aspectMask, VkImageView& imageView );
		
	void CopyDataFromBuffer( VkCommandBuffer commandBuffer, VkBuffer source,
							 VkImage destination, VkImageLayout layout,
							 Array<VkBufferImageCopy>& bufferCopyInfo );


	bool CreateSampler( VkImage image, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipMapMode,
						VkSamplerAddressMode addressModeU, VkSamplerAddressMode addressModeV,
						VkSamplerAddressMode addressModeW, float mipLodBias, bool anisotropyEnabled,
						float maxAnisotropy, bool compareEnabled, VkCompareOp compareOp,
						float minLod, float maxLod, VkBorderColor borderColor, bool unnormalizedCoords,
						VkSampler& sampler );

	void DestroySampler( VkSampler& sampler );
	void DestroyImageView( VkImageView& imageView );
	void DestroyImage( VkImage& image );
}
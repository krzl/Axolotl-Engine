#include "stdafx.h"
#include "RenderTextureBinding.h"
#include "Common.h"

namespace axlt::vk {

	SparseArray<RenderTextureBinding> RenderTextureBinding::bindings;

	static VkFormat GetDepthFormat() {
		FixedArray<VkFormat, 5> depthFormats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};

		for( VkFormat format : depthFormats ) {
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties( physicalDevice, format, &formatProps );
			if( formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) {
				return format;
			}
		}

		throw 0;
	}

	RenderTextureBinding& RenderTextureBinding::CreateBinding( const ResourceHandle<RenderTextureResource>& texture ) {
		RenderTextureBinding& binding = *(RenderTextureBinding*)bindings.Emplace().element;
		bindingMap.Add( texture->GetInstanceId(), &binding );

		return binding;
	}

	void RenderTextureBinding::UpdateBinding( const ResourceHandle<RenderTextureResource>& texture ) {
		VkImageCreateInfo colorImageCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			0,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_R8G8B8A8_UNORM,
			{
				texture->width,
				texture->height,
				1
			},
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr,
			VK_IMAGE_LAYOUT_UNDEFINED
		};

		VkResult result = vkCreateImage( device, &colorImageCreateInfo, nullptr, &colorAttachment.image );
		if (result != VK_SUCCESS) {
			printf( "Could not create image\n" );
			return;
		}

		VkMemoryRequirements colorMemoryRequirements;
		vkGetImageMemoryRequirements( device, colorAttachment.image, &colorMemoryRequirements );

		if (!AllocateMemory( colorMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &colorAttachment.imageMemory )) {
			return;
		}

		result = vkBindImageMemory( device, colorAttachment.image, colorAttachment.imageMemory, 0 );
		if (result != VK_SUCCESS) {
			printf( "Could not bind image memory\n" );
			return;
		}

		VkImageViewCreateInfo colorImageViewCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			colorAttachment.image,
			VK_IMAGE_VIEW_TYPE_2D,
			colorImageCreateInfo.format,
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			},
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				1,
				0,
				1
			}
		};

		result = vkCreateImageView( device, &colorImageViewCreateInfo, nullptr, &colorAttachment.imageView );
		if (result != VK_SUCCESS) {
			printf( "Could not create image view\n" );
			return;
		}

		VkSamplerCreateInfo colorSamplerCreateInfo = {
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			nullptr,
			0,
			VK_FILTER_LINEAR,
			VK_FILTER_LINEAR,
			VK_SAMPLER_MIPMAP_MODE_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VK_FALSE,
			0,
			VK_FALSE,
			VK_COMPARE_OP_ALWAYS,
			0.0f,
			1.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			VK_FALSE
		};

		result = vkCreateSampler( device, &colorSamplerCreateInfo, nullptr, &sampler );
		if (result != VK_SUCCESS) {
			printf( "Could not create sampler\n" );
			return;
		}

		FixedArray<VkAttachmentDescription, 2> attachmentDescriptions;

		attachmentDescriptions.Emplace( VkAttachmentDescription {
			0,
			colorImageCreateInfo.format,
			colorImageCreateInfo.samples,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		} );

		if( texture->hasDepth ) {
			VkImageCreateInfo depthImageCreateInfo = {
				VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				nullptr,
				0,
				VK_IMAGE_TYPE_2D,
				GetDepthFormat(),
				{
					texture->width,
					texture->height,
					1
				},
				1,
				1,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_SHARING_MODE_EXCLUSIVE,
				0,
				nullptr,
				VK_IMAGE_LAYOUT_UNDEFINED
			};

			result = vkCreateImage( device, &depthImageCreateInfo, nullptr, &depthAttachment.image );
			if (result != VK_SUCCESS) {
				printf( "Could not create image\n" );
				return;
			}

			VkMemoryRequirements depthMemoryRequirements;
			vkGetImageMemoryRequirements( device, colorAttachment.image, &depthMemoryRequirements );

			if (!AllocateMemory( depthMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthAttachment.imageMemory )) {
				return;
			}

			result = vkBindImageMemory( device, depthAttachment.image, depthAttachment.imageMemory, 0 );
			if (result != VK_SUCCESS) {
				printf( "Could not bind image memory\n" );
				return;
			}

			VkImageViewCreateInfo depthImageViewCreateInfo = {
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				nullptr,
				0,
				depthAttachment.image,
				VK_IMAGE_VIEW_TYPE_2D,
				depthImageCreateInfo.format,
				{
					VK_COMPONENT_SWIZZLE_IDENTITY,
					VK_COMPONENT_SWIZZLE_IDENTITY,
					VK_COMPONENT_SWIZZLE_IDENTITY,
					VK_COMPONENT_SWIZZLE_IDENTITY
				},
				{
					VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
					0,
					1,
					0,
					1
				}
			};

			result = vkCreateImageView( device, &depthImageViewCreateInfo, nullptr, &depthAttachment.imageView );
			if( result != VK_SUCCESS ) {
				printf( "Could not create image view\n" );
				return;
			}
			
			attachmentDescriptions.Emplace( VkAttachmentDescription {
				0,
				depthImageCreateInfo.format,
				depthImageCreateInfo.samples,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			} );
		}

		VkAttachmentReference colorReference = {
			0,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		};

		VkAttachmentReference depthReference = {
			1,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpassDescription = {
			0,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			0,
			nullptr,
			1,
			&colorReference,
			nullptr,
			texture->hasDepth ? &depthReference : nullptr,
			0,
			nullptr
		};

		FixedArray<VkSubpassDependency, 2> dependencies;

		dependencies.Emplace( VkSubpassDependency {
			VK_SUBPASS_EXTERNAL,
			0,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_DEPENDENCY_BY_REGION_BIT
		} );

		dependencies.Emplace( VkSubpassDependency{
			0,
			VK_SUBPASS_EXTERNAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_DEPENDENCY_BY_REGION_BIT
		} );

		VkRenderPassCreateInfo renderPassCreateInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			nullptr,
			0,
			attachmentDescriptions.GetSize(),
			attachmentDescriptions.GetData(),
			1,
			&subpassDescription,
			dependencies.GetSize(),
			dependencies.GetData()
		};

		result = vkCreateRenderPass( device, &renderPassCreateInfo, nullptr, &renderPass );
		if( result != VK_SUCCESS ) {
			printf( "Could not create render pass\n" );
			return;
		}

		FixedArray<VkImageView, 2> attachments;
		attachments.Emplace( colorAttachment.imageView );
		if( texture->hasDepth ) {
			attachments.Emplace( depthAttachment.imageView );
		}

		VkFramebufferCreateInfo framebufferCreateInfo = {
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			renderPass,
			attachments.GetSize(),
			attachments.GetData(),
			texture->width,
			texture->height,
			1
		};

		result = vkCreateFramebuffer( device, &framebufferCreateInfo, nullptr, &framebuffer );
		if( result != VK_SUCCESS ) {
			printf( "Could not create framebuffer\n" );
			return;
		}
	}

	void RenderAttachmentData::Reset() {
		if( image != VK_NULL_HANDLE ) {
			vkDestroyImage( device, image, nullptr );
			image = VK_NULL_HANDLE;
		}
		if( imageView != VK_NULL_HANDLE ) {
			vkDestroyImageView( device, imageView, nullptr );
			imageView = VK_NULL_HANDLE;
		}
		if( imageMemory != VK_NULL_HANDLE ) {
			vkFreeMemory( device, imageMemory, nullptr );
			imageMemory = VK_NULL_HANDLE;
		}
	}

	void RenderTextureBinding::Reset() {
		if( framebuffer != VK_NULL_HANDLE ) {
			vkDestroyFramebuffer( device, framebuffer, nullptr );
			framebuffer = VK_NULL_HANDLE;
		}
		if( renderPass != VK_NULL_HANDLE ) {
			vkDestroyRenderPass( device, renderPass, nullptr );
			renderPass = VK_NULL_HANDLE;
		}
		if( sampler != VK_NULL_HANDLE ) {
			vkDestroySampler( device, sampler, nullptr );
			sampler = VK_NULL_HANDLE;
		}
		colorAttachment.Reset();
		depthAttachment.Reset();
	}

	void RenderTextureBinding::RemoveBinding() {
		const uint64_t ptrDiff = (uint64_t) this - (uint64_t)bindings.GetData();
		bindings.Remove( (uint32_t) ( ptrDiff / sizeof( RenderTextureBinding ) ) );
	}
}

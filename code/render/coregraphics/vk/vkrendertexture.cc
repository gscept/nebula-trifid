//------------------------------------------------------------------------------
// vkrendertexture.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkrendertexture.h"
#include "vktypes.h"
#include "vkrenderdevice.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkRenderTexture, 'VKRT', Base::RenderTextureBase);
//------------------------------------------------------------------------------
/**
*/
VkRenderTexture::VkRenderTexture()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkRenderTexture::~VkRenderTexture()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTexture::Setup()
{
	RenderTextureBase::Setup();
	VkSampleCountFlagBits sampleCount = this->msaaEnabled ? VK_SAMPLE_COUNT_16_BIT : VK_SAMPLE_COUNT_1_BIT;

	VkExtent3D extents;
	extents.width = this->width;
	extents.height = this->height;
	extents.depth = this->depth;

	VkImageUsageFlags usageFlags = this->usage == ColorAttachment ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT :
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	VkImageCreateInfo imgInfo =
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		NULL,
		0,
		VK_IMAGE_TYPE_2D,
		VkTypes::AsVkFramebufferFormat(this->format),
		extents,
		1,
		1,
		sampleCount,
		VK_IMAGE_TILING_OPTIMAL,
		usageFlags,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		NULL,
		VK_IMAGE_LAYOUT_UNDEFINED
	};

	// create image for rendering
	VkResult res = vkCreateImage(VkRenderDevice::dev, &imgInfo, NULL, &this->img);
	n_assert(res == VK_SUCCESS);

	// allocate buffer backing and bind to image
	uint32_t size;
	VkRenderDevice::Instance()->AllocateImageMemory(this->img, this->mem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
	vkBindImageMemory(VkRenderDevice::dev, this->img, this->mem, 0);

	VkImageSubresourceRange subres;
	subres.baseArrayLayer = 0;
	subres.baseMipLevel = 0;
	subres.layerCount = 1;
	subres.levelCount = 1;
	subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	VkImageViewCreateInfo viewInfo =
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		NULL,
		0,
		this->img,
		VK_IMAGE_VIEW_TYPE_2D,
		VkTypes::AsVkFramebufferFormat(this->format),
		VkTypes::AsVkMapping(this->format),
		subres
	};

	res = vkCreateImageView(VkRenderDevice::dev, &viewInfo, NULL, &this->view);
	n_assert(res == VK_SUCCESS);

	if (this->usage == ColorAttachment)
	{
		// setup actual texture
		if (sampleCount > 1)
		{
			this->texture->SetupFromVkMultisampleTexture(this->img, this->mem, this->view, this->width, this->height, this->format, 0, true, true);
		}
		else
		{
			this->texture->SetupFromVkTexture(this->img, this->mem, this->view, this->width, this->height, this->format, 0, true, true);
		}

		// clear image and transition layout
		VkClearColorValue clear = { 0, 0, 0, 0 };
		VkRenderDevice::Instance()->PushImageLayoutTransition(VkDeferredCommand::Graphics, VkRenderDevice::ImageMemoryBarrier(this->img, subres, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL));
		VkRenderDevice::Instance()->PushImageColorClear(this->img, VkDeferredCommand::Graphics, VK_IMAGE_LAYOUT_GENERAL, clear, subres);
		VkRenderDevice::Instance()->PushImageLayoutTransition(VkDeferredCommand::Graphics, VkRenderDevice::ImageMemoryBarrier(this->img, subres, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
	}
	else
	{
		// clear image and transition layout
		VkClearDepthStencilValue clear = { 1, 0 };
		VkRenderDevice::Instance()->PushImageLayoutTransition(VkDeferredCommand::Graphics, VkRenderDevice::ImageMemoryBarrier(this->img, subres, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL));
		VkRenderDevice::Instance()->PushImageDepthStencilClear(this->img, VkDeferredCommand::Graphics, VK_IMAGE_LAYOUT_GENERAL, clear, subres);
		VkRenderDevice::Instance()->PushImageLayoutTransition(VkDeferredCommand::Graphics, VkRenderDevice::ImageMemoryBarrier(this->img, subres, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL));
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTexture::Discard()
{
	// base class will destroy image, thus freeing image memory and image object
	RenderTextureBase::Discard();
	vkDestroyImageView(VkRenderDevice::dev, this->view, NULL);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTexture::Resize()
{

}


} // namespace Vulkan
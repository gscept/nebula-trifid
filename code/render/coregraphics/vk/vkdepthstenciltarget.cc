//------------------------------------------------------------------------------
// vkdepthstenciltarget.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkdepthstenciltarget.h"
#include "../displaydevice.h"
#include "vkrenderdevice.h"

namespace Vulkan
{

using namespace CoreGraphics;
__ImplementClass(Vulkan::VkDepthStencilTarget, 'VKDS', Base::DepthStencilTargetBase);
//------------------------------------------------------------------------------
/**
*/
VkDepthStencilTarget::VkDepthStencilTarget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkDepthStencilTarget::~VkDepthStencilTarget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkDepthStencilTarget::Setup()
{
	// setup base class
	DepthStencilTargetBase::Setup();

	// if we have a relative size on the dept-stencil target, calculate actual size
	if (this->useRelativeSize)
	{
		DisplayDevice* displayDevice = DisplayDevice::Instance();
		this->SetWidth(SizeT(displayDevice->GetDisplayMode().GetWidth() * this->relWidth));
		this->SetHeight(SizeT(displayDevice->GetDisplayMode().GetHeight() * this->relHeight));
	}

	VkExtent3D extents;
	extents.width = this->width;
	extents.height = this->height;
	extents.depth = 1;

	VkImageCreateInfo imgInfo =
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		NULL,
		0,
		VK_IMAGE_TYPE_2D,
		VK_FORMAT_D24_UNORM_S8_UINT,
		extents,
		1,
		1,
		VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		1,
		&VkRenderDevice::Instance()->renderQueueIdx,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	// create image
	VkResult res = vkCreateImage(VkRenderDevice::dev, &imgInfo, NULL, &this->image);
	n_assert(res == VK_SUCCESS);

	// allocate buffer backing and bind to image
	uint32_t size;
	VkRenderDevice::Instance()->AllocateImageMemory(this->image, this->mem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
	vkBindImageMemory(VkRenderDevice::dev, this->image, this->mem, 0);

	VkImageSubresourceRange subres;
	subres.baseArrayLayer = 0;
	subres.baseMipLevel = 0;
	subres.layerCount = 1;
	subres.levelCount = 1;
	VkComponentMapping mapping;
	mapping.r = VK_COMPONENT_SWIZZLE_R;
	mapping.g = VK_COMPONENT_SWIZZLE_G;
	mapping.b = VK_COMPONENT_SWIZZLE_B;
	mapping.a = VK_COMPONENT_SWIZZLE_A;
	VkImageViewCreateInfo viewInfo =
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		NULL,
		0,
		this->image,
		VK_IMAGE_VIEW_TYPE_2D,
		VK_FORMAT_D24_UNORM_S8_UINT,
		mapping,
		subres
	};

	res = vkCreateImageView(VkRenderDevice::dev, &viewInfo, NULL, &this->view);
	n_assert(res == VK_SUCCESS);
}

//------------------------------------------------------------------------------
/**
*/
void
VkDepthStencilTarget::Discard()
{
	DepthStencilTargetBase::Discard();
	vkDestroyImageView(VkRenderDevice::dev, this->view, NULL);
	vkDestroyImage(VkRenderDevice::dev, this->image, NULL);
	vkFreeMemory(VkRenderDevice::dev, this->mem, NULL);
}

//------------------------------------------------------------------------------
/**
*/
void
VkDepthStencilTarget::OnDisplayResized(SizeT width, SizeT height)
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkDepthStencilTarget::BeginPass()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkDepthStencilTarget::EndPass()
{

}

} // namespace Vulkan
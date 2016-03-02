//------------------------------------------------------------------------------
// vkrendertarget.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkrendertarget.h"
#include "vkrenderdevice.h"
#include "vktypes.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkRenderTarget, 'VURT', Base::RenderTargetBase);
//------------------------------------------------------------------------------
/**
*/
VkRenderTarget::VkRenderTarget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkRenderTarget::~VkRenderTarget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::Setup()
{
	// call parent class
	RenderTargetBase::Setup();

	if (this->isDefaultRenderTarget)
	{

	}
	else
	{
		SizeT resolveWidth = this->resolveTextureDimensionsValid ? this->resolveTextureWidth : this->width;
		SizeT resolveHeight = this->resolveTextureDimensionsValid ? this->resolveTextureHeight : this->height;

		VkExtent3D extents;
		extents.width = resolveWidth;
		extents.height = resolveHeight;
		extents.depth = 1;

		VkSampleCountFlagBits sampleCount = 
			 this->antiAliasQuality == CoreGraphics::AntiAliasQuality::None ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT :
			this->antiAliasQuality == CoreGraphics::AntiAliasQuality::Low ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT :
			this->antiAliasQuality == CoreGraphics::AntiAliasQuality::Medium ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT : VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;

		VkImageCreateInfo imgInfo = 
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			NULL,
			0,
			VK_IMAGE_TYPE_2D,
			VkTypes::AsVkFormat(this->colorBufferFormat),
			extents,
			1,
			1,
			sampleCount,
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
		VkRenderDevice::Instance()->AllocateImageMemory(this->image, this->imageMem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
		vkBindImageMemory(VkRenderDevice::dev, this->image, this->imageMem, 0);

		VkImageSubresourceRange subres;
		subres.baseArrayLayer = 0;
		subres.baseMipLevel = 0;
		subres.layerCount = 1;
		subres.levelCount = 1;
		VkImageViewCreateInfo viewInfo =
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			NULL,
			0,
			this->image,
			VK_IMAGE_VIEW_TYPE_2D,
			VkTypes::AsVkFormat(this->colorBufferFormat),
			VkTypes::AsVkMapping(this->colorBufferFormat),
			subres
		};

		res = vkCreateImageView(VkRenderDevice::dev, &viewInfo, NULL, &this->imageView);
		n_assert(res == VK_SUCCESS);

		uint32_t numviews = 1;
		VkImageView views[2];
		views[0] = this->imageView;

		if (this->depthStencilTarget.isvalid()) views[numviews++] = this->depthStencilTarget->GetVkImageView();

		VkFramebufferCreateInfo fbInfo = 
		{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			NULL,
			0,
			this->pass,
			numviews,
			views,
			resolveWidth, resolveHeight, 1		// dimensions is 
		};

		// create framebuffer
		res = vkCreateFramebuffer(VkRenderDevice::dev, &fbInfo, NULL, &this->framebuffer);
		n_assert(res == VK_SUCCESS);

		// we create a very default attachment behavior where we assume to write 
		// to the framebuffer and read from it once the operations are done
		VkAttachmentDescription attachment;
		attachment.format = VkTypes::AsVkFormat(this->colorBufferFormat);
		attachment.samples = sampleCount;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = VK_ATTACHMENT_UNUSED;
		VkAttachmentReference resolveAttachmentRef;
		resolveAttachmentRef.attachment = VK_ATTACHMENT_UNUSED;

		VkAttachmentReference attachmentRef;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachmentRef.attachment = 0;

		// create just one subpass which is just using our single color attachment and optional depth stencil attachment
		VkSubpassDescription subpass;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = VK_NULL_HANDLE;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &attachmentRef;
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = VK_NULL_HANDLE;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &resolveAttachmentRef;

		if (this->depthStencilTarget.isvalid())
		{
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			depthAttachmentRef.attachment = 1;
		}

		// setup render pass info
		VkRenderPassCreateInfo renderPassInfo =
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			NULL,
			0,
			1,
			&attachment,
			1,
			&subpass,
			0,
			VK_NULL_HANDLE
		};

		// create pass
		res = vkCreateRenderPass(VkRenderDevice::dev, &renderPassInfo, NULL, &this->pass);
		n_assert(res == VK_SUCCESS);
		
		// setup info
		this->framebufferPipelineInfo.renderPass = this->pass;
		this->framebufferPipelineInfo.subpass = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::Discard()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::OnDisplayResized(SizeT width, SizeT height)
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::SetResolveRect(const Math::rectangle<int>& r)
{
	this->viewports.SetSize(1);
	VkViewport viewport;
	viewport.width = (float)r.width();
	viewport.height = (float)r.height();
	viewport.x = (float)r.left;
	viewport.y = (float)r.top;
	this->viewports[0] = viewport;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::SetResolveRectArray(const Util::Array<Math::rectangle<int> >& rects)
{
	this->viewports.SetSize(rects.Size());
	IndexT i;
	for (i = 0; i < rects.Size(); i++)
	{
		VkViewport viewport;
		viewport.width = (float)rects[i].width();
		viewport.height = (float)rects[i].height();
		viewport.x = (float)rects[i].left;
		viewport.y = (float)rects[i].top;
		this->viewports[i] = viewport;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::ResetResolveRects()
{
	this->viewports.Clear();
}

//------------------------------------------------------------------------------
/**
*/
const Util::FixedArray<VkViewport>&
VkRenderTarget::GetVkViewports()
{
	return this->viewports;
}

} // namespace Vulkan
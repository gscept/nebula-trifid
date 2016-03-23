//------------------------------------------------------------------------------
// vkrendertarget.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkrendertarget.h"
#include "vkrenderdevice.h"
#include "vktypes.h"
#include "resources/resourcemanager.h"
#include "../displaydevice.h"

namespace Vulkan
{

using namespace Resources;
using namespace CoreGraphics;
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

	this->viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	this->viewportInfo.pNext = NULL;
	this->viewportInfo.flags = 0;
	this->viewportInfo.scissorCount = 0;
	this->viewportInfo.pScissors = NULL;

	if (this->isDefaultRenderTarget)
	{
		DisplayDevice* displayDevice = DisplayDevice::Instance();
		this->SetWidth(displayDevice->GetDisplayMode().GetWidth());
		this->SetHeight(displayDevice->GetDisplayMode().GetHeight());
		this->SetAntiAliasQuality(AntiAliasQuality::None);
		this->SetColorBufferFormat(displayDevice->GetDisplayMode().GetPixelFormat());

		VkViewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = (float)displayDevice->GetDisplayMode().GetWidth();
		viewport.height = (float)displayDevice->GetDisplayMode().GetHeight();
		viewport.minDepth = 0;
		viewport.maxDepth = FLT_MAX;
		this->viewports.Resize(1);
		this->viewports[0] = viewport;

		VkRect2D scissor;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = displayDevice->GetDisplayMode().GetWidth();
		scissor.extent.height = displayDevice->GetDisplayMode().GetHeight();
		this->scissors.Resize(1);
		this->scissors[0] = scissor;

		this->viewportInfo.viewportCount = this->viewports.Size();
		this->viewportInfo.pViewports = this->viewports.Begin();
		this->viewportInfo.scissorCount = this->scissors.Size();
		this->viewportInfo.pScissors = this->scissors.Begin();
	}
	else
	{
		if (this->relativeSizeValid)
		{
			DisplayDevice* displayDevice = DisplayDevice::Instance();
			this->SetWidth(SizeT(displayDevice->GetDisplayMode().GetWidth() * this->relWidth));
			this->SetHeight(SizeT(displayDevice->GetDisplayMode().GetHeight() * this->relHeight));
		}		

		SizeT resolveWidth = this->resolveTextureDimensionsValid ? this->resolveTextureWidth : this->width;
		SizeT resolveHeight = this->resolveTextureDimensionsValid ? this->resolveTextureHeight : this->height;

		// setup viewport
		VkViewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = (float)resolveWidth;
		viewport.height = (float)resolveHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		this->viewports.Resize(1);
		this->viewports[0] = viewport;

		// setup scissor rect
		VkRect2D scissor;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = resolveWidth;
		scissor.extent.height = resolveHeight;
		this->scissors.Resize(1);
		this->scissors[0] = scissor;

		this->viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		this->viewportInfo.pNext = NULL;
		this->viewportInfo.flags = 0;
		this->viewportInfo.viewportCount = this->viewports.Size();
		this->viewportInfo.pViewports = this->viewports.Begin();
		this->viewportInfo.scissorCount = this->scissors.Size();
		this->viewportInfo.pScissors = this->scissors.Begin();

		if (this->depthStencilTarget.isvalid()) this->clearValues.Resize(2);
		else									this->clearValues.Resize(1);

		this->clearValues[0].color.float32[0] = this->clearColor.x();
		this->clearValues[0].color.float32[1] = this->clearColor.y();
		this->clearValues[0].color.float32[2] = this->clearColor.z();
		this->clearValues[0].color.float32[3] = this->clearColor.w();

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
		uint32_t numattachments = 1;
		VkImageView views[2];
		views[0] = this->imageView;

		if (this->depthStencilTarget.isvalid())
		{
			views[numviews++] = this->depthStencilTarget->GetVkImageView();
			this->clearValues[1].depthStencil.depth = this->depthStencilTarget->GetClearDepth();
			this->clearValues[1].depthStencil.stencil = this->depthStencilTarget->GetClearStencil();
		}

		// we create a very default attachment behavior where we assume to write 
		// to the framebuffer and read from it once the operations are done
		VkAttachmentDescription attachment[2];
		attachment[0].format = VkTypes::AsVkFramebufferFormat(this->colorBufferFormat);
		attachment[0].samples = sampleCount;
		attachment[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;						// set to clear
		attachment[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment[0].stencilLoadOp = this->clearFlags & CoreGraphics::RenderTarget::ClearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		attachment[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = VK_ATTACHMENT_UNUSED;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		VkAttachmentReference resolveAttachmentRef;
		resolveAttachmentRef.attachment = VK_ATTACHMENT_UNUSED;
		resolveAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference attachmentRef;
		attachmentRef.attachment = 0;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// create just one subpass which is just using our single color attachment and optional depth stencil attachment
		VkSubpassDescription subpass;
		subpass.flags = 0;
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

			attachment[1].format = VK_FORMAT_D24_UNORM_S8_UINT;
			attachment[1].samples = VK_SAMPLE_COUNT_1_BIT;
			attachment[1].loadOp = depthStencilTarget->GetClearFlags() & CoreGraphics::DepthStencilTarget::ClearDepth ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			attachment[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachment[1].stencilLoadOp = depthStencilTarget->GetClearFlags() & CoreGraphics::DepthStencilTarget::ClearStencil ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			attachment[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachment[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachment[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			numattachments++;
		}

		// setup render pass info
		VkRenderPassCreateInfo renderPassInfo =
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			NULL,
			0,
			numattachments,
			attachment,
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
		this->framebufferPipelineInfo.pViewportState = &this->viewportInfo;

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

		// setup texture resource
		if (this->resolveTextureResId.IsValid())
		{
			this->resolveTexture = ResourceManager::Instance()->CreateUnmanagedResource(this->resolveTextureResId, Texture::RTTI).downcast<Texture>();
		}
		else
		{
			// just create a texture natively without managing it
			this->resolveTexture = CoreGraphics::Texture::Create();
		}

		// setup actual texture
		if (sampleCount > 1)
		{
			this->resolveTexture->SetupFromVkMultisampleTexture(this->image, this->imageMem, this->colorBufferFormat, 0, true, true);
		}
		else
		{
			this->resolveTexture->SetupFromVkTexture(this->image, this->imageMem, this->colorBufferFormat, 0, true, true);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::Discard()
{
	RenderTargetBase::Discard();
	vkDestroyImageView(VkRenderDevice::dev, this->imageView, NULL);
	vkDestroyImage(VkRenderDevice::dev, this->image, NULL);
	vkFreeMemory(VkRenderDevice::dev, this->imageMem, NULL);
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
	this->viewports.Resize(1);
	VkViewport viewport;
	viewport.width = (float)r.width();
	viewport.height = (float)r.height();
	viewport.x = (float)r.left;
	viewport.y = (float)r.top;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	this->viewports[0] = viewport;

	this->scissors.Resize(1);
	VkRect2D scissor;
	scissor.offset.x = r.left;
	scissor.offset.y = r.top;
	scissor.extent.width = r.width();
	scissor.extent.height = r.height();
	this->scissors[0] = scissor;

	this->viewportInfo.viewportCount = this->viewports.Size();
	this->viewportInfo.pViewports = this->viewports.Begin();
	this->viewportInfo.scissorCount = this->scissors.Size();
	this->viewportInfo.pScissors = this->scissors.Begin();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::SetResolveRectArray(const Util::Array<Math::rectangle<int> >& rects)
{
	this->viewports.Resize(rects.Size());
	this->scissors.Resize(rects.Size());
	
	IndexT i;
	for (i = 0; i < rects.Size(); i++)
	{
		VkViewport viewport;
		viewport.width = (float)rects[i].width();
		viewport.height = (float)rects[i].height();
		viewport.x = (float)rects[i].left;
		viewport.y = (float)rects[i].top;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		this->viewports[i] = viewport;

		VkRect2D scissor;
		scissor.offset.x = rects[i].left;
		scissor.offset.y = rects[i].top;
		scissor.extent.width = rects[i].width();
		scissor.extent.height = rects[i].height();
		this->scissors[i] = scissor;
	}

	this->viewportInfo.viewportCount = this->viewports.Size();
	this->viewportInfo.pViewports = this->viewports.Begin();
	this->viewportInfo.scissorCount = this->scissors.Size();
	this->viewportInfo.pScissors = this->scissors.Begin();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::ResetResolveRects()
{
	this->viewports.Clear();
}

} // namespace Vulkan
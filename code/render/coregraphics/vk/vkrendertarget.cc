//------------------------------------------------------------------------------
// vkrendertarget.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkrendertarget.h"
#include "vkrenderdevice.h"
#include "vktypes.h"
#include "resources/resourcemanager.h"
#include "vkdisplaydevice.h"
#include "vkshaderserver.h"
#include "coregraphics/displaydevice.h"
#include "vkutilities.h"
#include "vkscheduler.h"

namespace Vulkan
{

using namespace Resources;
using namespace CoreGraphics;
__ImplementClass(Vulkan::VkRenderTarget, 'VURT', Base::RenderTargetBase);
//------------------------------------------------------------------------------
/**
*/
VkRenderTarget::VkRenderTarget() :
	swapbufferIdx(0),
	vkTargetImage(0),
	vkTargetImageView(0),
	vkTargetImageMem(0),
	shader(0),
	dimensionsArray(0)
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
	VkScheduler* scheduler = VkScheduler::Instance();

	this->viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	this->viewportInfo.pNext = NULL;
	this->viewportInfo.flags = 0;
	this->viewportInfo.scissorCount = 0;
	this->viewportInfo.pScissors = NULL;
	this->viewportInfo.viewportCount = 0;
	this->viewportInfo.pViewports = NULL;

	VkSampleCountFlagBits sampleCount =
		this->antiAliasQuality == CoreGraphics::AntiAliasQuality::None ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT :
		this->antiAliasQuality == CoreGraphics::AntiAliasQuality::Low ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT :
		this->antiAliasQuality == CoreGraphics::AntiAliasQuality::Medium ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT : VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;

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
		viewport.maxDepth = 1;
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

		VkAttachmentReference attachmentRef;
		attachmentRef.attachment = 0;
		attachmentRef.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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
		subpass.pDepthStencilAttachment = VK_NULL_HANDLE;
		subpass.pResolveAttachments = VK_NULL_HANDLE;

		VkAttachmentDescription attachment;
		attachment.flags = 0;
		attachment.format = VkTypes::AsVkFramebufferFormat(this->colorBufferFormat);
		attachment.samples = sampleCount;
		attachment.loadOp = this->clearFlags & CoreGraphics::RenderTarget::ClearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;	// set to clear
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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
		VkResult res = vkCreateRenderPass(VkRenderDevice::dev, &renderPassInfo, NULL, &this->pass);
		n_assert(res == VK_SUCCESS);

		this->swapbuffers.Resize(VkRenderDevice::Instance()->numBackbuffers);
		this->swapimages = VkRenderDevice::Instance()->backbuffers;

		this->vkClearValues.Resize(2);
		this->vkClearValues[0].color.float32[0] = this->clearColor.x();
		this->vkClearValues[0].color.float32[1] = this->clearColor.y();
		this->vkClearValues[0].color.float32[2] = this->clearColor.z();
		this->vkClearValues[0].color.float32[3] = this->clearColor.w();

		IndexT i;
		for (i = 0; i < this->swapbuffers.Size(); i++)
		{
			const VkFramebufferCreateInfo fbInfo =
			{
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				NULL,
				0,
				this->pass,
				1,
				&VkRenderDevice::Instance()->backbufferViews[i],
				this->width,
				this->height,
				1,
			};

			// create framebuffer
			res = vkCreateFramebuffer(VkRenderDevice::dev, &fbInfo, NULL, &this->swapbuffers[i]);
			n_assert(res == VK_SUCCESS);
		}

		VkImageSubresourceRange subres;
		subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subres.baseArrayLayer = 0;
		subres.baseMipLevel = 0;
		subres.layerCount = 1;
		subres.levelCount = 1;

		VkClearColorValue clear = { 0, 0, 0, 0 };

		for (i = 0; i < this->swapimages.Size(); i++)
		{
			scheduler->PushImageLayoutTransition(VkDeferredCommand::Graphics, VkUtilities::ImageMemoryBarrier(this->swapimages[i], subres, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL));
			scheduler->PushImageColorClear(this->swapimages[i], VkDeferredCommand::Graphics, VK_IMAGE_LAYOUT_GENERAL, clear, subres);
			scheduler->PushImageLayoutTransition(VkDeferredCommand::Graphics, VkUtilities::ImageMemoryBarrier(this->swapimages[i], subres, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
		}

		// setup info
		this->framebufferPipelineInfo.renderPass = this->pass;
		this->framebufferPipelineInfo.subpass = 0;
		this->framebufferPipelineInfo.pViewportState = &this->viewportInfo;
	}
	else
	{
		// create shader state for 
		this->shader = VkShaderServer::Instance()->CreateShaderState("shd:shared", { NEBULAT_PASS_GROUP });
		this->dimensionsArray = this->shader->GetVariableByName("RenderTargetDimensions");

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
		viewport.minDepth = 0;
		viewport.maxDepth = 1;
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

		this->vkClearValues.Resize(2);
		this->vkClearValues[0].color.float32[0] = this->clearColor.x();
		this->vkClearValues[0].color.float32[1] = this->clearColor.y();
		this->vkClearValues[0].color.float32[2] = this->clearColor.z();
		this->vkClearValues[0].color.float32[3] = this->clearColor.w();

		VkExtent3D extents;
		extents.width = resolveWidth;
		extents.height = resolveHeight;
		extents.depth = 1;

		VkImageCreateInfo imgInfo = 
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			NULL,
			0,
			VK_IMAGE_TYPE_2D,
			VkTypes::AsVkFramebufferFormat(this->colorBufferFormat),
			extents,
			1,
			1,
			sampleCount,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			NULL,
			VK_IMAGE_LAYOUT_UNDEFINED
		};

		// create image for rendering
		VkResult res = vkCreateImage(VkRenderDevice::dev, &imgInfo, NULL, &this->vkTargetImage);
		n_assert(res == VK_SUCCESS);

		// allocate buffer backing and bind to image
		uint32_t size;
		VkUtilities::AllocateImageMemory(this->vkTargetImage, this->vkTargetImageMem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
		vkBindImageMemory(VkRenderDevice::dev, this->vkTargetImage, this->vkTargetImageMem, 0);

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
			this->vkTargetImage,
			VK_IMAGE_VIEW_TYPE_2D,
			VkTypes::AsVkFramebufferFormat(this->colorBufferFormat),
			VkTypes::AsVkMapping(this->colorBufferFormat),
			subres
		};

		res = vkCreateImageView(VkRenderDevice::dev, &viewInfo, NULL, &this->vkTargetImageView);
		n_assert(res == VK_SUCCESS);

		uint32_t numviews = 1;
		uint32_t numattachments = 1;
		VkImageView views[3];
		views[0] = this->vkTargetImageView;

		if (this->depthStencilTarget.isvalid())
		{
			views[numviews++] = this->depthStencilTarget->GetVkImageView();
			this->vkClearValues[1].depthStencil.depth = this->depthStencilTarget->GetClearDepth();
			this->vkClearValues[1].depthStencil.stencil = this->depthStencilTarget->GetClearStencil();
		}

		// we create a very default attachment behavior where we assume to write 
		// to the framebuffer and read from it once the operations are done
		VkAttachmentDescription attachment[2];
		attachment[0].flags = 0;
		attachment[0].format = VkTypes::AsVkFramebufferFormat(this->colorBufferFormat);
		attachment[0].samples = sampleCount;
		attachment[0].loadOp = this->clearFlags & CoreGraphics::RenderTarget::ClearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;						// set to clear
		attachment[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		attachment[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment[0].initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		attachment[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference attachmentRef;
		attachmentRef.attachment = 0;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = VK_ATTACHMENT_UNUSED;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		VkAttachmentReference resolveAttachmentRef;
		resolveAttachmentRef.attachment = VK_ATTACHMENT_UNUSED;
		resolveAttachmentRef.layout = VK_IMAGE_LAYOUT_GENERAL;

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
		subpass.pResolveAttachments = VK_NULL_HANDLE;

		if (this->depthStencilTarget.isvalid())
		{
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			depthAttachmentRef.attachment = 1;

			attachment[1].flags = 0;
			attachment[1].format = VK_FORMAT_D32_SFLOAT_S8_UINT;
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
		res = vkCreateFramebuffer(VkRenderDevice::dev, &fbInfo, NULL, &this->vkFramebuffer);
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
			this->resolveTexture->SetupFromVkMultisampleTexture(this->vkTargetImage, this->vkTargetImageMem, this->vkTargetImageView, this->width, this->height, this->colorBufferFormat, 0, true, true);
		}
		else
		{
			this->resolveTexture->SetupFromVkTexture(this->vkTargetImage, this->vkTargetImageMem, this->vkTargetImageView, this->width, this->height, this->colorBufferFormat, 0, true, true);
		}

		// change image layout
		VkClearColorValue clear = { 0.5f, 0.5f, 0, 0 };
		scheduler->PushImageLayoutTransition(VkDeferredCommand::Graphics, VkUtilities::ImageMemoryBarrier(this->vkTargetImage, subres, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL));
		scheduler->PushImageColorClear(this->vkTargetImage, VkDeferredCommand::Graphics, VK_IMAGE_LAYOUT_GENERAL, clear, subres);
		scheduler->PushImageLayoutTransition(VkDeferredCommand::Graphics, VkUtilities::ImageMemoryBarrier(this->vkTargetImage, subres, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::Discard()
{
	if (this->shader.isvalid())
	{
		this->shader->Discard();
		this->shader = 0;
	}
	vkDestroyRenderPass(VkRenderDevice::dev, this->pass, NULL);
	vkDestroyFramebuffer(VkRenderDevice::dev, this->vkFramebuffer, NULL);
	RenderTargetBase::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::BeginPass()
{
	RenderTargetBase::BeginPass();

	if (this->isDefaultRenderTarget)
	{
		this->SwapBuffers();
		this->SwitchToRender();

		// clear after binding as attachment
	}	
	else
	{
		// change image layout
		VkImageSubresourceRange subres;
		subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subres.baseArrayLayer = 0;
		subres.baseMipLevel = 0;
		subres.layerCount = 1;
		subres.levelCount = 1;
		//VkRenderDevice::Instance()->ImageLayoutTransition(VkDeferredCommand::Graphics, VkRenderDevice::ImageMemoryBarrier(this->vkTargetImage, subres, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));

		// clear after binding as attachment
		Math::float4 val;
		val.x() = (float)this->width;
		val.y() = (float)this->height;
		val.z() = 1 / val.x();
		val.w() = 1 / val.y();
		this->dimensionsArray->SetFloat4Array(&val, 1);
		//this->shader->Commit();
	}
}

//------------------------------------------------------------------------------
/**
	Okay, we really dont have to transition the images when we end the pass, but we can't really do better either.
	The other option would be to transition whenever (and if) this render target is used by some shader somewhere.
*/
void
VkRenderTarget::EndPass()
{
	if (this->isDefaultRenderTarget)
	{
		this->SwitchToPresent();
	}
	else
	{
		// change image layout
		VkImageSubresourceRange subres;
		subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subres.baseArrayLayer = 0;
		subres.baseMipLevel = 0;
		subres.layerCount = 1;
		subres.levelCount = 1;
		//VkRenderDevice::Instance()->ImageLayoutTransition(VkDeferredCommand::Graphics, VkRenderDevice::ImageMemoryBarrier(this->vkTargetImage, subres, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
	}

	RenderTargetBase::EndPass();
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
	VkViewport viewport;
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)this->width;
	viewport.height = (float)this->height;
	viewport.minDepth = 0;
	viewport.maxDepth = FLT_MAX;
	this->viewports.Resize(1);
	this->viewports[0] = viewport;

	this->viewportInfo.viewportCount = this->viewports.Size();
	this->viewportInfo.pViewports = this->viewports.Begin();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::Copy(const Ptr<CoreGraphics::RenderTarget>& tex)
{
	VkImageBlit region;
	region.srcSubresource.mipLevel = 0;
	region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.srcSubresource.baseArrayLayer = 0;
	region.srcSubresource.layerCount = 1;
	region.srcOffsets[0].x = region.srcOffsets[0].y = region.srcOffsets[0].z = 0;
	region.srcOffsets[1].x = this->GetResolveTextureWidth();
	region.srcOffsets[1].y = this->GetResolveTextureHeight();
	region.srcOffsets[1].z = 1;

	region.dstSubresource.mipLevel = 0;
	region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.dstSubresource.baseArrayLayer = 0;
	region.dstSubresource.layerCount = 1;
	region.dstOffsets[0].x = region.dstOffsets[0].y = region.dstOffsets[0].z = 0;
	region.dstOffsets[1].x = tex->GetResolveTextureWidth();
	region.dstOffsets[1].y = tex->GetResolveTextureHeight();
	region.dstOffsets[1].z = 1;

	// copy between images
	//vkCmdCopyImage(VkRenderDevice::mainCmdGfxBuffer, this->image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, tex->image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, &region);
	vkCmdBlitImage(VkRenderDevice::mainCmdDrawBuffer, this->vkTargetImage, VK_IMAGE_LAYOUT_GENERAL, tex->vkTargetImage, VK_IMAGE_LAYOUT_GENERAL, 1, &region, VK_FILTER_NEAREST);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::SwapBuffers()
{
	n_assert(this->isDefaultRenderTarget);

	VkRenderDevice* dev = VkRenderDevice::Instance();
	VkResult res = vkAcquireNextImageKHR(dev->dev, dev->swapchain, UINT64_MAX, dev->displaySemaphore, VK_NULL_HANDLE, &dev->currentBackbuffer);
	if (res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		// this means our swapchain needs a resize!
	}
	else
	{
		n_assert(res == VK_SUCCESS);
	}

	this->vkFramebuffer = this->swapbuffers[dev->currentBackbuffer];
	this->swapbufferIdx = dev->currentBackbuffer;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::SwitchToRender()
{
	n_assert(this->isDefaultRenderTarget);
	VkImageSubresourceRange subres;
	subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subres.baseArrayLayer = 0;
	subres.baseMipLevel = 0;
	subres.layerCount = 1;
	subres.levelCount = 1;

	// transition between present and output
	VkUtilities::ImageLayoutTransition(VkDeferredCommand::Graphics, VkUtilities::ImageMemoryBarrier(this->swapimages[this->swapbufferIdx], subres, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::SwitchToPresent()
{
	n_assert(this->isDefaultRenderTarget);
	VkImageSubresourceRange subres;
	subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subres.baseArrayLayer = 0;
	subres.baseMipLevel = 0;
	subres.layerCount = 1;
	subres.levelCount = 1;

	// transition between present and output
	VkUtilities::ImageLayoutTransition(VkDeferredCommand::Graphics, VkUtilities::ImageMemoryBarrier(this->swapimages[this->swapbufferIdx], subres, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::Clear(uint flags)
{
	VkImageSubresourceRange subres;
	subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subres.baseArrayLayer = 0;
	subres.baseMipLevel = 0;
	subres.layerCount = 1;
	subres.levelCount = 1;
	VkScheduler* scheduler = VkScheduler::Instance();

	// if we are doing a deferred clear (possibly outside of command buffer recording), push a state transition, clear, and revert transition
	scheduler->PushImageLayoutTransition(VkDeferredCommand::Graphics, VkUtilities::ImageMemoryBarrier(this->vkTargetImage, subres, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL));
	if (flags != 0)
	{
		if (0 != (flags & ClearColor))
		{
			scheduler->PushImageColorClear(this->vkTargetImage, VkDeferredCommand::Graphics, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, this->vkClearValues[0].color, subres);
		}
	}
	scheduler->PushImageLayoutTransition(VkDeferredCommand::Graphics, VkUtilities::ImageMemoryBarrier(this->vkTargetImage, subres, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::SetClearColor(const Math::float4& c)
{
	this->vkClearValues[0].color.float32[0] = c.x();
	this->vkClearValues[0].color.float32[1] = c.y();
	this->vkClearValues[0].color.float32[2] = c.z();
	this->vkClearValues[0].color.float32[3] = c.w();
	RenderTargetBase::SetClearColor(c);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::SetClearDepth(float f)
{
	this->vkClearValues[1].depthStencil.depth = f;
	RenderTargetBase::SetClearDepth(f);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTarget::SetClearStencil(int i)
{
	this->vkClearValues[1].depthStencil.stencil = i;
	RenderTargetBase::SetClearStencil(i);
}

} // namespace Vulkan
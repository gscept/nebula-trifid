//------------------------------------------------------------------------------
// vkmultiplerendertarget.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkmultiplerendertarget.h"
#include "vktypes.h"
#include "vkrenderdevice.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkMultipleRenderTarget, 'VKMR', Base::MultipleRenderTargetBase);
//------------------------------------------------------------------------------
/**
*/
VkMultipleRenderTarget::VkMultipleRenderTarget() :
	numattachments(0),
	numviews(0),
	numcolorreferences(0),
	usedepthstencil(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkMultipleRenderTarget::~VkMultipleRenderTarget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkMultipleRenderTarget::AddRenderTarget(const Ptr<CoreGraphics::RenderTarget>& rt)
{
	MultipleRenderTargetBase::AddRenderTarget(rt);
	CoreGraphics::AntiAliasQuality::Code aa = rt->GetAntiAliasQuality();
	VkSampleCountFlagBits sampleCount =
		aa == CoreGraphics::AntiAliasQuality::None ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT :
		aa == CoreGraphics::AntiAliasQuality::Low ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT :
		aa == CoreGraphics::AntiAliasQuality::Medium ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT : VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;

	this->attachments[this->numattachments].flags = 0;
	this->attachments[this->numattachments].format = VkTypes::AsVkFramebufferFormat(rt->GetColorBufferFormat());
	this->attachments[this->numattachments].samples = sampleCount;
	this->attachments[this->numattachments].loadOp = rt->GetClearFlags() & CoreGraphics::RenderTarget::ClearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
	this->attachments[this->numattachments].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	this->attachments[this->numattachments].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	this->attachments[this->numattachments].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	this->attachments[this->numattachments].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	this->attachments[this->numattachments].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	this->resolveReferences[this->numcolorreferences].attachment = VK_ATTACHMENT_UNUSED;
	this->resolveReferences[this->numcolorreferences].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	this->attachmentReferences[this->numcolorreferences].attachment = this->numcolorreferences;
	this->attachmentReferences[this->numcolorreferences].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	this->views[this->numviews] = rt->GetVkImageView();

	this->numcolorreferences++;
	this->numviews++;
	this->numattachments++;
}

//------------------------------------------------------------------------------
/**
*/
void
VkMultipleRenderTarget::SetDepthStencilTarget(const Ptr<CoreGraphics::DepthStencilTarget>& dt)
{
	MultipleRenderTargetBase::SetDepthStencilTarget(dt);
	this->attachments[this->numattachments].flags = 0;
	this->attachments[this->numattachments].format = VK_FORMAT_D32_SFLOAT_S8_UINT;
	this->attachments[this->numattachments].samples = VK_SAMPLE_COUNT_1_BIT;
	this->attachments[this->numattachments].loadOp = dt->GetClearFlags() & CoreGraphics::DepthStencilTarget::ClearDepth ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
	this->attachments[this->numattachments].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	this->attachments[this->numattachments].stencilLoadOp = dt->GetClearFlags() & CoreGraphics::DepthStencilTarget::ClearStencil ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
	this->attachments[this->numattachments].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	this->attachments[this->numattachments].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	this->attachments[this->numattachments].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	this->depthReference.attachment = VK_ATTACHMENT_UNUSED;
	this->depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	this->views[this->numattachments] = dt->GetVkImageView();
	this->usedepthstencil = true;
	this->numviews++;
	this->numattachments++;
}

//------------------------------------------------------------------------------
/**
*/
void
VkMultipleRenderTarget::Setup()
{
	MultipleRenderTargetBase::Setup();

	// create just one subpass which is just using our single color attachment and optional depth stencil attachment
	VkSubpassDescription subpass;
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = VK_NULL_HANDLE;
	subpass.colorAttachmentCount = this->numcolorreferences;
	subpass.pColorAttachments = this->attachmentReferences;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = VK_NULL_HANDLE;
	subpass.pDepthStencilAttachment = this->usedepthstencil ? &this->depthReference : VK_NULL_HANDLE;
	subpass.pResolveAttachments = this->resolveReferences;

	// setup render pass info
	VkRenderPassCreateInfo renderPassInfo =
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		NULL,
		0,
		this->numattachments,
		this->attachments,
		1,
		&subpass,
		0,
		VK_NULL_HANDLE
	};

	// create pass
	VkResult res;
	res = vkCreateRenderPass(VkRenderDevice::dev, &renderPassInfo, NULL, &this->pass);
	n_assert(res == VK_SUCCESS);

	// setup info
	this->framebufferPipelineInfo.renderPass = this->pass;
	this->framebufferPipelineInfo.subpass = 0;

	VkFramebufferCreateInfo fbInfo =
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		NULL,
		0,
		this->pass,
		this->numattachments,
		this->views,
		this->renderTarget[0]->GetResolveTextureWidth(), this->renderTarget[0]->GetResolveTextureHeight(), 1		// dimensions is 
	};

	// create framebuffer
	res = vkCreateFramebuffer(VkRenderDevice::dev, &fbInfo, NULL, &this->framebuffer);
	n_assert(res == VK_SUCCESS);

	// setup viewports, get the first viewport from all render targets
	// also setup clear colors
	this->viewports.Resize(this->numRenderTargets);	
	this->scissors.Resize(this->numRenderTargets);
	this->clearColors.Resize(this->numRenderTargets);
	IndexT i;
	for (i = 0; i < this->numRenderTargets; i++)
	{
		this->viewports[i] = this->renderTarget[i]->GetVkViewports()[0];
		this->scissors[i] = this->renderTarget[i]->GetVkScissorRects()[0];
		VkClearValue clearValue;
		const Math::float4& clear = this->renderTarget[i]->GetClearColor();
		clearValue.color.float32[0] = clear.x();
		clearValue.color.float32[1] = clear.y();
		clearValue.color.float32[2] = clear.z();
		clearValue.color.float32[3] = clear.w();
		this->clearColors[i] = clearValue;
	}

	this->viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	this->viewportInfo.pNext = NULL;
	this->viewportInfo.flags = 0;

	this->viewportInfo.viewportCount = this->viewports.Size();
	this->viewportInfo.pViewports = this->viewports.Begin();
	this->viewportInfo.scissorCount = this->scissors.Size();
	this->viewportInfo.pScissors = this->scissors.Begin();

	// setup info
	this->framebufferPipelineInfo.renderPass = this->pass;
	this->framebufferPipelineInfo.subpass = 0;
	this->framebufferPipelineInfo.pViewportState = &this->viewportInfo;
}

//------------------------------------------------------------------------------
/**
*/
void
VkMultipleRenderTarget::Discard()
{
	MultipleRenderTargetBase::Discard();
	vkDestroyFramebuffer(VkRenderDevice::dev, this->framebuffer, NULL);
	vkDestroyRenderPass(VkRenderDevice::dev, this->pass, NULL);
}

} // namespace Vulkan
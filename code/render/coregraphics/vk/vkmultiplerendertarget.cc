//------------------------------------------------------------------------------
// vkmultiplerendertarget.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkmultiplerendertarget.h"
#include "vktypes.h"
#include "vkrenderdevice.h"
#include "vkshaderserver.h"

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
	usedepthstencil(false),
	shader(0),
	dimensionsArray(0)
{
	VkViewport vp = { 0, 0, 0, 0, 0, 0 };
	VkRect2D sc = { { 0, 0 }, { 0, 0 } };
	VkClearValue cl;
	cl.color.float32[0] = 0;
	cl.color.float32[1] = 0;
	cl.color.float32[2] = 0;
	cl.color.float32[3] = 0;
	cl.depthStencil.depth = 0;
	cl.depthStencil.stencil = 0;
	IndexT i;
	for (i = 0; i < MaxNumRenderTargets; i++)
	{
		this->colorLoadOps[i] = VK_ATTACHMENT_LOAD_OP_LOAD;
		this->viewports[i] = vp;
		this->scissors[i] = sc;
		this->clearValues[i] = cl;
	}	
	this->depthLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	this->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
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
VkMultipleRenderTarget::SetClearFlags(IndexT i, uint clearFlags)
{
	this->colorLoadOps[i] = clearFlags & CoreGraphics::RenderTarget::ClearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
}

//------------------------------------------------------------------------------
/**
*/
void
VkMultipleRenderTarget::SetDepthStencilClearFlags(uint clearFlags)
{
	this->depthLoadOp = clearFlags & CoreGraphics::DepthStencilTarget::ClearDepth ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
	this->stencilLoadOp = clearFlags & CoreGraphics::DepthStencilTarget::ClearStencil ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
}

//------------------------------------------------------------------------------
/**
*/
void
VkMultipleRenderTarget::Setup()
{
	MultipleRenderTargetBase::Setup();

	// create shader state for 
	this->shader = VkShaderServer::Instance()->CreateShaderState("shd:shared", { NEBULAT_PASS_GROUP });
	this->dimensionsArray = this->shader->GetVariableByName("RenderTargetDimensions");

	IndexT i;
	for (i = 0; i < this->numRenderTargets; i++)
	{
		const Ptr<CoreGraphics::RenderTarget>& rt = this->renderTarget[i];
		VkAttachmentDescription& att = this->attachments[this->numattachments];

		CoreGraphics::AntiAliasQuality::Code aa = rt->GetAntiAliasQuality();
		VkSampleCountFlagBits sampleCount =
			aa == CoreGraphics::AntiAliasQuality::None ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT :
			aa == CoreGraphics::AntiAliasQuality::Low ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT :
			aa == CoreGraphics::AntiAliasQuality::Medium ? VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT : VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;

		att.flags = 0;
		att.format = VkTypes::AsVkFramebufferFormat(rt->GetColorBufferFormat());
		att.samples = sampleCount;
		att.loadOp = this->colorLoadOps[i];				// default to load, this might change later
		att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		att.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		att.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference& resolveRef = this->resolveReferences[this->numcolorreferences];
		VkAttachmentReference& colorRef = this->colorReferences[this->numcolorreferences];

		resolveRef.attachment = VK_ATTACHMENT_UNUSED;
		resolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorRef.attachment = this->numattachments;
		colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		this->views[this->numviews] = rt->GetVkImageView();
		this->viewports[this->numviews] = rt->GetVkViewports()[0];
		this->scissors[this->numviews] = rt->GetVkScissorRects()[0];

		this->vkClearValues.Append(this->clearValues[i]);
		this->vkViewports.Append(this->viewports[this->numviews]);
		this->vkScissors.Append(this->scissors[this->numviews]);

		this->numcolorreferences++;
		this->numviews++;
		this->numattachments++;
	}

	if (this->depthStencilTarget.isvalid())
	{
		VkAttachmentDescription& att = this->attachments[this->numattachments];
		att.flags = 0;
		att.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
		att.samples = VK_SAMPLE_COUNT_1_BIT;
		att.loadOp = this->depthLoadOp;
		att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		att.stencilLoadOp = this->stencilLoadOp;
		att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		att.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		att.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		this->depthReference.attachment = this->numattachments;
		this->depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		this->views[this->numattachments] = this->depthStencilTarget->GetVkImageView();
		this->viewports[this->numviews] = this->depthStencilTarget->GetVkViewport();
		this->scissors[this->numviews] = this->depthStencilTarget->GetVkScissorRect();
		this->usedepthstencil = true;

		this->vkClearValues.Append(this->depthStencilClearValue);
		this->vkViewports.Append(this->viewports[this->numviews]);
		this->vkScissors.Append(this->scissors[this->numviews]);

		this->numviews++;
		this->numattachments++;
	}

	// create just one subpass which is just using our single color attachment and optional depth stencil attachment
	VkSubpassDescription subpass;
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = VK_NULL_HANDLE;
	subpass.colorAttachmentCount = this->numcolorreferences;
	subpass.pColorAttachments = this->colorReferences;
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
		this->numviews,
		this->views,
		this->renderTarget[0]->GetWidth(), this->renderTarget[0]->GetHeight(), 1		// dimensions is 
	};

	// create framebuffer
	res = vkCreateFramebuffer(VkRenderDevice::dev, &fbInfo, NULL, &this->framebuffer);
	n_assert(res == VK_SUCCESS);

	this->viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	this->viewportInfo.pNext = NULL;
	this->viewportInfo.flags = 0;

	this->viewportInfo.viewportCount = this->numviews;
	this->viewportInfo.pViewports = this->viewports;
	this->viewportInfo.scissorCount = this->numviews;
	this->viewportInfo.pScissors = this->scissors;

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
	if (this->shader.isvalid())
	{
		this->shader->Discard();
		this->shader = 0;
	}
	vkDestroyFramebuffer(VkRenderDevice::dev, this->framebuffer, NULL);
	vkDestroyRenderPass(VkRenderDevice::dev, this->pass, NULL);
}

//------------------------------------------------------------------------------
/**
*/
void
VkMultipleRenderTarget::SetClearColor(IndexT i, const Math::float4& color)
{
	VkClearValue clearValue;
	clearValue.color.float32[0] = color.x();
	clearValue.color.float32[1] = color.y();
	clearValue.color.float32[2] = color.z();
	clearValue.color.float32[3] = color.w();
	this->clearValues[i] = clearValue;
	MultipleRenderTargetBase::SetClearColor(i, color);
}

//------------------------------------------------------------------------------
/**
*/
void
VkMultipleRenderTarget::SetClearDepth(float d)
{
	this->depthStencilClearValue.depthStencil.depth = d;
	MultipleRenderTargetBase::SetClearDepth(d);
}

//------------------------------------------------------------------------------
/**
*/
void
VkMultipleRenderTarget::SetClearStencil(int s)
{
	this->depthStencilClearValue.depthStencil.stencil = s;
	MultipleRenderTargetBase::SetClearStencil(s);
}

//------------------------------------------------------------------------------
/**
*/
void
VkMultipleRenderTarget::BeginPass()
{
	Util::FixedArray<Math::float4> dimensions;
	dimensions.Resize(this->numRenderTargets);
	IndexT i;
	for (i = 0; i < this->numRenderTargets; i++)
	{
		uint clearFlags = this->renderTarget[i]->GetClearFlags();
		this->renderTarget[i]->SetClearFlags(this->clearFlags[i]);
		this->renderTarget[i]->SetClearColor(this->clearColor[i]);
		this->renderTarget[i]->BeginPass();

		// setup render target dimensions
		Math::float4& val = dimensions[i];
		val.x() = (float)this->renderTarget[i]->GetWidth();
		val.y() = (float)this->renderTarget[i]->GetHeight();
		val.z() = 1 / val.x();
		val.w() = 1 / val.w();
	}
	this->dimensionsArray->SetFloat4Array(dimensions.Begin(), dimensions.Size());
	//this->shader->Commit();
}

} // namespace Vulkan
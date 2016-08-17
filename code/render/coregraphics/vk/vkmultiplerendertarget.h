#pragma once
//------------------------------------------------------------------------------
/**
	Implements a Vulkan multiple renderable texture target.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/multiplerendertargetbase.h"
namespace Vulkan
{
class VkMultipleRenderTarget : public Base::MultipleRenderTargetBase
{
	__DeclareClass(VkMultipleRenderTarget);
public:
	/// constructor
	VkMultipleRenderTarget();
	/// destructor
	virtual ~VkMultipleRenderTarget();

	/// set clear flags for color target
	void SetClearFlags(IndexT i, uint clearFlags);
	/// set clear flags for depth-stencil
	void SetDepthStencilClearFlags(uint clearFlags);
	/// set clear color
	void SetClearColor(IndexT i, const Math::float4& color);
	/// set clear depth
	void SetClearDepth(float d);
	/// set clear stencil value
	void SetClearStencil(int s);
	/// returns render pass 
	const VkRenderPass& GetVkRenderPass() const;
	/// returns framebuffer
	const VkFramebuffer& GetVkFramebuffer() const;
	/// returns the clear values
	const Util::Array<VkClearValue>& GetVkClearValues() const;
	/// get viewports
	const Util::Array<VkViewport>& GetVkViewports() const;
	/// get scissors
	const Util::Array<VkRect2D>& GetVkScissorRects() const;

	/// setup render target
	void Setup();
	/// discard render target
	void Discard();

	/// returns a const ref to the pipeline information created by this render target
	const VkGraphicsPipelineCreateInfo& GetVkPipelineInfo();
private:

	VkRenderPass pass;
	VkFramebuffer framebuffer;
	VkImageView views[MaxNumRenderTargets];
	VkAttachmentDescription attachments[MaxNumRenderTargets];
	VkAttachmentReference resolveReferences[MaxNumRenderTargets];
	VkAttachmentReference colorReferences[MaxNumRenderTargets];
	VkAttachmentReference depthReference;

	VkAttachmentLoadOp colorLoadOps[MaxNumRenderTargets];
	VkClearValue clearValues[MaxNumRenderTargets];
	VkViewport viewports[MaxNumRenderTargets];
	VkRect2D scissors[MaxNumRenderTargets];

	VkAttachmentLoadOp depthLoadOp;
	VkAttachmentLoadOp stencilLoadOp;
	VkClearValue depthStencilClearValue;
	uint32_t numviews;
	uint32_t numcolorreferences;
	uint32_t numattachments;
	VkBool32 usedepthstencil;

	Util::Array<VkClearValue> vkClearValues;
	Util::Array<VkViewport> vkViewports;
	Util::Array<VkRect2D> vkScissors;
	VkPipelineViewportStateCreateInfo viewportInfo;
	VkGraphicsPipelineCreateInfo framebufferPipelineInfo;
};

//------------------------------------------------------------------------------
/**
*/
inline const VkGraphicsPipelineCreateInfo&
VkMultipleRenderTarget::GetVkPipelineInfo()
{
	return this->framebufferPipelineInfo;
}


//------------------------------------------------------------------------------
/**
*/
inline const VkRenderPass&
VkMultipleRenderTarget::GetVkRenderPass() const
{
	return this->pass;
}

//------------------------------------------------------------------------------
/**
*/
inline const VkFramebuffer&
VkMultipleRenderTarget::GetVkFramebuffer() const
{
	return this->framebuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<VkClearValue>&
VkMultipleRenderTarget::GetVkClearValues() const
{
	return this->vkClearValues;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<VkViewport>&
VkMultipleRenderTarget::GetVkViewports() const
{
	return this->vkViewports;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<VkRect2D>&
VkMultipleRenderTarget::GetVkScissorRects() const
{
	return this->vkScissors;
}

} // namespace Vulkan
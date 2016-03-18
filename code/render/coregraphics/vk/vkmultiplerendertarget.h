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

	/// add render target
	void AddRenderTarget(const Ptr<CoreGraphics::RenderTarget>& rt);
	/// set depth-stencil target
	void SetDepthStencilTarget(const Ptr<CoreGraphics::DepthStencilTarget>& dt);
	/// returns render pass 
	const VkRenderPass& GetVkRenderPass() const;
	/// returns framebuffer
	const VkFramebuffer& GetVkFramebuffer() const;

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
	VkAttachmentReference attachmentReferences[MaxNumRenderTargets];
	VkAttachmentReference depthReference;
	uint32_t numviews;
	uint32_t numcolorreferences;
	uint32_t numattachments;
	VkBool32 usedepthstencil;

	Util::FixedArray<VkViewport> viewports;

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
} // namespace Vulkan
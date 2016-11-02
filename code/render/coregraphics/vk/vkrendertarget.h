#pragma once
//------------------------------------------------------------------------------
/**
	Implements a Vulkan renderable texture
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/rendertargetbase.h"
#include "coregraphics/shaderstate.h"
namespace Vulkan
{
class VkRenderTarget : public Base::RenderTargetBase
{
	__DeclareClass(VkRenderTarget);
public:
	/// constructor
	VkRenderTarget();
	/// destructor
	virtual ~VkRenderTarget();

	/// setup the render target object
	void Setup();
	/// discard the render target object
	void Discard();

	/// begin pass
	void BeginPass();
	/// end pass
	void EndPass();

	/// called after we change the display size
	void OnDisplayResized(SizeT width, SizeT height);

	/// returns a const ref to the pipeline information created by this render target
	const VkGraphicsPipelineCreateInfo& GetVkPipelineInfo();
	/// returns render pass 
	const VkRenderPass& GetVkRenderPass() const;
	/// returns framebuffer
	const VkFramebuffer& GetVkFramebuffer() const;
	/// returns list of clear values
	const Util::FixedArray<VkClearValue>& GetVkClearValues() const;

	/// set the current resolve rectangle (in pixels)
	void SetResolveRect(const Math::rectangle<int>& r);
	/// set array of resolve rects
	void SetResolveRectArray(const Util::Array<Math::rectangle<int> >& rects);
	/// remove resolve rects from render target
	void ResetResolveRects();
	/// get the vulkan viewports
	const Util::FixedArray<VkViewport>& GetVkViewports() const;
	/// get the vulkan scissor rectangles
	const Util::FixedArray<VkRect2D>& GetVkScissorRects() const;

	/// copy from this render target to the target texture
	void Copy(const Ptr<CoreGraphics::RenderTarget>& tex);

	/// set clear color 
	void SetClearColor(const Math::float4& c);
	/// set clear depth
	void SetClearDepth(float f);
	/// set clear stencil
	void SetClearStencil(int i);
	/// force-clears render target, useful if clearing is required outside a frame shader
	virtual void Clear(uint flags);

	/// get the vulkan image
	const VkImageView& GetVkImageView() const;


private:

	/// increase backbuffer index
	void SwapBuffers();
	/// transition backbuffer to be rendered to (only for default render target)
	void SwitchToRender();
	/// transition backbuffer to be presented (only for default render target)
	void SwitchToPresent();

	VkImage vkTargetImage;
	VkImageView vkTargetImageView;
	VkDeviceMemory vkTargetImageMem;

	VkFramebuffer vkFramebuffer;
	Util::FixedArray<VkViewport> viewports;
	Util::FixedArray<VkRect2D> scissors;

	Ptr<CoreGraphics::ShaderState> shader;
	Ptr<CoreGraphics::ShaderVariable> dimensionsArray;

	Util::FixedArray<VkClearValue> vkClearValues;
	VkPipelineViewportStateCreateInfo viewportInfo;
	VkGraphicsPipelineCreateInfo framebufferPipelineInfo;
	VkRenderPass pass;

	uint32_t swapbufferIdx;
	Util::FixedArray<VkFramebuffer> swapbuffers;
	Util::FixedArray<VkImage> swapimages;
};


//------------------------------------------------------------------------------
/**
*/
inline const VkGraphicsPipelineCreateInfo&
VkRenderTarget::GetVkPipelineInfo()
{
	return this->framebufferPipelineInfo;
}

//------------------------------------------------------------------------------
/**
*/
inline const VkImageView&
VkRenderTarget::GetVkImageView() const
{
	return this->vkTargetImageView;
}

//------------------------------------------------------------------------------
/**
*/
inline const VkRenderPass&
VkRenderTarget::GetVkRenderPass() const
{
	return this->pass;
}

//------------------------------------------------------------------------------
/**
*/
inline const VkFramebuffer&
VkRenderTarget::GetVkFramebuffer() const
{
	return this->vkFramebuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::FixedArray<VkClearValue>&
VkRenderTarget::GetVkClearValues() const
{
	return this->vkClearValues;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::FixedArray<VkViewport>&
VkRenderTarget::GetVkViewports() const
{
	return this->viewports;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::FixedArray<VkRect2D>&
VkRenderTarget::GetVkScissorRects() const
{
	return this->scissors;
} 
} // namespace Vulkan
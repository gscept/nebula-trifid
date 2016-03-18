#pragma once
//------------------------------------------------------------------------------
/**
	Implements a Vulkan renderable texture
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/rendertargetbase.h"
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

	/// called after we change the display size
	void OnDisplayResized(SizeT width, SizeT height);

	/// returns a const ref to the pipeline information created by this render target
	const VkGraphicsPipelineCreateInfo& GetVkPipelineInfo();

	/// set the current resolve rectangle (in pixels)
	void SetResolveRect(const Math::rectangle<int>& r);
	/// set array of resolve rects
	void SetResolveRectArray(const Util::Array<Math::rectangle<int> >& rects);
	/// remove resolve rects from render target
	void ResetResolveRects();
	/// get the vulkan viewports
	const Util::FixedArray<VkViewport>& GetVkViewports();

	/// get the vulkan image
	const VkImageView& GetVkImageView() const;
private:

	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMem;
	VkFramebuffer framebuffer;
	Util::FixedArray<VkViewport> viewports;

	VkPipelineViewportStateCreateInfo viewportInfo;
	VkGraphicsPipelineCreateInfo framebufferPipelineInfo;
	VkRenderPass pass;
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
	return this->imageView;
}

} // namespace Vulkan
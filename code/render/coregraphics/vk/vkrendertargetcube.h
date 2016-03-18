#pragma once
//------------------------------------------------------------------------------
/**
	Implements a renderable cube texture in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/rendertargetcubebase.h"
namespace Vulkan
{
class VkRenderTargetCube : public Base::RenderTargetCubeBase
{
	__DeclareClass(VkRenderTargetCube);
public:
	/// constructor
	VkRenderTargetCube();
	/// destructor
	virtual ~VkRenderTargetCube();

	/// returns a const ref to the pipeline information created by this render target
	const VkGraphicsPipelineCreateInfo& GetVkPipelineInfo();
private:

	VkPipelineViewportStateCreateInfo viewportInfo;
	VkGraphicsPipelineCreateInfo framebufferPipelineInfo;
};

//------------------------------------------------------------------------------
/**
*/
inline const VkGraphicsPipelineCreateInfo&
VkRenderTargetCube::GetVkPipelineInfo()
{
	return this->framebufferPipelineInfo;
}

} // namespace Vulkan
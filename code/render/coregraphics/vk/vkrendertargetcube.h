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
private:
};
} // namespace Vulkan
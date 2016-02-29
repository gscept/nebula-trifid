#pragma once
//------------------------------------------------------------------------------
/**
	Implements a Vulkan depth-stencil renderable texture.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/depthstenciltargetbase.h"
namespace Vulkan
{
class VkDepthStencilTarget : public Base::DepthStencilTargetBase
{
	__DeclareClass(VkDepthStencilTarget);
public:
	/// constructor
	VkDepthStencilTarget();
	/// destructor
	virtual ~VkDepthStencilTarget();

	/// return handle to the view
	VkImageView GetVkImageView();
private:

	VkImage image;
	VkDeviceMemory mem;
	VkImageView view;
};


//------------------------------------------------------------------------------
/**
*/
inline VkImageView
VkDepthStencilTarget::GetVkImageView()
{
	return this->view;
}

} // namespace Vulkan
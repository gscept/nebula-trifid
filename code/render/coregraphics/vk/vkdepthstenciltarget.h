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

	/// setup depth-stencil target
	void Setup();
	/// discard depth-stencil target
	void Discard();

	/// called after we change the display size
	void OnDisplayResized(SizeT width, SizeT height);

	/// begins pass
	void BeginPass();
	/// ends pass
	void EndPass();

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
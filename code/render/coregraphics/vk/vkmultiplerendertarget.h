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
private:
};
} // namespace Vulkan
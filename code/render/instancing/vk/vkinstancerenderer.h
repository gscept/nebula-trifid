#pragma once
//------------------------------------------------------------------------------
/**
	Implements a Vulkan specific renderer for instanced draws.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "instancing/base/instancerendererbase.h"
namespace Vulkan
{
class VkInstanceRenderer : public Base::InstanceRendererBase
{
	__DeclareClass(VkInstanceRenderer);
public:
	/// constructor
	VkInstanceRenderer();
	/// destructor
	virtual ~VkInstanceRenderer();
private:
};
} // namespace Vulkan
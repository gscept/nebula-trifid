#pragma once
//------------------------------------------------------------------------------
/**
	Implements a text renderer using Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/textrendererbase.h"
namespace Vulkan
{
class VkTextRenderer : public Base::TextRendererBase
{
	__DeclareClass(VkTextRenderer);
public:
	/// constructor
	VkTextRenderer();
	/// destructor
	virtual ~VkTextRenderer();
private:
};
} // namespace Vulkan
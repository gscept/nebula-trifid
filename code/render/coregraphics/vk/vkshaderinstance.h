#pragma once
//------------------------------------------------------------------------------
/**
	Implements a shader instance (local variables and such) in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shaderinstancebase.h"
namespace Vulkan
{
class VkShaderInstance : public Base::ShaderInstanceBase
{
	__DeclareClass(VkShaderInstance);
public:
	/// constructor
	VkShaderInstance();
	/// destructor
	virtual ~VkShaderInstance();
private:
};
} // namespace Vulkan
#pragma once
//------------------------------------------------------------------------------
/**
	Implements a shader variable instance in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shadervariableinstancebase.h"
namespace Vulkan
{
class VkShaderVariableInstance : public Base::ShaderVariableInstanceBase
{
	__DeclareClass(VkShaderVariableInstance);
public:
	/// constructor
	VkShaderVariableInstance();
	/// destructor
	virtual ~VkShaderVariableInstance();
private:
};
} // namespace Vulkan
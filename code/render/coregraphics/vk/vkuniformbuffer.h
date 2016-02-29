#pragma once
//------------------------------------------------------------------------------
/**
	Implements a uniform buffer used for shader uniforms in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/constantbufferbase.h"
namespace Vulkan
{
class VkUniformBuffer : public Base::ConstantBufferBase
{
	__DeclareClass(VkUniformBuffer);
public:
	/// constructor
	VkUniformBuffer();
	/// destructor
	virtual ~VkUniformBuffer();
private:
};
} // namespace Vulkan
#pragma once
//------------------------------------------------------------------------------
/**
	Implements a read/write image in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shaderreadwritetexturebase.h"
namespace Vulkan
{
class VkShaderImage : public Base::ShaderReadWriteTextureBase
{
	__DeclareClass(VkShaderImage);
public:
	/// constructor
	VkShaderImage();
	/// destructor
	virtual ~VkShaderImage();
private:
};
} // namespace Vulkan
//------------------------------------------------------------------------------
// vkshadervariable.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshadervariable.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkShaderVariable, 'VKSV', Base::ShaderVariableBase);
//------------------------------------------------------------------------------
/**
*/
VkShaderVariable::VkShaderVariable()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShaderVariable::~VkShaderVariable()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariable::BindToUniformBuffer(const Ptr<CoreGraphics::ConstantBuffer>& buffer, uint32_t offset, uint32_t size, int8_t* defaultValue)
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariable::Setup(AnyFX::VkVariable* var)
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariable::Setup(AnyFX::VkVarbuffer* var)
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariable::Setup(AnyFX::VkVarblock* var)
{

}


} // namespace Vulkan
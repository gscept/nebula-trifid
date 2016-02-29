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
VkShaderVariable::Setup(AnyFX::EffectVariable* var)
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariable::Setup(AnyFX::EffectVarbuffer* var)
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariable::Setup(AnyFX::EffectVarblock* var)
{

}


} // namespace Vulkan
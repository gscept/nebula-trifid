//------------------------------------------------------------------------------
// vkshader.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshader.h"
#include "coregraphics/constantbuffer.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkShader, 'VKSH', Base::ShaderBase);
//------------------------------------------------------------------------------
/**
*/
VkShader::VkShader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShader::~VkShader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::Unload()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::BeginUpdate()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::EndUpdate()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::Reload()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::Cleanup()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::OnLostDevice()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::OnResetDevice()
{

}

} // namespace Vulkan
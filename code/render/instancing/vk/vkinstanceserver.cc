//------------------------------------------------------------------------------
// vkinstanceserver.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkinstanceserver.h"
#include "instancing/instancerenderer.h"
#include "coregraphics/shaderserver.h"

using namespace CoreGraphics;
using namespace Instancing;

namespace Vulkan
{

__ImplementSingleton(Vulkan::VkInstanceServer);
__ImplementClass(Vulkan::VkInstanceServer, 'VKIS', Base::InstanceServerBase);
//------------------------------------------------------------------------------
/**
*/
VkInstanceServer::VkInstanceServer()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
VkInstanceServer::~VkInstanceServer()
{
	__DestructSingleton
}

//------------------------------------------------------------------------------
/**
*/
bool
VkInstanceServer::Open()
{
	if (InstanceServerBase::Open())
	{
		this->renderer = InstanceRenderer::Create();
		this->renderer->Setup();
		this->instancingFeatureBits = ShaderServer::Instance()->FeatureStringToMask("Instanced");
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
VkInstanceServer::Close()
{
	this->renderer->Close();
	this->renderer = 0;
	InstanceServerBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
VkInstanceServer::Render(IndexT frameIndex)
{

}

} // namespace Vulkan
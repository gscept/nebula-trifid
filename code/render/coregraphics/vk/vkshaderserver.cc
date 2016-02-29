//------------------------------------------------------------------------------
// vkshaderserver.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshaderserver.h"
#include "effectfactory.h"
#include "coregraphics/streamshaderloader.h"
#include "vkrenderdevice.h"

using namespace Resources;
using namespace CoreGraphics;
namespace Vulkan
{

__ImplementClass(Vulkan::VkShaderServer, 'VKSS', Base::ShaderServerBase);
//------------------------------------------------------------------------------
/**
*/
VkShaderServer::VkShaderServer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShaderServer::~VkShaderServer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
VkShaderServer::Open()
{
	n_assert(!this->IsOpen());

	// create anyfx factory
	this->factory = n_new(AnyFX::EffectFactory);
	ShaderServerBase::Open();

	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderServer::Close()
{
	n_assert(this->IsOpen());
	n_delete(this->factory);
	ShaderServerBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderServer::ReloadShader(Ptr<CoreGraphics::Shader> shader)
{
	n_assert(0 != shader);
	shader->SetLoader(StreamShaderLoader::Create());
	shader->SetAsyncEnabled(false);
	shader->Load();
	if (shader->IsLoaded())
	{
		shader->SetLoader(0);
	}
	else
	{
		n_error("Failed to load shader '%s'!", shader->GetResourceId().Value());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderServer::LoadShader(const Resources::ResourceId& shdName)
{
	n_assert(shdName.IsValid());
	Ptr<Shader> shader = Shader::Create();
	shader->SetResourceId(shdName);
	shader->SetLoader(StreamShaderLoader::Create());
	shader->SetAsyncEnabled(false);
	shader->Load();
	if (shader->IsLoaded())
	{
		shader->SetLoader(0);
		this->shaders.Add(shdName, shader);
	}
	else
	{
		n_warning("Failed to explicitly load shader '%s'!", shdName.Value());
		shader->Unload();
	}
}

} // namespace Vulkan
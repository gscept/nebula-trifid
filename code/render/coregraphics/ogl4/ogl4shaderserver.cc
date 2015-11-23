//------------------------------------------------------------------------------
//  OGL4ShaderServer.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/streamshaderloader.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/ogl4/ogl4shaderserver.h"
#include "materials/materialserver.h"
#include "framesync/framesynctimer.h"
#include "afxapi.h"

namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4ShaderServer, 'D1SS', Base::ShaderServerBase);
__ImplementSingleton(OpenGL4::OGL4ShaderServer);

using namespace Resources;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
OGL4ShaderServer::OGL4ShaderServer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
OGL4ShaderServer::~OGL4ShaderServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
OGL4ShaderServer::Open()
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
OGL4ShaderServer::Close()
{
    n_assert(this->IsOpen());
    ShaderServerBase::Close();
}

//------------------------------------------------------------------------------
/**
	Must be called from within Shader
*/
void 
OGL4ShaderServer::ReloadShader(Ptr<CoreGraphics::Shader> shader)
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
OGL4ShaderServer::LoadShader(const Resources::ResourceId& shdName)
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
} // namespace OpenGL4


//------------------------------------------------------------------------------
//  OGL4Shader.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/ogl4/ogl4shader.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shader.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"

namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4Shader, 'O4SD', Base::ShaderBase);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
OGL4Shader::OGL4Shader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
OGL4Shader::~OGL4Shader()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4Shader::Unload()
{
	n_assert(0 == this->shaderInstances.Size());
	this->Cleanup();
    ShaderBase::Unload();
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4Shader::Cleanup()
{
	// delete effect
	delete this->ogl4Effect;
}


//------------------------------------------------------------------------------
/**
*/
void
OGL4Shader::OnLostDevice()
{
	// notify our instances
    IndexT i;
    for (i = 0; i < this->shaderInstances.Size(); i++)
    {
        this->shaderInstances[i].downcast<OGL4ShaderInstance>()->OnLostDevice();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4Shader::OnResetDevice()
{
	// notify our instances
    IndexT i;
    for (i = 0; i < this->shaderInstances.Size(); i++)
    {
        this->shaderInstances[i].downcast<OGL4ShaderInstance>()->OnResetDevice();
    }

}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4Shader::Reload()
{
	this->Cleanup();
	Ptr<ShaderBase> thisPtr(this);
	ShaderBase::Unload();
	ShaderServer::Instance()->ReloadShader(thisPtr.downcast<Shader>());
	for (int i = 0; i < this->shaderInstances.Size(); i++)
	{
		this->shaderInstances[i]->Cleanup();
		this->shaderInstances[i]->Reload(thisPtr.downcast<Shader>());
	}
}


} // namespace OpenGL4


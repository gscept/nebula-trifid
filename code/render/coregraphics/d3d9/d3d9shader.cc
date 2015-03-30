//------------------------------------------------------------------------------
//  d3d9shader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/d3d9/d3d9shader.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shader.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9Shader, 'D9SD', Base::ShaderBase);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
D3D9Shader::D3D9Shader() :
    d3d9Effect(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
D3D9Shader::~D3D9Shader()
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
D3D9Shader::Unload()
{
    if (0 != this->d3d9Effect)
    {
        this->d3d9Effect->Release();
        this->d3d9Effect = 0;
    }
    ShaderBase::Unload();
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9Shader::OnLostDevice()
{
    if (0 != this->d3d9Effect)
    {
        HRESULT hr = this->d3d9Effect->OnLostDevice();
        n_assert(SUCCEEDED(hr));

        // notify our instances
        IndexT i;
        for (i = 0; i < this->shaderInstances.Size(); i++)
        {
            this->shaderInstances[i].downcast<D3D9ShaderInstance>()->OnLostDevice();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9Shader::OnResetDevice()
{
    if (0 != this->d3d9Effect)
    {
        HRESULT hr = this->d3d9Effect->OnResetDevice();
        n_assert(SUCCEEDED(hr));

        // notify our instances
        IndexT i;
        for (i = 0; i < this->shaderInstances.Size(); i++)
        {
            this->shaderInstances[i].downcast<D3D9ShaderInstance>()->OnResetDevice();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
D3D9Shader::Reload()
{
	n_error("D3D9Shader::Reload() is not yet implemented!");
}
} // namespace Direct3D9

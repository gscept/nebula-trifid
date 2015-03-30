//------------------------------------------------------------------------------
//  d3d9shaderserver.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/d3d9/d3d9shaderserver.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9ShaderServer, 'D9SS', Base::ShaderServerBase);
__ImplementSingleton(Direct3D9::D3D9ShaderServer);

using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
D3D9ShaderServer::D3D9ShaderServer() :
    d3d9EffectPool(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
D3D9ShaderServer::~D3D9ShaderServer()
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
D3D9ShaderServer::Open()
{
    n_assert(!this->IsOpen());
    n_assert(0 == this->d3d9EffectPool);

    // create d3d9effect pool object
    HRESULT hr = D3DXCreateEffectPool(&this->d3d9EffectPool);
    n_assert(SUCCEEDED(hr));

    // let parent class load all shaders
    ShaderServerBase::Open();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShaderServer::Close()
{
    n_assert(this->IsOpen());
    
    // release the d3dx effect pool object
    this->d3d9EffectPool->Release();
    this->d3d9EffectPool = 0;

    ShaderServerBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void 
D3D9ShaderServer::ReloadShader( Ptr<CoreGraphics::Shader> shader )
{
	n_error("D3D9ShaderServer::ReloadShader() is not yet implemented!");
}

//------------------------------------------------------------------------------
/**
*/
void 
D3D9ShaderServer::LoadShader( const Resources::ResourceId& shdName )
{
	n_error("D3D9ShaderServer::LoadShader() is not yet implemented!");
}

} // namespace Direct3D9

#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9ShaderServer
    
    D3D9 implementation of ShaderServer.
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/shaderserverbase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9ShaderServer : public Base::ShaderServerBase
{
    __DeclareClass(D3D9ShaderServer);
    __DeclareSingleton(D3D9ShaderServer);
public:
    /// constructor
    D3D9ShaderServer();
    /// destructor
    virtual ~D3D9ShaderServer();
    
    /// open the shader server
    bool Open();
    /// close the shader server
    void Close();

	/// reloads a shader
	void ReloadShader(Ptr<CoreGraphics::Shader> shader);
	/// explicitly loads a shader by resource id
	void LoadShader(const Resources::ResourceId& shdName);

    /// get pointer to global effect pool
    ID3DXEffectPool* GetD3D9EffectPool() const;

private:
    ID3DXEffectPool* d3d9EffectPool;
};

//------------------------------------------------------------------------------
/**
*/
inline ID3DXEffectPool* 
D3D9ShaderServer::GetD3D9EffectPool() const
{
    n_assert(0 != this->d3d9EffectPool);
    return this->d3d9EffectPool;
}

} // namespace Direct3D9
//------------------------------------------------------------------------------
    
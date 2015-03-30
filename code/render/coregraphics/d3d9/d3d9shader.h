#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9Shader
    
    D3D9 implementation of Shader.

    @todo lost/reset device handling
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/shaderbase.h"
#include "coregraphics/shaderinstance.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9Shader : public Base::ShaderBase
{
    __DeclareClass(D3D9Shader);
public:
    /// constructor
    D3D9Shader();
    /// destructor
    virtual ~D3D9Shader();
   
    /// unload the resource, or cancel the pending load
    virtual void Unload();
    /// get pointer to d3d effect
    ID3DXEffect* GetD3D9Effect() const;

	/// reloads a shader and all its resources
	void Reload();

private:
    friend class D3D9StreamShaderLoader;

    /// set d3d9 effect object
    void SetD3D9Effect(ID3DXEffect* ptr);
    /// called by d3d9 shader server when d3d9 device is lost
    void OnLostDevice();
    /// called by d3d9 shader server when d3d9 device is reset
    void OnResetDevice();

    ID3DXEffect* d3d9Effect;
};

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9Shader::SetD3D9Effect(ID3DXEffect* ptr)
{
    n_assert(0 != ptr);
    n_assert(0 == this->d3d9Effect);
    this->d3d9Effect = ptr;
}

//------------------------------------------------------------------------------
/**
*/
inline ID3DXEffect*
D3D9Shader::GetD3D9Effect() const
{
    n_assert(0 != this->d3d9Effect);
    return this->d3d9Effect;
}

} // namespace Direct3D9
//------------------------------------------------------------------------------

    
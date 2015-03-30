#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9ShaderInstance
    
    D3D9 implementation of CoreGraphics::ShaderInstance.
    
    @todo lost/reset device handling

    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/shaderinstancebase.h"
#include "coregraphics/shaderfeature.h"

namespace CoreGraphics
{
    class ShaderBase;
}

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9ShaderInstance : public Base::ShaderInstanceBase
{
    __DeclareClass(D3D9ShaderInstance);
public:
    /// constructor
    D3D9ShaderInstance();
    /// destructor
    virtual ~D3D9ShaderInstance();    
    /// get pointer to d3d9 effect object
    ID3DXEffect* GetD3D9Effect() const;

    /// select active variation by feature mask
    bool SelectActiveVariation(CoreGraphics::ShaderFeature::Mask featureMask);
    /// begin rendering through the currently selected variation, returns no. passes
    SizeT Begin();
    /// begin pass
    void BeginPass(IndexT passIndex);
    /// commit changes before rendering
    void Commit();
    /// end pass
    void EndPass();
    /// end rendering through variation
    void End();

protected:
    friend class Base::ShaderBase;
    friend class D3D9Shader;

    /// setup the shader instance from its original shader object
    virtual void Setup(const Ptr<CoreGraphics::Shader>& origShader);
    /// cleanup the shader instance
    virtual void Cleanup();
    /// called by d3d9 shader server when d3d9 device is lost
    void OnLostDevice();
    /// called by d3d9 shader server when d3d9 device is reset
    void OnResetDevice();

    ID3DXEffect* d3d9Effect;        //> cloned D3DX effect object
};

//------------------------------------------------------------------------------
/**
*/
inline ID3DXEffect*
D3D9ShaderInstance::GetD3D9Effect() const
{
    n_assert(0 != this->d3d9Effect);
    return this->d3d9Effect;
}

} // namespace Direct3D9
//------------------------------------------------------------------------------

    
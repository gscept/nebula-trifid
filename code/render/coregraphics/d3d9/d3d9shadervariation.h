#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9ShaderVariation

    Under Direct3D9, a shader variation is represented by an d3dx effect 
    technique which must be annotated by a FeatureMask string.

    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/shadervariationbase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9ShaderVariation : public Base::ShaderVariationBase
{
    __DeclareClass(D3D9ShaderVariation);
public:
    /// constructor
    D3D9ShaderVariation();
    /// destructor
    virtual ~D3D9ShaderVariation();
    /// get the D3DX technique handle
    D3DXHANDLE GetD3D9Technique() const;
    /// get the D3DX effect which owns this variation
    ID3DXEffect* GetD3D9Effect() const;

private:
    friend class D3D9ShaderInstance;
    /// setup from D3DX technique handle
    void Setup(ID3DXEffect* effect, D3DXHANDLE hTechnique);

    ID3DXEffect* d3d9Effect;
    D3DXHANDLE hTechnique;
};

//------------------------------------------------------------------------------
/**
*/
inline D3DXHANDLE
D3D9ShaderVariation::GetD3D9Technique() const
{
    n_assert(0 != this->hTechnique);
    return this->hTechnique;
}

//------------------------------------------------------------------------------
/**
*/
inline ID3DXEffect*
D3D9ShaderVariation::GetD3D9Effect() const
{
    n_assert(0 != this->d3d9Effect);
    return this->d3d9Effect;
}

} // namespace Direct3D9
//------------------------------------------------------------------------------

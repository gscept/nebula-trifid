//------------------------------------------------------------------------------
//  d3d9shaderinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/d3d9/d3d9shaderinstance.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shader.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/shadervariation.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9ShaderInstance, 'D9SI', Base::ShaderInstanceBase);

using namespace Util;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
D3D9ShaderInstance::D3D9ShaderInstance() :
    d3d9Effect(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
D3D9ShaderInstance::~D3D9ShaderInstance()
{
    n_assert(0 == this->d3d9Effect);
}

//------------------------------------------------------------------------------
/**
    This method is called by Shader::CreateInstance() to setup the 
    new shader instance.
*/
void
D3D9ShaderInstance::Setup(const Ptr<Shader>& origShader)
{
    n_assert(origShader.isvalid());
    n_assert(0 == this->d3d9Effect);
    HRESULT hr;
    const Ptr<D3D9Shader>& d3d9Shader = origShader.upcast<D3D9Shader>();
    IDirect3DDevice9* d3d9Device = D3D9RenderDevice::Instance()->GetDirect3DDevice();

    // call parent class
    ShaderInstanceBase::Setup(origShader);

    // create a clone of the original d3d9 effect object
    hr = d3d9Shader->GetD3D9Effect()->CloneEffect(d3d9Device, &this->d3d9Effect);
    n_assert(SUCCEEDED(hr));
    n_assert(0 != this->d3d9Effect);
    D3DXEFFECT_DESC desc = { 0 };    
    hr = this->d3d9Effect->GetDesc(&desc);
    n_assert(SUCCEEDED(hr));

    // setup shader variables
    IndexT paramIndex;
    for (paramIndex = 0; paramIndex < (SizeT)desc.Parameters; paramIndex++)
    {
        D3DXHANDLE hParam = this->d3d9Effect->GetParameter(NULL, paramIndex);
        Ptr<ShaderVariable> shaderVariable = ShaderVariable::Create();
        shaderVariable->Setup(this->d3d9Effect, hParam);
        this->variables.Append(shaderVariable);
        this->variablesByName.Add(shaderVariable->GetName(), shaderVariable);
        this->variablesBySemantic.Add(shaderVariable->GetSemantic(), shaderVariable);
    }

    // setup variations (these are represented by annotated D3DX effect techniques)
    IndexT techniqueIndex;
    for (techniqueIndex = 0; techniqueIndex < (SizeT)desc.Techniques; techniqueIndex++)
    {
        D3DXHANDLE hTechnique = this->d3d9Effect->GetTechnique(techniqueIndex);
        Ptr<ShaderVariation> shaderVariation = ShaderVariation::Create();
        shaderVariation->Setup(this->d3d9Effect, hTechnique);
        this->variations.Add(shaderVariation->GetFeatureMask(), shaderVariation);
    }
    n_assert(this->variations.Size() > 0);

    // select a proper default active variation
    this->SelectActiveVariation(this->variations.KeyAtIndex(0));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShaderInstance::Cleanup()
{
    n_assert(0 != this->d3d9Effect);
    this->d3d9Effect->Release();
    this->d3d9Effect = 0;
    ShaderInstanceBase::Cleanup();
}

//------------------------------------------------------------------------------
/**
*/
bool
D3D9ShaderInstance::SelectActiveVariation(ShaderFeature::Mask featureMask)
{
    if (ShaderInstanceBase::SelectActiveVariation(featureMask))
    {
        D3DXHANDLE d3d9Technique = this->activeVariation->GetD3D9Technique();
        HRESULT hr = this->d3d9Effect->SetTechnique(d3d9Technique);
        n_assert(SUCCEEDED(hr));
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShaderInstance::OnLostDevice()
{
    n_assert(0 != this->d3d9Effect);
    HRESULT hr = this->d3d9Effect->OnLostDevice();
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShaderInstance::OnResetDevice()
{
    n_assert(0 != this->d3d9Effect);
    HRESULT hr = this->d3d9Effect->OnResetDevice();
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
SizeT
D3D9ShaderInstance::Begin()
{
    n_assert(0 != this->d3d9Effect);
    ShaderInstanceBase::Begin();
    HRESULT hr;
    UINT numPasses = 0;
    hr = this->d3d9Effect->Begin(&numPasses, D3DXFX_DONOTSAVESTATE);
    n_assert(SUCCEEDED(hr));
    return numPasses;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShaderInstance::BeginPass(IndexT passIndex)
{
    n_assert(0 != this->d3d9Effect);
    ShaderInstanceBase::BeginPass(passIndex);
    HRESULT hr = this->d3d9Effect->BeginPass(passIndex);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShaderInstance::Commit()
{
    n_assert(0 != this->d3d9Effect);
    ShaderInstanceBase::Commit();
    HRESULT hr = this->d3d9Effect->CommitChanges();
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShaderInstance::EndPass()
{
    n_assert(0 != this->d3d9Effect);
    ShaderInstanceBase::EndPass();
    HRESULT hr = this->d3d9Effect->EndPass();
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShaderInstance::End()
{
    n_assert(0 != this->d3d9Effect);
    ShaderInstanceBase::End();
    HRESULT hr = this->d3d9Effect->End();
    n_assert(SUCCEEDED(hr));
}

} // namespace Direct3D9

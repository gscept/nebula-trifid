//------------------------------------------------------------------------------
//  d3d9shadervariation.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/d3d9/d3d9shadervariation.h"
#include "coregraphics/shaderserver.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9ShaderVariation, 'D9SV', Base::ShaderVariationBase);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
D3D9ShaderVariation::D3D9ShaderVariation() :
    d3d9Effect(0),
    hTechnique(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
D3D9ShaderVariation::~D3D9ShaderVariation()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShaderVariation::Setup(ID3DXEffect* effect, D3DXHANDLE handle)
{
    n_assert(0 != effect);
    n_assert(0 != handle);
    n_assert(0 == this->d3d9Effect);
    n_assert(0 == this->hTechnique);
    n_assert(0 == this->featureMask);
    n_assert(0 == this->numPasses);

    this->d3d9Effect = effect;
    this->hTechnique = handle;

    // setup name and feature mask
    D3DXTECHNIQUE_DESC desc;
    HRESULT hr = this->d3d9Effect->GetTechniqueDesc(this->hTechnique, &desc);
    n_assert(SUCCEEDED(hr));
    this->SetName(Name(desc.Name));
    this->SetNumPasses(desc.Passes);

    // generate the feature mask from the "Mask" annotation
    D3DXHANDLE hFeatureAnnotation = this->d3d9Effect->GetAnnotationByName(this->hTechnique, "Mask");
    if (NULL != hFeatureAnnotation)
    {
        LPCSTR strPtr = 0;
        hr = this->d3d9Effect->GetString(hFeatureAnnotation, &strPtr);
        n_assert(SUCCEEDED(hr));
        ShaderFeature::Mask mask = ShaderServer::Instance()->FeatureStringToMask(strPtr);
        this->SetFeatureMask(mask);
    }
    else
    {
        // issue a warning
        n_printf("WARNING: No Features annotation in technique '%s'!\n", this->GetName().Value());
    }
}

} // namespace Direct3D9

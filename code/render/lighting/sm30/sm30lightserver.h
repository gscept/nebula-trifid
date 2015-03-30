#pragma once
#ifndef LIGHTING_SM30LIGHTSERVER_H
#define LIGHTING_SM30LIGHTSERVER_H
//------------------------------------------------------------------------------
/**
    @class Lighting::SM30LightServer
  
    The standard light server for platforms which are capable of
    ShaderModel 3.0 or better. 

    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "lighting/base/lightserverbase.h"
#include "resources/managedtexture.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/shaderfeature.h"

//------------------------------------------------------------------------------
namespace Lighting
{
class SM30LightServer : public LightServerBase
{
    __DeclareClass(SM30LightServer);
public:
    /// constructor
    SM30LightServer();
    /// destructor
    virtual ~SM30LightServer();    

    /// open the light server
    void Open();
    /// close the light server
    void Close();
    /// apply lighting parameters for a visible model entity 
    void ApplyModelEntityLights(const Ptr<Graphics::ModelEntity>& modelEntity);

private:
    static const SizeT MaxLocalLights = 4;
    
    Util::FixedArray<CoreGraphics::ShaderFeature::Mask> shdFeatureMasks;
    CoreGraphics::ShaderFeature::Mask allLightsFeatureMask;

    // local light parameters
    Math::point lightPos[MaxLocalLights];
    float lightInvRange[MaxLocalLights];
    Math::matrix44 lightProjTransform[MaxLocalLights];
    Math::float4 lightColor[MaxLocalLights];
    Math::float4 lightProjMapUvOffset[MaxLocalLights];
    LightType::Code lightType[MaxLocalLights];
    bool lightCastShadows[MaxLocalLights];
    Math::float4 lightShadowBufferUvOffset[MaxLocalLights];
    Ptr<Resources::ManagedTexture> lightProjMap; 

    // shared shader parameters
    Ptr<CoreGraphics::ShaderVariable> shdGlobalLightDir;
    Ptr<CoreGraphics::ShaderVariable> shdGlobalLightColor;
    Ptr<CoreGraphics::ShaderVariable> shdGlobalAmbientLightColor;
    Ptr<CoreGraphics::ShaderVariable> shdGlobalBackLightColor;
    Ptr<CoreGraphics::ShaderVariable> shdGlobalBackLightOffset;
    Ptr<CoreGraphics::ShaderVariable> shdGlobalLightCastShadows;
    Ptr<CoreGraphics::ShaderVariable> shdGlobalLightPSSMDistances;
    Ptr<CoreGraphics::ShaderVariable> shdGlobalLightPSSMTransforms;
    Ptr<CoreGraphics::ShaderVariable> shdGlobalLightPSSMShadowBuffer;

    Ptr<CoreGraphics::ShaderVariable> shdLightPos;
    Ptr<CoreGraphics::ShaderVariable> shdLightInvRange;
    Ptr<CoreGraphics::ShaderVariable> shdLightProjTransform;
    Ptr<CoreGraphics::ShaderVariable> shdLightColor;
    Ptr<CoreGraphics::ShaderVariable> shdLightProjMapUvOffset;
    Ptr<CoreGraphics::ShaderVariable> shdLightType;
    Ptr<CoreGraphics::ShaderVariable> shdLightCastShadows;
    Ptr<CoreGraphics::ShaderVariable> shdLightShadowBufferUvOffset;
    Ptr<CoreGraphics::ShaderVariable> shdLightShadowBuffer;
    Ptr<CoreGraphics::ShaderVariable> shdLightShadowBufferSize;
    Ptr<CoreGraphics::ShaderVariable> shdLightProjMap;
};

} // namespace Lighting
//------------------------------------------------------------------------------
#endif

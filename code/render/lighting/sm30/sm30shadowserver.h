#pragma once
#ifndef LIGHTING_SM30SHADOWSERVER_H
#define LIGHTING_SM30SHADOWSERVER_H
//------------------------------------------------------------------------------
/**
    @class Lighting::SM30ShadowServer
    
    Shadow server for platforms which support at least shader model 3.0.
    Implements simple shadow mapping for local lights and parallel-split-
    shadow-mapping for the global light source.

	DEPRECATED!
        
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "lighting/base/shadowserverbase.h"
#include "frame/frameshader.h"
#include "frame/frameposteffect.h"
#include "coregraphics/rendertarget.h"
#include "lighting/pssmutil.h"

//------------------------------------------------------------------------------
namespace Lighting
{
class SM30ShadowServer : public ShadowServerBase
{
    __DeclareClass(SM30ShadowServer);
public:
    /// constructor
    SM30ShadowServer();
    /// destructor
    virtual ~SM30ShadowServer();

    /// open the shadow server
    void Open();
    /// close the shadow server
    void Close();

    /// update shadow buffer
    void UpdateShadowBuffers();

    /// get pointer to shadow buffer for local lights
    const Ptr<CoreGraphics::Texture>& GetLocalLightShadowBufferTexture() const;
    /// get pointer to PSSM shadow buffer for global lights
    const Ptr<CoreGraphics::Texture>& GetGlobalLightShadowBufferTexture() const;
    /// get array of PSSM split distances
    const float* GetSplitDistances() const;
    /// get array of PSSM LightProjTransforms
    const Math::matrix44* GetSplitTransforms() const;   

private:  
    /// update local light shadow buffers
    void UpdateSpotLightShadowBuffers();
    /// update global light shadow buffers
    void UpdatePSSMShadowBuffers();
    /// sort local lights by priority
    virtual void SortLights();

    Ptr<Frame::FrameShader> frameShader;
    Ptr<Frame::FramePostEffect> finalPostEffect;
    Ptr<CoreGraphics::RenderTarget> localLightShadowBuffer;
    Ptr<CoreGraphics::RenderTarget> pssmShadowBuffer;    
    PSSMUtil pssmUtil;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
SM30ShadowServer::GetLocalLightShadowBufferTexture() const
{
    return this->localLightShadowBuffer->GetResolveTexture();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
SM30ShadowServer::GetGlobalLightShadowBufferTexture() const
{
    return this->pssmShadowBuffer->GetResolveTexture();
}

} // namespace Lighting
//------------------------------------------------------------------------------
#endif

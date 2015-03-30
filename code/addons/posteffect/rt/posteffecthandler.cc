//------------------------------------------------------------------------------
//  PostEffecthandler.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "posteffect/rt/posteffecthandler.h"
#include "io/memorystream.h"
#include "graphics/graphicsserver.h"
#include "posteffect/rt/posteffectserver.h"
#include "threading/objectref.h"

namespace PostEffect
{
__ImplementClass(PostEffect::PostEffectHandler, 'POEH', Interface::InterfaceHandlerBase);

using namespace IO;
using namespace Util;
using namespace Messaging;
using namespace CoreGraphics;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
PostEffectHandler::PostEffectHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PostEffectHandler::~PostEffectHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
PostEffectHandler::HandleMessage(const Ptr<Message>& msg)
{
    n_assert(msg.isvalid());
    if (msg->CheckId(SetupPostEffectSystem::Id))
    {
        this->OnSetupPostEffectSystem();
    }
    else if (msg->CheckId(ResetPostEffectSystem::Id))
    {
        this->OnResetPostEffectSystem();
    }
	else if (msg->CheckId(Fade::Id))
	{
		this->OnFade(msg.cast<Fade>());
	}
    else if (msg->CheckId(BlendColor::Id))
    {
        this->OnBlendColor(msg.cast<BlendColor>());
    }
    else if (msg->CheckId(BlendHdr::Id))
    {
        this->OnBlendHdr(msg.cast<BlendHdr>());
    }
    else if (msg->CheckId(BlendDepthOfField::Id))
    {
        this->OnBlendDepthOfField(msg.cast<BlendDepthOfField>());
    }
    else if (msg->CheckId(BlendLight::Id))
    {
        this->OnBlendLight(msg.cast<BlendLight>());
    }
    else if (msg->CheckId(BlendFog::Id))
    {
        this->OnBlendFog(msg.cast<BlendFog>());
    }
    else if (msg->CheckId(BlendSky::Id))
    {
        this->OnBlendSky(msg.cast<BlendSky>());
    }
	else if (msg->CheckId(SetSkyEntity::Id))
	{
		this->OnSetSkyEntity(msg.cast<SetSkyEntity>());
	}
    else if (msg->CheckId(PreloadTexture::Id))
    {
        this->OnPreloadTexture(msg.cast<PreloadTexture>());
    }
    else if (msg->CheckId(UnloadTexture::Id))
    {
        this->OnUnloadTexture(msg.cast<UnloadTexture>());
    }
    else
    {
        // unknown message
        return false;
    }
    // fallthrough: message was handled
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::OnUnloadTexture(const Ptr<UnloadTexture>& msg)
{
    // notify posteffect server to release pointer to texture
//    PostEffectServer::Instance()->UnloadTexture(msg->GetResource());
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::OnPreloadTexture(const Ptr<PreloadTexture>& msg)
{
    PostEffectServer::Instance()->PreloadTexture(msg->GetResource());
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::OnResetPostEffectSystem()
{
    // reset stuff
    PostEffectServer::Instance()->Close();
    PostEffectServer::Instance()->Open();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::OnSetupPostEffectSystem()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectHandler::OnFade( const Ptr<Fade>& msg )
{
	this->fadeMsg = msg;
	this->fadeMsg->SetDeferred(true);
	PostEffectServer::Instance()->StartFade(msg->GetBlendTime(), msg->GetStart(), msg->GetTarget());
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectHandler::OnSetSkyEntity( const Ptr<SetSkyEntity>& msg )
{
	const Ptr<ModelEntity>& entity = msg->GetEntity().downcast<ModelEntity>();
	PostEffectServer::Instance()->SetSkyEntity(entity);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::DoWork()
{       
    if (PostEffectServer::HasInstance())
    {
        if (this->fadeMsg.isvalid())
        {
            if (PostEffectServer::Instance()->GetFadeMode() == PostEffectServer::NoFade)
            {
                this->fadeMsg->SetDeferredHandled(true);
                this->fadeMsg = 0;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::OnBlendDepthOfField(const Ptr<BlendDepthOfField>& msg)
{
    // create new target
    Ptr<DepthOfFieldParams> targetParas = DepthOfFieldParams::Create();
    targetParas->SetFilterSize(msg->GetFilterSize());
    targetParas->SetFocusDistance(msg->GetFocusDistance());
    targetParas->SetFocusLength(msg->GetFocusLength());

    // start
    PostEffectServer::Instance()->StartBlending(targetParas.cast<ParamBase>(), msg->GetBlendTime(), PostEffectServer::DoF);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::OnBlendFog(const Ptr<BlendFog>& msg)
{
    // create new target
    Ptr<FogParams> targetParas = FogParams::Create();
    targetParas->SetFogColorAndIntensity(msg->GetColor());
    targetParas->SetFogFarDistance(msg->GetFarDistance());
    targetParas->SetFogHeight(msg->GetHeight());
    targetParas->SetFogNearDistance(msg->GetNearDistance());

    // start
    PostEffectServer::Instance()->StartBlending(targetParas.cast<ParamBase>(), msg->GetBlendTime(), PostEffectServer::Fog);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::OnBlendHdr(const Ptr<BlendHdr>& msg)
{
    // create new target
    Ptr<HdrParams> targetParas = HdrParams::Create();
    targetParas->SetHdrBloomColor(msg->GetBloomColor());
    targetParas->SetHdrBloomIntensity(msg->GetBloomIntensity());
    targetParas->SetHdrBloomThreshold(msg->GetBloomThreshold());

    // start
    PostEffectServer::Instance()->StartBlending(targetParas.cast<ParamBase>(), msg->GetBlendTime(), PostEffectServer::Hdr);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::OnBlendLight(const Ptr<BlendLight>& msg)
{
    // create new target
    Ptr<LightParams> targetParas = LightParams::Create();
    targetParas->SetBackLightFactor(msg->GetBackLightFactor());
    targetParas->SetLightAmbientColor(msg->GetLightAmbientColor());
    targetParas->SetLightColor(msg->GetLightColor());
    targetParas->SetLightOppositeColor(msg->GetLightOppositeColor());
    targetParas->SetLightShadowIntensity(msg->GetShadowIntensity());
	targetParas->SetLightShadowBias(msg->GetShadowBias());
    targetParas->SetLightTransform(msg->GetLightTransform());
	targetParas->SetLightCastShadows(msg->GetCastShadows());
    targetParas->SetLightIntensity(msg->GetLightIntensity());

    // start
    PostEffectServer::Instance()->StartBlending(targetParas.cast<ParamBase>(), msg->GetBlendTime(), PostEffectServer::Light);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::OnBlendSky(const Ptr<BlendSky>& msg)
{
    // create new target
    Ptr<SkyParams> targetParas = SkyParams::Create();
    targetParas->SetSkyBrightness(msg->GetBrightness());
    targetParas->SetSkyContrast(msg->GetContrast());
    targetParas->SetSkyTexturePath(msg->GetTexture());
    targetParas->ResetTextureBlendFactor();
 
    // start
    PostEffectServer::Instance()->StartBlending(targetParas.cast<ParamBase>(), msg->GetBlendTime(), PostEffectServer::Sky);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectHandler::OnBlendColor(const Ptr<BlendColor>& msg)
{
    // create new target
    Ptr<ColorParams> targetParas = ColorParams::Create();
    targetParas->SetColorBalance(msg->GetBalance());
    targetParas->SetColorSaturation(msg->GetSaturation());
	targetParas->SetColorMaxLuminance(msg->GetMaxLuminance());

    // start blending on server
    PostEffectServer::Instance()->StartBlending(targetParas.cast<ParamBase>(), msg->GetBlendTime(), PostEffectServer::Color);
}
} // namespace Debug

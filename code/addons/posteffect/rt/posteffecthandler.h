#pragma once
//------------------------------------------------------------------------------
/**
    @class Debug::PostEffectHandler
    
    Handles debug-visualization messages in the graphics thread.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "interface/interfacehandlerbase.h"
#include "messaging/message.h"
#include "posteffect/posteffectprotocol.h"


//------------------------------------------------------------------------------
namespace PostEffect
{
class PostEffectHandler : public Interface::InterfaceHandlerBase
{
    __DeclareClass(PostEffectHandler);
public:
    /// constructor
    PostEffectHandler();
    /// destructor
    virtual ~PostEffectHandler();  

    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);
    /// do per-frame work
    virtual void DoWork();

private:
    /// on setup 
    void OnSetupPostEffectSystem();
    /// on reset
    void OnResetPostEffectSystem();

	/// handle fade message
	void OnFade(const Ptr<Fade>& msg);
	/// handle setting the sky entity
	void OnSetSkyEntity(const Ptr<SetSkyEntity>& msg);

    /// handle BlendDepthOfField message
    void OnBlendDepthOfField(const Ptr<BlendDepthOfField>& msg);
    /// handle BlendFog message
    void OnBlendFog(const Ptr<BlendFog>& msg);
    /// handle BlendHdr message
    void OnBlendHdr(const Ptr<BlendHdr>& msg);
    /// handle BlendLight message
    void OnBlendLight(const Ptr<BlendLight>& msg);
    /// handle BlendSky message
    void OnBlendSky(const Ptr<BlendSky>& msg);
    /// handle BlendColor message
    void OnBlendColor(const Ptr<BlendColor>& msg);

    /// handle PreloadTexture message
    void OnPreloadTexture(const Ptr<PreloadTexture>& msg);
    /// handle PreloadTexture message
    void OnUnloadTexture(const Ptr<UnloadTexture>& msg);

	Ptr<Fade> fadeMsg;
};

} // namespace PostEffect
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// posteffectrtplugin.cc
// (C) 2009 Radon Labs GmbH
// (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "posteffect/rt/posteffectrtplugin.h"
#include "graphics/graphicsinterface.h"

namespace PostEffect
{
__ImplementClass(PostEffectRTPlugin,'PERP',RenderModules::RTPlugin);

using namespace Graphics;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
    Constructor	
*/
PostEffectRTPlugin::PostEffectRTPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
	Destructor
*/
PostEffectRTPlugin::~PostEffectRTPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
    Called when registered to the render-thread side
*/
void
PostEffectRTPlugin::OnRegister()
{
    // setup the posteffect server
    this->postEffectServer = PostEffectServer::Create();
    this->postEffectServer->Open();

    // setup the posteffect handler
    this->postEffectHandler = PostEffectHandler::Create();
    GraphicsInterface::Instance()->AttachHandler(this->postEffectHandler.cast<Handler>());
}

//------------------------------------------------------------------------------
/**
    Called when unregistered from the render-thread side
*/
void
PostEffectRTPlugin::OnUnregister()
{
    // discard the posteffect handler
	this->postEffectHandler->Close();
	GraphicsInterface::Instance()->RemoveHandler(this->postEffectHandler.cast<Handler>());	
    this->postEffectHandler = 0;

    // discard the posteffect server
    this->postEffectServer->Close();
    this->postEffectServer = 0;
}

//------------------------------------------------------------------------------
/**
    Called before rendering
*/
void
PostEffectRTPlugin::OnRenderBefore(IndexT frameId, Timing::Time time)
{
    this->postEffectServer->OnFrame(time);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectRTPlugin::OnDiscardStage(const Ptr<Graphics::Stage>& stage)
{
	this->postEffectServer->SetGlobalLightEntity(0);
}

} // namespace PostEffect
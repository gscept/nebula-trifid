//------------------------------------------------------------------------------
// fxrtplugin.cc
// (C) Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicsinterface.h"
#include "fx/rt/fxrtplugin.h"

namespace FX
{
__ImplementClass(FXRTPlugin, 'FXRP', RenderModules::RTPlugin);

using namespace Graphics;
using namespace Messaging;


//------------------------------------------------------------------------------
/**
*/
FXRTPlugin::FXRTPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FXRTPlugin::~FXRTPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
	Called when plugin is registered on the render-thread side
*/
void
FXRTPlugin::OnRegister()
{
    // Setup the fx server
    this->fxServer = FxServer::Create();
    this->fxServer->Open();

    // Setup fx handler
    this->fxHandler = FxHandler::Create();
    GraphicsInterface::Instance()->AttachHandler(this->fxHandler.cast<Handler>());
}

//------------------------------------------------------------------------------
/**
    Called when plugin is unregistered from render-thread side	
*/
void
FXRTPlugin::OnUnregister()
{
    // Discard fx handler
    this->fxHandler = 0;

    // Discard fx server
    this->fxServer->Close();
    this->fxServer = 0;
}

//------------------------------------------------------------------------------
/**
    Called before rendering happens on render-thread side	
*/
void
FXRTPlugin::OnRenderBefore(IndexT frameId, Timing::Time time)
{
    this->fxServer->OnFrame(time);
}

} // namespace FX
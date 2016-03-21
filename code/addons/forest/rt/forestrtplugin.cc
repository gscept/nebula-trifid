//------------------------------------------------------------------------------
// posteffectrtplugin.cc
// (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "forest/rt/forestrtplugin.h"
#include "graphics/graphicsinterface.h"

namespace Forest
{
__ImplementClass(ForestRTPlugin,'FORP',RenderModules::RTPlugin);

using namespace Graphics;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
    Constructor	
*/
ForestRTPlugin::ForestRTPlugin()
{
}

//------------------------------------------------------------------------------
/**
	Destructor
*/
ForestRTPlugin::~ForestRTPlugin()
{
}

//------------------------------------------------------------------------------
/**
    Called when registered to the render-thread side
*/
void
ForestRTPlugin::OnRegister()
{
    // setup the posteffect server
    this->forestServer = ForestServer::Create();
    this->forestServer->Open();

    // setup the posteffect handler
    this->forestHandler = ForestHandler::Create();
    GraphicsInterface::Instance()->AttachHandler(this->forestHandler.cast<Handler>());

}

//------------------------------------------------------------------------------
/**
    Called when unregistered from the render-thread side
*/
void
ForestRTPlugin::OnUnregister()
{
    // discard the forest handler
    this->forestHandler = 0;

    // discard the forest server
    this->forestServer = 0;
}

//------------------------------------------------------------------------------
/**
    Called before rendering
*/
void
ForestRTPlugin::OnRenderBefore(IndexT frameId, Timing::Time time)
{
    this->forestServer->OnRenderBefore(frameId, time);
}

//------------------------------------------------------------------------------
/**
*/
void
ForestRTPlugin::OnRenderAfter(IndexT frameId, Timing::Time time)
{
    this->forestServer->OnRenderAfter(frameId, time);
}

} // namespace Forest
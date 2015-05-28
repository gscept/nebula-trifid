//------------------------------------------------------------------------------
// videortplugin.cc
// (C) Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "graphics/graphicsinterface.h"
#include "videortplugin.h"

namespace Video
{
__ImplementClass(Video::VideoRTPlugin,'VRTP',RenderModules::RTPlugin);

using namespace Graphics;
using namespace Messaging;
using namespace RenderModules;

//------------------------------------------------------------------------------
/**
    Constructor
*/
VideoRTPlugin::VideoRTPlugin()
{
}

//------------------------------------------------------------------------------
/**
    Destructor	
*/
VideoRTPlugin::~VideoRTPlugin()
{
    n_assert(!videoServer.isvalid());
}

//------------------------------------------------------------------------------
/**
    Called when registered on the render-thread side	
*/
void
VideoRTPlugin::OnRegister()
{
    // Setup the video server
    this->videoServer = VideoServer::Create();
    this->videoServer->Open();
}

//------------------------------------------------------------------------------
/**
    Called when unregistered on the render-thread side	
*/
void
VideoRTPlugin::OnUnregister()
{	
    // Discard video server
    this->videoServer = 0;
}

//------------------------------------------------------------------------------
/**
    Called after rendering    	
*/
void
VideoRTPlugin::OnRenderAfter(IndexT frameId, Timing::Time time)
{
    this->videoServer->OnFrame(time);
}

//------------------------------------------------------------------------------
/**
    Called before rendering    	
*/
void
VideoRTPlugin::OnRenderBefore(IndexT frameId, Timing::Time time)
{
    this->videoServer->OnRenderBefore(time);
}

//------------------------------------------------------------------------------
/**
*/
void
VideoRTPlugin::OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch)
{
	if (frameBatch->GetType() == CoreGraphics::BatchType::UI)
	{
		this->videoServer->RenderBatch();
	}
}

}
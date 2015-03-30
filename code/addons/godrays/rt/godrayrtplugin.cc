//------------------------------------------------------------------------------
//  godrayrtplugin.h
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "godrayrtplugin.h"
#include "graphics/graphicsinterface.h"

namespace Godrays
{
__ImplementClass(Godrays::GodrayRTPlugin, 'GRPL', RenderModules::RTPlugin);

using namespace Graphics;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
GodrayRTPlugin::GodrayRTPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GodrayRTPlugin::~GodrayRTPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayRTPlugin::OnRegister()
{
	// create message handler and attach
	this->handler = GodrayHandler::Create();
	GraphicsInterface::Instance()->AttachHandler(this->handler.upcast<Messaging::Handler>());

	this->server = GodrayServer::Create();
	//this->server->Open();
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayRTPlugin::OnUnregister()
{
	// remove handler
#if NEBULA_RENDER_THREAD
	this->handler->Close();
#endif
	GraphicsInterface::Instance()->RemoveHandler(this->handler.upcast<Messaging::Handler>());
	this->handler = 0;

	//this->server->Close();
	//this->server = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayRTPlugin::OnRenderBefore( IndexT frameId, Timing::Time time )
{
	//this->server->OnRenderBefore();
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayRTPlugin::OnRenderAfter( IndexT frameId, Timing::Time time )
{
	//this->server->OnRenderAfter();
}


} // namespace Godrays
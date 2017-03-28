//------------------------------------------------------------------------------
//  qtremotehandler.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "messaging/staticmessagehandler.h"
#include "qtremoteserver.h"
#include "remoteinterface/qtremoteprotocol.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "appgame/gameapplication.h"
#include "basegamefeature/statehandlers/gamestatehandler.h"

using namespace QtRemoteInterfaceAddon;
using namespace Graphics;
namespace Messaging
{

//------------------------------------------------------------------------------
/**
*/
__Handler(QtRemoteServer, ReloadResource)
{
	// quite simply just send to graphics interface
	GraphicsInterface::Instance()->Send(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
__Handler(QtRemoteServer, ReloadResourceIfExists)
{
	// quite simply just send to graphics interface
	GraphicsInterface::Instance()->Send(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
__Handler(QtRemoteServer, ReloadModelByResource)
{
	// quite simply just send to graphics interface
	GraphicsInterface::Instance()->Send(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
__Handler(QtRemoteServer, LoadLevel)
{
	const Ptr<BaseGameFeature::GameStateHandler>& state = App::GameApplication::Instance()->FindStateHandlerByName("Reload").cast<BaseGameFeature::GameStateHandler>();
	state->SetLevelName(msg->GetLevel());
	App::GameApplication::Instance()->RequestState("Reload");
}


//------------------------------------------------------------------------------
/**
*/
__Handler(QtRemoteServer, KeepAlive)
{
	// do nothing
}

//------------------------------------------------------------------------------
/**
*/
__Dispatcher(QtRemoteServer)
{
	__Handle(QtRemoteServer, ReloadResource);
	__Handle(QtRemoteServer, ReloadResourceIfExists);
	__Handle(QtRemoteServer, ReloadModelByResource);	
	__Handle(QtRemoteServer, LoadLevel);
	__Handle(QtRemoteServer, KeepAlive);
}

} // namespace Messaging
//------------------------------------------------------------------------------
//  godrayhandler.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "godrayhandler.h"
#include "godrayserver.h"
#include "internalgraphics/internalmodelentity.h"

namespace Godrays
{
__ImplementClass(Godrays::GodrayHandler, 'GDHA', Interface::InterfaceHandlerBase);

//------------------------------------------------------------------------------
/**
*/
GodrayHandler::GodrayHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GodrayHandler::~GodrayHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
GodrayHandler::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	if (msg->CheckId(SetSunTexture::Id))
	{
		this->OnSetSunTexture(msg.downcast<SetSunTexture>());
	}
	else if (msg->CheckId(SetSunColor::Id))
	{
		this->OnSetSunColor(msg.downcast<SetSunColor>());
	}
	else if (msg->CheckId(SetSunGlobalLightRelative::Id))
	{
		this->OnSetSunGlobalLightRelative(msg.downcast<SetSunGlobalLightRelative>());
	}
	else if (msg->CheckId(SetSunPosition::Id))
	{
		this->OnSetSunPosition(msg.downcast<SetSunPosition>());
	}
	else
	{
		return false;
	}

	// fallthrough, our message has been handled
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayHandler::OnSetSunTexture( const Ptr<SetSunTexture>& msg )
{
	GodrayServer::Instance()->SetSunTexture(msg->GetTexture());
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayHandler::OnSetSunColor( const Ptr<SetSunColor>& msg )
{
	GodrayServer::Instance()->SetSunColor(msg->GetColor());
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayHandler::OnSetSunGlobalLightRelative( const Ptr<SetSunGlobalLightRelative>& msg )
{
	GodrayServer::Instance()->SetSunLightRelative(msg->GetIsRelative());
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayHandler::OnSetSunPosition( const Ptr<SetSunPosition>& msg )
{
	GodrayServer::Instance()->SetSunPosition(msg->GetPosition());
}

} // namespace Godrays
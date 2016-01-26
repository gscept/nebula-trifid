//------------------------------------------------------------------------------
//  ConvexAreaMarkerProperty.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "convexareamarkerproperty.h"
#include "navigation/navigationattributes.h"
#include "basegamefeature/basegameprotocol.h"
#include "navigationserver.h"

namespace Navigation
{
__ImplementClass(Navigation::ConvexAreaMarkerProperty, 'NVAM', Game::Property);

//------------------------------------------------------------------------------
/**
*/
ConvexAreaMarkerProperty::ConvexAreaMarkerProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ConvexAreaMarkerProperty::~ConvexAreaMarkerProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexAreaMarkerProperty::SetupAcceptedMessages()
{
	this->RegisterMessage(UpdateAreaId::Id);	
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexAreaMarkerProperty::OnStart()
{
	this->startingAreaId = this->entity->GetInt(Attr::NavMeshAreaFlags);
	this->UpdateArea(this->startingAreaId);
}

//------------------------------------------------------------------------------
/**
*/
void
ConvexAreaMarkerProperty::UpdateArea(unsigned int areaId)
{
	Util::Array<Util::String> navMeshes = this->entity->GetString(Attr::NavMeshMeshString).Tokenize(";");
	for (int i = 0; i < navMeshes.Size(); i++)
	{
		Navigation::NavigationServer::Instance()->UpdateAreaId(navMeshes[i], this->entity->GetMatrix44(Attr::Transform).get_position(), areaId, 0xffff);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexAreaMarkerProperty::OnDeactivate()
{
	this->UpdateArea(this->startingAreaId);
	Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexAreaMarkerProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	if (msg->CheckId(UpdateAreaId::Id))
	{
		Ptr<UpdateAreaId> umsg = msg.cast<UpdateAreaId>();
		this->UpdateArea(umsg->GetAreaId());
	}
}

}; // namespace Navigation

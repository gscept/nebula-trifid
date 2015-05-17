//------------------------------------------------------------------------------
//  ConvexAreaMarkerProperty.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "convexareamarkerproperty.h"
#include "navigationattributes.h"
#include "basegamefeature\basegameprotocol.h"
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
	this->startingAreaId = this->entity->GetInt(Attr::NavMeshArea);
	this->UpdateArea(this->startingAreaId);
}

//------------------------------------------------------------------------------
/**
*/
void
ConvexAreaMarkerProperty::UpdateArea(unsigned int areaId)
{
	Navigation::NavigationServer::Instance()->UpdateAreaId(this->entity->GetString(Attr::NavMeshMeshString), this->entity->GetMatrix44(Attr::Transform).get_position(), areaId);
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexAreaMarkerProperty::OnDeactivate()
{
	this->UpdateArea(this->startingAreaId);
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

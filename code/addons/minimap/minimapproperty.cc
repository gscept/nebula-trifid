//------------------------------------------------------------------------------
//  minimapproperty.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "minimapproperty.h"
#include "game/entity.h"
#include "messaging/staticmessagehandler.h"
#include "minimapmanager.h"
#include "minimap.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

using namespace Messaging;

namespace Minimap
{
__ImplementClass(Minimap::MinimapProperty, 'MMPP', Game::Property);

using namespace Game;

//------------------------------------------------------------------------------
/**
*/
MinimapProperty::MinimapProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MinimapProperty::~MinimapProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/** 
    Called when property is attached to a game entity. 
*/
void
MinimapProperty::OnActivate()
{
	this->texture = this->GetEntity()->GetString(Attr::MinimapIcon);
	Minimap::MinimapManager::Instance()->RegisterEntity(this->texture, this->entity);
    Property::OnActivate();	
}

//------------------------------------------------------------------------------
/** 
    Called when property is going to be removed from its game entity.    
*/
void
MinimapProperty::OnDeactivate()
{    
    Property::OnDeactivate();
    Minimap::MinimapManager::Instance()->UnregisterEntity(this->texture, this->entity);
}


}; // namespace Minimap

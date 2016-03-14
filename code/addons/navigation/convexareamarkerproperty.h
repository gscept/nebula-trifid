#pragma once
//------------------------------------------------------------------------------
/**
    @class Navigation::ConvexAreaMarkerProperty

    Volume that can be used to define areas with tags in leveleditor
      
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "DetourNavMesh.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class ConvexAreaMarkerProperty : public Game::Property
{
	__DeclareClass(ConvexAreaMarkerProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    ConvexAreaMarkerProperty();
    /// destructor
    virtual ~ConvexAreaMarkerProperty();    
	/// override to register accepted messages
	virtual void SetupAcceptedMessages();
	/// called from within Entity::OnStart() after OnLoad when the complete world exist    
	virtual void OnStart();
	/// called from Entity::DeactivateProperties()
	virtual void OnDeactivate();
	/// handle a single message
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
private:
	///
	void UpdateArea(unsigned int areaId);
	
	unsigned int startingAreaId;
};
__RegisterClass(ConvexAreaMarkerProperty);
}; // namespace Navigation
//------------------------------------------------------------------------------

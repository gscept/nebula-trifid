#pragma once
//------------------------------------------------------------------------------
/**
    @class VR::ViveInteractProperty

    (C) 2016 Individual contributors, see AUTHORS file	
*/
#include "game/property.h"
#include "graphics/modelentity.h"
#include "vivemote.h"
#include "managers/trackermanager.h"


//------------------------------------------------------------------------------
namespace VR
{


class ViveInteractProperty : public Game::Property
{
	__DeclareClass(ViveInteractProperty);
	__SetupExternalAttributes();
public:
  
    /// constructor
	ViveInteractProperty();
    /// destructor
	virtual ~ViveInteractProperty();
        
    /// setup callbacks for this property, call by entity in OnActivate()
    virtual void SetupCallbacks();
	/// override to register accepted messages
	virtual void SetupAcceptedMessages();

    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
       
    /// called on begin of frame
    virtual void OnBeginFrame();    
    
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);	
    
protected:
    Ptr<VR::ViveMote> mote;
    Tracking::TrackingMode trackingEnable;
    Ptr<Graphics::ModelEntity> stick;
    Ptr<Graphics::ModelEntity> redstick;
    Ptr<Graphics::ModelEntity> floorMarker;
};
__RegisterClass(ViveInteractProperty);

}; // namespace Property
//------------------------------------------------------------------------------

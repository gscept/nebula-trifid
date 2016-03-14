#pragma once
//------------------------------------------------------------------------------
/**
    @class Properties::AmbienceBubbleProperty
  
    Implement a postprocessing "ambience bubble" in the world which
    changes several postprocessing attributes when the player
    enters it.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "attr/attribute.h"
#include "posteffect/posteffectentity.h"
#include "graphics/globallightentity.h"

//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class AmbienceBubbleProperty : public Game::Property
{
    __DeclareClass(AmbienceBubbleProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    AmbienceBubbleProperty();
    /// destructor
    virtual ~AmbienceBubbleProperty(); 

    /// setup callbacks for this property, call by entity in OnActivate()
    virtual void SetupCallbacks();    
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// called when game debug visualization is on
    virtual void OnRenderDebug();
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

private:
    /// update the posteffect property from our entity attributes
    void UpdatePostEffectEntityFromAttrs();

    Ptr<PostEffect::PostEffectEntity> postEffectEntity;	
};
__RegisterClass(AmbienceBubbleProperty);

} // namespace Properties
//------------------------------------------------------------------------------



#pragma once
//------------------------------------------------------------------------------
/**
    @class Properties::LightProperty

    A light property adds a light source object (Graphics::LightEntity) 
    to a game entity.
    
    (C) 2005 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "graphics/abstractlightentity.h"
#include "graphics/billboardentity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "graphicsfeature/graphicsutil/lightflickerutil.h"


//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class LightProperty : public Game::Property
{
	__DeclareClass(LightProperty);
	__SetupExternalAttributes();
public:
    /// setup callbacks for this property, call by entity in OnActivate()
    virtual void SetupCallbacks();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();

	/// set transform as a spot light
    void SetSpotLightTransform();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
    /// called before rendering happens
    virtual void OnRender();

protected:
    /// update embedded Nebula light object from attributes
    void UpdateLightFromAttributes();
    /// update the light flicker utility from entity attributes
    void UpdateLightFlickerUtilFromAttributes();
    /// update light entity transform
    virtual void UpdateLightTransform(const Math::matrix44& transform);

    Ptr<Graphics::AbstractLightEntity> lightEntity;	
    LightFlickerUtil lightFlickerUtil;
};
__RegisterClass(LightProperty);

}; // namespace GraphicsFeature
//------------------------------------------------------------------------------


#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::MouseGripperProperty

    The MouseGripperProperty allows picking dynamic physics object
    and throw it around.  
  
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "graphicsfeature/properties/inputproperty.h"
#include "physics/util/mousegripper.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
class MouseGripperProperty : public GraphicsFeature::InputProperty
{
	__DeclareClass(MouseGripperProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    MouseGripperProperty();
    /// destructor
    virtual ~MouseGripperProperty();

    /// setup callbacks for this property
    virtual void SetupCallbacks();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

    
    virtual void OnBeginFrame();
    
    virtual void OnMoveBefore();
    /// called after movement has happened
    virtual void OnMoveAfter();
    /// on render debug
    virtual void OnRenderDebug();
    
protected:
    /// handle left mouse btn
    void HandleLeftMouseBtnDown();
    /// handle left mouse btn
    void HandleLeftMouseBtnUp();
    
private:
    Ptr<Physics::MouseGripper> physicsGripper;
};
__RegisterClass(MouseGripperProperty);

}; // namespace Properties
//------------------------------------------------------------------------------
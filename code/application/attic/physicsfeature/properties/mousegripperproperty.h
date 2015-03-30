#ifndef PROPERTIES_MOUSEGRIPPERPROPERTY_H
#define PROPERTIES_MOUSEGRIPPERPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::MouseGripperProperty

    The MouseGripperProperty allows picking dynamic physics object
    and throw it around.  
  
    (C) 2008 Radon Labs GmbH
*/
#include "properties/inputproperty.h"
#include "physics/mousegripper.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
class MouseGripperProperty : public GraphicsFeature::InputProperty
{
	__DeclareClass(MouseGripperProperty);
public:
    /// constructor
    MouseGripperProperty();
    /// destructor
    virtual ~MouseGripperProperty();

    /// setup callbacks for this property
    virtual void SetupCallbacks();
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

    /// called after movement has happened
    virtual void OnBeginFrame();
    /// called before movement has happened
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
#endif
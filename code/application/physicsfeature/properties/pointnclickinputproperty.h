#pragma once
//------------------------------------------------------------------------------
/**
    An basic point'n'click third person input property which creates Goto 
    messages when clicking into the environment.

    (C) 2005 Radon Labs GmbH
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "graphicsfeature/properties/inputproperty.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
class PointNClickInputProperty : public GraphicsFeature::InputProperty
{
	__DeclareClass(PointNClickInputProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    PointNClickInputProperty();
    /// destructor
    virtual ~PointNClickInputProperty();
    /// setup callbacks for this property
    virtual void SetupCallbacks();
    /// called on begin of frame
    virtual void OnBeginFrame();
	/// called from Entity::ActivateProperties()
	virtual void OnActivate();
protected:
    /// handle the default action on left mouse click
    virtual void OnLmbDown();
    /// handle continuous left mouse press
    virtual void OnLmbPressed();
    /// handle a right mouse button click
    virtual void OnRmbDown();
    /// handle camera orbit movement
    virtual void OnMmbPressed();
    /// handle camera zoom in
    virtual void OnCameraZoomIn();
    /// handle camera zoom out
    virtual void OnCameraZoomOut();

private:
    /// send a MoveGoto message to the world intersection point
    void SendMoveGoto();

    Timing::Time moveGotoTime;
};
__RegisterClass(PointNClickInputProperty);
};

//------------------------------------------------------------------------------

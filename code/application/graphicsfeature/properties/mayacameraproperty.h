#pragma once
//------------------------------------------------------------------------------
/**
	@class GraphicsFeature::MayaCameraProperty

    A manually controlled camera property which implements different
    control models.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "graphicsfeature/properties/cameraproperty.h"
#include "renderutil/mayacamerautil.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "math/pfeedbackloop.h"

//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class MayaCameraProperty : public CameraProperty
{
	__DeclareClass(MayaCameraProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    MayaCameraProperty();
    /// destructor
    virtual ~MayaCameraProperty();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();  
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
	/// setup callbacks for this property
    virtual void SetupCallbacks();
    /// called before rendering happens
    virtual void OnRender();
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
    /// called when input focus is gained
    virtual void OnObtainInputFocus();
    /// called when input focus is lost
    virtual void OnLoseInputFocus();
	/// set center of interest
	void SetCameraFocus(const Math::point& centerOfInterest, Math::scalar distance=0.0f);
	/// reset camera
	void ResetCamera();

private:
	/// maya utility
	RenderUtil::MayaCameraUtil mayaCameraUtil;

	/// camera interpolation vars
	Math::point focusDestination;
	bool focusReached;
	Math::scalar eyeDestination;
	bool eyePositionReached;
};
__RegisterClass(MayaCameraProperty);
}; // namespace GraphicsFeature
//------------------------------------------------------------------------------
   
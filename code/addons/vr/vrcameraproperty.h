#pragma once
//------------------------------------------------------------------------------
/**
@class VR::VRCameraProperty

A specialized camera property for VR hmds

(C) 2016 Individual contributors, see AUTHORS file
*/
#include "properties/cameraproperty.h"
#include "graphics/cameraentity.h"

//------------------------------------------------------------------------------
namespace VR
{
	class VRCameraProperty : public GraphicsFeature::CameraProperty
	{
		__DeclareClass(VRCameraProperty);
		__SetupExternalAttributes();
	public:
		
		/// setup callback methods
		virtual void SetupCallbacks();
		/// override to register accepted messages
		virtual void SetupAcceptedMessages();
		/// handle a single message
		virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
		/// called from Entity::ActivateProperties()
		virtual void OnActivate();
		/// 
		virtual void OnDeactivate();
		/// called on begin of frame
		virtual void OnBeginFrame();
        /// called when camera focus is obtained
        virtual void OnObtainCameraFocus();

	protected:

	};
	__RegisterClass(VRCameraProperty);

} // namespace VR

//------------------------------------------------------------------------------


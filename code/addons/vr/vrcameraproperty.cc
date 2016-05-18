//------------------------------------------------------------------------------
//  vr/vrcameraproperty.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vrcameraproperty.h"
#include "managers/focusmanager.h"
#include "game/entity.h"
#include "graphics/stage.h"
#include "graphics/cameraentity.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "vrmanager.h"
#include "openvr.h"
#include "basegamefeature/basegameprotocol.h"

using namespace Math;

namespace VR
{
__ImplementClass(VR::VRCameraProperty, 'VRCM', GraphicsFeature::CameraProperty);

using namespace Game;
using namespace Messaging;
using namespace BaseGameFeature;
using namespace Math;
	

//------------------------------------------------------------------------------
/**
This method is called once when the property is attached to the entity.
*/
void
VRCameraProperty::OnActivate()
{
	CameraProperty::OnActivate();
	
}

//------------------------------------------------------------------------------
/**

*/
void
VRCameraProperty::OnDeactivate()
{
	CameraProperty::OnDeactivate();

}


//------------------------------------------------------------------------------
/**

*/
void
VRCameraProperty::OnBeginFrame()
{
	// only do something if we have the camera focus
	if (FocusManager::Instance()->GetCameraFocusEntity() == this->entity)
	{
		if (VRManager::Instance()->HasHMD())
		{
			Math::matrix44 mat = VRManager::Instance()->GetTrackedObject(VR::HMDisplay);
			mat = Math::matrix44::transpose(mat);
			Ptr<BaseGameFeature::SetTransform> msg = BaseGameFeature::SetTransform::Create();
			msg->SetMatrix(mat);
			this->entity->SetMatrix44(Attr::Transform, mat);
			__SendSync(this->entity, msg);
		}
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
VRCameraProperty::SetupAcceptedMessages()
{	
	CameraProperty::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**

*/
void
VRCameraProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	CameraProperty::HandleMessage(msg);
}

//------------------------------------------------------------------------------
/**

*/
void 
VRCameraProperty::SetupCallbacks()
{
	this->entity->RegisterPropertyCallback(this, BeginFrame);
	CameraProperty::SetupCallbacks();
}


} // namespace VR
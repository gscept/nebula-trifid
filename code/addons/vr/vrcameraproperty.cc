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
#include "vrview.h"

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
    this->tracking = Tracking::IDLE;
	CameraProperty::OnActivate();
	Math::matrix44 startTrans = this->entity->GetMatrix44(Attr::Transform);
	VRManager::Instance()->SetTrackingOrigin(startTrans);
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
        switch (this->tracking)
        {
        case Tracking::PLAYBACK:           
            return;
        default:
            if (VRManager::Instance()->HasHMD())
            {
                Math::matrix44 mat = VRManager::Instance()->GetTrackedObject(VR::HMDisplay);
                Ptr<BaseGameFeature::SetTransform> msg = BaseGameFeature::SetTransform::Create();
                msg->SetMatrix(mat);
                this->entity->SetMatrix44(Attr::Transform, mat);
                __SendSync(this->entity, msg);
            }
            break;
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
    this->RegisterMessage(SetTrackingMode::Id);
}

//------------------------------------------------------------------------------
/**

*/
void
VRCameraProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	CameraProperty::HandleMessage(msg);
    if (msg->CheckId(SetTrackingMode::Id))
    {
        SetTrackingMode * rmsg = msg.cast<SetTrackingMode>().get_unsafe();
        this->tracking = rmsg->GetMode();
    }
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

//------------------------------------------------------------------------------
/**

*/
void
VRCameraProperty::OnObtainCameraFocus()
{
    CameraProperty::OnObtainCameraFocus();
    
        Graphics::CameraSettings camSettings;
        vr::IVRSystem* hmd = VRManager::Instance()->GetHMD();
        float top, bot, left, right;
        hmd->GetProjectionRaw(vr::Eye_Left, &left, &right, &top, &bot);

        vr::HmdMatrix44_t m1 = hmd->GetProjectionMatrix(vr::Eye_Left, 0.01f, 2500.0f, vr::API_OpenGL);
        Math::matrix44 leftEyeProj;
        leftEyeProj.loadu(&m1.m[0][0]);
        leftEyeProj = Math::matrix44::transpose(leftEyeProj);

        vr::HmdMatrix44_t m2 = hmd->GetProjectionMatrix(vr::Eye_Right, 0.01f, 2500.0f, vr::API_OpenGL);
        Math::matrix44 rightEyeProj;
        rightEyeProj.loadu(&m2.m[0][0]);
        rightEyeProj = Math::matrix44::transpose(rightEyeProj);

        if (this->defaultView->IsA(VR::VrView::RTTI))
        {
            Ptr<VR::VrView> vrView = this->defaultView.cast<VR::VrView>();

            float focal = m1.m[0][0];
            float fov = 2.0f * atan2f(1.0f, focal);
            //fov = n_deg2rad(110.0f);
            vrView->CameraSetting(VrView::LeftCamera).SetProjectionMatrix(leftEyeProj, fov, (right - left) / (bot - top), 0.01f, 2500.0f);

            float top2, bot2, left2, right2;
            hmd->GetProjectionRaw(vr::Eye_Right, &left2, &right2, &top2, &bot2);
            focal = m2.m[0][0];
            fov = 2.0f * atan2f(1.0f, focal);
            vrView->CameraSetting(VrView::RightCamera).SetProjectionMatrix(leftEyeProj, fov, (right2 - left2) / (bot2 - top2), 0.01f, 2500.0f);


            vrView->CameraSetting(VrView::DefaultCamera).SetupPerspectiveFov(fov, (right2 - left) / (bot - top), 0.01f, 2500.0f);
            this->cameraEntity->SetCameraSettings(vrView->CameraSetting(VrView::DefaultCamera));
        }
       
}
} // namespace VR
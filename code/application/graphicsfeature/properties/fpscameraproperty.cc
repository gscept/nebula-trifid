//------------------------------------------------------------------------------
//  properties/fpscameraproperty.cc
//  (C) Patrik Nyman, Mariusz Waclawek
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/fpscameraproperty.h"
#include "managers/focusmanager.h"
#include "input/inputserver.h"
#include "input/mouse.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "characters/character.h"
#include "coregraphics/displaydevice.h"
	
namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::FpsCameraProperty, 'FPSC', GraphicsFeature::CameraProperty);

using namespace Game;
using namespace Math;
using namespace BaseGameFeature;
using namespace GraphicsFeature;
using namespace Input;

//------------------------------------------------------------------------------
/**
*/
FpsCameraProperty::FpsCameraProperty():
	headIndex(InvalidIndex)
{
	this->applyEntityTransform = false;
}

//------------------------------------------------------------------------------
/**
*/
FpsCameraProperty::~FpsCameraProperty()
{
     // empty
}

//------------------------------------------------------------------------------
/**
*/
void
FpsCameraProperty::SetupCallbacks()
{
	CameraProperty::SetupCallbacks();	
	this->entity->RegisterPropertyCallback(this, BeginFrame);
}

//------------------------------------------------------------------------------
/**
*/
void
FpsCameraProperty::OnStart()
{
	this->head = this->entity->GetString(Attr::CameraJoint);
	float fov = this->entity->GetFloat(Attr::CameraFov);
	float nearPlane = this->entity->GetFloat(Attr::CameraMinDistance);
	float farPlane = this->entity->GetFloat(Attr::CameraMaxDistance);
	this->sensitivity = this->entity->GetFloat(Attr::CameraSensitivity) * 0.001f; //Because who says 0.003 sensetivity? You say 3.0, hence we devide by 1000 to make it look "nicer"
	this->ylimit = n_deg2rad(this->entity->GetFloat(Attr::CameraHighStop) - 0.01f); 	
	this->skin = this->entity->GetString(Attr::CharacterSkin);
	
	Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
	__SendSync(this->entity, msg);	
	this->modelEntity = msg->GetEntity();	

	CameraProperty::OnStart();

	//Camera settings
	Graphics::CameraSettings settings = this->cameraEntity->GetCameraSettings();
	float aspect = settings.GetAspect();
	settings.SetupPerspectiveFov(n_deg2rad(fov), aspect, nearPlane, farPlane);
	this->cameraEntity->SetCameraSettings(settings);
	
	//setup initial rotation of entity as represented in level editor
	Ptr<SetTransform> setTransformMsg = SetTransform::Create();
	setTransformMsg->SetMatrix(this->cameraEntity->GetTransform());
	__SendSync(this->entity, setTransformMsg); 	

	CoreGraphics::DisplayDevice::Instance()->GetWindow(0)->SetCursorVisible(false);
}

//------------------------------------------------------------------------------
/**
*/
void
FpsCameraProperty::OnBeginFrame()
{
	// only do something if we have the input focus && camera focus
	if (FocusManager::Instance()->GetInputFocusEntity() == this->entity && FocusManager::Instance()->GetCameraFocusEntity() == this->entity)
	{
		InputServer* inputServer = InputServer::Instance();
		const Ptr<Mouse>& mouse = inputServer->GetDefaultMouse();

		Math::float2 mouseMovement = mouse->GetMovement();
		if (this->headIndex == InvalidIndex && !this->head.IsEmpty() && this->modelEntity->HasCharacter())
		{ 
			this->headIndex = this->modelEntity->GetCharacter()->Skeleton().GetJointIndexByName(this->head);
		}
		

		Ptr<MoveRotate> rot_msg = MoveRotate::Create();
		float roty = mouseMovement.x() * this->sensitivity;
		rot_msg->SetAngle(roty);
		__SendSync(this->entity, rot_msg);

		//apply a joint transform if we have one
		Math::matrix44 trans = this->entity->GetMatrix44(Attr::Transform);
		if (headIndex != InvalidIndex)
		{
			Math::matrix44 headPos = this->GetJointPos(headIndex);				
			if (this->entity->GetBool(Attr::CameraTrackPosition))
			{
				trans.set_position(trans.get_position() + Math::vector(headPos.get_position()));
			}
			else
			{				
				trans = matrix44::multiply(headPos, trans);
			}			
		}			

		//Rotate along the X axis
		this->rotx += mouseMovement.y() * this->sensitivity;
		//Pitch control, dont let the camera spin fully
		this->rotx = n_clamp(this->rotx, -this->ylimit, this->ylimit);
			
		this->cameraEntity->SetTransform(Math::matrix44::multiply(Math::matrix44::rotationx(this->rotx), trans));		
	}
}

//------------------------------------------------------------------------------
/**
*/
const Math::matrix44 &
FpsCameraProperty::GetJointPos(IndexT index) const
{
	n_assert(this->modelEntity.isvalid());
	//Get joint position
	const Ptr<Characters::CharacterInstance>& charInst = this->modelEntity.cast<Graphics::ModelEntity>()->GetCharacterInstance();
	charInst->WaitUpdateDone();
	return charInst->Skeleton().GetJointMatrix(index);
}

//------------------------------------------------------------------------------
/**
*/
void
FpsCameraProperty::SetupAcceptedMessages()
{
	this->RegisterMessage(InputFocus::Id);
	CameraProperty::SetupAcceptedMessages();	
}

//------------------------------------------------------------------------------
/**
*/
void
FpsCameraProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	if (msg->CheckId(InputFocus::Id))
	{
		CoreGraphics::DisplayDevice::Instance()->GetWindow(0)->SetCursorLocked(msg.cast<InputFocus>()->GetObtainFocus());
	}		
	CameraProperty::HandleMessage(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
FpsCameraProperty::OnObtainCameraFocus()
{
	CameraProperty::OnObtainCameraFocus();
	if (!this->skin.IsEmpty())
	{
		Ptr<Graphics::HideSkin> msg_hide = Graphics::HideSkin::Create();
		msg_hide->SetSkin(this->skin);
		msg_hide->SetDistribute(false);
		__SendSync(this->entity, msg_hide);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FpsCameraProperty::OnLoseCameraFocus()
{
	CameraProperty::OnLoseCameraFocus();
	if (!this->skin.IsEmpty())
	{
		Ptr<Graphics::ShowSkin> msg_show = Graphics::ShowSkin::Create();
		msg_show->SetSkin(this->skin);
		msg_show->SetDistribute(false);
		__SendSync(this->entity, msg_show);
	}
}

}; // namespace GraphicsFeature

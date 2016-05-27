//------------------------------------------------------------------------------
//  vr/viveinteractproperty.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "viveinteractproperty.h"
#include "vr/vrprotocol.h"
#include "vrmanager.h"
#include "vivemote.h"
#include "game/entity.h"
#include "basegamefeature/basegameprotocol.h"
#include "input/base/gamepadbase.h"
#include "coregraphics/rendershape.h"
#include "debugrender/debugshaperenderer.h"
#include "physics/filterset.h"
#include "physics/physicsserver.h"


namespace VR
{
	__ImplementClass(VR::ViveInteractProperty, 'VVIP', Game::Property);

//------------------------------------------------------------------------------
/**
*/
ViveInteractProperty::ViveInteractProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ViveInteractProperty::~ViveInteractProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ViveInteractProperty::SetupCallbacks()
{
	this->entity->RegisterPropertyCallback(this, BeginFrame);
}

//------------------------------------------------------------------------------
/**
  
*/
void
ViveInteractProperty::OnActivate()
{
	Game::Property::OnActivate();
	Util::String hand = this->entity->GetString(Attr::ControllerHand);
	if (hand == "Left")
	{
		this->mote = VRManager::Instance()->GetViveMote(LeftControl);
	}
	else
	{
		this->mote = VRManager::Instance()->GetViveMote(RightControl);
	}
}

//------------------------------------------------------------------------------
/**
   
*/
void
ViveInteractProperty::OnDeactivate()
{
	Game::Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
   
*/
void
ViveInteractProperty::OnBeginFrame()
{
	Math::matrix44 trans = this->mote->GetTransform();
	Ptr<BaseGameFeature::SetTransform> st = BaseGameFeature::SetTransform::Create();	
	st->SetMatrix(trans);
	__SendSync(this->entity, st);
	CoreGraphics::RenderShape::RenderShapeVertex linePoints[2];
	if (this->mote->GetAxisValue(Base::GamePadBase::RightTriggerAxis) > 0.1f)
	{
		linePoints[0].pos = trans.get_position();
		linePoints[0].color.set(1.0f, 0.0f, 0.0f, 1.0f);

		Physics::FilterSet fs;
		Util::Array<Ptr<Physics::Contact>> points = Physics::PhysicsServer::Instance()->GetScene()->RayCheck(trans.get_position(), trans.get_zaxis()* (-50.f), fs, Physics::BaseScene::Test_Closest);
		if (points.Size())
		{
			linePoints[1].pos = points[0]->GetPoint();
			linePoints[1].color.set(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			linePoints[1].pos = linePoints[0].pos - (trans.get_zaxis() * 50.0f);
			linePoints[1].color.set(1.0f, 0.0f, 0.0f, 1.0f);
		}
		Debug::DebugShapeRenderer::Instance()->DrawPrimitives(Math::matrix44::identity(),
			CoreGraphics::PrimitiveTopology::LineList,
			1,
			linePoints,
			Math::float4(1.0f),
			CoreGraphics::RenderShape::AlwaysOnTop);
		if (this->mote->ButtonUp(Base::GamePadBase::RightShoulderButton) && !points.IsEmpty())
		{
			Math::matrix44 origin;
			origin.set_position(points[0]->GetPoint());
			VRManager::Instance()->SetTrackingOrigin(origin);
		}
	}
	
}

//------------------------------------------------------------------------------
/**
*/
void
ViveInteractProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    
}

void ViveInteractProperty::SetupAcceptedMessages()
{

}

}; // namespace Game

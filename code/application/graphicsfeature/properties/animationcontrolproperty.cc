//------------------------------------------------------------------------------
//  AnimationControlProperty.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/animationcontrolproperty.h"
#include "graphics/graphicsprotocol.h"
#include "graphicsfeatureunit.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "animation/animjobenqueuemode.h"
#include "physicsfeature/physicsprotocol.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::AnimationControlProperty, 'ANCT', Game::Property);

using namespace Util;
using namespace Graphics;
using namespace Math;
using namespace BaseGameFeature;
using namespace Animation;

//------------------------------------------------------------------------------
/**
*/
AnimationControlProperty::AnimationControlProperty():
	walking(false),
	death(false),
	strafing(false)
{
	// empty
}
//------------------------------------------------------------------------------
/**
*/
void
AnimationControlProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(MoveDirection::Id);
    this->RegisterMessage(MoveStop::Id);
	this->RegisterMessage(MoveJump::Id);
	this->RegisterMessage(PlayerUse::Id);
	this->RegisterMessage(PlayerDeath::Id);
    Game::Property::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationControlProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(msg);
    if (msg->CheckId(MoveDirection::Id))
    {
        this->Walk(msg);
    }
    else if (msg->CheckId(MoveStop::Id))
    {
		this->Idle();
    }
	else if (msg->CheckId(MoveJump::Id))
	{
		this->Jump();
	}
	else if (msg->CheckId(PlayerDeath::Id))
	{
		this->Death();
	}
	else if (msg->CheckId(PlayerUse::Id))
	{
		this->Use();
	}	
    else
    {
        Game::Property::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
AnimationControlProperty::Walk(const Ptr<Messaging::Message>& msg)
{
	bool wasStrafing = this->strafing;	
	Math::vector forward;
	if (this->physObj.isvalid())
	{
		forward = this->physObj->GetTransform().get_zaxis();
	}
	else
	{
		forward = this->entity->GetMatrix44(Attr::Transform).get_zaxis();
	}
	
	Ptr<MoveDirection> md = msg.cast<MoveDirection>();
	Math::vector dir = md->GetDirection();
	// convert camera relative vector into absolute vector if necessary
	if (md->GetCameraRelative())
	{
		const Ptr<Graphics::View>& curView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
		Graphics::CameraEntity* camera = curView->GetCameraEntity();
		n_assert(camera);
		matrix44 camTransform = camera->GetTransform();
		camTransform.set_position(float4(0.0f, 0.0f, 0.0f, 1.0f));
		dir = matrix44::transform(dir, camTransform);
	}
	dir.y() = 0.0f;

	dir = Math::vector::normalize(dir);
	
	float dot = Math::vector::dot3(dir,forward);
	
	if(this->entity->GetBool(Attr::UseStrafeAnim))
	{		
		// detect if strafing	
		if(Math::n_abs(dot)<0.5)
		{
			this->strafing = true;			
		}
		else
		{
			this->strafing = false;
		}
	}
	
	if(!walking || this->strafing != wasStrafing)
	{
		if(this->strafing)
		{			
			Attr::AttrId animattr;
			float timeFactor;
			if(Math::vector::dot3(dir,this->entity->GetMatrix44(Attr::Transform).get_xaxis()) < 0)
			{
				if(GetEntity()->GetBool(Attr::UseReversedAnim))
				{
					animattr = Attr::StrafeLeftAnim;
					timeFactor = -1.0f;
				}
				else
				{
					animattr = Attr::StrafeRightAnim;
					timeFactor = 1.0f;
				}				
			}
			else
			{
				animattr = Attr::StrafeLeftAnim;
				timeFactor = 1.0f;
			}

			const Util::String& walkAnim = GetEntity()->GetString(animattr);
            AnimUtil::QueueAnimation(this->entity,walkAnim,MovementTrackIndex,0.0f,200,200,AnimJobEnqueueMode::IgnoreIfSameClipActive,timeFactor);			
		}
		else
		{
			Attr::AttrId animattr;
			float timeFactor;
			if(dot < 0)
			{				
				if(GetEntity()->GetBool(Attr::UseReversedAnim))
				{
					animattr = Attr::WalkAnim;
					timeFactor = -1.0f;
				}
				else
				{
					animattr = Attr::BackAnim;
					timeFactor = 1.0f;
				}
			}
			else
			{
				animattr = Attr::WalkAnim;
				timeFactor = 1.0f;
			}

			const Util::String& walkAnim = GetEntity()->GetString(animattr);
            AnimUtil::QueueAnimation(this->entity,walkAnim,MovementTrackIndex,0.0f,200,200,AnimJobEnqueueMode::IgnoreIfSameClipActive,timeFactor);				
		}

		walking = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
AnimationControlProperty::Death()
{
	Util::String walkAnim = GetEntity()->GetString(Attr::DeathAnim);
	if(walkAnim.Length() == 0 && GetEntity()->GetBool(Attr::UseFallbackAnim))
	{
		walkAnim = GetEntity()->GetString(Attr::StartAnimation);
	}
    AnimUtil::QueueAnimation(this->entity,walkAnim,MovementTrackIndex + 1,0.0f,200,200,AnimJobEnqueueMode::Intercept);				
	death = true;	
}

//------------------------------------------------------------------------------
/**
*/
void 
AnimationControlProperty::Stop()
{   
	this->walking = false;
    Ptr<Graphics::AnimStopTrack> msg = Graphics::AnimStopTrack::Create();
    msg->SetTrackIndex(MovementTrackIndex);
	msg->SetAllowFadeOut(true);
    this->entity->SendSync(msg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
AnimationControlProperty::Idle()
{
    AnimUtil::QueueAnimation(this->entity,this->GetEntity()->GetString(Attr::IdleAnim),MovementTrackIndex,0.0f,200,200,AnimJobEnqueueMode::Intercept);					
	this->walking = false;		
}

//------------------------------------------------------------------------------
/**
*/
void 
AnimationControlProperty::Jump()
{
	
	Util::String jumpAnim = GetEntity()->GetString(Attr::JumpAnim);
	if(jumpAnim.Length() == 0 && GetEntity()->GetBool(Attr::UseFallbackAnim))
	{
		jumpAnim = GetEntity()->GetString(Attr::StartAnimation);
	}
    AnimUtil::QueueAnimation(this->entity,jumpAnim,MovementTrackIndex + 1,1.0f,200,200,AnimJobEnqueueMode::Intercept);					
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AnimationControlProperty::Use()
{

	Util::String useAnim = GetEntity()->GetString(Attr::UseAnim);
	if(useAnim.Length() == 0 && GetEntity()->GetBool(Attr::UseFallbackAnim))
	{
		useAnim = GetEntity()->GetString(Attr::StartAnimation);
	}
    AnimUtil::QueueAnimation(this->entity,useAnim,MovementTrackIndex + 1,1.0f,200,200,AnimJobEnqueueMode::Intercept);					
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationControlProperty::OnActivate()
{
	Game::Property::OnActivate();
	Ptr<PhysicsFeature::GetPhysicsObject> msg = PhysicsFeature::GetPhysicsObject::Create();
	__SendSync(this->entity, msg);
	if (msg->Handled())
	{
		this->physObj = msg->GetObject();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationControlProperty::OnDeactivate()
{
	this->physObj = 0;
	Game::Property::OnDeactivate();
}

} // namespace GraphicsFeature
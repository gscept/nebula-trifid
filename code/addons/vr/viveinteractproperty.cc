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
#include "physicsbody.h"
#include "graphicsfeatureunit.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"

using namespace Graphics;
using namespace GraphicsFeature;
using namespace Math;
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
    this->stick = ModelEntity::Create();
    this->stick->SetResourceId("mdl:vive/pointer.n3");
    this->stick->SetTransform(this->entity->GetMatrix44(Attr::Transform));
    this->stick->SetLoadSynced(true);
    this->stick->SetCastsShadows(false);
    this->stick->SetVisible(false);
    const Ptr<Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
    stage->AttachEntity(this->stick.upcast<Graphics::GraphicsEntity>());

    this->redstick = ModelEntity::Create();
    this->redstick->SetResourceId("mdl:vive/redpointer.n3");
    this->redstick->SetTransform(this->entity->GetMatrix44(Attr::Transform));
    this->redstick->SetLoadSynced(true);
    this->redstick->SetCastsShadows(false);
    this->redstick->SetVisible(false);    
    stage->AttachEntity(this->redstick.upcast<Graphics::GraphicsEntity>());

    this->floorMarker = ModelEntity::Create();
    this->floorMarker->SetResourceId("mdl:vive/primitive_plane.n3");
    this->floorMarker->SetTransform(this->entity->GetMatrix44(Attr::Transform));
    this->floorMarker->SetLoadSynced(true);
    this->floorMarker->SetCastsShadows(false);
    this->floorMarker->SetVisible(false);    
    stage->AttachEntity(this->floorMarker.upcast<Graphics::GraphicsEntity>());

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
    const Ptr<Graphics::Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
    stage->RemoveEntity(this->stick.upcast<GraphicsEntity>());
    this->stick = 0;
    stage->RemoveEntity(this->redstick.upcast<GraphicsEntity>());
    this->redstick = 0;
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


    Math::point here = trans.get_position();
    Math::point target = trans.get_position();
    Ptr<Game::Entity> hitObject;
    Physics::FilterSet fs;
    bool hit = false;

    if ((n_abs(this->mote->GetAxisValue(Base::GamePadBase::RightThumbXAxis)) > 0.0f) || (n_abs(this->mote->GetAxisValue(Base::GamePadBase::RightThumbYAxis)) > 0.0f))
    {

        Math::vector up(0, 1, 0);

        float curdist = FLT_MAX;

        Util::Array<Ptr<Physics::Contact>> points = Physics::PhysicsServer::Instance()->GetScene()->RayCheck(trans.get_position(), trans.get_zaxis()* (-500.f), fs, Physics::BaseScene::Test_All);
        if (!points.IsEmpty())
        {
            for (int i = 0; i < points.Size(); i++)
            {
                if (Math::float4::dot3(points[i]->GetNormalVector(), up) > 0.8f)
                {
                    float dist = (here - points[i]->GetPoint()).length3();
                    if (dist < curdist)
                    {
                        curdist = dist;
                        target = points[i]->GetPoint();
                        hit = true;
                    }
                }
            }
        }
        if (!hit)
        {
            target = here - (trans.get_zaxis() * 500.0f);            
            this->floorMarker->SetVisible(false);
        }
        else
        {
            matrix44 marker = this->floorMarker->GetTransform();
            marker.set_position(target+vector(0,0.1,0));
            this->floorMarker->SetTransform(marker);
            this->floorMarker->SetVisible(true);            
        }
        this->redstick->SetVisible(true);
        matrix44 stickTrans = matrix44::lookatlh(here, target, Math::vector(0, 1, 0));
        matrix44 stickScale = matrix44::scaling(float4(0.1f, 0.1f, (target - here).length3(), 1.0f));
        this->redstick->SetTransform(matrix44::multiply(stickScale, stickTrans));

        if (this->mote->ButtonUp(Base::GamePadBase::RightThumbButton) && hit)
        {

            Math::matrix44 origin;
            origin.set_position(target);
            VRManager::Instance()->SetTrackingOrigin(origin);
        }

        if ((n_abs(this->mote->GetAxisValue(Base::GamePadBase::RightThumbXAxis)) > 0.6f))
        {

        }
    }
    else
    {
        this->redstick->SetVisible(false);
        this->floorMarker->SetVisible(false);
    }

    if (this->mote->GetAxisValue(Base::GamePadBase::RightTriggerAxis) > 0.1f)
    {
        Util::Array<Ptr<Physics::Contact>> spoints = Physics::PhysicsServer::Instance()->GetScene()->RayCheck(trans.get_position(), trans.get_zaxis()* (-500.f), fs, Physics::BaseScene::Test_Closest);
        this->stick->SetVisible(true);

        if (spoints.Size() > 0)
        {
            target = spoints[0]->GetPoint();
            if (spoints[0]->GetCollisionObject().isvalid() != 0 && spoints[0]->GetCollisionObject()->GetUserData()->object.isvalid() && spoints[0]->GetCollisionObject()->GetUserData()->object->IsA(Game::Entity::RTTI))
            {
                hitObject = spoints[0]->GetCollisionObject()->GetUserData()->object.cast<Game::Entity>();
            }
        }
        else
        {
            target = here - (trans.get_zaxis() * 500.0f);
        }

        matrix44 stickTrans = matrix44::lookatlh(here, target, Math::vector(0, 1, 0));
        matrix44 stickScale = matrix44::scaling(float4(0.1f, 0.1f, (target - here).length3(), 1.0f));
        this->stick->SetTransform(matrix44::multiply(stickScale, stickTrans));


        if (hitObject.isvalid())
        {
            Ptr<VR::ViveHover> msg = VR::ViveHover::Create();
            __SendSync(hitObject, msg);
            if (this->mote->ButtonDown(Base::GamePadBase::RightShoulderButton))
            {
                if (hit && hitObject.isvalid())
                {
                    Ptr<VR::ViveClick> msg = VR::ViveClick::Create();
                    __SendSync(hitObject, msg);
                }
            }
        }
    }
    else
    {
        this->stick->SetVisible(false);
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

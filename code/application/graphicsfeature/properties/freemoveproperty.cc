//------------------------------------------------------------------------------
//  properties/freemoveproperty.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/freemoveproperty.h"
#include "game/entity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "basegamefeature/basegameprotocol.h"
#include "messaging/staticmessagehandler.h"
#include "graphicsfeatureunit.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::FreeMoveProperty, 'FRPR', BaseGameFeature::TransformableProperty);

using namespace BaseGameFeature;

//------------------------------------------------------------------------------
/**
*/
FreeMoveProperty::FreeMoveProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FreeMoveProperty::~FreeMoveProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
FreeMoveProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(MoveDirection::Id);
    this->RegisterMessage(MoveRotate::Id);
	BaseGameFeature::TransformableProperty::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
FreeMoveProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(msg != 0);
    
    if (msg->CheckId(MoveDirection::Id))
    {
		const Ptr<MoveDirection> & mmsg = msg.downcast<MoveDirection>();
		Math::matrix44 trans = this->entity->GetMatrix44(Attr::Transform);
		if (mmsg->GetCameraRelative())
		{
			Math::matrix44 camtrans = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView()->GetCameraEntity()->GetTransform();
			camtrans.set_position(Math::point(0.0f));
			Math::vector dir = mmsg->GetDirection();
			dir = Math::matrix44::transform(dir, camtrans);
			trans.translate(dir);
		}
		else
		{			
			trans.translate(Math::vector(mmsg->GetDirection()));
		}
		Ptr<SetTransform> tmsg = SetTransform::Create();
		tmsg->SetMatrix(trans);
		__SendSync(this->entity, tmsg);
    }
    else if (msg->CheckId(MoveRotate::Id))
    {
		Math::matrix44 trans = this->entity->GetMatrix44(Attr::Transform);
		const Ptr<MoveRotate> & mmsg = msg.downcast<MoveRotate>();
		float angle = mmsg->GetAngle();
		Math::point pos = trans.get_position();
		trans.set_position(Math::point(0.0f));
		trans = Math::matrix44::multiply(Math::matrix44::rotationy(angle), trans);
		trans.set_position(pos);
		Ptr<SetTransform> tmsg = SetTransform::Create();
		tmsg->SetMatrix(trans);
		__SendSync(this->entity, tmsg);
    }
    else
    {
		BaseGameFeature::TransformableProperty::HandleMessage(msg);
    }
}

}; // namespace BaseGameFeature

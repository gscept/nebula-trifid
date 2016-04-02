//------------------------------------------------------------------------------
//  callbackmanager.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "callbackmanager.h"
#include "baseattributecontroller.h"
#include "util/variant.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "graphicsfeature/properties/graphicsproperty.h"
#ifdef LVLEDITOR_PHYSICS
#include "physicsfeature/properties/physicsproperty.h"
#endif

using namespace Attr;
using namespace Util;
using namespace Math;

namespace QtAttributeControllerAddon
{
__ImplementClass(CallbackManager, 'CBMR', Game::Manager);
__ImplementSingleton(CallbackManager);

//------------------------------------------------------------------------------
/**
*/
CallbackManager::CallbackManager()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
CallbackManager::~CallbackManager()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void 
CallbackManager::OnValueChanged(BaseAttributeController* controller)
{
	n_assert(0 != controller);

	Ptr<Game::Entity> entity = controller->GetGameEntity();
	n_assert(entity.isvalid());
	AttrId attrId = controller->GetAttributeId();
	Util::Variant value = controller->GetValue();
	Variant::Type type = controller->GetAttributeType();

	if (Matrix44Type == type )
	{
		// special handling for matrices
		// note: Attr::Transform should be updated via a SetTransform message (better to this in a manager derived from this)
		matrix44 matrix = value.GetMatrix44();
		float4 position = matrix.getrow0();
		float4 scale = matrix.getrow1();
		float4 rot = matrix.getrow2();
		quaternion qrot(rot);
		
		matrix44 newMatrix = matrix44::transformation(float4::zerovector(), quaternion::identity(), scale, float4::zerovector(), qrot, position);
		if (attrId == Attr::Transform)
		{
			entity->SetMatrix44(attrId, newMatrix);
		}		
		Ptr<BaseGameFeature::SetAttribute> msg = BaseGameFeature::SetAttribute::Create();
		Attribute attr(attrId);
		attr.SetMatrix44(newMatrix);
		msg->SetAttr(attr);

		entity->SendSync(msg.cast<Messaging::Message>());
	}
	else if(attrId == Attr::Graphics)
	{
		// trigger reload
		entity->SetString(Attr::Graphics,controller->GetValue().GetString());
		Ptr<GraphicsFeature::GraphicsProperty> prop = entity->FindProperty(GraphicsFeature::GraphicsProperty::RTTI).cast<GraphicsFeature::GraphicsProperty>();
		prop->OnDeactivate();
		prop->OnActivate();
#ifdef LVLEDITOR_PHYSICS
		Ptr<PhysicsFeature::PhysicsProperty> physicsProp = entity->FindProperty(PhysicsFeature::PhysicsProperty::RTTI).cast<PhysicsFeature::PhysicsProperty>();
		physicsProp->OnDeactivate();
		physicsProp->OnActivate();
#endif

		Ptr<BaseGameFeature::SetAttribute> msg = BaseGameFeature::SetAttribute::Create();
		Attribute attr(attrId);
		attr.SetValue(value);
		msg->SetAttr(attr);

		entity->SendSync(msg.cast<Messaging::Message>());
	}
	else
	{
		Ptr<BaseGameFeature::SetAttribute> msg = BaseGameFeature::SetAttribute::Create();
		Attribute attr(attrId);
		attr.SetValue(value);
		msg->SetAttr(attr);
		
		entity->SendSync(msg.cast<Messaging::Message>());

		// notify entity that its attributes have been updated
		Ptr<BaseGameFeature::AttributesUpdated> attrsUpdatedMsg = BaseGameFeature::AttributesUpdated::Create();
		entity->SendSync(attrsUpdatedMsg.cast<Messaging::Message>());
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
CallbackManager::OnValueChanged(VariableControllerWidget* controller)
{
	// implement in subclass
}

}
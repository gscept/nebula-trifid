//------------------------------------------------------------------------------
//  leveleditor2/editorproperty.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/editorproperty.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "messaging/staticmessagehandler.h"
#include "basegamefeature/basegameprotocol.h"
#include "entityguid.h"
#include "leveleditor2app.h"

using namespace LevelEditor2;
using namespace BaseGameFeature;
using namespace Math;

namespace Attr
{
	DefineInt(EntityType, 'ETTY', ReadWrite);
	DefineString(EntityCategory,'ETCA',ReadWrite);	
	DefineString(EntityLevel,'ETLV',ReadWrite);	
	DefineGuid(EntityGuid,'ETGU',ReadWrite);
	DefineGuid(ParentGuid,'PAGU',ReadWrite);
	DefineBool(IsSelected,'ISCT',ReadWrite);
    DefineBool(IsLocked, 'ISLK', ReadWrite);
}

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorProperty,GetEntityValues)
{
	msg->SetProps(obj->GetProperties());
	msg->SetAttrs(obj->GetAttributes());
	msg->SetHandled(true);
}

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorProperty,SetAttributes)
{
	obj->SetAttributes(msg->GetAttrs());
}

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorProperty,SetProperties)
{
	obj->SetProperties(msg->GetProps());
}

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorProperty,SetAttribute)
{
	int etype = obj->entity->GetInt(Attr::EntityType);
	if(etype == LevelEditor2::Game && msg->GetAttr().GetAttrId() != Attr::Transform)
	{
		obj->attributes.SetAttr(msg->GetAttr());
	}
	else
	{
		obj->GetEntity()->SetAttr(msg->GetAttr());
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorProperty,GetAttribute)
{
	int etype = obj->entity->GetInt(Attr::EntityType);
	if(etype == LevelEditor2::Game && msg->GetAttributeId() != Attr::Transform)
	{
		msg->SetAttr(obj->attributes.GetAttr(msg->GetAttributeId()));
		msg->SetHandled(true);		
	}
	else
	{
		msg->SetAttr(obj->entity->GetAttr(msg->GetAttributeId()));
		msg->SetHandled(true);	
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorProperty, SetTransform)
{
	// create a update transform msg, to update the transformation
	const Ptr<SetTransform>& setTransform = msg.downcast<SetTransform>();
	Ptr<UpdateTransform> updateTransform = UpdateTransform::Create();
	updateTransform->SetMatrix(setTransform->GetMatrix());
	obj->GetEntity()->SendSync(updateTransform.upcast<Messaging::Message>());
	

	Util::Array<Ptr<Game::Entity>> children = LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->GetDirectChildren(obj->GetEntity()->GetGuid(Attr::EntityGuid));
	matrix44 trans = msg->GetMatrix();
	matrix44 inv = matrix44::inverse(obj->GetEntity()->GetMatrix44(Attr::Transform));
	matrix44 delta = matrix44::multiply(trans,inv);
	if (msg->GetDistribute())
	{
		Ptr<UpdateChildrenTransforms> umsg = UpdateChildrenTransforms::Create();
		umsg->SetBaseTransform(obj->GetEntity()->GetMatrix44(Attr::Transform));
		umsg->SetInvBaseTransform(inv);
		umsg->SetDeltaTransform(delta);
		for (IndexT i = 0; i < children.Size(); i++)
		{
			children[i]->SendSync(umsg.cast<Messaging::Message>());
		}
	}
	obj->GetEntity()->SetMatrix44(Attr::Transform, updateTransform->GetMatrix());
}

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorProperty, UpdateChildrenTransforms)
{

	Util::Array<Ptr<Game::Entity>> children = LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->GetDirectChildren(obj->GetEntity()->GetGuid(Attr::EntityGuid));

	for(IndexT i = 0 ; i < children.Size() ; i++)
	{		
		children[i]->SendSync(msg.cast<Messaging::Message>());
	}
	matrix44 m = matrix44::multiply(obj->GetEntity()->GetMatrix44(Attr::Transform),msg->GetInvBaseTransform());
	m = matrix44::multiply(m, msg->GetDeltaTransform());
	m = matrix44::multiply(m, msg->GetBaseTransform());
	
	obj->GetEntity()->SetMatrix44(Attr::Transform, m);

	Ptr<UpdateTransform> updateTransform = UpdateTransform::Create();
	updateTransform->SetMatrix(m);
	obj->GetEntity()->SendSync(updateTransform.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
__Dispatcher(EditorProperty)
{
	__Handle(EditorProperty, GetEntityValues);
	__Handle(EditorProperty, SetAttributes);
	__Handle(EditorProperty, SetProperties);
	__Handle(EditorProperty, UpdateChildrenTransforms);
	__Handle(EditorProperty, SetAttribute);
	__Handle(EditorProperty, GetAttribute);
	__Handle(EditorProperty, SetTransform);
}

namespace LevelEditor2
{

__ImplementClass(LevelEditor2::EditorProperty, 'LPRO', Game::Property);

using namespace Game;
using namespace Messaging;
using namespace Math;
using namespace BaseGameFeature;

//------------------------------------------------------------------------------
/**
*/
EditorProperty::EditorProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
EditorProperty::SetupDefaultAttributes()
{
	SetupAttr(Attr::Transform, false);
	SetupAttr(Attr::EntityType, false);
	SetupAttr(Attr::EntityCategory, false);
	SetupAttr(Attr::EntityLevel, false);
	SetupAttr(Attr::EntityGuid, false);
	SetupAttr(Attr::ParentGuid, false);
	SetupAttr(Attr::IsSelected, false);
    SetupAttr(Attr::IsLocked, false);
}

//------------------------------------------------------------------------------
/**
*/
void
EditorProperty::SetupAcceptedMessages()
{
	this->RegisterMessage(LevelEditor2::GetEntityValues::Id);
	this->RegisterMessage(LevelEditor2::SetAttributes::Id);
	this->RegisterMessage(LevelEditor2::SetProperties::Id);
	this->RegisterMessage(LevelEditor2::UpdateChildrenTransforms::Id);
	this->RegisterMessage(BaseGameFeature::SetAttribute::Id);
	this->RegisterMessage(BaseGameFeature::GetAttribute::Id);
	this->RegisterMessage(BaseGameFeature::SetTransform::Id);	
}

//------------------------------------------------------------------------------
/**
*/
void 
EditorProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	__Dispatch(EditorProperty,this,msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
EditorProperty::SetProperties(const Util::Array<Core::Rtti*> & props)
{
	this->propertyArray = props;
}

//------------------------------------------------------------------------------
/**
*/
void 
EditorProperty::SetAttributes(const Attr::AttributeContainer & attrs)
{
	this->attributes = attrs;
}

}
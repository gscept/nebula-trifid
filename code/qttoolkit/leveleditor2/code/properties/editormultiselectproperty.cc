//------------------------------------------------------------------------------
//  leveleditor2/editormultuselectproperty.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/editormultiselectproperty.h"
#include "leveleditor2protocol.h"
#include "messaging/staticmessagehandler.h"
#include "basegamefeature/basegameprotocol.h"
#include "entityguid.h"
#include "leveleditor2app.h"

using namespace LevelEditor2;
using namespace BaseGameFeature;
using namespace Math;
using namespace Attr;

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorMultiselectProperty, GetEntityValues)
{
	Util::Array<Core::Rtti*> props;
	msg->SetProps(props);
	msg->SetAttrs(obj->attributes);
	msg->SetHandled(true);
}

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorMultiselectProperty, SetAttribute)
{
	obj->attributes.SetAttr(msg->GetAttr());
	for (int i = 0; i < obj->entities.Size(); i++)
	{
		__SendSync(obj->entities[i], msg);
	}	
}

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorMultiselectProperty, GetAttribute)
{	
	msg->SetAttr(obj->attributes.GetAttr(msg->GetAttributeId()));
	msg->SetHandled(true);			
}

//------------------------------------------------------------------------------
/**
*/
__Handler(EditorMultiselectProperty, SetMultiSelection)
{
	obj->entities = msg->GetEntities();
	obj->attributes.Clear();
	for (int i = 0; i < obj->entities.Size(); i++)
	{
		Ptr<GetEntityValues> gmsg = GetEntityValues::Create();
		__SendSync(obj->entities[i], gmsg);

		const Util::Dictionary<AttrId, Attribute>& attrs = gmsg->GetAttrs().GetAttrs();
		const Util::Dictionary<AttrId, Attribute>& objattrs = obj->attributes.GetAttrs();
		if (i == 0)
		{
			for (int j = 0; j < attrs.Size(); j++)
			{
				obj->attributes.AddAttr(attrs.ValueAtIndex(j));
			}
		}
		else
		{
			for (int j = 0; j < objattrs.Size(); j++)
			{

				if (!attrs.Contains(objattrs.KeyAtIndex(j)))
				{
					obj->attributes.RemoveAttr(objattrs.KeyAtIndex(j));
				}
			}
		}	
	}
	if (!obj->attributes.HasAttr(Attr::Transform))
	{
		obj->attributes.AddAttr(Attr::Attribute(Attr::Transform, Math::matrix44::identity()));
	}
}

//------------------------------------------------------------------------------
/**
*/
__Dispatcher(EditorMultiselectProperty)
{
	__Handle(EditorMultiselectProperty, GetEntityValues);	
	__Handle(EditorMultiselectProperty, SetAttribute);
	__Handle(EditorMultiselectProperty, GetAttribute);
	__Handle(EditorMultiselectProperty, SetMultiSelection);	
}

namespace LevelEditor2
{

__ImplementClass(LevelEditor2::EditorMultiselectProperty, 'LMSO', Game::Property);

using namespace Game;
using namespace Messaging;
using namespace Math;
using namespace BaseGameFeature;

//------------------------------------------------------------------------------
/**
*/
EditorMultiselectProperty::EditorMultiselectProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
EditorMultiselectProperty::SetupDefaultAttributes()
{
	SetupAttr(Attr::Transform, false);
	SetupAttr(Attr::EntityType, false);
	SetupAttr(Attr::EntityCategory, false);	
	SetupAttr(Attr::EntityGuid, false);		
}

//------------------------------------------------------------------------------
/**
*/
void
EditorMultiselectProperty::SetupAcceptedMessages()
{
	this->RegisterMessage(LevelEditor2::GetEntityValues::Id);
	this->RegisterMessage(LevelEditor2::SetAttributes::Id);
	this->RegisterMessage(LevelEditor2::SetProperties::Id);	
	this->RegisterMessage(LevelEditor2::SetMultiSelection::Id);
	this->RegisterMessage(BaseGameFeature::SetAttribute::Id);
	this->RegisterMessage(BaseGameFeature::GetAttribute::Id);
	this->RegisterMessage(BaseGameFeature::SetTransform::Id);	
}

//------------------------------------------------------------------------------
/**
*/
void 
EditorMultiselectProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	__Dispatch(EditorMultiselectProperty, this, msg);
}

}
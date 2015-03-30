#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::EditorProperty

    Property for storing information related to the entity in the leveleditor.	
    
    (C) 2012 Johannes Hirche, LTU Skelleftea
*/
#include "game/property.h"
#include "attr/attributedefinition.h"
#include "core/rtti.h"
#include "attr/attributecontainer.h"
#include "messaging/staticmessagehandler.h"


//------------------------------------------------------------------------------
namespace Attr
{
	DeclareInt(EntityType, 'ETTY', ReadWrite);
	DeclareString(EntityCategory,'ETCA',ReadWrite);	
	DeclareString(EntityLevel,'ETLV',ReadWrite);	
	DeclareGuid(EntityGuid,'ETGU',ReadWrite);
	DeclareGuid(ParentGuid,'PAGU',ReadWrite);
	DeclareBool(IsSelected,'ISCT',ReadWrite);
};

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class EditorProperty : public Game::Property
{
	__DeclareClass(EditorProperty);
public:    
	EditorProperty();

	virtual void SetupDefaultAttributes();
	virtual void SetupAcceptedMessages();
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

	const Util::Array<Core::Rtti*> & GetProperties() const;
	Attr::AttributeContainer GetAttributes() const;
	void SetProperties(const Util::Array<Core::Rtti*> & props);
	void SetAttributes(const Attr::AttributeContainer & attrs);

protected:
	friend class Messaging::StaticMessageHandler;   

	Util::Array<Core::Rtti*> propertyArray;
	Attr::AttributeContainer attributes;
	Math::matrix44 old;

};
__RegisterClass(EditorProperty);


//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Core::Rtti*> &
EditorProperty::GetProperties() const
{
	return this->propertyArray;
}


//------------------------------------------------------------------------------
/**
*/
inline Attr::AttributeContainer
EditorProperty::GetAttributes() const
{
	return this->attributes;
}



}; // namespace LevelEditor2
//------------------------------------------------------------------------------


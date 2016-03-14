#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::BaseAttributeController
    
    Base class for attribute controllers. Contains reference to the entity, and
    an attribute (id plus type) of the entity.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include <QWidget>
#include "attr/attrid.h"
#include "util/variant.h"
#include "core/ptr.h"
#include "game/entity.h"

//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
class BaseAttributeController : public QWidget
{
public:		
	/// constructor
	BaseAttributeController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType);
	/// destructor
	~BaseAttributeController();

	/// get game entity
	Ptr<Game::Entity> GetGameEntity() const;
	/// get attribute id
	Attr::AttrId GetAttributeId() const;
	/// get attribute type
	Util::Variant::Type GetAttributeType() const;

	/// get the value this controller currently has
	virtual Util::Variant GetValue() const = 0;
	/// make the controller view-only
	virtual void Lock() = 0;
	
	/// event filter for Qt's retarded mousewheel focus behaviour
	bool eventFilter(QObject *obj, QEvent *e);

protected slots:
	//FIXME: optimally these slots should be connectable directly from the derived classes constructors
	/// when the value of the derived controller is changed
	void OnValueChanged();

protected:
	/// constructor
	BaseAttributeController(QWidget*parent);
	friend class AttributeControllerWidget;

	Ptr<Game::Entity> entity;
	Attr::AttrId attributeId;
	Util::Variant::Type type;
};

//------------------------------------------------------------------------------
/**
*/
inline Ptr<Game::Entity> 
BaseAttributeController::GetGameEntity() const
{
	return this->entity;
}

//------------------------------------------------------------------------------
/**
*/
inline Attr::AttrId 
BaseAttributeController::GetAttributeId() const
{
	return this->attributeId;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::Variant::Type 
BaseAttributeController::GetAttributeType() const
{
	return this->type;
}

}
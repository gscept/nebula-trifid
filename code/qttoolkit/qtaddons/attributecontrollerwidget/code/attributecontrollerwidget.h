#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::AttributeControllerWidget
    
    A widget which represents an attribute that belongs to a nebula entity.
    Depending on the attribute type, a specific controller is created for 
    editing the value of the attribute.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include <QWidget>
#include "util/string.h"
#include "core/ptr.h"
#include "util/variant.h"
#include "attr/attribute.h"
#include "idldocument/idlattribute.h"

namespace Ui
{
	class AttributeControllerWidget;
}

namespace Game
{
	class Entity;
}

namespace Db
{
	class ValueTable;
}

namespace Tools
{
	class IDLAttribute;
}
//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
class BaseAttributeController;

class AttributeControllerWidget : public QWidget
{
	Q_OBJECT
public:
	/// constructor
	AttributeControllerWidget(const Ptr<Game::Entity>& entity, const Attr::AttrId& attrId, Util::Variant::Type attrType);
	
	/// destructor
	~AttributeControllerWidget();

	/// make the widgets view-only
	void LockWidgets();
	/// get the attribute controller
	BaseAttributeController* GetControllerWidget();

private:
	/// initialize input widget, and add it to the container
	void InitInputWidget(const Ptr<Game::Entity>& entity, const Attr::AttrId& attrId, Util::Variant::Type attrType);
	/// initialize input widget, and add it to the container using an IDLAttribute
	void InitInputWidget(const Ptr<Game::Entity>& entity, const Ptr<Tools::IDLAttribute>& idl);


	Ui::AttributeControllerWidget* ui;
	BaseAttributeController* baseAttributeController;
};

//------------------------------------------------------------------------------
/**
*/
inline BaseAttributeController* 
AttributeControllerWidget::GetControllerWidget()
{
	return this->baseAttributeController;
}

}
//------------------------------------------------------------------------------
//  baseattributecontroller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "baseattributecontroller.h"
#include "callbackmanager.h"
#include "qapplication.h"

using namespace Attr;
using namespace Util;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
BaseAttributeController::BaseAttributeController(QWidget* parent):QWidget(parent)
{
	/// empty
}

//------------------------------------------------------------------------------
/**
*/
BaseAttributeController::BaseAttributeController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType):
	QWidget(parent),
	entity(_entity),
	attributeId(_attrId),
	type(_attrType)
{
	n_assert2(CallbackManager::HasInstance(), "A callbackmanager must exist! Make sure to create one before creating a BaseAttributeController.");
}

//------------------------------------------------------------------------------
/**
*/
BaseAttributeController::~BaseAttributeController()
{
	this->entity = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseAttributeController::OnValueChanged()
{
	CallbackManager::Instance()->OnValueChanged(this);
}

//------------------------------------------------------------------------------
/**
*/
bool
BaseAttributeController::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::Wheel)
	{
		QWidget* widget = qobject_cast<QWidget*>(obj);
		if (widget && !widget->hasFocus())
			return true;
	}
	return false;
}

}


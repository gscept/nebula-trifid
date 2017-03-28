#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::BoolController
    
    A widget which represents an editable boolean attribute.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include <QWidget>
#include "util/string.h"
#include "core/ptr.h"
#include "util/variant.h"
#include "baseattributecontroller.h"

namespace Ui
{
	class BoolController;
}

namespace Game
{
	class Entity;
}

//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
class BoolController : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
	BoolController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType);
	/// destructor
	~BoolController();

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

protected slots:
	/// when the value of the derived controller is changed
	void OnValueChanged(){BaseAttributeController::OnValueChanged();}

private:
	Ui::BoolController* ui;
};
}

#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::Float4Controller
    
    A widget which represents an editable 4-float vector.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include <QWidget>
#include "util/string.h"
#include "core/ptr.h"
#include "util/variant.h"
#include "baseattributecontroller.h"

namespace Ui
{
	class Float4Controller;
}

namespace Game
{
	class Entity;
}

//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
class Float4Controller : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
	Float4Controller(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType);
	/// destructor
	~Float4Controller();

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

protected slots:
	/// when the value of the derived controller is changed
	void OnValueChanged(){BaseAttributeController::OnValueChanged();}

private:
	Ui::Float4Controller* ui;
};
}

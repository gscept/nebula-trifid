#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::PhysicsMaterialController
    
    A widget which represents a dropbox with with available physics materials
    
    (C) 2016 Individual contributors, see AUTHORS file
*/
#include <QWidget>
#include "util/string.h"
#include "core/ptr.h"
#include "util/variant.h"
#include "baseattributecontroller.h"

namespace Ui
{
	class StringListController;
}

namespace Game
{
	class Entity;
}

namespace Tools
{
	class IDLAttribute;
}

//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
class PhysicsMaterialController : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
    PhysicsMaterialController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Ptr<Tools::IDLAttribute>& _attr);
	/// destructor
    ~PhysicsMaterialController();

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

protected slots:
	/// when the value of the derived controller is changed
	void currentIndexChanged(int);

private:

	Ui::StringListController* ui;
	Util::String currentValue;
};
}

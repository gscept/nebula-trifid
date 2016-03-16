#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::StringListController
    
    A widget which represents a dropbox with string values
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
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
class StringListController : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
	StringListController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Ptr<Tools::IDLAttribute>& _attr);
	/// destructor
	~StringListController();

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

protected slots:
	/// when the value of the derived controller is changed
	void currentIndexChanged (int){BaseAttributeController::OnValueChanged();}

private:
	Ui::StringListController* ui;
};
}

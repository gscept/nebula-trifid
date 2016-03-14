#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::ResourceStringController
    
    A widget which represents a resource string with browse and optional edit
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include <QWidget>
#include "util/string.h"
#include "core/ptr.h"
#include "util/variant.h"
#include "baseattributecontroller.h"

namespace Ui
{
	class ResourceStringController;
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
class ScriptEditor;
class ResourceStringController : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
	ResourceStringController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Ptr<Tools::IDLAttribute>& _attr);
	/// destructor
	~ResourceStringController();

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

protected slots:
	/// when the value of the derived controller is changed
	void currentIndexChanged (int){BaseAttributeController::OnValueChanged();}
	/// browse button clicked
	void OnBrowse();
	/// edit button clicked
	void OnEdit();
	/// line edit updated
	void OnLineEdit();

private:
	Ui::ResourceStringController* ui;
	Ptr<Tools::IDLAttribute> attr;
	Util::String resourceType;
	Util::String fileSuffix;
	bool editable;
	bool fullName;
};
}

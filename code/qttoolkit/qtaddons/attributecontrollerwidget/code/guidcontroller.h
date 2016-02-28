#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::GuidController
    
    A widget which represents a GUID attribute.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include <QWidget>
#include "util/string.h"
#include "core/ptr.h"
#include "util/variant.h"
#include "baseattributecontroller.h"

namespace Ui
{
	class GuidController;
}

namespace Game
{
	class Entity;
}

//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
class GuidController : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
	GuidController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType);
	/// destructor
	~GuidController();

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

protected slots:
	/// when the value of the derived controller is changed
	void OnValueChanged(){BaseAttributeController::OnValueChanged();}

private:
	Ui::GuidController* ui;
};
}

#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::StringController
    
    A widget which represents an editable string attribute.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include <QWidget>
#include "util/string.h"
#include "core/ptr.h"
#include "util/variant.h"
#include "baseattributecontroller.h"

namespace Ui
{
	class StringController;
}

namespace Game
{
	class Entity;
}

//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
class StringController : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
	StringController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType);
	/// destructor
	~StringController();

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

private slots:
	/// when the text has been changed
	void OnTextChanged(const QString &string);

private:

	Ui::StringController* ui;
};
} // namespace QtAttributeControllerAddon

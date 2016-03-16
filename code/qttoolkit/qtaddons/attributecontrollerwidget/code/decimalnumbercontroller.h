#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::DecimalNumberController
    
    A widget which represents an editable decimal number attribute.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include <QWidget>
#include "util/string.h"
#include "core/ptr.h"
#include "util/variant.h"
#include "baseattributecontroller.h"

namespace Ui
{
	class DecimalNumberController;
}

namespace Game
{
	class Entity;
}

//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
class DecimalNumberController : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
	DecimalNumberController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType);
	/// destructor
	~DecimalNumberController();

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

protected slots:
	/// when the value of the derived controller is changed
	void OnValueChanged();
	void UpdateSlider();
	void OnSliderChanged(int);

private:
	///
	

	Ui::DecimalNumberController* ui;
};
}

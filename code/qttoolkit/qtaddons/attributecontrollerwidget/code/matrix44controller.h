#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::Matrix44Controller
    
    A widget which represents an editable 4x4 matrix.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include <QWidget>
#include "util/string.h"
#include "core/ptr.h"
#include "util/variant.h"
#include "baseattributecontroller.h"

namespace Ui
{
	class Matrix44Controller;
}

namespace Game
{
	class Entity;
}

//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
class Matrix44Controller : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
	Matrix44Controller(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType);
	/// destructor
	~Matrix44Controller();

	/// gets the values from the entity matrix and sets them to the widgets
	void UpdateWidgetValues();
	/// updates widgets with the values of the given matrix
	void UpdateWidgetValues(const Math::matrix44& matrix);

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

protected slots:
	/// when the value of the derived controller is changed
	void OnValueChanged(double){BaseAttributeController::OnValueChanged();}

	/// when pressing the view-matrix button
	void OnButtonClicked();

private:
	Ui::Matrix44Controller* ui;
	void BlockSignals(bool enable);
};

} // namespace QtAttributeControllerAddon

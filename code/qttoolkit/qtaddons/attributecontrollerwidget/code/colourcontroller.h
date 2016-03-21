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
	class ColourController;
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
class ColourController : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
	ColourController(QWidget* parent, const Ptr<Game::Entity>& entity, const Ptr<Tools::IDLAttribute>& _attr);
	/// destructor
	~ColourController();

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

protected slots:
	/// when the value of the derived controller is changed
	void OnValueChanged();
	/// colourButton has been pressed
	void OnSelectColour();

private:
	Ui::ColourController* ui;
	const Ptr<Tools::IDLAttribute> & attr;
	QPixmap colourIcon;
};
}

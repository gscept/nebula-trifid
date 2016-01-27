//------------------------------------------------------------------------------
//  integercontroller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "integercontroller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_integercontroller.h"
#include "basegamefeature/basegameprotocol.h"

using namespace Util;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
IntegerController::IntegerController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType):
	BaseAttributeController(parent, _entity, _attrId, _attrType)
{
	// setup ui
	this->ui = new Ui::IntegerController();
	this->ui->setupUi(this);

	this->ui->spinBox->installEventFilter(this);

	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	_entity->SendSync(msg.cast<Messaging::Message>());

	this->ui->spinBox->setValue(msg->GetAttr().GetInt());

	bool connected = false;
	connected = connect(this->ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged()));
	n_assert(connected);
}

//------------------------------------------------------------------------------
/**
*/
IntegerController::~IntegerController()
{
	this->entity = 0;
}

//------------------------------------------------------------------------------
/**
*/
Util::Variant 
IntegerController::GetValue() const
{
	int value = this->ui->spinBox->value();
	return Util::Variant(value);
}

//------------------------------------------------------------------------------
/**
*/
void 
IntegerController::Lock()
{
	this->ui->spinBox->setEnabled(false);
	this->ui->spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
}

}
//------------------------------------------------------------------------------
//  boolcontroller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "boolcontroller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_boolcontroller.h"
#include "basegamefeature/basegameprotocol.h"

using namespace Util;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
BoolController::BoolController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType):
	BaseAttributeController(parent, _entity, _attrId, _attrType)
{ 
	// setup ui
	this->ui = new Ui::BoolController();
	this->ui->setupUi(this);

	this->ui->checkBox->installEventFilter(this);

	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	_entity->SendSync(msg.cast<Messaging::Message>());
	bool isTrue = msg->GetAttr().GetBool();

	this->ui->checkBox->setChecked(isTrue);

	bool connected = false;
	connected = connect(this->ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(OnValueChanged()));
	n_assert(connected);
}

//------------------------------------------------------------------------------
/**
*/
BoolController::~BoolController()
{
}

//------------------------------------------------------------------------------
/**
*/
Util::Variant 
BoolController::GetValue() const
{
	bool value = this->ui->checkBox->isChecked();
	return Util::Variant(value);
}

//------------------------------------------------------------------------------
/**
*/
void 
BoolController::Lock()
{
	this->ui->checkBox->setEnabled(false);
}

}
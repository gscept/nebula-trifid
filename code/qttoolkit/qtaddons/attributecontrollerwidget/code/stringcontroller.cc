//------------------------------------------------------------------------------
//  stringcontroller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "stringcontroller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_stringcontroller.h"
#include "basegamefeature/basegameprotocol.h"

using namespace Util;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
StringController::StringController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType):
	BaseAttributeController(parent, _entity, _attrId, _attrType)
{
	// setup ui
	this->ui = new Ui::StringController();
	this->ui->setupUi(this);

	this->ui->lineEdit->installEventFilter(this);

	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	_entity->SendSync(msg.cast<Messaging::Message>());

	this->ui->lineEdit->setText(msg->GetAttr().GetString().AsCharPtr());

	bool connected = false;
	connected = connect(this->ui->lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(OnTextChanged(const QString &)));
	n_assert(connected);
}

//------------------------------------------------------------------------------
/**
*/
StringController::~StringController()
{
	this->entity = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
StringController::OnTextChanged(const QString &string)
{
	BaseAttributeController::OnValueChanged();
}

//------------------------------------------------------------------------------
/**
*/
Util::Variant 
StringController::GetValue() const
{
	Util::String value = this->ui->lineEdit->text().toUtf8().constData();
	return Util::Variant(value);
}

//------------------------------------------------------------------------------
/**
*/
void 
StringController::Lock()
{
	this->ui->lineEdit->setEnabled(false);
}

}
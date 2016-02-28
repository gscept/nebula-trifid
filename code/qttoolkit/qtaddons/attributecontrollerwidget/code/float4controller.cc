//------------------------------------------------------------------------------
//  float4controller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "float4controller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_float4controller.h"
#include "basegamefeature/basegameprotocol.h"

using namespace Util;
using namespace Math;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
Float4Controller::Float4Controller(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType):
	BaseAttributeController(parent, _entity, _attrId, _attrType)
{ 
	// setup ui
	this->ui = new Ui::Float4Controller();
	this->ui->setupUi(this);

	this->ui->doubleSpinBox_X->installEventFilter(this);
	this->ui->doubleSpinBox_Y->installEventFilter(this);
	this->ui->doubleSpinBox_Z->installEventFilter(this);
	this->ui->doubleSpinBox_W->installEventFilter(this);


	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	_entity->SendSync(msg.cast<Messaging::Message>());

	float4 vector = msg->GetAttr().GetFloat4();
	this->ui->doubleSpinBox_X->setValue((double)vector.x());
	this->ui->doubleSpinBox_Y->setValue((double)vector.y());
	this->ui->doubleSpinBox_Z->setValue((double)vector.z());
	this->ui->doubleSpinBox_W->setValue((double)vector.w());

	bool connected = true;
	connected &= connect(this->ui->doubleSpinBox_X, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged()));
	connected &= connect(this->ui->doubleSpinBox_Y, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged()));
	connected &= connect(this->ui->doubleSpinBox_W, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged()));
	connected &= connect(this->ui->doubleSpinBox_Z, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged()));
	n_assert(connected);
}

//------------------------------------------------------------------------------
/**
*/
Float4Controller::~Float4Controller()
{
}

//------------------------------------------------------------------------------
/**
*/
Util::Variant 
Float4Controller::GetValue() const
{
	float x = (float)this->ui->doubleSpinBox_X->value();
	float y = (float)this->ui->doubleSpinBox_Y->value();
	float z = (float)this->ui->doubleSpinBox_Z->value();
	float w = (float)this->ui->doubleSpinBox_W->value();

	return Util::Variant(float4(x, y, z, w));
}

//------------------------------------------------------------------------------
/**
*/
void 
Float4Controller::Lock()
{
	this->ui->doubleSpinBox_X->setEnabled(false);
	this->ui->doubleSpinBox_Y->setEnabled(false);
	this->ui->doubleSpinBox_Z->setEnabled(false);
	this->ui->doubleSpinBox_W->setEnabled(false);
	this->ui->doubleSpinBox_X->setButtonSymbols(QAbstractSpinBox::NoButtons);
	this->ui->doubleSpinBox_Y->setButtonSymbols(QAbstractSpinBox::NoButtons);
	this->ui->doubleSpinBox_Z->setButtonSymbols(QAbstractSpinBox::NoButtons);
	this->ui->doubleSpinBox_W->setButtonSymbols(QAbstractSpinBox::NoButtons);
}

}
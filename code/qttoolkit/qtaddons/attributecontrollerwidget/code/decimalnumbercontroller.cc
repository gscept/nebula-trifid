//------------------------------------------------------------------------------
//  decimalnumbercontroller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "decimalnumbercontroller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_decimalnumbercontroller.h"
#include "basegamefeature/basegameprotocol.h"
#include "editorblueprintmanager.h"

using namespace Util;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
DecimalNumberController::DecimalNumberController(QWidget* parent, const Ptr<Game::Entity>& entity, const Attr::AttrId& attrId, Util::Variant::Type attrType):
	BaseAttributeController(parent, entity, attrId, attrType)
{ 
	// setup ui
	this->ui = new Ui::DecimalNumberController();
	this->ui->setupUi(this);

	this->ui->doubleSpinBox->installEventFilter(this);
	this->ui->minSpinBox->installEventFilter(this);
	this->ui->maxSpinBox->installEventFilter(this);
	this->ui->horizontalSlider->installEventFilter(this);


	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	entity->SendSync(msg.cast<Messaging::Message>());
	this->ui->doubleSpinBox->setValue((double)msg->GetAttr().GetFloat());

	connect(this->ui->minSpinBox, SIGNAL(editingFinished()), this, SLOT(UpdateSlider()));
	connect(this->ui->maxSpinBox, SIGNAL(editingFinished()), this, SLOT(UpdateSlider()));
	connect(this->ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(OnSliderChanged(int)));

	this->ui->minSpinBox->setValue((double)msg->GetAttr().GetFloat() - 1.0f);
	this->ui->maxSpinBox->setValue((double)msg->GetAttr().GetFloat() + 1.0f);
	
	Ptr<Toolkit::EditorBlueprintManager> blueprintManager = Toolkit::EditorBlueprintManager::Instance();

	if (blueprintManager->HasAttributeByName(attrId.GetName()))
	{
		Ptr<Tools::IDLAttribute> idl = blueprintManager->GetAttributeByName(attrId.GetName());
		if (idl->HasAttribute("minValue"))
		{
			this->ui->minSpinBox->setValue((double)idl->GetAttribute("minValue").AsFloat());
			this->ui->minSpinBox->setEnabled(false);
			this->ui->doubleSpinBox->setMinimum((double)idl->GetAttribute("minValue").AsFloat());
		}		
		if (idl->HasAttribute("maxValue"))
		{
			this->ui->maxSpinBox->setValue((double)idl->GetAttribute("maxValue").AsFloat());
			this->ui->maxSpinBox->setEnabled(false);
			this->ui->doubleSpinBox->setMaximum((double)idl->GetAttribute("maxValue").AsFloat());
		}		
	}

	this->UpdateSlider();

	bool connected = false;
	connected = connect(this->ui->doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged()));
	n_assert(connected);
}

//------------------------------------------------------------------------------
/**
*/
DecimalNumberController::~DecimalNumberController()
{
}

//------------------------------------------------------------------------------
/**
*/
void
DecimalNumberController::OnValueChanged()
{
	this->UpdateSlider();
	BaseAttributeController::OnValueChanged();
}
//------------------------------------------------------------------------------
/**
*/
void
DecimalNumberController::OnSliderChanged(int value)
{
	float maxVal = this->ui->maxSpinBox->value();
	float minVal = this->ui->minSpinBox->value();
	float val = (float)value / 100.0f;
	this->ui->doubleSpinBox->setValue(minVal + val * (maxVal - minVal));
}


//------------------------------------------------------------------------------
/**
*/
void
DecimalNumberController::UpdateSlider()
{
	float maxVal = this->ui->maxSpinBox->value();
	float minVal = this->ui->minSpinBox->value();
	float val = this->ui->doubleSpinBox->value();
	if (val < minVal)
	{
		this->ui->minSpinBox->blockSignals(true);
		this->ui->minSpinBox->setValue(val);
		this->ui->minSpinBox->blockSignals(false);
	}
	else if (val > maxVal)
	{
		this->ui->maxSpinBox->blockSignals(true);
		this->ui->maxSpinBox->setValue(val);
		this->ui->maxSpinBox->blockSignals(false);
	}
	this->ui->horizontalSlider->blockSignals(true);
	this->ui->horizontalSlider->setValue((int)(100.0f * (val - minVal) / (maxVal - minVal)));
	this->ui->horizontalSlider->blockSignals(false);
}

//------------------------------------------------------------------------------
/**
*/
Util::Variant 
DecimalNumberController::GetValue() const
{
	double value = this->ui->doubleSpinBox->value();
	return Util::Variant((float)value);
}

//------------------------------------------------------------------------------
/**
*/
void 
DecimalNumberController::Lock()
{
	this->ui->doubleSpinBox->setEnabled(false);
	this->ui->horizontalSlider->setEnabled(false);
	this->ui->maxSpinBox->setEnabled(false);
	this->ui->minSpinBox->setEnabled(false);
	this->ui->doubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
}

}
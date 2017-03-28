//------------------------------------------------------------------------------
//  transformcontroller.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "transformcontroller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_transformcontroller.h"
#include "math/transform44.h"
#include "basegamefeature/basegameprotocol.h"

using namespace Util;
using namespace Math;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
    TransformController::TransformController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType):
	BaseAttributeController(parent, _entity, _attrId, _attrType)
{ 
	// setup ui
	this->ui = new Ui::TransformController();
	this->ui->setupUi(this);

	this->ui->doubleSpinBox_posX->installEventFilter(this);
	this->ui->doubleSpinBox_posY->installEventFilter(this);
	this->ui->doubleSpinBox_posZ->installEventFilter(this);
	this->ui->doubleSpinBox_scaX->installEventFilter(this);
	this->ui->doubleSpinBox_scaY->installEventFilter(this);
	this->ui->doubleSpinBox_scaZ->installEventFilter(this);
	this->ui->doubleSpinBox_rotX->installEventFilter(this);
	this->ui->doubleSpinBox_rotY->installEventFilter(this);
	this->ui->doubleSpinBox_rotZ->installEventFilter(this);


	this->ui->matrixViewFrame->hide();

	this->UpdateWidgetValues();

	bool connected = true;
	connected &= connect(this->ui->viewMatrixPushButton, SIGNAL(clicked(bool)), this, SLOT(OnButtonClicked()));
	connected &= connect(this->ui->doubleSpinBox_posX, SIGNAL(valueChanged (double)), this, SLOT(OnValueChanged(double)));
	connected &= connect(this->ui->doubleSpinBox_posY, SIGNAL(valueChanged (double)), this, SLOT(OnValueChanged(double)));
	connected &= connect(this->ui->doubleSpinBox_posZ, SIGNAL(valueChanged (double)), this, SLOT(OnValueChanged(double)));
	connected &= connect(this->ui->doubleSpinBox_scaX, SIGNAL(valueChanged (double)), this, SLOT(OnValueChanged(double)));
	connected &= connect(this->ui->doubleSpinBox_scaY, SIGNAL(valueChanged (double)), this, SLOT(OnValueChanged(double)));
	connected &= connect(this->ui->doubleSpinBox_scaZ, SIGNAL(valueChanged (double)), this, SLOT(OnValueChanged(double)));
	connected &= connect(this->ui->doubleSpinBox_rotX, SIGNAL(valueChanged (double)), this, SLOT(OnValueChanged(double)));
	connected &= connect(this->ui->doubleSpinBox_rotY, SIGNAL(valueChanged (double)), this, SLOT(OnValueChanged(double)));
	connected &= connect(this->ui->doubleSpinBox_rotZ, SIGNAL(valueChanged (double)), this, SLOT(OnValueChanged(double)));
	n_assert(connected);
}

//------------------------------------------------------------------------------
/**
*/
TransformController::~TransformController()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Util::Variant 
TransformController::GetValue() const
{
	double posX = this->ui->doubleSpinBox_posX->value();
	double posY = this->ui->doubleSpinBox_posY->value();
	double posZ = this->ui->doubleSpinBox_posZ->value();
	double scaX = this->ui->doubleSpinBox_scaX->value();
	double scaY = this->ui->doubleSpinBox_scaY->value();
	double scaZ = this->ui->doubleSpinBox_scaZ->value();
	double rotX = n_deg2rad(this->ui->doubleSpinBox_rotX->value());
	double rotY = n_deg2rad(this->ui->doubleSpinBox_rotY->value());
	double rotZ = n_deg2rad(this->ui->doubleSpinBox_rotZ->value());

    transform44 trans;
    trans.setposition(float4(posX, posY, posZ, 0));
    trans.setscale(float4(scaX, scaY, scaZ, 0));
    trans.setrotate(quaternion::rotationyawpitchroll(rotY, rotZ, rotX));
	
	return Util::Variant(trans);
}

//------------------------------------------------------------------------------
/**
*/
void 
TransformController::OnButtonClicked()
{
	// toggle the hidden-flag of the matrix view frame
	bool hidden = this->ui->matrixViewFrame->isHidden();
	this->ui->matrixViewFrame->setHidden(!hidden);
}

//------------------------------------------------------------------------------
/**
*/
void 
TransformController::UpdateWidgetValues()
{

	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	this->entity->SendSync(msg.cast<Messaging::Message>());

	this->t = msg->GetAttr().GetTransform44();
	this->UpdateWidgetValues(this->t);
}

//------------------------------------------------------------------------------
/**
*/
void 
TransformController::UpdateWidgetValues(const Math::transform44& trans)
{
    this->t = trans;

    float4 position = this->t.getposition();
    float4 scale = this->t.getscale();    
    float4 rot;
    quaternion::to_euler(this->t.getrotate(), rot);
	
	this->BlockSignals(true);

	this->ui->doubleSpinBox_posX->setValue((double)position.x());	
	this->ui->doubleSpinBox_posY->setValue((double)position.y());
	this->ui->doubleSpinBox_posZ->setValue((double)position.z());
	this->ui->doubleSpinBox_scaX->setValue((double)scale.x());
	this->ui->doubleSpinBox_scaY->setValue((double)scale.y());
	this->ui->doubleSpinBox_scaZ->setValue((double)scale.z());
	
	this->ui->doubleSpinBox_rotX->setValue(n_rad2deg(rot.x()));
	this->ui->doubleSpinBox_rotY->setValue(n_rad2deg(rot.y()));
	this->ui->doubleSpinBox_rotZ->setValue(n_rad2deg(rot.z()));

	this->BlockSignals(false);

}

//------------------------------------------------------------------------------
/**
*/
void 
TransformController::Lock()
{
	this->ui->doubleSpinBox_posX->setEnabled(false);
	this->ui->doubleSpinBox_posY->setEnabled(false);
	this->ui->doubleSpinBox_posZ->setEnabled(false);
	this->ui->doubleSpinBox_scaX->setEnabled(false);
	this->ui->doubleSpinBox_scaY->setEnabled(false);
	this->ui->doubleSpinBox_scaZ->setEnabled(false);
	this->ui->doubleSpinBox_rotX->setEnabled(false);
	this->ui->doubleSpinBox_rotY->setEnabled(false);
	this->ui->doubleSpinBox_rotZ->setEnabled(false);
	this->ui->doubleSpinBox_posX->setButtonSymbols(QAbstractSpinBox::NoButtons);
	this->ui->doubleSpinBox_posY->setButtonSymbols(QAbstractSpinBox::NoButtons);
	this->ui->doubleSpinBox_posZ->setButtonSymbols(QAbstractSpinBox::NoButtons);
	this->ui->doubleSpinBox_scaX->setButtonSymbols(QAbstractSpinBox::NoButtons);
	this->ui->doubleSpinBox_scaY->setButtonSymbols(QAbstractSpinBox::NoButtons);
	this->ui->doubleSpinBox_scaZ->setButtonSymbols(QAbstractSpinBox::NoButtons);
}

//------------------------------------------------------------------------------
/**
*/
void 
TransformController::BlockSignals(bool enable)
{
	this->ui->doubleSpinBox_posX->blockSignals(enable);
	this->ui->doubleSpinBox_posY->blockSignals(enable);
	this->ui->doubleSpinBox_posZ->blockSignals(enable);
	this->ui->doubleSpinBox_scaX->blockSignals(enable);
	this->ui->doubleSpinBox_scaY->blockSignals(enable);
	this->ui->doubleSpinBox_scaZ->blockSignals(enable);
	this->ui->doubleSpinBox_rotX->blockSignals(enable);
	this->ui->doubleSpinBox_rotY->blockSignals(enable);
	this->ui->doubleSpinBox_rotZ->blockSignals(enable);
}

} // namespace QtAttributeControllerAddon
//------------------------------------------------------------------------------
//  matrix44controller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "matrix44controller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_matrix44controller.h"
#include "math/transform44.h"
#include "basegamefeature/basegameprotocol.h"

using namespace Util;
using namespace Math;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
Matrix44Controller::Matrix44Controller(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType):
	BaseAttributeController(parent, _entity, _attrId, _attrType)
{ 
	// setup ui
	this->ui = new Ui::Matrix44Controller();
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
Matrix44Controller::~Matrix44Controller()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Util::Variant 
Matrix44Controller::GetValue() const
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

	// create a matrix with position at the first row, and scale at the second
	matrix44 matrix;
	matrix.set(float4::zerovector(), float4::zerovector(), float4::zerovector(), float4::zerovector());
	matrix.setrow0(float4(posX, posY, posZ, 0));
	matrix.setrow1(float4(scaX, scaY, scaZ, 0));
	quaternion q = quaternion::rotationyawpitchroll(rotY,rotZ,rotX);
	matrix.setrow2(float4(q.x(),q.y(),q.z(),q.w()));

	return Util::Variant(matrix);
}

//------------------------------------------------------------------------------
/**
*/
void 
Matrix44Controller::OnButtonClicked()
{
	// toggle the hidden-flag of the matrix view frame
	bool hidden = this->ui->matrixViewFrame->isHidden();
	this->ui->matrixViewFrame->setHidden(!hidden);
}

//------------------------------------------------------------------------------
/**
*/
void 
Matrix44Controller::UpdateWidgetValues()
{

	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	this->entity->SendSync(msg.cast<Messaging::Message>());

	matrix44 matrix = msg->GetAttr().GetMatrix44();
	this->UpdateWidgetValues(matrix);
}

//------------------------------------------------------------------------------
/**
*/
void 
Matrix44Controller::UpdateWidgetValues(const Math::matrix44& matrix)
{
	float4 position, scale;
	quaternion q;
	matrix.decompose(scale, q, position);
	
	this->BlockSignals(true);

	this->ui->doubleSpinBox_posX->setValue((double)position.x());	
	this->ui->doubleSpinBox_posY->setValue((double)position.y());
	this->ui->doubleSpinBox_posZ->setValue((double)position.z());
	this->ui->doubleSpinBox_scaX->setValue((double)scale.x());
	this->ui->doubleSpinBox_scaY->setValue((double)scale.y());
	this->ui->doubleSpinBox_scaZ->setValue((double)scale.z());

	float4 row0 = matrix.getrow0();
	float4 row1 = matrix.getrow1();
	float4 row2 = matrix.getrow2();
	float4 row3 = matrix.getrow3();

	this->ui->doubleSpinBox_m0_0->setValue((double)row0.x());
	this->ui->doubleSpinBox_m0_1->setValue((double)row0.y());
	this->ui->doubleSpinBox_m0_2->setValue((double)row0.z());
	this->ui->doubleSpinBox_m0_3->setValue((double)row0.w());

	this->ui->doubleSpinBox_m1_0->setValue((double)row1.x());
	this->ui->doubleSpinBox_m1_1->setValue((double)row1.y());
	this->ui->doubleSpinBox_m1_2->setValue((double)row1.z());
	this->ui->doubleSpinBox_m1_3->setValue((double)row1.w());

	this->ui->doubleSpinBox_m2_0->setValue((double)row2.x());
	this->ui->doubleSpinBox_m2_1->setValue((double)row2.y());
	this->ui->doubleSpinBox_m2_2->setValue((double)row2.z());
	this->ui->doubleSpinBox_m2_3->setValue((double)row2.w());

	this->ui->doubleSpinBox_m3_0->setValue((double)row3.x());
	this->ui->doubleSpinBox_m3_1->setValue((double)row3.y());
	this->ui->doubleSpinBox_m3_2->setValue((double)row3.z());
	this->ui->doubleSpinBox_m3_3->setValue((double)row3.w());

	float q3 = q.x();
	float q2 = q.y();
	float q1 = q.z();
	float q0 = q.w();

	float rx = atan2(2.0f*(q0*q1+q2*q3),1.0f - 2.0f * (q1*q1 + q2*q2));
	float ry = asin(2.0f*(q0*q2 -q3*q1));
	float rz = atan2(2.0f*(q0*q3 + q1*q2),1.0f - 2.0f * (q2*q2 + q3*q3));
	this->ui->doubleSpinBox_rotX->setValue(n_rad2deg(rx));
	this->ui->doubleSpinBox_rotY->setValue(n_rad2deg(ry));
	this->ui->doubleSpinBox_rotZ->setValue(n_rad2deg(rz));

	this->BlockSignals(false);

}

//------------------------------------------------------------------------------
/**
*/
void 
Matrix44Controller::Lock()
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
Matrix44Controller::BlockSignals(bool enable)
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
//------------------------------------------------------------------------------
//  colourcontroller.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "colourcontroller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_colourcontroller.h"
#include "basegamefeature/basegameprotocol.h"
#include "idldocument/idlattribute.h"
#include "qcolordialog.h"

using namespace Util;
using namespace Math;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
ColourController::ColourController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Ptr<Tools::IDLAttribute>& _attr) :
	BaseAttributeController(parent),
	attr(_attr),
	colourIcon(16,16)
{ 
	// setup ui
	this->ui = new Ui::ColourController();
	this->ui->setupUi(this);
	this->entity = _entity;

	this->ui->rBox->installEventFilter(this);
	this->ui->gBox->installEventFilter(this);
	this->ui->bBox->installEventFilter(this);
	this->ui->aBox->installEventFilter(this);


	this->attributeId = Attr::AttrId(_attr->GetName());
	this->type = Util::Variant::Float4;

	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	_entity->SendSync(msg.cast<Messaging::Message>());

	float4 vector = msg->GetAttr().GetFloat4();
	this->ui->rBox->setValue((double)vector.x());
	this->ui->gBox->setValue((double)vector.y());
	this->ui->bBox->setValue((double)vector.z());
	this->ui->aBox->setValue((double)vector.w());

	QColor qcolor(vector.x() * 255, vector.y() * 255, vector.z() * 255, vector.w() * 255);	

#ifndef USE_COLOUR_ICON
	this->ui->colourButton->setPalette(QPalette(qcolor));
	this->ui->colourButton->update();
#else
	this->colourIcon.fill(qcolor);
	this->ui->colourButton->setIcon(QIcon(this->colourIcon));
#endif

	bool connected = true;
	connected &= connect(this->ui->rBox, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged()));
	connected &= connect(this->ui->gBox, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged()));
	connected &= connect(this->ui->bBox, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged()));
	connected &= connect(this->ui->aBox, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged()));
	connected &= connect(this->ui->colourButton, SIGNAL(clicked(bool)), this, SLOT(OnSelectColour()));
	n_assert(connected);
}

//------------------------------------------------------------------------------
/**
*/
ColourController::~ColourController()
{
}


//------------------------------------------------------------------------------
/**
*/
void
ColourController::OnSelectColour()
{
	int r = (int)(255.0f * (float)this->ui->rBox->value());
	int g = (int)(255.0f * (float)this->ui->gBox->value());
	int b = (int)(255.0f * (float)this->ui->bBox->value());
	int a = (int)(255.0f * (float)this->ui->aBox->value());

	QColor qcolor(r, g, b, a);

	// open color dialog
	QColorDialog dialog(qcolor);
	dialog.setOption(QColorDialog::ShowAlphaChannel, true);

	int val = dialog.exec();
	if (val == QDialog::Accepted)
	{
		QColor diaColor = dialog.currentColor();
		float r = diaColor.red() / 255.0f;
		float g = diaColor.green() / 255.0f;
		float b = diaColor.blue() / 255.0f;
		float a = diaColor.alpha() / 255.0f;

		this->ui->rBox->blockSignals(true);
		this->ui->rBox->setValue((double)r);
		this->ui->rBox->blockSignals(false);

		this->ui->gBox->blockSignals(true);
		this->ui->gBox->setValue((double)g);
		this->ui->gBox->blockSignals(false);

		this->ui->bBox->blockSignals(true);
		this->ui->bBox->setValue((double)b);
		this->ui->bBox->blockSignals(false);

		this->ui->aBox->blockSignals(true);
		this->ui->aBox->setValue((double)a);
		this->ui->aBox->blockSignals(false);
	}
	this->OnValueChanged();
}


//------------------------------------------------------------------------------
/**
*/
void 
ColourController::OnValueChanged()
{
	int r = (int)(255.0f * (float)this->ui->rBox->value());
	int g = (int)(255.0f * (float)this->ui->gBox->value());
	int b = (int)(255.0f * (float)this->ui->bBox->value());
	int a = (int)(255.0f * (float)this->ui->aBox->value());

	QColor qcolor(r, g, b, a);

#ifndef USE_COLOUR_ICON

	// this will change the whole button colour
	this->ui->colourButton->setPalette(QPalette(qcolor));
	this->ui->colourButton->update();
#else
	this->colourIcon.fill(qcolor);
	this->ui->colourButton->setIcon(QIcon(this->colourIcon));
#endif
	BaseAttributeController::OnValueChanged();
}


//------------------------------------------------------------------------------
/**
*/
Util::Variant 
ColourController::GetValue() const
{
	float x = (float)this->ui->rBox->value();
	float y = (float)this->ui->gBox->value();
	float z = (float)this->ui->bBox->value();
	float w = (float)this->ui->aBox->value();

	return Util::Variant(float4(x, y, z, w));
}

//------------------------------------------------------------------------------
/**
*/
void 
ColourController::Lock()
{
	this->ui->rBox->setEnabled(false);
	this->ui->gBox->setEnabled(false);
	this->ui->bBox->setEnabled(false);
	this->ui->aBox->setEnabled(false);
}

}
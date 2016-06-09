//------------------------------------------------------------------------------
//  physicsmaterialcontroller.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsmaterialcontroller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_stringlistcontroller.h"
#include "basegamefeature/basegameprotocol.h"
#include "idldocument/idlattribute.h"
#include "physics/materialtable.h"

using namespace Util;
using namespace Attr;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
PhysicsMaterialController::PhysicsMaterialController(QWidget* parent, const Ptr<Game::Entity>& entity, const Ptr<Tools::IDLAttribute>& idlattr) :BaseAttributeController(parent)
{
	this->entity = entity;	
	this->attributeId = AttrId(idlattr->GetName());
	this->type = Util::Variant::String;

	// setup ui
	this->ui = new Ui::StringListController();
	this->ui->setupUi(this);

	this->ui->comboBox->installEventFilter(this);


	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	__SendSync(entity, msg);

	this->currentValue = msg->GetAttr().GetString();

    int selection = 0;

    this->ui->comboBox->clear();
    Util::Array<Util::StringAtom> materials = Physics::MaterialTable::GetMaterials();
    for (IndexT i = 0; i < materials.Size(); i++)
    {
        this->ui->comboBox->addItem(materials[i].AsString().AsCharPtr());
        if (materials[i] == currentValue)
        {
            selection = i;
        }
    }
    this->ui->comboBox->setCurrentIndex(selection);
    bool connected = false;
    connected = connect(this->ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    n_assert(connected);
}

//------------------------------------------------------------------------------
/**
*/
PhysicsMaterialController::~PhysicsMaterialController()
{
	this->entity = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysicsMaterialController::currentIndexChanged(int)
{
	BaseAttributeController::OnValueChanged(); 
}
//------------------------------------------------------------------------------
/**
*/
Util::Variant 
PhysicsMaterialController::GetValue() const
{
	Util::String value = this->ui->comboBox->currentText().toUtf8().constData();
	return Util::Variant(value);
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysicsMaterialController::Lock()
{
	this->ui->comboBox->setEnabled(false);	
}

}
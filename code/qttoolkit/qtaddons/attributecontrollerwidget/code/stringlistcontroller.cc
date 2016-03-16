//------------------------------------------------------------------------------
//  stringlistcontroller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "stringlistcontroller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_stringlistcontroller.h"
#include "basegamefeature/basegameprotocol.h"
#include "idldocument/idlattribute.h"

using namespace Util;
using namespace Attr;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
StringListController::StringListController(QWidget* _parent, const Ptr<Game::Entity>& _entity, const Ptr<Tools::IDLAttribute>& _attr):BaseAttributeController(_parent)
{
	this->entity = _entity;	
	this->attributeId = AttrId(_attr->GetName());
	this->type = Util::Variant::String;

	// setup ui
	this->ui = new Ui::StringListController();
	this->ui->setupUi(this);

	this->ui->comboBox->installEventFilter(this);

	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	_entity->SendSync(msg.cast<Messaging::Message>());

	String itemString = _attr->GetAttributes()["listValues"];
	Array<String> items = itemString.Tokenize(";");

	for(IndexT i = 0 ; i < items.Size();  i++)
	{
		this->ui->comboBox->addItem(items[i].AsCharPtr());
	}

	int itemId = this->ui->comboBox->findText(msg->GetAttr().GetString().AsCharPtr());
	
	n_assert2(itemId != -1, "Wrong value for attribute");

	this->ui->comboBox->setCurrentIndex(itemId);

	bool connected = false;
	connected = connect(this->ui->comboBox, SIGNAL(currentIndexChanged (int)), this, SLOT(currentIndexChanged(int)));
	n_assert(connected);
}

//------------------------------------------------------------------------------
/**
*/
StringListController::~StringListController()
{
	this->entity = 0;
}

//------------------------------------------------------------------------------
/**
*/
Util::Variant 
StringListController::GetValue() const
{
	Util::String value = this->ui->comboBox->currentText().toUtf8().constData();
	return Util::Variant(value);
}

//------------------------------------------------------------------------------
/**
*/
void 
StringListController::Lock()
{
	this->ui->comboBox->setEnabled(false);	
}

}
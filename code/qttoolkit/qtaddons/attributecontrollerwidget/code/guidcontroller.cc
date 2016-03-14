//------------------------------------------------------------------------------
//  guidcontroller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "guidcontroller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_guidcontroller.h"
#include "basegamefeature/basegameprotocol.h"

using namespace Util;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
GuidController::GuidController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType):
	BaseAttributeController(parent, _entity, _attrId, _attrType)
{ 
	// setup ui
	this->ui = new Ui::GuidController();
	this->ui->setupUi(this);

	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	_entity->SendSync(msg.cast<Messaging::Message>());

	Util::Guid guid = msg->GetAttr().GetGuid();
	this->ui->label->setText(guid.AsString().AsCharPtr());

	// no signal here, for the moment not intented to be editable
}

//------------------------------------------------------------------------------
/**
*/
GuidController::~GuidController()
{
}

//------------------------------------------------------------------------------
/**
*/
Util::Variant 
GuidController::GetValue() const
{
	Util::String text = this->ui->label->text().toUtf8().constData();
	Util::Guid guid = Util::Guid::FromString(text);
	return Util::Variant(guid);
}

//------------------------------------------------------------------------------
/**
*/
void 
GuidController::Lock()
{
	// currently guids are shown via a label and are by default not editable
}

}
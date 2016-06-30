//------------------------------------------------------------------------------
//  attributecontrollerwidget.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attributecontrollerwidget.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_attributecontrollerwidget.h"
#include "integercontroller.h"
#include "decimalnumbercontroller.h"
#include "stringcontroller.h"
#include "float4controller.h"
#include "boolcontroller.h"
#include "guidcontroller.h"
#include "matrix44controller.h"
#include "idldocument/idlattribute.h"
#include "stringlistcontroller.h"
#include "resourcestringcontroller.h"
#include "colourcontroller.h"
#include "animclipcontroller.h"
#include "editorfeatures/editorblueprintmanager.h"
#include "physicsmaterialcontroller.h"
#include "transformcontroller.h"

using namespace Util;
using namespace Attr;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
AttributeControllerWidget::~AttributeControllerWidget()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AttributeControllerWidget::AttributeControllerWidget(const Ptr<Game::Entity>& entity, const Attr::AttrId& attrId, Util::Variant::Type attrType) :
	baseAttributeController(0)
{
	// setup ui
	this->ui = new Ui::AttributeControllerWidget();
	this->ui->setupUi(this);

	// set the labels
	this->ui->nameLabel->setText(attrId.GetName().AsCharPtr());
	//this->ui->typeLabel->setText(Variant::TypeToString(attrType).AsCharPtr());

	Ptr<Toolkit::EditorBlueprintManager> blueprintManager = Toolkit::EditorBlueprintManager::Instance();

	if (blueprintManager->HasAttributeByName(attrId.GetName()))
	{
		Ptr<Tools::IDLAttribute> idl = blueprintManager->GetAttributeByName(attrId.GetName());
		if (idl->HasDisplayName())
		{
			this->ui->nameLabel->setText(idl->GetDisplayName().AsCharPtr());			
			this->ui->nameLabel->setToolTip(attrId.GetName().AsCharPtr());
		}				
		if (idl->GetAttributes().Contains("editType"))
		{
			this->InitInputWidget(entity, idl);
			return;
		}
	}
	
	// fallback to initialize the type-specific controllers
	this->InitInputWidget(entity, attrId, attrType);
}

//------------------------------------------------------------------------------
/**
*/
void
AttributeControllerWidget::InitInputWidget(const Ptr<Game::Entity>& entity, const Ptr<Tools::IDLAttribute>& idl)
{
	n_assert(entity.isvalid());
	BaseAttributeController* controller = 0;

	Util::String editType = idl->GetAttributes()["editType"];
	if (editType == "list")
	{
		controller = new StringListController(this, entity, idl);
	}
	else if (editType == "resource")
	{
		controller = new ResourceStringController(this, entity, idl);
	}
	else if (editType == "colour")
	{
		controller = new ColourController(this, entity, idl);
	}
	else if (editType == "animclip")
	{
		controller = new AnimClipController(this, entity, idl);
	}
    else if (editType == "physicmaterial")
    {
        controller = new PhysicsMaterialController(this, entity, idl);
    }
	this->ui->verticalLayout->addWidget(controller);
	n_assert(0 == this->baseAttributeController);
	this->baseAttributeController = controller;
}


//------------------------------------------------------------------------------
/**
*/
void 
AttributeControllerWidget::InitInputWidget(const Ptr<Game::Entity>& _entity, const Attr::AttrId& _attrId, Util::Variant::Type _attrType)
{
	n_assert(_entity.isvalid());

	

	BaseAttributeController* controller = 0;

	switch(_attrType)
	{
	case IntType:
		controller = new IntegerController(this, _entity, _attrId, _attrType);
		break;
	case FloatType:
		controller = new DecimalNumberController(this, _entity, _attrId, _attrType);
		break;
	case BoolType:
		controller = new BoolController(this, _entity, _attrId, _attrType);
		break;
	case Float4Type:
		controller = new Float4Controller(this, _entity, _attrId, _attrType);
		break;
	case StringType:
		controller = new StringController(this, _entity, _attrId, _attrType);
		break;
	case Matrix44Type:
		controller = new Matrix44Controller(this, _entity, _attrId, _attrType);
		break;
    case Transform44Type:
        controller = new TransformController(this, _entity, _attrId, _attrType);
        break;
	case GuidType:
		controller = new GuidController(this, _entity, _attrId, _attrType);
		break;
	default:
		n_error("AttributeControllerWidget::InitInputWidget: An attribute controller widget is not implemented for type '%s'!", Variant::TypeToString(_attrType).AsCharPtr());
	}

	n_assert(0 != controller);
	this->ui->verticalLayout->addWidget(controller);

	n_assert(0 == this->baseAttributeController);
	this->baseAttributeController = controller;	
}

//------------------------------------------------------------------------------
/**
*/
void 
AttributeControllerWidget::LockWidgets()
{
	n_assert(0 != this->baseAttributeController);
	
	this->baseAttributeController->Lock();
}

}
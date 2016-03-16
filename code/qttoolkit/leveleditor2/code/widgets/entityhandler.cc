//------------------------------------------------------------------------------
//  entityhandler.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "leveleditor2app.h"
#include <QListWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPlastiqueStyle>
#include <QDir>

#include "blueprinthandler.h"
#include "editorfeatures/editorblueprintmanager.h"
#include "leveleditor2entitymanager.h"

using namespace Ui;
using namespace BaseGameFeature;
using namespace Toolkit;

namespace LevelEditor2
{

//------------------------------------------------------------------------------
/**
*/
EntityHandler::EntityHandler(QWidget* parent) : 
    QDialog(parent)
{
	this->ui.setupUi(this);
	this->selectedProperty = "";
	

	connect(this->ui.attachButton, SIGNAL(pressed()),this, SLOT(Attach()));
	connect(this->ui.detachButton, SIGNAL(pressed()),this, SLOT(Detach()));
	connect(this->ui.buttonBox, SIGNAL(accepted()),this, SLOT(SaveEntity()));
    connect(this->ui.upButton, SIGNAL(pressed()), this, SLOT(Up()));
    connect(this->ui.downButton, SIGNAL(pressed()), this, SLOT(Down()));
}


//------------------------------------------------------------------------------
/**
*/
EntityHandler::~EntityHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
EntityHandler::SetupDialog()
{
	this->ui.entityNameTextbox->clear();
	this->ui.attachedPropertiesList->clear();
	this->ui.availablePropertiesList->clear();

	QStringList usedProps;
	QStringList otherProps;

	if (this->selectedProperty != "") // editing an existing entity
	{

		this->ui.entityNameTextbox->setReadOnly(true);
		EditorBlueprintManager::BluePrint bp = EditorBlueprintManager::Instance()->GetCategory(this->selectedProperty.toAscii().constData());
		for(IndexT i = 0; i<bp.properties.Size();i++)
		{
			usedProps.append(bp.properties[i]->GetName().AsCharPtr());
		}
	}
	else
	{
		this->ui.entityNameTextbox->setReadOnly(false);
	}

	Util::Array<Ptr<Tools::IDLProperty>> allprops = EditorBlueprintManager::Instance()->GetAllProperties();	
	for(IndexT i = 0;i<allprops.Size();i++)
	{
		if(!usedProps.contains(allprops[i]->GetName().AsCharPtr()))
		{
			otherProps.append(allprops[i]->GetName().AsCharPtr());
		}
	}
		
	this->ui.entityNameTextbox->setText(this->selectedProperty);

	this->ui.attachedPropertiesList->addItems(usedProps);
	this->ui.availablePropertiesList->addItems(otherProps);		
	this->Attach();
}

//------------------------------------------------------------------------------
/**
*/
void
EntityHandler::SetSelectedEntity(QString propertyName)
{
	this->selectedProperty = propertyName;
}

//------------------------------------------------------------------------------
/**
*/
void
EntityHandler::Attach()
{
	QList<int> rows;
    QList<QListWidgetItem*> items = this->ui.availablePropertiesList->selectedItems();

	for (int i = 0; i < items.size(); i++)
	{
		rows.append(this->ui.availablePropertiesList->row(this->ui.availablePropertiesList->selectedItems().at(i)));
	}

	for (int i = 0; i < rows.size(); i++)
	{
		QListWidgetItem* item = this->ui.availablePropertiesList->takeItem(rows.at(i) - i); // subtract i to compensate for the items moving up in the list
		this->ui.attachedPropertiesList->addItem(item);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
EntityHandler::Detach()
{
	QList<int> rows;
    QList<QListWidgetItem*> items = this->ui.attachedPropertiesList->selectedItems();

	for (int i = 0; i < items.size(); i++)
	{
		rows.append(this->ui.attachedPropertiesList->row(this->ui.attachedPropertiesList->selectedItems().at(i)));
	}

	for (int i = 0; i < rows.size(); i++)
	{
		QListWidgetItem* item = this->ui.attachedPropertiesList->takeItem(rows.at(i) - i); // subtract i to compensate for the items moving up in the list
		this->ui.availablePropertiesList->addItem(item);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityHandler::Up()
{
    QList<QListWidgetItem*> items = this->ui.attachedPropertiesList->selectedItems();

    for (int i = 0; i < items.size(); i++)
    {
        int index = this->ui.attachedPropertiesList->row(items[i]);
        QListWidgetItem* item = this->ui.attachedPropertiesList->takeItem(index);
        this->ui.attachedPropertiesList->insertItem(Math::n_max(index-1, 0), item);
        this->ui.attachedPropertiesList->setItemSelected(item, true);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityHandler::Down()
{
    QList<QListWidgetItem*> items = this->ui.attachedPropertiesList->selectedItems();

    for (int i = 0; i < items.size(); i++)
    {
        int index = this->ui.attachedPropertiesList->row(items[i]);
        QListWidgetItem* item = this->ui.attachedPropertiesList->takeItem(index);
        this->ui.attachedPropertiesList->insertItem(Math::n_min(index+1, this->ui.attachedPropertiesList->count()), item);
        this->ui.attachedPropertiesList->setItemSelected(item, true);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EntityHandler::SaveEntity()
{
	if(this->ui.entityNameTextbox->text() == "")
	{
		QMessageBox::warning((QWidget*)parent(), "Warning!", "Category name can't be empty.", QMessageBox::Ok);
		return;
	}

	Util::String category = this->ui.entityNameTextbox->text().toAscii().constData();
	Util::Array<Util::String> props;
		 
	for(int i=0;i<this->ui.attachedPropertiesList->count();i++)
	{
		props.Append(this->ui.attachedPropertiesList->item(i)->text().toAscii().constData());
	}
	EditorBlueprintManager::Instance()->SetCategory(category,props);
	EditorBlueprintManager::Instance()->SaveBlueprint("proj:data/tables/blueprints.xml");

	LevelEditor2EntityManager::Instance()->UpdateCategoryEntities(category);
	this->hide();
	emit(BlueprintsChanged());
}

}
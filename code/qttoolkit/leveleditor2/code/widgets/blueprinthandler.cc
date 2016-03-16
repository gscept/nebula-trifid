//------------------------------------------------------------------------------
//  blueprinthandler.cc
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
#include "properties/editorproperty.h"

using namespace Ui;
using namespace BaseGameFeature;
using namespace Toolkit;

namespace LevelEditor2
{

//------------------------------------------------------------------------------
/**
*/
BlueprintsHandler::BlueprintsHandler(QWidget* i_pParent) : QDialog(i_pParent)
{
	m_pEntityDialog = new EntityHandler(this);

	m_uiBlueprintsUI.setupUi(this);

	connect(m_uiBlueprintsUI.newEntityButton,SIGNAL(pressed()),this,SLOT(NewEntity()));
	connect(m_uiBlueprintsUI.editEntityButton,SIGNAL(pressed()),this,SLOT(EditEntity()));
	connect(m_uiBlueprintsUI.deleteEntityButton,SIGNAL(pressed()),this,SLOT(DeleteEntity()));
	connect(m_pEntityDialog,SIGNAL(BlueprintsChanged()),this,SLOT(BlueprintsChanged()));
	connect(m_uiBlueprintsUI.duplicateBlueprintButton, SIGNAL(pressed()), this, SLOT(DuplicateBlueprint()));
}

//------------------------------------------------------------------------------
/**
*/
void
BlueprintsHandler::SetupDialog()
{
	m_uiBlueprintsUI.entityList->clear();


	QStringList catList;

	IndexT i;
	for(i=0;i<EditorBlueprintManager::Instance()->GetNumCategories();i++)
	{
		catList.append(EditorBlueprintManager::Instance()->GetCategoryByIndex(i).AsCharPtr());
	}
	m_uiBlueprintsUI.entityList->addItems(catList);
}

//------------------------------------------------------------------------------
/**
*/
void
BlueprintsHandler::BlueprintsChanged()
{
	SetupDialog();
}

//------------------------------------------------------------------------------
/**
*/
void
BlueprintsHandler::NewEntity()
{
	m_pEntityDialog->SetSelectedEntity("");
	m_pEntityDialog->SetupDialog();
	m_pEntityDialog->show();
	m_pEntityDialog->raise();
	
}

//------------------------------------------------------------------------------
/**
*/
void
BlueprintsHandler::EditEntity()
{
	if(m_uiBlueprintsUI.entityList->selectedItems().size() == 0) // make sure an entity is selected
		return;	
	m_pEntityDialog->SetSelectedEntity(m_uiBlueprintsUI.entityList->currentItem()->text());
	m_pEntityDialog->SetupDialog();
	m_pEntityDialog->show();
	m_pEntityDialog->raise();
	
}

//------------------------------------------------------------------------------
/**
*/
void
BlueprintsHandler::DeleteEntity()
{
	if(m_uiBlueprintsUI.entityList->selectedItems().size() == 0) // make sure an entity is selected
		return;
	
	// FIXMEFIXMEFIXME this will only check the current level, better than nothing i guess
	Util::String cat = m_uiBlueprintsUI.entityList->currentItem()->text().toAscii().constData();
	Util::Array<Attr::Attribute> matchAttrs;
	matchAttrs.Append(Attr::Attribute(Attr::EntityType,Game));
	matchAttrs.Append(Attr::Attribute(Attr::EntityCategory,cat));

	Util::Array<Ptr<Game::Entity> > entities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttrs(matchAttrs);
	if(entities.Size()>0)
	{
		QMessageBox::warning((QWidget*)parent(), "Error", "You have entities of the selected category, you cannot remove it", QMessageBox::Ok);
		return;
	}
	EditorBlueprintManager::Instance()->RemoveCategory(cat);
	EditorBlueprintManager::Instance()->SaveBlueprint("proj:data/tables/blueprints.xml");
	SetupDialog();
		
}

//------------------------------------------------------------------------------
/**
*/
void
BlueprintsHandler::DuplicateBlueprint()
{
	if(m_uiBlueprintsUI.entityList->selectedItems().size() == 0) // make sure an entity is selected
		return;

	int numBlueprints = m_uiBlueprintsUI.entityList->count();

	/// Get new name
	QListWidgetItem* entry = m_uiBlueprintsUI.entityList->selectedItems().at(0);
	Util::String categoryName(entry->text().toAscii());
	Util::String newCategoryName(entry->text().toAscii());
	newCategoryName.Append("2");

	// Make sure a duplicate doesn't already exist
	for (int i = 0; i < numBlueprints; ++i)
	{
		QListWidgetItem* blueprint = m_uiBlueprintsUI.entityList->item(i);
		Util::String text(blueprint->text().toAscii());
		// Return if it already exists.
		if (newCategoryName == text)
		{
			return;
		}
	}

	Util::Array<Util::String> newProperties;
	/// Selected blueprint to copy from
	EditorBlueprintManager::BluePrint bp = EditorBlueprintManager::Instance()->GetCategory(categoryName);
	for(IndexT i = 0; i<bp.properties.Size();i++)
	{
		newProperties.Append(bp.properties[i]->GetName().AsCharPtr());
	}

	/// Add le categoroi
	EditorBlueprintManager::Instance()->SetCategory(newCategoryName, newProperties);
	EditorBlueprintManager::Instance()->SaveBlueprint("proj:data/tables/blueprints.xml");

	// Update stuff
	SetupDialog();

	// Set the new one as selected
	m_pEntityDialog->SetSelectedEntity(QString(newCategoryName.AsCharPtr()));
}

}
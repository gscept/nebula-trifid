//------------------------------------------------------------------------------
//  createentitydialoghandler.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "createentitydialoghandler.h"
#include "leveleditor2app.h"
#include "leveleditor2entitymanager.h"
#include "managers/categorymanager.h"
#include "actions/actionmanager.h"
#include "style/nebulastyletool.h"
#include <QListWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPlastiqueStyle>
#include <QDir>

using namespace Ui;
using namespace BaseGameFeature;

namespace LevelEditor2
{

//------------------------------------------------------------------------------
/**
*/
CreateEntityDialogHandler::CreateEntityDialogHandler(QWidget *parent /*= 0*/) :
	entityType(InvalidType),
	QDialog(parent)
{
	this->ui.setupUi(this);
	// note: palette is inherited

	bool result = true;
	result &= connect(this->ui.categoryListWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(CategoryChosen(QListWidgetItem *)));
	result &= connect(this->ui.templateListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(ImportChosenEntity()));
	result &= connect(this->ui.categoryListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(ImportChosenEntity()));
	result &= connect(this->ui.importPushButton, SIGNAL(clicked()),this,SLOT(ImportChosenEntity()));
	result &= connect(this->ui.cancelPushButton, SIGNAL(clicked()),this,SLOT(close()));
	result &= connect(this->ui.placementCheckBox, SIGNAL(stateChanged(int)),this,SLOT(PlacementChanged(int)));

	// add convenience actions, to select the category or template list when pressing left/right key
	QAction* actionRightKey = new QAction(this->ui.categoryListWidget);
	QAction* actionLeftKey = new QAction(this->ui.templateListWidget);
	actionRightKey->setShortcut(tr("Right"));
	actionLeftKey->setShortcut(tr("Left"));
	result &= connect(actionRightKey, SIGNAL(triggered()), this, SLOT(RightKeyPressed()));
	result &= connect(actionLeftKey, SIGNAL(triggered()), this, SLOT(LeftKeyPressed()));
	n_assert(result);

	this->ui.categoryListWidget->addAction(actionRightKey);	
	this->ui.templateListWidget->addAction(actionLeftKey);		
}

//------------------------------------------------------------------------------
/**
*/
CreateEntityDialogHandler::~CreateEntityDialogHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
CreateEntityDialogHandler::CategoryChosen(QListWidgetItem * item)
{
	if (0 == item //< this is triggered when the category listwidget is cleared
		|| Light == this->entityType)
	{
		return;
	}

	this->ui.templateListWidget->clear();

	QStringList entityStringList;

	if (Environment == this->entityType)
	{
		QDir directory;
		IO::URI model("mdl:");
		model.AppendLocalPath(item->text().toUtf8().constData());
		directory.setPath(model.GetHostAndLocalPath().AsCharPtr());
		QStringList nameFilters;
		nameFilters << "*.n3";

		entityStringList = directory.entryList(nameFilters , QDir::Files);
		entityStringList.replaceInStrings(".n3", "");
	}
	else if (Game == this->entityType)
	{
		Ptr<Toolkit::EditorBlueprintManager> blueprintManager = Toolkit::EditorBlueprintManager::Instance();

		Util::String categoryName = item->text().toUtf8().constData();
		const Util::Array<Toolkit::EditorBlueprintManager::TemplateEntry> & category = blueprintManager->GetTemplates(categoryName);

		entityStringList.append(" -empty entity-");
 		
		IndexT t;
		for (t = 0; t < category.Size(); t++)
		{				
			entityStringList.append(category[t].id.AsCharPtr());
		}				
	}
	else
	{
		n_error("CreateEntityDialogHandler::CategoryChosen: Category type %d not implemented!", this->entityType);
	}

	this->ui.templateListWidget->insertItems(0, entityStringList);
	this->ui.templateListWidget->setCurrentRow(0);
}

//------------------------------------------------------------------------------
/**
*/
void 
CreateEntityDialogHandler::ImportChosenEntity()
{
	n_assert(InvalidType != this->entityType);

	// make sure there is a valid selection
	if((0 == this->ui.templateListWidget->currentItem() && (Environment == this->entityType || Game == this->entityType))
		|| 0 == this->ui.categoryListWidget->currentItem())
	{
		//QMessageBox::warning((QWidget*)parent(), "Selection error", "You must have a valid selection.", QMessageBox::Ok);
		return;
	}

	// import the entity
	Util::String category = this->ui.categoryListWidget->currentItem()->text().toUtf8().constData();

    if (this->entityType == Light)
    {
        Util::String errorMessage;
        EntityGuid entityId;
        ActionManager::Instance()->CreateEntity(this->entityType, category, category, entityId, errorMessage);
    }
    else
    {
        QList<QListWidgetItem*> items = this->ui.templateListWidget->selectedItems();

        if (items.size())
        {
            Util::Array<EntityGuid> ids;

            for (int i = 0; i < items.size(); i++)
            {
                Util::String item = items[i]->text().toUtf8().constData();

                Util::String errorMessage;
                EntityGuid entityId;
                if (!ActionManager::Instance()->CreateEntity(this->entityType, category, item, entityId, errorMessage))
                {
                    QMessageBox::warning((QWidget*)parent(), "Could not create entity", errorMessage.AsCharPtr(), QMessageBox::Ok);
                    continue;
                }
                else
                {
                    ids.Append(entityId);
                }
            }
            if (!ids.IsEmpty())
            {
                LevelEditor2App::Instance()->GetCurrentStateHandler().cast<LevelEditor2::LevelEditorState>()->UpdateSelection(ids);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
CreateEntityDialogHandler::NewEnvironmentEntity()
{
	this->ui.placementCheckBox->setChecked(BaseGameFeature::LoaderServer::Instance()->GetUserProfile()->GetBool("CreateCameraRelative"));

	this->entityType = Environment;

	this->ui.templateListWidget->clear();
	this->ui.categoryListWidget->clear();

	QStringList categoryList;

	QDir directory;
	IO::URI model("mdl:");
	directory.setPath(model.GetHostAndLocalPath().AsCharPtr());
	if (!directory.exists())
	{
		QMessageBox::warning((QWidget*)parent(), "Directory doesn't exist!", "Make sure you have exported your resources, and have set the correct working directory!", QMessageBox::Ok);
		return;
	}

	categoryList = directory.entryList(QDir::Dirs);
	categoryList.removeFirst();
	categoryList.removeFirst();	// first elements are "." and ".."

	this->ui.categoryListWidget->insertItems(0, categoryList);

	this->show();
	this->raise();
}

//------------------------------------------------------------------------------
/**
*/
void 
CreateEntityDialogHandler::NewLightEntity()
{
	this->ui.placementCheckBox->setChecked(BaseGameFeature::LoaderServer::Instance()->GetUserProfile()->GetBool("CreateCameraRelative"));
	this->entityType = Light;

	this->ui.templateListWidget->clear();
	this->ui.categoryListWidget->clear();

	QStringList categoryList;
	categoryList << "PointLight" << "SpotLight";

	this->ui.categoryListWidget->insertItems(0, categoryList);

	this->show();
	this->raise();
}

//------------------------------------------------------------------------------
/**
*/
void 
CreateEntityDialogHandler::NewGameEntity()
{
	this->ui.placementCheckBox->setChecked(BaseGameFeature::LoaderServer::Instance()->GetUserProfile()->GetBool("CreateCameraRelative"));
	this->entityType = Game;

	this->ui.templateListWidget->clear();
	this->ui.categoryListWidget->clear();

	Ptr<Toolkit::EditorBlueprintManager> blueprintManager = Toolkit::EditorBlueprintManager::Instance();
	SizeT numCategories = blueprintManager->GetNumCategories();

	QStringList categoryList;
	IndexT i;
	for (i = 0; i < numCategories ; i++)
	{		
		categoryList.append(blueprintManager->GetCategoryByIndex(i).AsCharPtr());
	}

	this->ui.categoryListWidget->insertItems(0, categoryList);

	this->show();
	this->raise();
}

//------------------------------------------------------------------------------
/**
*/
void 
CreateEntityDialogHandler::PlacementChanged(int)
{
	BaseGameFeature::LoaderServer::Instance()->GetUserProfile()->SetBool("CreateCameraRelative",this->ui.placementCheckBox->isChecked());
}

//------------------------------------------------------------------------------
/**
*/
void 
CreateEntityDialogHandler::RightKeyPressed()
{
	if (Light != this->entityType)
	{
		this->ui.templateListWidget->setFocus();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
CreateEntityDialogHandler::LeftKeyPressed()
{
	this->ui.categoryListWidget->setFocus();
}

} // namespace LevelEditor2
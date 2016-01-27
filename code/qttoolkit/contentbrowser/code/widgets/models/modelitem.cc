//------------------------------------------------------------------------------
//  modelitem.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelitem.h"
#include "modelimporter/modelimporterwindow.h"
#include "contentbrowserapp.h"
#include <QtGui/QMessageBox>

using namespace Util;
namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
ModelItem::ModelItem() : 
	ui(0)
{
	this->type = ModelItemType;
}

//------------------------------------------------------------------------------
/**
*/
ModelItem::~ModelItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ModelItem::OnActivated()
{
	// activate base item
	BaseItem::OnActivated();

	// cast base handler to model handler
	Ptr<ModelHandler> handler = this->itemHandler.downcast<ModelHandler>();

	// set ui for item handler
	handler->SetUI(this->ui);

	// split name into category and file
	QStringList resourceParts = this->name.split("/");

	// get file name and category
	String file = resourceParts[1].toLatin1().constData();
	String category = resourceParts[0].toLatin1().constData();
	file.StripFileExtension();

	// sets the name of the model label
	this->ui->modelName->setText(file.AsCharPtr());

	// set resource
	handler->SetModelResource(file);
	handler->SetModelCategory(category);

	// preview
	if (handler->Preview())
	{
		// setup widget only if load was successful
		handler->Setup();
	}	
	else
	{
		handler->Mute();
		String message;
		message.Format("Model '%s/%s' failed to load. Possible causes: \n    No .attributes exists for this model.\n    Model is character, but has no animations.", category.AsCharPtr(), file.AsCharPtr());
		QMessageBox::warning(NULL, "Failed to load resource", message.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ModelItem::OnClicked()
{
	BaseItem::OnClicked();
}

//------------------------------------------------------------------------------
/**
*/
bool 
ModelItem::OnDeactivated()
{
	return this->itemHandler->Discard();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelItem::OnRightClicked(const QPoint& pos)
{
	// create menu
	QMenu menu(this->treeWidget());

	// get window
	ContentBrowser::ContentBrowserWindow* window = ContentBrowser::ContentBrowserApp::Instance()->GetWindow();

	// create actions
	QAction* action1 = new QAction("Delete model", NULL);
	QAction* action2 = new QAction("Delete model and associated resources", NULL);
	QAction* action3 = new QAction("Reconfigure model", NULL);

	// setup menu
	menu.addAction(action3);
	menu.addSeparator();
	menu.addAction(action1);
	menu.addAction(action2);

	// execute menu
	QAction* action = menu.exec(this->treeWidget()->mapToGlobal(pos));

	// get file
	String file = this->text(0).toUtf8().constData();
	file.StripFileExtension();

	// get category
	String category = this->parent()->text(0).toUtf8().constData();

	// get pointer to io server
	const Ptr<IO::IoServer>& ioServer = IO::IoServer::Instance();

	if (action == action1)
	{
		String resource;

		// delete model
		resource.Format("mdl:%s/%s.n3", category.AsCharPtr(), file.AsCharPtr());
		ioServer->DeleteFile(resource);

		// delete model data
		resource.Format("src:assets/%s/%s.constants", category.AsCharPtr(), file.AsCharPtr());
		if (ioServer->FileExists(resource)) ioServer->DeleteFile(resource);
		resource.Format("src:assets/%s/%s.attributes", category.AsCharPtr(), file.AsCharPtr());
		if (ioServer->FileExists(resource)) ioServer->DeleteFile(resource);
		resource.Format("src:assets/%s/%s.physics", category.AsCharPtr(), file.AsCharPtr());
		if (ioServer->FileExists(resource)) ioServer->DeleteFile(resource);

		// remove item
		window->RemoveModelItem(this);
	}
	else if (action == action2)
	{
		// create message box to potentially stop a wrongful deletion
		QString title;
		title.sprintf("Are you sure you want to delete model %s/%s.n3?", category.AsCharPtr(), file.AsCharPtr());
		QString message;
		message.sprintf("The files: \n\n"
					   "Animation: %s/%s.nax3\n"
					   "Mesh: %s/%s.nvx2\n"
					   "Intermediate: %s/%s.fbx\n"
					   "Model attributes: %s/%s.attributes\n\n"
					   "Will be deleted too!", 
					   category.AsCharPtr(), file.AsCharPtr(),
					   category.AsCharPtr(), file.AsCharPtr(),
					   category.AsCharPtr(), file.AsCharPtr(),
					   category.AsCharPtr(), file.AsCharPtr());
		QMessageBox::StandardButton response = QMessageBox::question(NULL, title, message, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if (response == QMessageBox::Ok)
		{
			// create new string to format resources, start with model
			String resource;

			// delete model
			resource.Format("mdl:%s/%s.n3", category.AsCharPtr(), file.AsCharPtr());
			ioServer->DeleteFile(resource);

			// delete animation
			resource.Format("ani:%s/%s.nax3", category.AsCharPtr(), file.AsCharPtr());
			if (ioServer->FileExists(resource)) ioServer->DeleteFile(resource);

			// delete mesh
			resource.Format("msh:%s/%s.nvx2", category.AsCharPtr(), file.AsCharPtr());
			if (ioServer->FileExists(resource)) ioServer->DeleteFile(resource);

			// delete model data
			resource.Format("src:assets/%s/%s.constants", category.AsCharPtr(), file.AsCharPtr());
			if (ioServer->FileExists(resource)) ioServer->DeleteFile(resource);
			resource.Format("src:assets/%s/%s.attributes", category.AsCharPtr(), file.AsCharPtr());
			if (ioServer->FileExists(resource)) ioServer->DeleteFile(resource);
			resource.Format("src:assets/%s/%s.physics", category.AsCharPtr(), file.AsCharPtr());
			if (ioServer->FileExists(resource)) ioServer->DeleteFile(resource);

			// also delete FBX
			resource.Format("src:assets/%s/%s.fbx", category.AsCharPtr(), file.AsCharPtr());
			if (ioServer->FileExists(resource)) ioServer->DeleteFile(resource);

			// remove item
			window->RemoveModelItemAndAssociated(this);
		}
	}
	else if (action == action3)
	{
		String resource;
		resource.Format("src:assets/%s/%s.fbx", category.AsCharPtr(), file.AsCharPtr());

		if (IO::IoServer::Instance()->FileExists(resource))
		{
			ModelImporter::ModelImporterWindow* importer = ContentBrowser::ContentBrowserApp::Instance()->GetWindow()->GetModelImporter();
			importer->SetUri(resource);
			importer->show();
			importer->raise();
			QApplication::processEvents();
			importer->Open();
		}
		else
		{
			QString message;
			message.sprintf("Could not locate resource '%s'", resource.AsCharPtr());
			QMessageBox::warning(NULL, "Could not locate resource intermediate!", message, QMessageBox::Ok);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
BaseItem* 
ModelItem::Clone()
{
	ModelItem* item = new ModelItem;
	item->SetName(this->name);
	item->SetWidget(this->frame);
	item->SetHandler(this->itemHandler);
	item->SetUi(this->ui);
	return item;
}

} // namespace Widgets
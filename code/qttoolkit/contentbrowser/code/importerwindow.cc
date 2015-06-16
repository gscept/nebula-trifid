//------------------------------------------------------------------------------
//  importerwindow.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QInputDialog>
#include "importerwindow.h"
#include "io/ioserver.h"
#include "util/array.h"

using namespace IO;
using namespace Util;
namespace ContentBrowser
{

//------------------------------------------------------------------------------
/**
*/
ImporterWindow::ImporterWindow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ImporterWindow::~ImporterWindow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ImporterWindow::OnNewCategory()
{
	QString category = QInputDialog::getText(this, "Create new category", "Category", QLineEdit::Normal);

	// just quit if we don't have any text
	if (category.isEmpty())
	{
		return;
	}

	String dir;
	dir.Format("src:%s/%s", this->resourceFolder.AsCharPtr(), category.toUtf8().constData());
	if (!IoServer::Instance()->DirectoryExists(dir))
	{
		IoServer::Instance()->CreateDirectory(dir);
	}

	// reinitialize ui
	this->SetupCategories(category);

	// set current category
	this->dialogUi.categoryBox->setCurrentIndex(this->dialogUi.categoryBox->findText(category));
}

//------------------------------------------------------------------------------
/**
*/
void
ImporterWindow::SetupCategories(const QString& category)
{
	// clear category box
	this->dialogUi.categoryBox->clear();

	// format string for resource folder
	String folder;
	folder.Format("src:%s", this->resourceFolder.AsCharPtr());

	// find all categories
	Array<String> directories = IoServer::Instance()->ListDirectories(folder, "*");

	// go through directories and add to category box
	IndexT i;
	for (i = 0; i < directories.Size(); i++)
	{
		this->dialogUi.categoryBox->addItem(directories[i].AsCharPtr());
	}

	// set active index
	this->dialogUi.categoryBox->setCurrentIndex(0);
}
} // namespace ContentBrowser
//------------------------------------------------------------------------------
//  textureitem.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "textureitem.h"
#include "contentbrowserapp.h"
#include <QImage>
#include <QMessageBox>

using namespace Util;
using namespace QtTools;
namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
TextureItem::TextureItem() : 
	ui(0)
{
	this->type = TextureItemType;
}

//------------------------------------------------------------------------------
/**
*/
TextureItem::~TextureItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
TextureItem::OnActivated()
{
	// activate base item
	BaseItem::OnActivated();

	// cast base handler to model handler
	Ptr<TextureHandler> handler = this->itemHandler.downcast<TextureHandler>();

	// set ui of item handler
	handler->SetUI(this->ui);

	// resolve texture path
	QString file = this->text(0);
	QTreeWidgetItem* parentItem = this->parent();
	QString category = parentItem->text(0);

	handler->SetTextureResource(file.toUtf8().constData());
	handler->SetTextureCategory(category.toUtf8().constData());

	// tell handler to load texture
	handler->LoadTexture();

	// setup item handler
	handler->Setup();
}

//------------------------------------------------------------------------------
/**
*/
void
TextureItem::OnClicked()
{
	BaseItem::OnClicked();
}

//------------------------------------------------------------------------------
/**
*/
void 
TextureItem::OnRightClicked( const QPoint& pos )
{
	// create menu
	QMenu menu(this->treeWidget());

	// create actions
	QAction* action1 = new QAction("Delete texture", NULL);
	QAction* action2 = new QAction("Reconfigure texture", NULL);

	// get window
	ContentBrowser::ContentBrowserWindow* window = ContentBrowser::ContentBrowserApp::Instance()->GetWindow();

	// setup menu
	menu.addAction(action2);
	menu.addSeparator();
	menu.addAction(action1);

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
		resource.Format("tex:%s/%s.dds", category.AsCharPtr(), file.AsCharPtr());

		// delete model
		ioServer->DeleteFile(resource);

        String extensions[] = {".png", ".bmp", ".psd", ".tga", ".jpg", ".dds"};
        IndexT i;
        for (i = 0; i < 6; i++)
        {
            // attempt to remove all files related to this texture
            resource.Format("src:assets/%s%s", file.AsCharPtr(), extensions[i].AsCharPtr());
            ioServer->DeleteFile(resource);
        }

		window->RemoveTextureItem(this);
	}
	else if (action == action2)
	{
		String resource;
		resource.Format("src:assets/%s", category.AsCharPtr());

		String fileName;
		fileName.Format("%s.*", file.AsCharPtr());

		Util::Array<String> files = ioServer->ListFiles(resource, fileName);

        // go through and remove all non-texture files
        IndexT i;
        for (i = 0; i < files.Size(); i++)
        {
            if (!(String::MatchPattern(files[i], "*.png") ||
                String::MatchPattern(files[i], "*.bmp") ||
                String::MatchPattern(files[i], "*.psd") ||
                String::MatchPattern(files[i], "*.tga") ||
                String::MatchPattern(files[i], "*.jpg") ||
                String::MatchPattern(files[i], "*.dds")))
            {
                files.EraseIndex(i--);
            }
        }

		if (files.Size() > 1)
		{
			QComboBox selections;
			IndexT i;
			for (i = 0; i < files.Size(); i++)
			{
				selections.addItem(files[i].AsCharPtr());
			}

			QMessageBox* box = new QMessageBox();
			box->setText("The texture: " + QString(file.AsCharPtr()) + " has more than one potential sources. Which one would you want to reconfigure?");
			box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			box->setDefaultButton(QMessageBox::Ok);
			box->layout()->addWidget(&selections);
			int result = box->exec();

			if (result == QMessageBox::Ok)
			{
				resource.Format("src:assets/%s/%s", category.AsCharPtr(), selections.itemText(selections.currentIndex()).toUtf8().constData());
				TextureImporter::TextureImporterWindow* importer = ContentBrowser::ContentBrowserApp::Instance()->GetWindow()->GetTextureImporter();
				importer->SetUri(resource);
				importer->show();
				importer->raise();
				QApplication::processEvents();
				importer->Open();
			}
			else
			{
				return;
			}
		}
		else
		{
			resource.Format("src:assets/%s/%s", category.AsCharPtr(), files[0].AsCharPtr());
			TextureImporter::TextureImporterWindow* importer = ContentBrowser::ContentBrowserApp::Instance()->GetWindow()->GetTextureImporter();
			importer->SetUri(resource);
			importer->show();
			importer->raise();
			QApplication::processEvents();
			importer->Open();
		}

		// use first occurence of file name to use as reconfigure target
	}
}

//------------------------------------------------------------------------------
/**
*/
BaseItem* 
TextureItem::Clone()
{
	TextureItem* item = new TextureItem;
	item->SetName(this->GetName());
	item->SetWidget(this->GetWidget());
	item->SetHandler(this->GetHandler());
	item->SetUi(this->GetUi());
	return item;
}


} // namespace Widgets
//------------------------------------------------------------------------------
//  tiledtextureitem.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tiledtextureitem.h"
#include "io/uri.h"
#include "io/ioserver.h"
#include "textureloaderthread.h"
#include "texturebrowser.h"
#include <QAction>
#include <QMenu>
#include <QComboBox>
#include <QMessageBox>
#include <QGraphicsSceneEvent>
#include <QLayout>
#include <QGraphicsScene>
#include "tiledgraphicsview.h"

using namespace Util;
namespace ResourceBrowser
{

//------------------------------------------------------------------------------
/**
*/
TiledTextureItem::TiledTextureItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
TiledTextureItem::~TiledTextureItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
TiledTextureItem::Setup()
{
	// setup base class
	TiledGraphicsItem::Setup();

	// reduce to category/file name
	this->texture = this->texture.ExtractLastDirName() + "/" + this->texture.ExtractFileName();
	Util::String temp = this->texture;
	temp = temp.ExtractFileName();
	temp.StripFileExtension();

	// get dds!
	this->texture.ChangeAssignPrefix("tex");
	this->texture.ChangeFileExtension("dds");

	// create a new texture unit
	this->loader = new TextureLoaderUnit;
	this->loader->path = this->texture;
	this->loader->texture = new QImage;
	connect(this->loader, SIGNAL(OnLoaded()), this, SLOT(OnPreviewLoaded()));
	TextureBrowser::loaderThread->Enqueue(this->loader);

	// format string with the 'clean' name
	QString format;
	format.sprintf("<p align=\"center\"><b>Texture</b><br>%s</p>", temp.AsCharPtr());
	this->label->setTextWidth(this->background->boundingRect().width());
	this->label->setHtml(format);
}

//------------------------------------------------------------------------------
/**
*/
void
TiledTextureItem::Discard()
{
	TiledGraphicsItem::Discard();

	// make sure our thread is using the mutex, then delete the loader unit
	this->loader->mutex.lock();
	delete this->loader->texture;
	delete this->loader;
}

//------------------------------------------------------------------------------
/**
*/
void
TiledTextureItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit this->OnSelected(QString(this->texture.AsCharPtr()));
	}
}

//------------------------------------------------------------------------------
/**
	Aww, we want to be able to reconfigure stuff, but this code is in the content browser...
*/
void
TiledTextureItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu menu;
	QAction* action1 = menu.addAction("Delete texture");
	QAction* action2 = menu.addAction("Reconfigure texture");

	// convert resource to an assign and extension free version
	String prefixLessResource = this->texture;
	prefixLessResource.StripAssignPrefix();
	prefixLessResource.StripFileExtension();

	// get category and file
	String category = this->texture.ExtractLastDirName();
	String file = this->texture.ExtractFileName();
	file.StripFileExtension();

	// setup menu
	//menu.addAction(action2);
	action2->setEnabled(false);
	//menu.addSeparator();
	menu.addAction(action1);

	// execute menu
	QAction* executedAction = menu.exec(event->screenPos());

	// get pointer to io server
	const Ptr<IO::IoServer>& ioServer = IO::IoServer::Instance();

	// delete texture
	if (executedAction == action1)
	{
		// delete texture
		ioServer->DeleteFile(this->texture);

		String extensions[] = { ".png", ".bmp", ".psd", ".tga", ".jpg", ".dds" };
		IndexT i;
		for (i = 0; i < 6; i++)
		{
			// attempt to remove all files related to this texture
			String resource;
			resource.Format("src:textures/%s", prefixLessResource.AsCharPtr());
			ioServer->DeleteFile(resource);
		}

		// erase item
		((TiledGraphicsView*)this->scene()->views()[0])->RemoveTiledItem(this);
	}
	else if (executedAction == action2)
	{
		// get resource without prefix and suffix
		String workTex = "src:textures/" + category;

		// make a string to match the pattern
		String pattern;
		pattern.Format("%s.*", file.AsCharPtr());

		// get all files based on this name
		Array<String> files = ioServer->ListFiles(workTex, pattern);

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

		// if we have more than one file, the user must select one!
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
				String resource;
				resource.Format("src:textures/%s/%s", category.AsCharPtr(), selections.itemText(selections.currentIndex()).toUtf8().constData());

				// we should move the texture importer to QtAddons since it might be used everywhere
				/*
				TextureImporter::TextureImporterWindow* importer = ContentBrowser::ContentBrowserApp::Instance()->GetWindow()->GetTextureImporter();
				importer->SetUri(resource);
				importer->show();
				importer->raise();
				QApplication::processEvents();
				importer->Open();
				*/
			}
			else
			{
				return;
			}
		}
		else if (files.Size() > 0)
		{
			String resource;
			resource.Format("src:textures/%s/%s", category.AsCharPtr(), files[0].AsCharPtr());

			/*
			TextureImporter::TextureImporterWindow* importer = ContentBrowser::ContentBrowserApp::Instance()->GetWindow()->GetTextureImporter();
			importer->SetUri(resource);
			importer->show();
			importer->raise();
			QApplication::processEvents();
			importer->Open();
			*/
		}
		else
		{
			QMessageBox::warning(NULL, "No work resource exists!", "Could not locate this texture in the work directory, has it been moved or deleted?", QMessageBox::Ok);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TiledTextureItem::OnPreviewLoaded()
{
	// set position of graphics
	this->graphics->setPixmap(QPixmap::fromImage(*this->loader->texture));
	this->graphics->setPos(
		this->background->boundingRect().width() / 2 - this->graphics->boundingRect().width() / 2,
		this->background->boundingRect().width() / 2 - this->graphics->boundingRect().height() / 2);

	// move label too
	this->label->setPos(this->label->pos().x(), this->graphics->boundingRect().height() + 20);
}

} // namespace ResourceBrowser
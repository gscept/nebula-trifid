//------------------------------------------------------------------------------
//  texturebrowser.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "assetbrowser.h"
#include "io/uri.h"
#include "io/ioserver.h"
#include "ui_assetbrowser.h"
#include "tileddirectoryitem.h"
#include "tiledtextureitem.h"
#include "tiledmodelitem.h"
#include "tiledsurfaceitem.h"

#include <QMenu>
using namespace Util;
using namespace IO;
namespace ResourceBrowser
{

ImageLoaderThread* AssetBrowser::loaderThread;
__ImplementClass(ResourceBrowser::AssetBrowser, 'TEBR', Core::RefCounted);
__ImplementSingleton(ResourceBrowser::AssetBrowser);

//------------------------------------------------------------------------------
/**
*/
AssetBrowser::AssetBrowser() :
	isExecuting(false),
	filter(All)
{
	this->ui = new Ui::AssetBrowser;
	this->ui->setupUi(this);
	connect(this->ui->backButton, SIGNAL(pressed()), this, SLOT(OnBack()));
	this->shortcut = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(this->shortcut, SIGNAL(activated()), this->ui->backButton, SLOT(animateClick()));

    connect(this->ui->texturesFilter, SIGNAL(toggled(bool)), this, SLOT(OnTexturesFilterChecked(bool)));
    connect(this->ui->modelsFilter, SIGNAL(toggled(bool)), this, SLOT(OnModelsFilterChecked(bool)));
    connect(this->ui->surfacesFilter, SIGNAL(toggled(bool)), this, SLOT(OnSurfacesFilterChecked(bool)));

	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
AssetBrowser::~AssetBrowser()
{
	delete this->ui;
	delete this->shortcut;
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
int
AssetBrowser::Execute(const QString& title, const AssetFilter& filter)
{
	this->setWindowTitle("Texture browser - " + title);
	AssetBrowser::loaderThread->Pause(false);
	this->SetFilter(filter);
	this->selectedResource = "";
	this->isExecuting = true;
	this->raise();
	return this->exec();
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::Open()
{
	// create thread
	AssetBrowser::loaderThread = new ImageLoaderThread(NULL);
	AssetBrowser::loaderThread->Start();
	this->SetupRoot();
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::Close()
{
	AssetBrowser::loaderThread->Stop();
	delete AssetBrowser::loaderThread;
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::showEvent(QShowEvent* event)
{
	QDialog::showEvent(event);
	AssetBrowser::loaderThread->Pause(false);
	this->ui->assetView->Rearrange();
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::closeEvent(QCloseEvent* event)
{
	AssetBrowser::loaderThread->Pause(true);
	this->isExecuting = false;
	QDialog::closeEvent(event);
	this->filter = All;
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::contextMenuEvent(QContextMenuEvent* event)
{
	emit this->ContextMenuOpened(event);
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnDirectoryClicked(const QString& dir, const QString& path)
{
	// clear first
	this->ui->assetView->Clear();
	this->ui->categoryLabel->setText(dir);

	// find all accepted file types, note that we are looking in work, not export!
	Ptr<IoServer> ioServer = IoServer::Instance();
    String basePath = String(path.toUtf8().constData());
    String category = String(dir.toUtf8().constData());
    String assetPath = basePath + "/" + category;
	Array<String> files;

	files = ioServer->ListFiles(assetPath, "*.tga");
	files.AppendArray(ioServer->ListFiles(assetPath, "*.bmp"));
	files.AppendArray(ioServer->ListFiles(assetPath, "*.dds"));
	files.AppendArray(ioServer->ListFiles(assetPath, "*.psd"));
	files.AppendArray(ioServer->ListFiles(assetPath, "*.png"));
	files.AppendArray(ioServer->ListFiles(assetPath, "*.jpg"));

	IndexT i;
	for (i = 0; i < files.Size(); i++)
	{
		const String& file = files[i];
		String textureFile = assetPath + "/" + file;

		// create new texture item
		TiledTextureItem* item = new TiledTextureItem;
		item->SetPath(basePath);
		item->SetCategory(category);
		item->SetFilename(file);

		// add to ui
        item->setVisible(this->filter & Textures);
		this->ui->assetView->AddTiledItem(item);

		// connect with browser to handle directory navigation
		connect(item, SIGNAL(OnSelected(const QString&)), this, SLOT(OnTextureClicked(const QString&)));
		connect(item, SIGNAL(ItemRightClicked(QGraphicsSceneContextMenuEvent*)), this, SLOT(OnItemRightClicked(QGraphicsSceneContextMenuEvent*)));
	}

	files = ioServer->ListFiles(assetPath, "*.fbx");
	for (i = 0; i < files.Size(); i++)
	{
		const String& file = files[i];

		// create new texture item
		TiledModelItem* item = new TiledModelItem;
		item->SetPath(basePath);
		item->SetCategory(category);
		item->SetFilename(file);

		// add to ui
        item->setVisible(this->filter & Models);
		this->ui->assetView->AddTiledItem(item);

		// connect with browser to handle directory navigation
		connect(item, SIGNAL(OnSelected(const QString&)), this, SLOT(OnModelClicked(const QString&)));
		connect(item, SIGNAL(ItemRightClicked(QGraphicsSceneContextMenuEvent*)), this, SLOT(OnItemRightClicked(QGraphicsSceneContextMenuEvent*)));
	}

	files = ioServer->ListFiles(assetPath, "*.sur");
	for (i = 0; i < files.Size(); i++)
	{
		const String& file = files[i];

		// create new texture item
		TiledSurfaceItem* item = new TiledSurfaceItem;
		item->SetPath(basePath);
		item->SetCategory(category);
		item->SetFilename(file);

		// add to ui
        item->setVisible(this->filter & Surfaces);
		this->ui->assetView->AddTiledItem(item);

		// connect with browser to handle directory navigation
        connect(item, SIGNAL(OnSelected(const QString&)), this, SLOT(OnSurfaceClicked(const QString&)));
		connect(item, SIGNAL(ItemRightClicked(QGraphicsSceneContextMenuEvent*)), this, SLOT(OnItemRightClicked(QGraphicsSceneContextMenuEvent*)));
	}


	// rearrange browser window
	this->ui->assetView->Rearrange();
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnTextureClicked(const QString& tex)
{
	if (this->isExecuting)
	{
		AssetBrowser::loaderThread->Pause(true);
		this->selectedResource = tex;
		this->accept();
		this->isExecuting = false;
	}	
    else
    {
        emit this->TextureSelected(tex);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnModelClicked(const QString& mdl)
{
	if (this->isExecuting)
	{
		AssetBrowser::loaderThread->Pause(true);
		this->selectedResource = mdl;
		this->accept();
		this->isExecuting = false;
	}
    else
    {
        emit this->ModelSelected(mdl);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnSurfaceClicked(const QString& sur)
{
	if (this->isExecuting)
	{
		AssetBrowser::loaderThread->Pause(true);
		this->selectedResource = sur;
		this->accept();
		this->isExecuting = false;
	}
    else
    {
        emit this->SurfaceSelected(sur);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnBack()
{
	AssetBrowser::loaderThread->Clear();
	this->SetupRoot();
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnItemRightClicked(QGraphicsSceneContextMenuEvent* event)
{
	TiledGraphicsItem* item = (TiledGraphicsItem*)this->sender();
	emit this->ItemContextMenuOpened(item, event);
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnTexturesFilterChecked(bool b)
{
    this->SetFilter(AssetFilter(b ? this->filter | Textures : this->filter &~ Textures));
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnModelsFilterChecked(bool b)
{
    this->SetFilter(AssetFilter(b ? this->filter | Models : this->filter &~ Models));
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnSurfacesFilterChecked(bool b)
{
    this->SetFilter(AssetFilter(b ? this->filter | Surfaces : this->filter &~ Surfaces));
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::SetupRoot()
{
	// clear first
	this->ui->assetView->Clear();
	this->ui->categoryLabel->setText("Root");

	Ptr<IoServer> ioServer = IoServer::Instance();
	String assetPath("src:assets");

	Array<String> dirs = ioServer->ListDirectories(assetPath, "*");
	IndexT i;
	for (i = 0; i < dirs.Size(); i++)
	{
		const String& dir = dirs[i];
		String textureDir = assetPath + "/" + dir;

		// create new texture dir
		TiledDirectoryItem* item = new TiledDirectoryItem;
		item->SetDirectory(dir);
        item->SetPath(assetPath);

		// add to ui
		this->ui->assetView->AddTiledItem(item);

		// connect with browser to handle directory navigation
        connect(item, SIGNAL(OnSelected(const QString&, const QString&)), this, SLOT(OnDirectoryClicked(const QString&, const QString&)));
	}

    // now do the same for system
    assetPath = "toolkit:work/assets";
    dirs = ioServer->ListDirectories(assetPath, "*");
    for (i = 0; i < dirs.Size(); i++)
    {
        const String& dir = dirs[i];
        String textureDir = assetPath + "/" + dir;

        // create new texture dir
        TiledDirectoryItem* item = new TiledDirectoryItem;
        item->SetDirectory(dir);
        item->SetPath(assetPath);

        // add to ui
		this->ui->assetView->AddTiledItem(item);

        // connect with browser to handle directory navigation
        connect(item, SIGNAL(OnSelected(const QString&, const QString&)), this, SLOT(OnDirectoryClicked(const QString&, const QString&)));
    }

	// rearrange browser window
	this->ui->assetView->Rearrange();
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::SetFilter(const AssetFilter& filter)
{
	// if the filter doesn't match, set this as our new filter and reset the browser
	if (this->filter != filter)
	{
        const QList<TiledGraphicsItem*>& items = this->ui->assetView->GetItems();
        IndexT i;
        for (i = 0; i < items.size(); i++)
        {
            TiledGraphicsItem* item = items[i];
            if (dynamic_cast<TiledTextureItem*>(item))
            {
                item->setVisible(filter & Textures);
            }
            else if (dynamic_cast<TiledModelItem*>(item))
            {
                item->setVisible(filter & Models);
            }
            else if (dynamic_cast<TiledSurfaceItem*>(item))
            {
                item->setVisible(filter & Surfaces);
            }
        }
        this->ui->assetView->Rearrange();
		this->filter = filter;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::RemoveItem(TiledGraphicsItem* item)
{
	this->ui->assetView->RemoveTiledItem(item);
}


} // namespace ResourceBrowser
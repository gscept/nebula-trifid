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

	// watch the assets folder
	IO::URI dir("src:assets");
	this->assetFolderWatcher.WatchDirectory(dir.LocalPath());
	connect(&this->assetFolderWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(UpdateAssetFolders()));
	this->UpdateAssetFolders();

	// connect clicking the root folder widget
	connect(this->ui->rootFolderWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnAssetFolderClicked(QListWidgetItem*)));

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
	this->setWindowTitle("Asset browser - " + title);

	// pause loader
	AssetBrowser::loaderThread->Pause(false);

	// save the filter being used, then apply the executable filter
	AssetFilter savedFilter = this->filter;
	this->SetFilter(filter);
	this->selectedResource = "";

	// disable any filters when executing
	this->ui->texturesFilter->setEnabled(false);
	this->ui->modelsFilter->setEnabled(false);
	this->ui->surfacesFilter->setEnabled(false);

	// make sure the asset browser is being executed and not opened 'normally'
	this->isExecuting = true;

	// show window
	int result = this->Execute();

	// enable filter buttons again
	this->ui->texturesFilter->setEnabled(true);
	this->ui->modelsFilter->setEnabled(true);
	this->ui->surfacesFilter->setEnabled(true);

	// reset state and return result
	this->SetFilter(savedFilter);
	this->isExecuting = false;
	return result;
}

//------------------------------------------------------------------------------
/**
*/
int
AssetBrowser::Execute()
{
	this->shouldStopExecuting = false;
	this->executeResult = -1;
	this->raise();
	this->show();
	while (!this->shouldStopExecuting)
	{
		QApplication::processEvents();
	}
	return executeResult;
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::Accept()
{
	this->executeResult = QDialog::Accepted;
	this->shouldStopExecuting = true;
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::Reject()
{
	this->executeResult = QDialog::Rejected;
	this->shouldStopExecuting = true;
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
	this->Reject();
	AssetBrowser::loaderThread->Stop();
	delete AssetBrowser::loaderThread;
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::SetWindowModal(QWidget* widget)
{
	// set window to be modal
	this->setParent(widget);
	this->setWindowFlags(Qt::Tool);
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::showEvent(QShowEvent* event)
{
	QDockWidget::showEvent(event);
	AssetBrowser::loaderThread->Pause(false);
	this->ui->assetView->Rearrange();
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::closeEvent(QCloseEvent* event)
{
	this->Reject();
	AssetBrowser::loaderThread->Pause(true);
	QDockWidget::closeEvent(event);
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

	files = ioServer->ListFiles(assetPath, "*.attributes");
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
		this->Accept();
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
		this->Accept();
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
		this->Accept();
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

	// load project assets
	String assetPath("src:assets");
	Array<String> dirs = ioServer->ListDirectories(assetPath, "*");
	IndexT i;
	for (i = 0; i < dirs.Size(); i++)
	{
		const String& dir = dirs[i];
		String assetDir = assetPath + "/" + dir;

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
        String assetDir = assetPath + "/" + dir;

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
		// disable signals so that we may check/uncheck the filter boxes if we modify the filter from outside the browser
		this->ui->texturesFilter->blockSignals(true);
		this->ui->modelsFilter->blockSignals(true);
		this->ui->surfacesFilter->blockSignals(true);

		// make sure the checkboxes gets modified
		this->ui->texturesFilter->setChecked(filter & Textures);
		this->ui->modelsFilter->setChecked(filter & Models);
		this->ui->surfacesFilter->setChecked(filter & Surfaces);

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

		// remember to unblock the signals again
		this->ui->texturesFilter->blockSignals(false);
		this->ui->modelsFilter->blockSignals(false);
		this->ui->surfacesFilter->blockSignals(false);
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

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::UpdateAssetFolders()
{
	// clear items first
	this->ui->rootFolderWidget->clear();

	Ptr<IoServer> ioServer = IoServer::Instance();
	QPixmap pix(":/icons/qtaddons/resourcebrowser/ui/folder_512.png");
	String assetPath("src:assets");
	Array<String> dirs = ioServer->ListDirectories(assetPath, "*");
	IndexT i;
	for (i = 0; i < dirs.Size(); i++)
	{
		const String& dir = dirs[i];
		String assetDir = assetPath + "/" + dir;
		QListWidgetItem* item = new QListWidgetItem(QIcon(pix), dir.AsCharPtr());
		this->ui->rootFolderWidget->addItem(item);
		item->setData(Qt::UserRole, QVariant("src:assets"));
	}

	assetPath = "toolkit:work/assets";
	dirs = ioServer->ListDirectories(assetPath, "*");
	for (i = 0; i < dirs.Size(); i++)
	{
		const String& dir = dirs[i];
		String assetDir = assetPath + "/" + dir;
		QListWidgetItem* item = new QListWidgetItem(QIcon(pix), dir.AsCharPtr());
		this->ui->rootFolderWidget->addItem(item);
		item->setData(Qt::UserRole, QVariant("toolkit:work/assets"));
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnAssetFolderClicked(QListWidgetItem* item)
{
	this->OnDirectoryClicked(item->text(), item->data(Qt::UserRole).toString());
}

} // namespace ResourceBrowser
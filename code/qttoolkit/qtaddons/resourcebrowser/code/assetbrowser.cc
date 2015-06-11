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
	isExecuting(false)
{
	this->ui = new Ui::AssetBrowser;
	this->ui->setupUi(this);
	connect(this->ui->backButton, SIGNAL(pressed()), this, SLOT(OnBack()));
	this->shortcut = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(this->shortcut, SIGNAL(activated()), this->ui->backButton, SLOT(animateClick()));
	//connect(this->ui->actionBack, SIGNAL(triggered()), this, SLOT(OnBack()));
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
AssetBrowser::Execute(const QString& title)
{
	this->setWindowTitle("Texture browser - " + title);
	AssetBrowser::loaderThread->Pause(false);
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
	this->ui->texturePreview->Rearrange();
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
}

//------------------------------------------------------------------------------
/**
*/
void
AssetBrowser::OnDirectoryClicked(const QString& dir)
{
	// clear first
	this->ui->texturePreview->Clear();
	this->ui->categoryLabel->setText(dir);

	// find all accepted file types, note that we are looking in work, not export!
	Ptr<IoServer> ioServer = IoServer::Instance();
	String assetPath = "src:assets/";
	Array<String> files = ioServer->ListFiles(assetPath, "*.tga");
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
		item->SetPath(textureFile);

		// add to ui
		this->ui->texturePreview->AddTiledItem(item);

		// connect with browser to handle directory navigation
		connect(item, SIGNAL(OnSelected(const QString&)), this, SLOT(OnTextureClicked(const QString&)));
	}

	files = ioServer->ListFiles(assetPath, "*.fbx");
	for (i = 0; i < files.Size(); i++)
	{
		const String& file = files[i];
		String fbxFile = assetPath + "/" + file;

		// create new texture item
		TiledModelItem* item = new TiledModelItem;
		item->SetPath(fbxFile);

		// add to ui
		this->ui->texturePreview->AddTiledItem(item);

		// connect with browser to handle directory navigation
		connect(item, SIGNAL(OnSelected(const QString&)), this, SLOT(OnModelClicked(const QString&)));
	}

	files = ioServer->ListFiles(assetPath, "*.sur");
	for (i = 0; i < files.Size(); i++)
	{
		const String& file = files[i];
		String fbxFile = assetPath + "/" + file;

		// create new texture item
		TiledSurfaceItem* item = new TiledSurfaceItem;
		item->SetPath(fbxFile);

		// add to ui
		this->ui->texturePreview->AddTiledItem(item);

		// connect with browser to handle directory navigation
		connect(item, SIGNAL(OnSelected(const QString&)), this, SLOT(OnModelClicked(const QString&)));
	}

	// rearrange browser window
	this->ui->texturePreview->Rearrange();
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
AssetBrowser::SetupRoot()
{
	// clear first
	this->ui->texturePreview->Clear();
	this->ui->categoryLabel->setText("Root");

	Ptr<IoServer> ioServer = IoServer::Instance();
	String assetPath("src:assets");

	Array<String> dirs = ioServer->ListDirectories(assetPath, "*");
	IndexT i;
	for (i = 0; i < dirs.Size(); i++)
	{
		const String& dir = dirs[i];
		String textureDir = texPath + "/" + dir;

		// create new texture dir
		TiledDirectoryItem* item = new TiledDirectoryItem;
		item->SetDirectory(dir);

		// add to ui
		this->ui->texturePreview->AddTiledItem(item);

		// connect with browser to handle directory navigation
		connect(item, SIGNAL(OnSelected(const QString&)), this, SLOT(OnDirectoryClicked(const QString&)));
	}

	// rearrange browser window
	this->ui->texturePreview->Rearrange();
}

} // namespace ResourceBrowser
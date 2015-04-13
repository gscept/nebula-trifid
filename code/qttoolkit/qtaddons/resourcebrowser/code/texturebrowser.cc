//------------------------------------------------------------------------------
//  texturebrowser.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "texturebrowser.h"
#include "io/uri.h"
#include "io/ioserver.h"
#include "ui_texturebrowser.h"
#include "tileddirectoryitem.h"
#include "tiledtextureitem.h"

using namespace Util;
using namespace IO;
namespace ResourceBrowser
{

TextureLoaderThread* TextureBrowser::loaderThread;
__ImplementClass(ResourceBrowser::TextureBrowser, 'TEBR', Core::RefCounted);
__ImplementSingleton(ResourceBrowser::TextureBrowser);

//------------------------------------------------------------------------------
/**
*/
TextureBrowser::TextureBrowser() :
	isExecuting(false)
{
	this->ui = new Ui::TextureBrowser;
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
TextureBrowser::~TextureBrowser()
{
	delete this->ui;
	delete this->shortcut;
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
int
TextureBrowser::Execute(const QString& title)
{
	this->setWindowTitle("Texture browser - " + title);
	TextureBrowser::loaderThread->Pause(false);
	this->selectedResource = "";
	this->isExecuting = true;
	this->raise();
	return this->exec();
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBrowser::Open()
{
	// create thread
	TextureBrowser::loaderThread = new TextureLoaderThread(NULL);
	TextureBrowser::loaderThread->Start();
	this->SetupRoot();
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBrowser::Close()
{
	TextureBrowser::loaderThread->Stop();
	delete TextureBrowser::loaderThread;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBrowser::showEvent(QShowEvent* event)
{
	QDialog::showEvent(event);
	TextureBrowser::loaderThread->Pause(false);
	this->ui->texturePreview->Rearrange();
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBrowser::closeEvent(QCloseEvent* event)
{
	TextureBrowser::loaderThread->Pause(true);
	this->isExecuting = false;
	QDialog::closeEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBrowser::OnDirectoryClicked(const QString& dir)
{
	// clear first
	this->ui->texturePreview->Clear();
	this->ui->categoryLabel->setText(dir);

	// find all accepted file types, note that we are looking in work, not export!
	Ptr<IoServer> ioServer = IoServer::Instance();
	String texPath("src:textures/" + String(dir.toUtf8().constData()));
	Array<String> texFiles = ioServer->ListFiles(texPath, "*.png");
	texFiles.AppendArray(ioServer->ListFiles(texPath, "*.tga"));
	texFiles.AppendArray(ioServer->ListFiles(texPath, "*.bmp"));
	texFiles.AppendArray(ioServer->ListFiles(texPath, "*.psd"));
	texFiles.AppendArray(ioServer->ListFiles(texPath, "*.dds"));

	String gfxPath = "src:gfxlib/" + String(dir.toUtf8().constData());
	Array<String> fbxFiles = ioServer->ListFiles(gfxPath, "*.fbx");

	IndexT i;
	for (i = 0; i < texFiles.Size(); i++)
	{
		const String& file = texFiles[i];
		String textureFile = texPath + "/" + file;

		// create new texture item
		TiledTextureItem* item = new TiledTextureItem;
		item->SetPath(textureFile);

		// add to ui
		this->ui->texturePreview->AddTiledItem(item);

		// connect with browser to handle directory navigation
		connect(item, SIGNAL(OnSelected(const QString&)), this, SLOT(OnTextureClicked(const QString&)));
	}

	/*
	for (i = 0; i < fbxFiles.Size(); i++)
	{
		const String& file = fbxFiles[i];
		String fbxFile = gfxPath + "/" + file;

		// create new texture item
		TiledTextureItem* item = new TiledTextureItem;
		item->SetPath(fbxFile);

		// add to ui
		this->ui->texturePreview->AddTiledItem(item);

		// connect with browser to handle directory navigation
		connect(item, SIGNAL(OnSelected(const QString&)), this, SLOT(OnGfxClicked(const QString&)));
	}
	*/

	// rearrange browser window
	this->ui->texturePreview->Rearrange();
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBrowser::OnTextureClicked(const QString& tex)
{
	if (this->isExecuting)
	{
		TextureBrowser::loaderThread->Pause(true);
		this->selectedResource = tex;
		this->accept();
		this->isExecuting = false;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBrowser::OnGfxClicked(const QString& gfx)
{
	if (this->isExecuting)
	{
		TextureBrowser::loaderThread->Pause(true);
		this->selectedResource = gfx;
		this->accept();
		this->isExecuting = false;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBrowser::OnBack()
{
	TextureBrowser::loaderThread->Clear();
	this->SetupRoot();
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBrowser::SetupRoot()
{
	// clear first
	this->ui->texturePreview->Clear();
	this->ui->categoryLabel->setText("Root");

	Ptr<IoServer> ioServer = IoServer::Instance();
	String texPath("src:textures");

	Array<String> dirs = ioServer->ListDirectories(texPath, "*");
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
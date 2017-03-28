//------------------------------------------------------------------------------
//  contentbrowser.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "contentbrowserwindow.h"
#include "contentbrowserapp.h"
#include "io/ioserver.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "widgets/animations/animationitem.h"
#include "widgets/audio/audioitem.h"
#include "widgets/meshes/meshitem.h"
#include "widgets/models/modelitem.h"
#include "widgets/textures/textureitem.h"
#include "widgets/ui/fontitem.h"
#include "math/float4.h"
#include <QPlastiqueStyle>
#include <QtGui/QFileDialog>
#include <QFileSystemModel>
#include <QApplication>
#include <QFileIconProvider>
#include <QUrl>
#include <QTreeWidget>
#include <QtGui/QColorDialog>
#include <QColor>
#include <QAction>
#include <QMenu>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSettings>
#include "posteffect/rt/params/lightparams.h"
#include "modelutil/modeldatabase.h"
#include "modelutil/modelbuilder.h"
#include "algorithm/algorithmprotocol.h"
#include "toolkitversion.h"
#include "widgets/audio/bankitem.h"
#include "faudio/audiodevice.h"
#include "widgets/ui/fontitem.h"
#include "code/tiledmodelitem.h"
#include "code/tiledtextureitem.h"
#include "code/tiledsurfaceitem.h"
#include "QGraphicsSceneEvent"

using namespace Math;
using namespace Util;
using namespace IO;
using namespace Widgets;
using namespace Graphics;
using namespace PostEffect;
using namespace ToolkitUtil;
using namespace Particles;
using namespace Algorithm;
using namespace ResourceBrowser;

const char geometryData[] = "@ByteArray(\x1\xd9\xd0\xcb\0\x1\0\0\0\0\0\x45\0\0\0\x6\0\0\a\t\0\0\x3\xf0\0\0\0M\0\0\0%\0\0\a\x1\0\0\x3\xe8\0\0\0\0\0\0)";
const char windowStateData[] = "@ByteArray(\0\0\0\xff\0\0\0\0\xfd\0\0\0\x2\0\0\0\0\0\0\x1~\0\0\x3\x9c\xfc\x2\0\0\0\x2\xfb\0\0\0\x18\0\x41\0s\0s\0\x65\0t\0\x42\0r\0o\0w\0s\0\x65\0r\x1\0\0\0(\0\0\x3\x9c\0\0\0\xb2\0\xff\xff\xff\xfb\0\0\0\x14\0\x64\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t\0\0\0\0(\0\0\x3\x9c\0\0\0\x93\0\xff\xff\xff\0\0\0\x1\0\0\x1\xd4\0\0\x3\x9c\xfc\x2\0\0\0\a\xfb\0\0\0\x1e\0m\0o\0\x64\0\x65\0l\0\x44\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t\x1\0\0\0(\0\0\x1\xcb\0\0\0n\0\a\xff\xff\xfb\0\0\0$\0m\0\x61\0t\0\x65\0r\0i\0\x61\0l\0\x44\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t\x1\0\0\x1\xf9\0\0\x1\xcb\0\0\0n\0\xff\xff\xff\xfb\0\0\0\"\0t\0\x65\0x\0t\0u\0r\0\x65\0\x44\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t\x2\0\0\x1\xe0\0\0\x1\x44\0\0\0\xc8\0\0\0\x64\xfb\0\0\0\x1e\0\x61\0u\0\x64\0i\0o\0\x44\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t\x2\0\0\x1\xe0\0\0\x1\x44\0\0\0\xc8\0\0\0\x64\xfb\0\0\0\x18\0u\0i\0\x44\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t\x2\0\0\x1\xe0\0\0\x1\x44\0\0\0\xc8\0\0\0\x64\xfb\0\0\0\x1c\0m\0\x65\0s\0h\0\x44\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t\x2\0\0\x1\xe0\0\0\x1\x44\0\0\0\xc8\0\0\0\x64\xfb\0\0\0&\0\x61\0n\0i\0m\0\x61\0t\0i\0o\0n\0\x44\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t\x2\0\0\x1\xe0\0\0\x1\x44\0\0\0\xc8\0\0\0\x64\0\0\x3W\0\0\x3\x9c\0\0\0\x4\0\0\0\x4\0\0\0\b\0\0\0\b\xfc\0\0\0\x1\0\0\0\x2\0\0\0\x1\0\0\0\xe\0t\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0)";
const QByteArray defaultGeometry(geometryData, sizeof(geometryData));
const QByteArray defaultWindowState(windowStateData, sizeof(windowStateData));
namespace ContentBrowser
{

//------------------------------------------------------------------------------
/**
*/
ContentBrowserWindow::ContentBrowserWindow() :
	modelItem(0),
	animItem(0),
	audioItem(0),
	meshItem(0),
	layoutItem(0),
	fontItem(0),
	textureItem(0),
    animationHandler(0),
    audioHandler(0),
    meshHandler(0),
    modelHandler(0),
    textureHandler(0),
	uiHandler(0),
	wireFrame(false),
	showAmbientOcclusion(true),
	showPhysics(false),
	showControls(false),
	showSky(true),
	lockLight(true)
{
	this->ui.setupUi(this);
	this->setTabPosition(Qt::DockWidgetArea::TopDockWidgetArea, QTabWidget::TabPosition::North);
	this->setTabPosition(Qt::DockWidgetArea::BottomDockWidgetArea, QTabWidget::TabPosition::North);
	this->setTabPosition(Qt::DockWidgetArea::LeftDockWidgetArea, QTabWidget::TabPosition::North);
	this->setTabPosition(Qt::DockWidgetArea::RightDockWidgetArea, QTabWidget::TabPosition::North);

	// create copy assigns so that we may modify resources without changing anything in the models/textures/surfaces
	AssignRegistry::Instance()->SetAssign(Assign("intsur", "root:intermediate/surfaces"));
	AssignRegistry::Instance()->SetAssign(Assign("intmdl", "root:intermediate/models"));
	AssignRegistry::Instance()->SetAssign(Assign("inttex", "root:intermediate/textures"));

	this->setContentsMargins(2, 2, 2, 2);

	// setup model info
	this->modelInfoWindow = this->ui.modelDockWidget;
	this->modelInfoWindow->setVisible(false);
	this->modelInfoUi.setupUi(this->ui.modelDockFrame);

    // setup texture info
	this->textureInfoWindow = this->ui.textureDockWidget;
	this->textureInfoWindow->setVisible(false);
	this->textureInfoUi.setupUi(this->ui.textureDockFrame);

    // setup material info
    this->materialInfoWindow = this->ui.materialDockWidget;
    this->materialInfoWindow->setVisible(false);
    this->materialInfoUi.setupUi(this->ui.materialDockFrame);

    // setup audio info
	this->audioInfoWindow = this->ui.audioDockWidget;
	this->audioInfoWindow->setVisible(false);
	this->audioInfoUi.setupUi(this->ui.audioDockFrame);	

    // setup ui info
	this->uiInfoWindow = this->ui.uiDockWidget;
	this->uiInfoWindow->setVisible(false);
	this->uiInfoUi.setupUi(this->ui.uiDockFrame);

    // setup mesh info
	this->meshInfoWindow = this->ui.meshDockWidget;
	this->meshInfoWindow->setVisible(false);

    // setup animation info
	this->animationInfoWindow = this->ui.animationDockWidget;
	this->animationInfoWindow->setVisible(false);

	// create importers
	this->modelImporterWindow = new ModelImporter::ModelImporterWindow;
	this->textureImporterWindow = new TextureImporter::TextureImporterWindow;
    this->postEffectController = new QtPostEffectAddon::PostEffectController;

    // create shady window
    this->shadyWindow = Shady::ShadyWindow::Create();
    this->shadyWindow->Setup();

	// create texture browser window
	this->assetBrowserWindow = ResourceBrowser::AssetBrowser::Create();
	this->assetBrowserWindow->setParent(this);
	this->assetBrowserWindow->Open();	

	// setup ui of particle wizard
	this->particleWizardUi.setupUi(&this->particleEffectWizard);
	this->particleWizardUi.errorLabel->setHidden(true);

	this->ui.nebulaFrame->setWindowFlags(Qt::SubWindow);

	// add to appropriate tab
	/*
	this->ui.infoTabWidget->addTab(this->modelInfoWidget, "Model");
	this->ui.infoTabWidget->addTab(this->textureInfoWidget, "Texture");
	this->ui.infoTabWidget->addTab(this->meshInfoWidget, "Mesh");
	this->ui.infoTabWidget->addTab(this->audioInfoWidget, "Audio");
	this->ui.infoTabWidget->addTab(this->animationInfoWidget, "Animation");
	this->ui.infoTabWidget->addTab(this->uiInfoWidget, "UI");

	// now disable all tabs
	this->ui.infoTabWidget->setTabEnabled(0, false);
	this->ui.infoTabWidget->setTabEnabled(1, false);
	this->ui.infoTabWidget->setTabEnabled(2, false);
	this->ui.infoTabWidget->setTabEnabled(3, false);
	this->ui.infoTabWidget->setTabEnabled(4, false);
	this->ui.infoTabWidget->setTabEnabled(5, false);
	*/

	// setup handlers
	this->animationHandler = AnimationHandler::Create();
	this->audioHandler = AudioHandler::Create();
	this->meshHandler = MeshHandler::Create();
	this->modelHandler = ModelHandler::Create();
	this->textureHandler = TextureHandler::Create();
	this->uiHandler = UIHandler::Create();
    this->materialHandler = MaterialHandler::Create();

    // setup material UI
    this->materialHandler->SetUI(&this->materialInfoUi);

    // setup model UI
    this->modelHandler->SetUI(&this->modelInfoUi);

	// setup texture UI
	this->textureHandler->SetUI(&this->textureInfoUi);

	// create and setup progress reporter
	this->progressReporter = ProgressReporter::Create();
	this->progressReporter->Open();

	// update library the first time
	this->UpdateLibrary();

	// connect ui handles to local slots
	connect(this->ui.actionImport, SIGNAL(triggered()), this, SLOT(ImportModel()));
	connect(this->ui.actionImport_Texture, SIGNAL(triggered()), this, SLOT(ImportTexture()));
	connect(this->ui.libraryTree, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(ItemActivated(QTreeWidgetItem*)));
	connect(this->ui.libraryTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(ItemClicked(QTreeWidgetItem*)));
	connect(this->ui.libraryTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(TreeRightClicked(const QPoint&)));
	connect(this->ui.actionUndo, SIGNAL(triggered()), this, SLOT(OnUndo()));
	connect(this->ui.actionRedo, SIGNAL(triggered()), this, SLOT(OnRedo()));
	connect(this->ui.actionSave_all, SIGNAL(triggered()), this, SLOT(OnSaveAll()));
	connect(this->ui.actionAsset_browser, SIGNAL(triggered()), this, SLOT(OnShowAssetBrowser()));
	connect(this->ui.actionEnvironment_probe, SIGNAL(triggered()), this, SLOT(OnShowEnvironmentProbeSettings()));
	connect(this->ui.actionShow_grid, SIGNAL(triggered()), this, SLOT(OnShowGrid()));
	connect(this->modelImporterWindow, SIGNAL(ImportDone(const Util::String&)), this, SLOT(OnModelImported(const Util::String&)));
	connect(this->textureImporterWindow, SIGNAL(ImportDone(const Util::String&)), this, SLOT(OnTextureImported(const Util::String&)));	

    connect(this->assetBrowserWindow, SIGNAL(TextureSelected(const QString&)), this, SLOT(OnTextureSelected(const QString&)));
    connect(this->assetBrowserWindow, SIGNAL(ModelSelected(const QString&)), this, SLOT(OnModelSelected(const QString&)));
    connect(this->assetBrowserWindow, SIGNAL(SurfaceSelected(const QString&)), this, SLOT(OnSurfaceSelected(const QString&)));
	connect(this->assetBrowserWindow, SIGNAL(ContextMenuOpened(QContextMenuEvent*)), this, SLOT(OnAssetBrowserRightClick(QContextMenuEvent*)));
	connect(this->assetBrowserWindow, SIGNAL(ItemContextMenuOpened(ResourceBrowser::TiledGraphicsItem*, QGraphicsSceneContextMenuEvent*)), this, SLOT(OnAssetBrowserItemRightClick(ResourceBrowser::TiledGraphicsItem*, QGraphicsSceneContextMenuEvent*)));

    // connect window actions
	connect(this->ui.actionShow_Model_Info, SIGNAL(triggered()), this, SLOT(OnShowModelInfo()));
	connect(this->ui.actionShow_Texture_Info, SIGNAL(triggered()), this, SLOT(OnShowTextureInfo()));
	connect(this->ui.actionShow_Audio_Info, SIGNAL(triggered()), this, SLOT(OnShowAudioInfo()));
	connect(this->ui.actionShow_UI_Info, SIGNAL(triggered()), this, SLOT(OnShowUIInfo()));
	connect(this->ui.actionShow_Animation_Info, SIGNAL(triggered()), this, SLOT((OnShowAnimationInfo)));
	connect(this->ui.actionShow_Mesh_Info, SIGNAL(triggered()), this, SLOT(OnShowMeshInfo()));
	connect(this->ui.actionShow_PostEffect_Controls, SIGNAL(triggered()), this, SLOT(OnShowPostEffectController()));
	connect(this->ui.actionCreate_Particle, SIGNAL(triggered()), this, SLOT(OnCreateParticleEffect()));
	connect(this->ui.actionConnect_with_Level_Editor, SIGNAL(triggered()), this, SLOT(OnConnectToLevelEditor()));
    connect(this->ui.actionShow_Shady, SIGNAL(triggered()), this->shadyWindow, SLOT(show()));
	connect(this->ui.actionRestore_default_layout, SIGNAL(triggered()), this, SLOT(ResetLayout()));
	connect(QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor"), SIGNAL(OnDisconnected()), this, SLOT(OnDisconnectFromLevelEditor()));

	connect(this->modelHandler, SIGNAL(ModelSavedAs(const Util::String&)), this, SLOT(ModelSavedWithNewName(const Util::String&)));
	connect(this->ui.refreshButton, SIGNAL(clicked()), this, SLOT(UpdateLibrary()));

    connect(this->ui.actionAbout, SIGNAL(triggered()), this, SLOT(OnAbout()));
    connect(this->ui.actionDebug_Page, SIGNAL(triggered()), this, SLOT(OnDebugPage()));

	// try to connect to level editor
	this->OnConnectToLevelEditor();

	// connect clicked signal with item handler
	connect(this->textureInfoUi.reload, SIGNAL(clicked()), this->textureHandler, SLOT(OnReload()));
	connect(this->ui.libraryTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(ItemChanged()));

    this->libraryTree = (LibraryTreeWidget*)this->ui.libraryTree;
}

//------------------------------------------------------------------------------
/**
*/
ContentBrowserWindow::~ContentBrowserWindow()
{
	// empty

}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::Setup()
{
	// get preview state and light
	const Ptr<PostEffectEntity>& postEffectEntity = ContentBrowserApp::Instance()->GetPostEffectEntity();

	// set entity for controller
	this->postEffectController->SetPostEffectEntity(postEffectEntity);
	this->postEffectController->LoadPresets();

	// load master bank file
	this->audioHandler->Setup();

	// create environment probe window, this must happen after we setup the graphics subsystem
	this->environmentProbeWindow = new Lighting::EnvironmentProbeWindow;
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::showEvent(QShowEvent* e)
{
	QMainWindow::showEvent(e);

	// restore the state of the window and all dock widgets
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "gscept", "Content browser");
	this->restoreGeometry(settings.value("geometry").toByteArray());
	this->restoreState(settings.value("windowState").toByteArray());
	bool assetBrowserShowing = settings.value("assetbrowser", true).toBool();

	// open asset browser if it was open
	if (assetBrowserShowing)
	{
		this->assetBrowserWindow->raise();
		this->assetBrowserWindow->show();
	}	
	else
	{
		this->assetBrowserWindow->close();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::closeEvent( QCloseEvent *e )
{
	bool canClose = true;
	if (animationHandler->IsSetup())
	{
		canClose &= this->animationHandler->Discard();
	}
	
	if (this->audioHandler->IsSetup())
	{
		canClose &= this->audioHandler->Discard();
	}

	if (this->uiHandler->IsSetup())
	{
		canClose &= this->uiHandler->Discard();
	}

	if (this->meshHandler->IsSetup())
	{
		canClose &= this->meshHandler->Discard();
	}
	
	if (this->modelHandler->IsSetup())
	{
		canClose &= this->modelHandler->Discard();
	}

	if (this->textureHandler->IsSetup())
	{
		canClose &= this->textureHandler->Discard();
	}

    if (this->materialHandler->IsSetup())
    {
        canClose &= this->materialHandler->Discard();
    }

	// abort window close if any handler is to be discarded
	if (!canClose)
	{
		e->ignore();
		return;
	}

	// request to exit the application
	ContentBrowserApp::Instance()->RequestState("Exit");

	// delete items
	IndexT i;
	for (i = 0; i < this->items.Size(); i++)
	{
        this->items[i]->Discard();
		delete this->items[i];
	}
	this->items.Clear();

    // cleanup any current items
    if (0 != this->modelItem) delete this->modelItem;
    if (0 != this->animItem) delete this->animItem;
    if (0 != this->audioItem) delete this->audioItem;
    if (0 != this->meshItem) delete this->meshItem;
    if (0 != this->textureItem) delete this->textureItem;
	if (0 != this->layoutItem) delete this->layoutItem;

	// clear objects
	this->progressReporter->Close();
	this->progressReporter = 0;
    this->shadyWindow->Close();
    this->shadyWindow = 0;

	this->animationHandler->Cleanup();
	this->animationHandler = 0;	
	this->audioHandler->Cleanup();
	this->audioHandler = 0;
	this->meshHandler->Cleanup();
	this->meshHandler = 0;
    this->modelHandler->Cleanup();
	this->modelHandler = 0;
	this->textureHandler->Cleanup();
	this->textureHandler = 0;
	this->materialHandler->Cleanup();
	this->materialHandler = 0;
	this->uiHandler->Cleanup();
	this->uiHandler = 0;

	// delete windows
	this->textureImporterWindow->close();
	this->modelImporterWindow->close();
	this->postEffectController->close();
	this->environmentProbeWindow->close();
	delete this->textureImporterWindow;
	delete this->modelImporterWindow;
    delete this->postEffectController;
	delete this->environmentProbeWindow;

	// save the state of the window and all dock widgets
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "gscept", "Content browser");
	settings.setValue("geometry", this->saveGeometry());
	settings.setValue("windowState", this->saveState());
	settings.setValue("assetbrowser", this->assetBrowserWindow->isVisible());

	// close asset browser
	this->assetBrowserWindow->close();
	this->assetBrowserWindow = 0;

	// close window
	QMainWindow::closeEvent(e);
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::ImportModel()
{
	QFileDialog fileDialog(this, "Import FBX", IO::URI("src:assets").LocalPath().AsCharPtr(), tr("*.fbx"));
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);

	if (fileDialog.exec() == QDialog::Accepted)
	{
		QString fullPath = fileDialog.selectedFiles()[0];
		IO::URI path(fullPath.toUtf8().constData());
		this->modelImporterWindow->SetUri(path);
		this->modelImporterWindow->show();
		this->modelImporterWindow->raise();
		QApplication::processEvents();
		this->modelImporterWindow->Open();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::ImportTexture()
{
	QFileDialog fileDialog(this, "Import Texture", IO::URI("src:assets").LocalPath().AsCharPtr(), tr("Image Files (*.png *.jpg *.bmp *.psd *.tga *.dds)"));
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);

	if (fileDialog.exec() == QDialog::Accepted)
	{
		QString fullPath = fileDialog.selectedFiles()[0];
		IO::URI path(fullPath.toUtf8().constData());
		this->textureImporterWindow->SetUri(path);
		this->textureImporterWindow->show();
		this->textureImporterWindow->raise();
		QApplication::processEvents();
		this->textureImporterWindow->Open();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::UpdateLibrary()
{
	// delete items
	IndexT i;
	for (i = 0; i < this->items.Size(); i++)
	{
		this->items[i]->Discard();
        delete this->items[i];
	}
	this->items.Clear();

	// clear tree
	this->ui.libraryTree->clear();	

	QStyle* style = QApplication::style();

	QTreeWidgetItem* topLevelItem = new QTreeWidgetItem;
	topLevelItem->setText(0, "Library");
	topLevelItem->setIcon(0, style->standardIcon(QStyle::SP_DriveHDIcon));
	this->ui.libraryTree->addTopLevelItem(topLevelItem);

	QTreeWidgetItem* animationItem = new QTreeWidgetItem;
	animationItem->setText(0, "animations");
	animationItem->setIcon(0, style->standardIcon(QStyle::SP_DirLinkIcon));
	topLevelItem->addChild(animationItem);
	this->UpdateAnimationLibrary(animationItem);

	QTreeWidgetItem* audioItem = new QTreeWidgetItem;
	audioItem->setText(0, "audio");
	audioItem->setIcon(0, style->standardIcon(QStyle::SP_DirLinkIcon));
	topLevelItem->addChild(audioItem);
	this->UpdateAudioLibrary(audioItem);

	QTreeWidgetItem* meshItem = new QTreeWidgetItem;
	meshItem->setText(0, "meshes");
	meshItem->setIcon(0, style->standardIcon(QStyle::SP_DirLinkIcon));
	topLevelItem->addChild(meshItem);
	this->UpdateMeshLibrary(meshItem);

	QTreeWidgetItem* modelItem = new QTreeWidgetItem;
	modelItem->setText(0, "models");
	modelItem->setIcon(0, style->standardIcon(QStyle::SP_DirLinkIcon));
	topLevelItem->addChild(modelItem);	
	this->UpdateModelLibrary(modelItem);

	QTreeWidgetItem* textureItem = new QTreeWidgetItem;
	textureItem->setText(0, "textures");
	textureItem->setIcon(0, style->standardIcon(QStyle::SP_DirLinkIcon));
	topLevelItem->addChild(textureItem);
	this->UpdateTextureLibrary(textureItem);	

	QTreeWidgetItem* uiItem = new QTreeWidgetItem;
	uiItem->setText(0, "ui");
	uiItem->setIcon(0, style->standardIcon(QStyle::SP_DirLinkIcon));
	topLevelItem->addChild(uiItem);
	this->UpdateUILibrary(uiItem);
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::UpdateAnimationLibrary( QTreeWidgetItem* animationItem )
{
	Array<String> directories = IoServer::Instance()->ListDirectories(IO::URI("ani:"), "*");
	IndexT dirIndex;
	for (dirIndex = 0; dirIndex < directories.Size(); dirIndex++)
	{
		String directory = directories[dirIndex];
		QTreeWidgetItem* directoryItem = new QTreeWidgetItem;
		directoryItem->setText(0, directory.AsCharPtr());
		QStyle* style = QApplication::style();
		directoryItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		animationItem->addChild(directoryItem);

		Array<String> files = IoServer::Instance()->ListFiles(IO::URI("ani:" + directory), "*");
		IndexT fileIndex;
		for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
		{
			String file = files[fileIndex];
			String resource = directory + "/" + file;
			AnimationItem* fileItem = new AnimationItem;

			// set handler
			fileItem->SetHandler(this->animationHandler.upcast<BaseHandler>());
			fileItem->SetName(resource.AsCharPtr());
			fileItem->SetWidget(this->animationInfoWindow);
			fileItem->setText(0, file.AsCharPtr());
			this->items.Append(fileItem);

			// if this is supposed to be our new item then we clone it again
			if (this->animItem && this->animItem->GetName() == fileItem->GetName())
			{
				delete this->animItem;
				this->animItem = static_cast<AnimationItem*>(fileItem->Clone());
			}

			// get icon info
			String path = IO::URI("ani:" + directory + "/" + file).GetHostAndLocalPath();
			QFileInfo info(path.AsCharPtr());
			QFileIconProvider ip;
			fileItem->setIcon(0, ip.icon(info));

			directoryItem->addChild(fileItem);
			
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::UpdateAudioLibrary( QTreeWidgetItem* audioItem )
{
	this->audioHandler->SetUI(&this->audioInfoUi);
	Array<String> banks = FAudio::AudioDevice::FindBankFiles();
	IndexT bankIndex;
	for (bankIndex = 0; bankIndex < banks.Size(); bankIndex++)
	{
		String directory = banks[bankIndex];		
		if (directory.GetFileExtension() == "strings" || directory == "Master Bank")
		{
			continue;
		}
		BankItem * bankItem = new BankItem;
		bankItem->SetHandler(this->audioHandler.upcast<BaseHandler>());
		bankItem->SetName(directory.AsCharPtr());
		bankItem->SetWidget(this->audioInfoWindow);		
		bankItem->setText(0, directory.AsCharPtr());
		QStyle* style = QApplication::style();
		bankItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		audioItem->addChild(bankItem);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::UpdateMeshLibrary( QTreeWidgetItem* meshItem )
{
	Array<String> directories = IoServer::Instance()->ListDirectories(IO::URI("msh:"), "*");
	IndexT dirIndex;
	for (dirIndex = 0; dirIndex < directories.Size(); dirIndex++)
	{
		String directory = directories[dirIndex];
		QTreeWidgetItem* directoryItem = new QTreeWidgetItem;
		directoryItem->setText(0, directory.AsCharPtr());
		QStyle* style = QApplication::style();
		directoryItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		meshItem->addChild(directoryItem);

		Array<String> files = IoServer::Instance()->ListFiles(IO::URI("msh:" + directory), "*");
		IndexT fileIndex;
		for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
		{
			String file = files[fileIndex];
			String resource = directory + "/" + file;
			MeshItem* fileItem = new MeshItem;

			// set handler
			fileItem->SetHandler(this->meshHandler.upcast<BaseHandler>());
			fileItem->SetName(resource.AsCharPtr());
			fileItem->SetWidget(this->meshInfoWindow);
			fileItem->setText(0, file.AsCharPtr());
			this->items.Append(fileItem);

			// if this is supposed to be our new item then we clone it again
			if (this->meshItem && this->meshItem->GetName() == fileItem->GetName())
			{
				delete this->meshItem;
				this->meshItem = static_cast<MeshItem*>(fileItem->Clone());
			}

			// get icon info
			String path = IO::URI("msh:" + directory + "/" + file).GetHostAndLocalPath();
			QFileInfo info(path.AsCharPtr());
			QFileIconProvider ip;
			fileItem->setIcon(0, ip.icon(info));


			directoryItem->addChild(fileItem);
			
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::UpdateModelLibrary( QTreeWidgetItem* modelItem )
{
	Array<String> directories = IoServer::Instance()->ListDirectories(IO::URI("mdl:"), "*");
	IndexT dirIndex;
	for (dirIndex = 0; dirIndex < directories.Size(); dirIndex++)
	{
		String directory = directories[dirIndex];
		QTreeWidgetItem* directoryItem = new QTreeWidgetItem;
		directoryItem->setText(0, directory.AsCharPtr());
		QStyle* style = QApplication::style();
		directoryItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		modelItem->addChild(directoryItem);

		Array<String> files = IoServer::Instance()->ListFiles(IO::URI("mdl:" + directory), "*");
		IndexT fileIndex;
		for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
		{
			String file = files[fileIndex];
			String resource = directory + "/" + file;
			ModelItem* fileItem = new ModelItem;

			// set handler
			fileItem->SetHandler(this->modelHandler.upcast<BaseHandler>());
			fileItem->SetName(resource.AsCharPtr());
			fileItem->SetWidget(this->modelInfoWindow);
			fileItem->SetUi(&this->modelInfoUi);
			fileItem->setText(0, file.AsCharPtr());
			this->items.Append(fileItem);

			// if this is supposed to be our new item then we clone it again
			if (this->modelItem && this->modelItem->GetName() == fileItem->GetName())
			{
				delete this->modelItem;
				this->modelItem = static_cast<ModelItem*>(fileItem->Clone());
			}

			// get icon info
			String path = IO::URI("mdl:" + directory + "/" + file).GetHostAndLocalPath();
			QFileInfo info(path.AsCharPtr());
			QFileIconProvider ip;
			fileItem->setIcon(0, ip.icon(info));

			directoryItem->addChild(fileItem);
			
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::UpdateTextureLibrary( QTreeWidgetItem* textureItem )
{
	Array<String> directories = IoServer::Instance()->ListDirectories(IO::URI("tex:"), "*");
	IndexT dirIndex;
	for (dirIndex = 0; dirIndex < directories.Size(); dirIndex++)
	{
		String directory = directories[dirIndex];
		QTreeWidgetItem* directoryItem = new QTreeWidgetItem;
		directoryItem->setText(0, directory.AsCharPtr());
		QStyle* style = QApplication::style();
		directoryItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		textureItem->addChild(directoryItem);

		Array<String> files = IoServer::Instance()->ListFiles(IO::URI("tex:" + directory), "*");
		IndexT fileIndex;
		for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
		{
			String file = files[fileIndex];
			String resource = directory + "/" + file;
			TextureItem* fileItem = new TextureItem;

			// set handler
			fileItem->SetHandler(this->textureHandler.upcast<BaseHandler>());
			fileItem->SetName(resource.AsCharPtr());
			fileItem->SetWidget(this->textureInfoWindow);
			fileItem->SetUi(&this->textureInfoUi);
			fileItem->setText(0, file.AsCharPtr());
			this->items.Append(fileItem);

			// if this is supposed to be our new item then we clone it again
			if (this->textureItem && this->textureItem->GetName() == fileItem->GetName())
			{
				delete this->textureItem;
				this->textureItem = static_cast<TextureItem*>(fileItem->Clone());
			}

			// get icon info
			String path = IO::URI("tex:" + directory + "/" + file).GetHostAndLocalPath();
			QFileInfo info(path.AsCharPtr());
			QFileIconProvider ip;
			fileItem->setIcon(0, ip.icon(info));

			directoryItem->addChild(fileItem);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::UpdateUILibrary(QTreeWidgetItem* uiItem)
{
	this->uiHandler->SetUI(&this->uiInfoUi);
	Util::Array<Util::String> directories = IO::IoServer::Instance()->ListDirectories(IO::URI("gui:"), "*");
	
	IndexT dirIndex;
	for (dirIndex = 0; dirIndex < directories.Size(); dirIndex++)
	{
		String directory = directories[dirIndex];
		QTreeWidgetItem* directoryItem = new QTreeWidgetItem;
		directoryItem->setText(0, directory.AsCharPtr());
		QStyle* style = QApplication::style();
		directoryItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		uiItem->addChild(directoryItem);

		QTreeWidgetItem* layoutDirectoryItem = new QTreeWidgetItem;
		layoutDirectoryItem->setText(0, "layouts");		
		layoutDirectoryItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		directoryItem->addChild(layoutDirectoryItem);

		Array<String> files = IoServer::Instance()->ListFiles(IO::URI("gui:" + directory), "*.rml");
		IndexT fileIndex;
		for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
		{
			String file = files[fileIndex];
			String resource = directory + "/" + file;
			LayoutItem* fileItem = new LayoutItem;

			// set handler
			fileItem->SetHandler(this->uiHandler.upcast<BaseHandler>());
			fileItem->SetName(resource.AsCharPtr());
			fileItem->SetWidget(this->uiInfoWindow);
			fileItem->SetUi(&this->uiInfoUi);
			fileItem->setText(0, file.AsCharPtr());
			this->items.Append(fileItem);

			// if this is supposed to be our new item then we clone it again
			if (this->layoutItem && this->layoutItem->GetName() == fileItem->GetName())
			{
				delete this->layoutItem;
				this->layoutItem = static_cast<LayoutItem*>(fileItem->Clone());
			}
			// get icon info
			String path = IO::URI("gui:" + directory + "/" + file).GetHostAndLocalPath();
			QFileInfo info(path.AsCharPtr());
			QFileIconProvider ip;
			fileItem->setIcon(0, ip.icon(info));

			layoutDirectoryItem->addChild(fileItem);
		}

		QTreeWidgetItem* fontDirectoryItem = new QTreeWidgetItem;
		fontDirectoryItem->setText(0, "fonts");
		fontDirectoryItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		directoryItem->addChild(fontDirectoryItem);

		files = IoServer::Instance()->ListFiles(IO::URI("gui:" + directory), "*.otf");
		files.AppendArray(IoServer::Instance()->ListFiles(IO::URI("gui:" + directory), "*.ttf"));
		
		for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
		{
			String file = files[fileIndex];
			String resource = directory + "/" + file;
			FontItem* fileItem = new FontItem;

			// set handler
			fileItem->SetHandler(this->uiHandler.upcast<BaseHandler>());
			fileItem->SetName(resource.AsCharPtr());
			fileItem->setText(0, file.AsCharPtr());
			fileItem->SetWidget(this->uiInfoWindow);
			this->items.Append(fileItem);

			// get icon info
			String path = IO::URI("gui:" + directory + "/" + file).GetHostAndLocalPath();
			QFileInfo info(path.AsCharPtr());
			QFileIconProvider ip;
			fileItem->setIcon(0, ip.icon(info));

			fontDirectoryItem->addChild(fileItem);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnModelImported( const Util::String& res )
{
	// split resource into category and file
	IndexT slashLocation = res.FindCharIndex('/');
	String cat = res.ExtractRange(0, slashLocation);
	String file = res.ExtractToEnd(slashLocation+1);

    //TODO: update mesh, model and animation if new
	QTreeWidgetItem* topLevelItem = this->ui.libraryTree->topLevelItem(0);
	QTreeWidgetItem* meshResourceItem = this->libraryTree->FindItem(topLevelItem, "meshes");
	QTreeWidgetItem* modelResourceItem = this->libraryTree->FindItem(topLevelItem, "models");
	QTreeWidgetItem* animResourceItem = this->libraryTree->FindItem(topLevelItem, "animations");

	QTreeWidgetItem* meshCatItem = this->libraryTree->FindItem(meshResourceItem, cat.AsCharPtr());
	QTreeWidgetItem* modelCatItem = this->libraryTree->FindItem(modelResourceItem, cat.AsCharPtr());
	QTreeWidgetItem* animCatItem = this->libraryTree->FindItem(animResourceItem, cat.AsCharPtr());

	file.ChangeFileExtension("nvx2");
	QTreeWidgetItem* meshFileItem = this->libraryTree->FindItem(meshCatItem, file.AsCharPtr());
	file.ChangeFileExtension("n3");
	QTreeWidgetItem* modelFileItem = this->libraryTree->FindItem(modelCatItem, file.AsCharPtr());
	file.ChangeFileExtension("nax3");
	QTreeWidgetItem* animFileItem = this->libraryTree->FindItem(animCatItem, file.AsCharPtr());

	// check if the exporter created an animation resource (format should still be nax3 here)
	String anim;
	anim.Format("ani:%s/%s", cat.AsCharPtr(), file.AsCharPtr());
	bool animationCreated = IoServer::Instance()->FileExists(anim);

	bool meshCatExists = meshCatItem != 0;
	bool modelCatExists = modelCatItem != 0;
	bool animCatExists = animCatItem != 0;

	bool meshFileExists = meshFileItem != 0;
	bool modelFileExists = modelFileItem != 0;
	bool animFileExists = animFileItem != 0;

	// expand library and the affected resources
	this->libraryTree->expandItem(topLevelItem);
	this->libraryTree->expandItem(modelResourceItem);

	// setup mesh category if it doesn't exist
	if (!meshCatExists)
	{
		meshCatItem = new QTreeWidgetItem;
		meshCatItem->setText(0, cat.AsCharPtr());

		// setup folder icon
		QStyle* style = QApplication::style();
		meshCatItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		meshResourceItem->addChild(meshCatItem);
	}

	// setup mesh file item if it doesn't exist
	if (!meshFileExists)
	{
		MeshItem* fileItem = new MeshItem;
		file.ChangeFileExtension("nvx2");
		fileItem->SetHandler(this->meshHandler.upcast<BaseHandler>());
		fileItem->SetName(res.AsCharPtr());
		fileItem->SetWidget(this->meshInfoWindow);
		fileItem->setText(0, file.AsCharPtr());

		// add item to item list
		this->items.Append(fileItem);

		// setup icon
		String path;
		path.Format("msh:%s/%s", cat.AsCharPtr(), file.AsCharPtr());
		path = URI(path).GetHostAndLocalPath();
		QFileInfo info(path.AsCharPtr());
		QFileIconProvider ip;
		fileItem->setIcon(0, ip.icon(info));

		// add to category
		meshCatItem->addChild(fileItem);
	}

	// setup model category if it doesn't exist
	if (!modelCatExists)
	{
		modelCatItem = new QTreeWidgetItem;
		modelCatItem->setText(0, cat.AsCharPtr());

		// setup folder icon
		QStyle* style = QApplication::style();
		modelCatItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		modelResourceItem->addChild(modelCatItem);		
	}

	// expand category item
	this->libraryTree->expandItem(modelCatItem);

	// setup mesh file item if it doesn't exist
	if (!modelFileExists)
	{
		ModelItem* fileItem = new ModelItem;
		file.ChangeFileExtension("n3");
		fileItem->SetHandler(this->modelHandler.upcast<BaseHandler>());
		fileItem->SetName(res.AsCharPtr());
		fileItem->SetWidget(this->modelInfoWindow);
		fileItem->SetUi(&this->modelInfoUi);
		fileItem->setText(0, file.AsCharPtr());

		// add item to item list
		this->items.Append(fileItem);

		// setup icon
		String path;
		path.Format("mdl:%s/%s", cat.AsCharPtr(), file.AsCharPtr());
		path = URI(path).GetHostAndLocalPath();
		QFileInfo info(path.AsCharPtr());
		QFileIconProvider ip;
		fileItem->setIcon(0, ip.icon(info));

		// add to category
		modelCatItem->addChild(fileItem);

		// set selected
		fileItem->setSelected(true);
	}
	else
	{
		// set selected
		modelFileItem->setSelected(true);
		file.StripFileExtension();
		if (this->modelHandler->IsSetup() && this->modelHandler->GetModelCategory() == cat && this->modelHandler->GetModelResource() == file)
		{
			this->modelHandler->DiscardNoCancel();
			this->modelHandler->Setup();
		}		
	}

	// setup animation category if it doesn't exist
	if (!animCatExists && animationCreated)
	{
		animCatItem = new QTreeWidgetItem;
		animCatItem->setText(0, cat.AsCharPtr());

		// setup folder icon
		QStyle* style = QApplication::style();
		animCatItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		animResourceItem->addChild(animCatItem);
	}

	// setup mesh file item if it doesn't exist
	if (!animFileExists)
	{
		// this may not be the case for ordinary meshes without animations attached
		if (animationCreated)
		{
			AnimationItem* fileItem = new AnimationItem;
			file.ChangeFileExtension("nax3");
			fileItem->SetHandler(this->animationHandler.upcast<BaseHandler>());
			fileItem->SetName(res.AsCharPtr());
			fileItem->SetWidget(this->animationInfoWindow);
			fileItem->setText(0, file.AsCharPtr());

			// add item to item list
			this->items.Append(fileItem);

			// setup icon
			String path;
			path.Format("ani:%s/%s", cat.AsCharPtr(), file.AsCharPtr());
			path = URI(path).GetHostAndLocalPath();
			QFileInfo info(path.AsCharPtr());
			QFileIconProvider ip;
			fileItem->setIcon(0, ip.icon(info));

			// add to category
			animCatItem->addChild(fileItem);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnTextureImported( const Util::String& res )
{
    // simply add texture resource (if it doesnt exist!)
    IndexT slashLocation = res.FindCharIndex('/');
    String cat = res.ExtractRange(0, slashLocation);
    String file = res.ExtractToEnd(slashLocation+1);
    bool fileExists = false;
    bool catExists = false;

    // get 'Library' item
    QTreeWidgetItem* topLevelItem = this->ui.libraryTree->topLevelItem(0);
    QTreeWidgetItem* resourceItem = this->libraryTree->FindItem(topLevelItem, "textures");
    QTreeWidgetItem* categoryItem = this->libraryTree->FindItem(resourceItem, cat.AsCharPtr());
    QTreeWidgetItem* textureItem = this->libraryTree->FindItem(categoryItem, file.AsCharPtr());

    fileExists = textureItem != 0;
    catExists = categoryItem != 0;
    
    n_assert2(resourceItem, "Could not find Library resource named 'textures'! This should never happen!");

    // if category doesn't exist, create a new item
    if (!catExists)
    {
        categoryItem = new QTreeWidgetItem;
        categoryItem->setText(0, cat.AsCharPtr());

        // setup folder icon
        QStyle* style = QApplication::style();
        categoryItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
        resourceItem->addChild(categoryItem);
    }

    // then if the file doesn't exist, add the file to the category
    if (!fileExists)
    {
        TextureItem* fileItem = new TextureItem;
        fileItem->SetHandler(this->textureHandler.upcast<BaseHandler>());
        fileItem->SetName(res.AsCharPtr());
        fileItem->SetWidget(this->textureInfoWindow);
        fileItem->SetUi(&this->textureInfoUi);
        fileItem->setText(0, file.AsCharPtr());

        // add item to item list
        this->items.Append(fileItem);

        // setup icon
        String path = IO::URI("tex:" + cat + "/" + file).GetHostAndLocalPath();
        QFileInfo info(path.AsCharPtr());
        QFileIconProvider ip;
        fileItem->setIcon(0, ip.icon(info));

        // add to category
        categoryItem->addChild(fileItem);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::ModelSavedWithNewName( const Util::String& res )
{
	// split resource into category and file
	IndexT slashLocation = res.FindCharIndex('/');
	String cat = res.ExtractRange(0, slashLocation);
	String file = res.ExtractToEnd(slashLocation+1);

	//TODO: update mesh, model and animation if new
	QTreeWidgetItem* topLevelItem = this->ui.libraryTree->topLevelItem(0);
	QTreeWidgetItem* modelResourceItem = this->libraryTree->FindItem(topLevelItem, "models");
	QTreeWidgetItem* modelCatItem = this->libraryTree->FindItem(modelResourceItem, cat.AsCharPtr());
	QTreeWidgetItem* modelFileItem = this->libraryTree->FindItem(modelCatItem, file.AsCharPtr());

	// check if category and file exists
	bool modelCatExists = modelCatItem != 0;
	bool modelFileExists = modelFileItem != 0;

	// setup model category if it doesn't exist
	if (!modelCatExists)
	{
		modelCatItem = new QTreeWidgetItem;
		modelCatItem->setText(0, cat.AsCharPtr());

		// setup folder icon
		QStyle* style = QApplication::style();
		modelCatItem->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		modelResourceItem->addChild(modelCatItem);
	}

	// setup mesh file item if it doesn't exist
	if (!modelFileExists)
	{
		ModelItem* fileItem = new ModelItem;
		fileItem->SetHandler(this->modelHandler.upcast<BaseHandler>());
		fileItem->SetName(res.AsCharPtr());
		fileItem->SetWidget(this->modelInfoWindow);
		fileItem->SetUi(&this->modelInfoUi);
		fileItem->setText(0, file.AsCharPtr());

		// add item to item list
		this->items.Append(fileItem);

		// setup icon
		String path = IO::URI("tex:" + cat + "/" + file).GetHostAndLocalPath();
		QFileInfo info(path.AsCharPtr());
		QFileIconProvider ip;
		fileItem->setIcon(0, ip.icon(info));

		// add to category
		modelCatItem->addChild(fileItem);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnTextureSelected(const QString& tex)
{
	bool b = true;
	if (this->textureHandler->IsSetup())
	{
		b = this->textureHandler->Discard();
	}

	if (b)
	{
		// split name into category and file
		QStringList resourceParts = tex.split("/");

		// get file name and category
		String category = resourceParts[0].toUtf8().constData();
		String file = resourceParts[1].toUtf8().constData();
		category.StripAssignPrefix();
		file.StripFileExtension();

		this->textureHandler->SetTextureCategory(category);
		this->textureHandler->SetTextureResource(file);

		// preview
		if (!this->textureHandler->LoadTexture())
		{
			String message;
			message.Format("Texture '%s/%s' failed to load. Possible causes: \n    Not a valid .png, .jpg, .dds, .tga or .psd file.", category.AsCharPtr(), file.AsCharPtr());
			QMessageBox::warning(NULL, "Failed to load resource", message.AsCharPtr());
		}
		else
		{
			this->textureInfoWindow->show();
			this->textureInfoWindow->raise();
			this->textureInfoWindow->setEnabled(true);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnModelSelected(const QString& mdl)
{
	bool b = true;
	if (this->modelHandler->IsSetup())
	{
		b = this->modelHandler->Discard();
	}
	
	if (b)
	{
		// split name into category and file
		QStringList resourceParts = mdl.split("/");

		// get file name and category
		String category = resourceParts[0].toUtf8().constData();
		String file = resourceParts[1].toUtf8().constData();
		category.StripAssignPrefix();
		file.StripFileExtension();

		this->modelHandler->SetModelCategory(category);
		this->modelHandler->SetModelResource(file);

		// preview
		if (this->modelHandler->Preview())
		{
			// setup widget only if load was successful
			this->modelInfoWindow->show();
			this->modelInfoWindow->raise();
			this->modelInfoWindow->setEnabled(true);
			this->modelHandler->Setup();
		}
		else
		{
			this->modelHandler->Mute();
			String message;
			message.Format("Model '%s/%s' failed to load. Possible causes: \n    No .attributes exists for this model.\n    Model is character, but has no animations.", category.AsCharPtr(), file.AsCharPtr());
			QMessageBox::warning(NULL, "Failed to load resource", message.AsCharPtr());
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnSurfaceSelected(const QString& sur)
{
	bool b = true;
    if (this->materialHandler->IsSetup())
    {
		b = this->materialHandler->Discard();
    }
	if (b)
	{ 
		this->materialInfoWindow->show();
		this->materialInfoWindow->raise();
		this->materialInfoWindow->setEnabled(true);
		this->materialHandler->Setup(sur);
	}    
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnAssetBrowserRightClick(QContextMenuEvent* event)
{
	QMenu menu;
	QAction* newSurface = menu.addAction("New surface...");
	menu.addSeparator();
	QAction* importTexture = menu.addAction("Import texture...");
	QAction* importModel = menu.addAction("Import model...");

	QAction* action = menu.exec(event->globalPos());
	if (action == newSurface)
	{
		this->materialHandler->NewSurface();
		this->materialInfoWindow->show();
		this->materialInfoWindow->raise();
	}
	else if (action == importTexture)
	{
		this->ui.actionImport_Texture->trigger();
	}
	else if (action == importModel)
	{
		this->ui.actionImport->trigger();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnAssetBrowserItemRightClick(TiledGraphicsItem* item, QGraphicsSceneContextMenuEvent* event)
{
	QMenu menu;

	String path = item->GetPath();
	String category = item->GetCategory();
	String filename = item->GetFilename();

	// create full resource path
	String res = String::Sprintf("%s/%s/%s", path.AsCharPtr(), category.AsCharPtr(), filename.AsCharPtr());

	// get pointer to io server
	const Ptr<IO::IoServer>& ioServer = IO::IoServer::Instance();

	TiledModelItem* modelItem = dynamic_cast<TiledModelItem*>(item);
	TiledTextureItem* textureItem = dynamic_cast<TiledTextureItem*>(item);
	TiledSurfaceItem* surfaceItem = dynamic_cast<TiledSurfaceItem*>(item);
	if (modelItem)
	{
		QAction* action1 = menu.addAction("Reconfigure model...");
		action1->setToolTip("Adjust settings and reimport this model.");
		QAction* action2 = menu.addAction("Delete model");
		action2->setToolTip("Deletes the model, making any references to it appear as the placeholder model and effectively removes .constants, .attributes and .physics from the working directory.");
		
		// execute menu
		QAction* executedAction = menu.exec(event->screenPos());
		if (executedAction == action1)
		{
			res.ChangeFileExtension("fbx");
			ModelImporter::ModelImporterWindow* importer = ContentBrowser::ContentBrowserApp::Instance()->GetWindow()->GetModelImporter();
			importer->SetUri(res);
			importer->show();
			importer->raise();
			QApplication::processEvents();
			importer->Open();
		}
		else if (executedAction == action2)
		{
			// delete work-counterpart
			res.ChangeFileExtension("fbx");
			ioServer->DeleteFile(res);
			res.ChangeFileExtension("attributes");
			ioServer->DeleteFile(res);
			res.ChangeFileExtension("constants");
			ioServer->DeleteFile(res);
			res.ChangeFileExtension("physics");
			ioServer->DeleteFile(res);

			// remove export target model
			res.ChangeAssignPrefix("mdl");
			res.ChangeFileExtension("n3");
			ioServer->DeleteFile(res);

			// remove export target mesh
			res.ChangeAssignPrefix("msh");
			res.ChangeFileExtension("nvx2");
			ioServer->DeleteFile(res);

			// remove export target animation
			res.ChangeAssignPrefix("ani");
			res.ChangeFileExtension("nax3");
			ioServer->DeleteFile(res);

			AssetBrowser::Instance()->RemoveItem(modelItem);
		}
	}
	else if (textureItem)
	{
		QAction* action1 = menu.addAction("Reconfigure texture...");
		action1->setToolTip("Adjust settings and reimport this texture");
		QAction* action2 = menu.addAction("Delete texture");
		action2->setToolTip("Deletes the texture, both in the working folder and the export (.dds) target");
		

		// execute menu
		QAction* executedAction = menu.exec(event->screenPos());

		// delete texture
		if (executedAction == action1)
		{
			// get resource without prefix and suffix
			String workTex = String::Sprintf("%s/%s", path.AsCharPtr(), category.AsCharPtr());

			// make a string to match the pattern
			String pattern = String::Sprintf("%s.*", filename.AsCharPtr());

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
				box->setText("The texture: " + QString(filename.AsCharPtr()) + " has more than one potential sources. Which one would you want to reconfigure?");
				box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
				box->setDefaultButton(QMessageBox::Ok);
				box->layout()->addWidget(&selections);
				int result = box->exec();

				if (result == QMessageBox::Ok)
				{
					String resource = String::Sprintf("%s/%s/%s", path.AsCharPtr(), category.AsCharPtr(), selections.itemText(selections.currentIndex()).toUtf8().constData());

					// we should move the texture importer to QtAddons since it might be used everywhere
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
			else if (files.Size() > 0)
			{
				String resource = String::Sprintf("%s/%s/%s", path.AsCharPtr(), category.AsCharPtr(), files[0].AsCharPtr());

				TextureImporter::TextureImporterWindow* importer = ContentBrowser::ContentBrowserApp::Instance()->GetWindow()->GetTextureImporter();
				importer->SetUri(resource);
				importer->show();
				importer->raise();
				QApplication::processEvents();
				importer->Open();
			}
			else
			{
				QMessageBox::warning(NULL, "No work resource exists!", "Could not locate this texture in the work directory, has it been moved or deleted?", QMessageBox::Ok);
			}
		}
		else if (executedAction == action2)
		{
			// delete texture
			ioServer->DeleteFile(res);

			String extensions[] = { ".png", ".bmp", ".psd", ".tga", ".jpg", ".dds" };
			IndexT i;
			for (i = 0; i < 6; i++)
			{
				// attempt to remove all files related to this texture
				String resource = String::Sprintf("%s/%s/%s.%s", path.AsCharPtr(), category.AsCharPtr(), filename.AsCharPtr(), extensions[i]);
				ioServer->DeleteFile(resource);
			}

			AssetBrowser::Instance()->RemoveItem(textureItem);
		}
	}
	else if (surfaceItem)
	{
		QAction* action = menu.addAction("Delete surface");
		action->setToolTip("Will render all objects using this surface have the placeholder material applied");

		// execute menu
		QAction* executedAction = menu.exec(event->screenPos());
		if (executedAction == action)
		{
			// delete work
			res.ChangeFileExtension("sur");
			ioServer->DeleteFile(res);

			// delete export target
			res.ChangeAssignPrefix("sur");
			ioServer->DeleteFile(res);

			AssetBrowser::Instance()->RemoveItem(surfaceItem);
		}
	}
}

//------------------------------------------------------------------------------
/**
	Called when a tree widget item is activated (on Windows double-click, on normal OS:es, single click)
*/
void
ContentBrowserWindow::ItemActivated( QTreeWidgetItem* item )
{
	// convert item to base item
	Widgets::BaseItem* baseItem = dynamic_cast<Widgets::BaseItem*>(item);

	// return if we don't have a base item
	if (!baseItem)
	{
		return;
	}

	// create bool to determine if item activation was ok
	bool activated = false;

	// select right item based on base item
	switch (baseItem->GetType())
	{
	case BaseItem::AnimationItemType:
		activated = this->OnAnimationItemActivated(baseItem);
		this->animationInfoWindow->show();
		this->animationInfoWindow->raise();
		this->ui.animationDockFrame->setEnabled(true);
		//this->ui.infoTabWidget->setTabEnabled(4, true);
		//this->ui.infoTabWidget->setCurrentIndex(4);
		break;
	case BaseItem::AudioItemType:
		activated = this->OnAudioItemActivated(baseItem);
		this->audioInfoWindow->show();
		this->audioInfoWindow->raise();
		this->ui.audioDockFrame->setEnabled(true);
		//this->ui.infoTabWidget->setTabEnabled(3, true);
		//this->ui.infoTabWidget->setCurrentIndex(3);
		break;
	case BaseItem::MeshItemType:
		activated = this->OnMeshItemActivated(baseItem);
		this->meshInfoWindow->show();
		this->meshInfoWindow->raise();
		this->ui.meshDockFrame->setEnabled(true);
		//this->ui.infoTabWidget->setTabEnabled(2, true);
		//this->ui.infoTabWidget->setCurrentIndex(2);
		break;
	case BaseItem::TextureItemType:
		activated = this->OnTextureItemActivated(baseItem);
		this->textureInfoWindow->show();
		this->textureInfoWindow->raise();
		this->ui.textureDockFrame->setEnabled(true);
		//this->ui.infoTabWidget->setTabEnabled(1, true);
		//this->ui.infoTabWidget->setCurrentIndex(1);
		break;
	case BaseItem::ModelItemType:
		activated = this->OnModelItemActivated(baseItem);
		this->modelInfoWindow->show();
		this->modelInfoWindow->raise();
		this->ui.modelDockFrame->setEnabled(true);
		//this->ui.infoTabWidget->setTabEnabled(0, true);
		//this->ui.infoTabWidget->setCurrentIndex(0);	
		break;
	case BaseItem::LayoutItemType:
		activated = this->OnUIItemActivated(baseItem);
		this->uiInfoWindow->show();
		this->uiInfoWindow->raise();
		this->ui.uiDockFrame->setEnabled(true);
		//this->ui.infoTabWidget->setTabEnabled(5, true);
		//this->ui.infoTabWidget->setCurrentIndex(5);
		break;
	case BaseItem::FontItemType: // wait, what is this?!
		activated = this->OnFontItemActivated(baseItem);
		this->uiInfoWindow->show();
		this->uiInfoWindow->raise();
		this->ui.uiDockFrame->setEnabled(true);
		//this->ui.infoTabWidget->setTabEnabled(5, true);
		//this->ui.infoTabWidget->setCurrentIndex(5);
		break;
	}

	// if the item is valid
	if (activated)
	{
		baseItem->OnActivated();
	}	
}

//------------------------------------------------------------------------------
/**
*/
bool 
ContentBrowserWindow::OnAnimationItemActivated( Widgets::BaseItem* item )
{
	bool b = true;
	if (this->animationHandler->IsSetup())
	{
		b = this->animationHandler->Discard();
	}

	// delete item if it's already been set
	if (this->animItem)
	{
		delete this->animItem;
	}

	// clone item
	this->animItem = static_cast<AnimationItem*>(item->Clone());

	return b;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ContentBrowserWindow::OnAudioItemActivated( Widgets::BaseItem* item )
{
	bool b = true;
	if (this->audioHandler->IsSetup())
	{
		b = this->audioHandler->Discard();
	}

	// delete item if it's already been set
	if (this->audioItem)
	{
		delete this->audioItem;
	}

	// clone item
	this->audioItem = static_cast<AudioItem*>(item->Clone());

	return b;
}

//------------------------------------------------------------------------------
/**
*/
bool
ContentBrowserWindow::OnUIItemActivated(Widgets::BaseItem* item)
{
	bool b = true;
	if (this->uiHandler->IsSetup())
	{
		b = this->uiHandler->Discard();
	}

	// delete item if it's already been set
	if (this->layoutItem)
	{
		delete this->layoutItem;
	}

	// clone item
	this->layoutItem = static_cast<LayoutItem*>(item->Clone());

	return b;
}


//------------------------------------------------------------------------------
/**
*/
bool
ContentBrowserWindow::OnFontItemActivated(Widgets::BaseItem* item)
{
	bool b = true;
	if (this->uiHandler->IsSetup())
	{
		b = this->uiHandler->Discard();
	}

	// delete item if it's already been set
	if (this->fontItem)
	{
		delete this->fontItem;
	}

	// clone item
	this->fontItem = static_cast<FontItem*>(item->Clone());

	return b;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ContentBrowserWindow::OnMeshItemActivated( Widgets::BaseItem* item )
{
	bool b = true;
	if (this->meshHandler->IsSetup())
	{
		b = this->meshHandler->Discard();
	}		

	// delete item if it's already been set
	if (this->meshItem)
	{
		delete this->meshItem;
	}

	// clone item
	this->meshItem = static_cast<MeshItem*>(item->Clone());

	return b;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ContentBrowserWindow::OnModelItemActivated( Widgets::BaseItem* item )
{
	bool b = true;
	if (this->modelHandler->IsSetup())
	{
		b = this->modelHandler->Discard();
	}
	
	// delete item if it's already been set
	if (this->modelItem)
	{
		delete this->modelItem;
	}

	// clone item
	this->modelItem = static_cast<ModelItem*>(item->Clone());

	return b;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ContentBrowserWindow::OnTextureItemActivated( Widgets::BaseItem* item )
{
	bool b = true;
	if (this->textureHandler->IsSetup())
	{
		b = this->textureHandler->Discard();
	}

	// delete item if it's already been set
	if (this->textureItem)
	{
		delete this->textureItem;
	}

	// clone item
	this->textureItem = static_cast<TextureItem*>(item->Clone());

	return b;
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::RemoveAnimationItem( Widgets::AnimationItem* item )
{
	if (item->parent())	item->parent()->removeChild(item);
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::RemoveAudioItem( Widgets::AudioItem* item )
{
	if (item->parent())	item->parent()->removeChild(item);
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::RemoveMeshItem( Widgets::MeshItem* item )
{
	if (item->parent())	item->parent()->removeChild(item);
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::RemoveModelItem( Widgets::ModelItem* item )
{
    // deparent item
    if (item->parent())	item->parent()->removeChild(item);
	if (this->modelItem && this->modelItem->GetName() == item->GetName())
	{
		this->SelectItem("models", "system", "placeholder.n3");
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::RemoveModelItemAndAssociated(Widgets::ModelItem* item)
{
	// deparent item
	if (item->parent())	item->parent()->removeChild(item);
	if (this->modelItem && this->modelItem->GetName() == item->GetName())
	{
		this->SelectItem("models", "system", "placeholder.n3");
	}

	String res = item->GetName().toUtf8().constData();
	IndexT slashLocation = res.FindCharIndex('/');
	String cat = res.ExtractRange(0, slashLocation);
	String file = res.ExtractToEnd(slashLocation+1);
	file.StripFileExtension();

	// find items
	QTreeWidgetItem* topLevelItem = this->libraryTree->topLevelItem(0);

	QTreeWidgetItem* assocItem = NULL;
	String search;
	search.Format("animations/%s/%s.nax3", cat.AsCharPtr(), file.AsCharPtr());
	assocItem = this->libraryTree->FindItem(topLevelItem, search.AsCharPtr());
	if (assocItem && assocItem->parent()) assocItem->parent()->removeChild(assocItem); 

	search.Format("meshes/%s/%s.nvx2", cat.AsCharPtr(), file.AsCharPtr());
	assocItem = this->libraryTree->FindItem(topLevelItem, search.AsCharPtr());
	if (assocItem && assocItem->parent()) assocItem->parent()->removeChild(assocItem);
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::RemoveTextureItem( Widgets::TextureItem* item )
{
	if (item->parent())	item->parent()->removeChild(item);
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::ItemClicked( QTreeWidgetItem* item )
{
	// get base item
	Widgets::BaseItem* actItem = dynamic_cast<Widgets::BaseItem*>(item);
	if (actItem)
	{
		actItem->OnClicked();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::ItemChanged()
{
	// update library
	//this->UpdateLibrary("");
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnUndo()
{
	ContentBrowserApp::Instance()->Undo();
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnRedo()
{
	ContentBrowserApp::Instance()->Redo();
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::SaveLibraryState()
{
	// create item iterator
	QTreeWidgetItemIterator it(this->ui.libraryTree);

	// go through each item
	while (*it)
	{
		this->libraryState.append((*it)->isExpanded());
		it++;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::RestoreLibraryState()
{
	// create item iterator
	QTreeWidgetItemIterator it(this->ui.libraryTree);

	// go through each item
	while (*it && !this->libraryState.isEmpty())
	{
		(*it)->setExpanded(this->libraryState.takeFirst());
		it++;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::SelectItem( const Util::String& topLevel, const Util::String& category, const Util::String& file, bool expand )
{
	// get 'Library' item
	QTreeWidgetItem* topLevelItem = this->ui.libraryTree->topLevelItem(0);

	IndexT i;
	for (i = 0; i < topLevelItem->childCount(); i++)
	{
		// get items
		QTreeWidgetItem* resourceItem = topLevelItem->child(i);

		if (resourceItem->text(0) == topLevel.AsCharPtr())
		{
			// go through categories and look for category
			IndexT j;
			for (j = 0; j < resourceItem->childCount(); j++)
			{
				// get items
				QTreeWidgetItem* item = resourceItem->child(j);

				if (item->text(0) == category.AsCharPtr())
				{
					IndexT k;
					for (k = 0; k < item->childCount(); k++)
					{
						// get child
						QTreeWidgetItem* child = item->child(k);

						// now, if child matches file, we're golden
						if (child->text(0) == file.AsCharPtr())
						{
							if (expand)
							{
								this->ui.libraryTree->setCurrentItem(child);
							}							
							BaseItem* item = static_cast<BaseItem*>(child);
							this->ItemActivated(item);
							return;
						}
					}

					// break loop, we can't have duplicate categories
					break;
				}
			}
			break;
		}
	}
}


//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::SelectFolder( const Util::String& topLevel, const Util::String& category, bool expand /*= false*/ )
{
    // get 'Library' item
    QTreeWidgetItem* topLevelItem = this->ui.libraryTree->topLevelItem(0);

    IndexT i;
    for (i = 0; i < topLevelItem->childCount(); i++)
    {
        // get items
        QTreeWidgetItem* resourceItem = topLevelItem->child(i);

        if (resourceItem->text(0) == topLevel.AsCharPtr())
        {
            // go through categories and look for category
            IndexT j;
            for (j = 0; j < resourceItem->childCount(); j++)
            {
                // get items
                QTreeWidgetItem* item = resourceItem->child(j);

                if (item->text(0) == category.AsCharPtr())
                {
                    if (expand)
                    {
                        this->ui.libraryTree->setCurrentItem(item);
                    }

                    // break loop, we can't have duplicate categories
                    break;
                }
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::dropEvent( QDropEvent* e )
{
	// get mime data
	const QMimeData* data = e->mimeData();

	// only start importers if data contains URLs
	if (data->hasUrls())
	{
		// get urls
		QList<QUrl> urls = data->urls();

		// only get first URL
		const QUrl& url = urls[0];
		String file = url.toLocalFile().toUtf8().constData();
		if (file.CheckFileExtension("fbx"))
		{
			this->modelImporterWindow->SetUri(file);
			this->modelImporterWindow->show();
			this->modelImporterWindow->raise();
			QApplication::processEvents();
			this->modelImporterWindow->Open();
		}
		else if (file.CheckFileExtension("png") || 
				 file.CheckFileExtension("jpg") ||
				 file.CheckFileExtension("bmp") ||
				 file.CheckFileExtension("psd") ||
				 file.CheckFileExtension("tga") ||
				 file.CheckFileExtension("dds"))
		{
			this->textureImporterWindow->SetUri(file);
			this->textureImporterWindow->show();
			this->textureImporterWindow->raise();
			QApplication::processEvents();
			this->textureImporterWindow->Open();
		}
	}
	else if (data->hasFormat("nebula/resourceid"))
	{
		Util::String res = data->data("nebula/resourceid").constData();
		Util::Array<Util::String> toks = res.Tokenize("/");
		this->SelectItem("models", toks[0], toks[1] + ".n3", true);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::dragEnterEvent( QDragEnterEvent* e )
{
	// get mime data
	const QMimeData* data = e->mimeData();

	// only start importers if data contains URLs
	if (data->hasUrls() || data->hasFormat("nebula/resourceid"))
	{
		e->accept();
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnShowModelInfo()
{
	this->modelInfoWindow->show();
	this->modelInfoWindow->raise();
	/*
	if (this->ui.infoTabWidget->isTabEnabled(0))
	{
		this->ui.infoTabWidget->setCurrentIndex(0);
	}
	*/
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnShowTextureInfo()
{
	this->textureInfoWindow->show();
	this->textureInfoWindow->raise();
	/*
	if (this->ui.infoTabWidget->isTabEnabled(1))
	{
		this->ui.infoTabWidget->setCurrentIndex(1);
	}
	*/
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnShowMeshInfo()
{
	this->meshInfoWindow->show();
	this->meshInfoWindow->raise();
	/*
	if (this->ui.infoTabWidget->isTabEnabled(2))
	{
		this->ui.infoTabWidget->setCurrentIndex(2);
	}
	*/
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnShowAudioInfo()
{
	this->audioInfoWindow->show();
	this->audioInfoWindow->raise();
	/*
	if (this->ui.infoTabWidget->isTabEnabled(3))
	{
		this->ui.infoTabWidget->setCurrentIndex(3);
	}
	*/
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnShowAnimationInfo()
{
	this->animationInfoWindow->show();
	this->animationInfoWindow->raise();
	/*
	if (this->ui.infoTabWidget->isTabEnabled(4))
	{
		this->ui.infoTabWidget->setCurrentIndex(4);
	}
	*/
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnShowUIInfo()
{
	this->uiInfoWindow->show();
	this->uiInfoWindow->raise();
	/*
	if (this->ui.infoTabWidget->isTabEnabled(5))
	{
		this->ui.infoTabWidget->setCurrentIndex(5);
	}
	*/
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnShowPostEffectController()
{
	this->postEffectController->show();
	this->postEffectController->raise();
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnShowAssetBrowser()
{
	this->assetBrowserWindow->show();
	this->assetBrowserWindow->raise();
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnShowEnvironmentProbeSettings()
{
	this->environmentProbeWindow->show();
	this->environmentProbeWindow->raise();
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnShowGrid()
{
	Grid::GridAddon::Instance()->SetVisible(this->ui.actionShow_grid->isChecked());
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnSaveAll()
{
	if (this->materialHandler->IsSetup()) this->materialHandler->Save();
	if (this->modelHandler->IsSetup()) this->modelHandler->OnSave();
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnCreateParticleEffect()
{
	// show and raise dialog
	int result = this->particleEffectWizard.exec();

	if (result == QDialog::Accepted)
	{
		// get particle name and category
		String origName = this->particleWizardUi.particleName->text().toUtf8().constData();
		String cat = this->particleWizardUi.particleCategory->text().toUtf8().constData();
		String mesh = this->particleWizardUi.emitterMesh->text().toUtf8().constData();

		// fix mesh name if empty
		if (mesh.IsEmpty()) mesh.Format("msh:system/plane.nvx2");

		// if this happens, trigger a new execution of this function
		if (origName.IsEmpty() || cat.IsEmpty() || !IoServer::Instance()->FileExists(mesh))
		{
			this->particleWizardUi.errorLabel->setHidden(false);
			this->ui.actionCreate_Particle->trigger();
			return;
		}
		else
		{
			this->particleWizardUi.errorLabel->setHidden(true);
		}

		String name = origName;		
		String combined = cat + "/" + name;
		String res = "mdl:" + combined + ".n3";
		String physRes = "phys:" + combined + ".np3";
		String work = "src:assets/" + combined;
		String constants = work + ".constants";
		String attributes = work + ".attributes";
		String physics = work + ".physics";

		result = IoServer::Instance()->CreateDirectory("src:assets/" + cat);
		n_assert(result);

		// increase resource counter if resource already exists
		IndexT i = 0;
		while (IoServer::Instance()->FileExists(constants) || 
			   IoServer::Instance()->FileExists(attributes) || 
			   IoServer::Instance()->FileExists(physics))
		{
			name  = origName + String::FromInt(i++);
			combined = cat + "/" + name;
			res = "mdl:" + combined + ".n3";
			physRes = "phys:" + combined + ".np3";
			work = "src:assets/" + combined;

			constants = work + ".constants";
			attributes = work + ".attributes";
			physics = work + ".physics";
		}

		// waah, create new particle effect!
		Ptr<ModelDatabase> modelDatabase = ModelDatabase::Instance();

		// create new constants and attributes
		Ptr<ModelAttributes> attrs = modelDatabase->LookupAttributes(combined);
		Ptr<ModelConstants> consts = modelDatabase->LookupConstants(combined);
		Ptr<ModelPhysics> phys = modelDatabase->LookupPhysics(combined);

		// reformat mesh string to have patmsh prefix
		String emitterMesh = mesh;
		emitterMesh.SubstituteString("msh:", "parmsh:");

		// we should now have an empty particle effect, so now we need a node
		ModelAttributes::AppendixNode node;
		node.name = "node_0";
		node.type = ModelAttributes::ParticleNode;
		node.path = "root/" + node.name;
		node.data.particle.primGroup = 0;

		// now add particle node to constants
		attrs->AddAppendixNode(node.name, node);

		// create state
		State state;
		state.material = "sur:system/particle_unlit";
		state.textures.Append(Texture("DiffuseMap", "tex:system/nebulalogo"));
		state.textures.Append(Texture("NormalMap", "tex:system/nobump"));

		// set state in attributes for this node
		attrs->SetState(node.path, state);

		// create set of emitter attributes, setup defaults
		EmitterAttrs emitterAttrs;
		emitterAttrs.SetBool(EmitterAttrs::Looping, true);
		emitterAttrs.SetFloat(EmitterAttrs::EmissionDuration, 25.0f);
		emitterAttrs.SetFloat(EmitterAttrs::Gravity, -9.82f);
		emitterAttrs.SetFloat(EmitterAttrs::ActivityDistance, 100.0f);
		emitterAttrs.SetFloat(EmitterAttrs::StartRotationMin, 0.0f);
		emitterAttrs.SetFloat(EmitterAttrs::StartRotationMax, 180.0f);
		emitterAttrs.SetInt(EmitterAttrs::AnimPhases, 1);
		EnvelopeCurve lifeTime;
		lifeTime.Setup(1, 1, 1, 1, 0.33f, 0.66f, 0, 0, EnvelopeCurve::Sine);
		EnvelopeCurve emissionFrequency;
		emissionFrequency.Setup(25, 25, 25, 25, 0.33f, 0.66f, 1, 1, EnvelopeCurve::Sine);
		emissionFrequency.SetLimits(0, 25);
		EnvelopeCurve alpha;
		alpha.Setup(0, 0.5f, 0.5f, 0, 0.33f, 0.66f, 1, 0, EnvelopeCurve::Sine);
		EnvelopeCurve size;
		size.Setup(0, 0.5f, 0.5f, 0.0f, 0.33f, 0.66f, 1, 1, EnvelopeCurve::Sine);
		EnvelopeCurve color;
		color.Setup(1, 1, 1, 1, 0.33f, 0.66f, 0, 0, EnvelopeCurve::Sine);
		emitterAttrs.SetEnvelope(EmitterAttrs::LifeTime, lifeTime);
		emitterAttrs.SetEnvelope(EmitterAttrs::VelocityFactor, lifeTime);
		emitterAttrs.SetEnvelope(EmitterAttrs::EmissionFrequency, emissionFrequency);
		emitterAttrs.SetEnvelope(EmitterAttrs::Alpha, alpha);
		emitterAttrs.SetEnvelope(EmitterAttrs::Size, size);
		emitterAttrs.SetEnvelope(EmitterAttrs::Alpha, color);
		emitterAttrs.SetEnvelope(EmitterAttrs::Red, color);
		emitterAttrs.SetEnvelope(EmitterAttrs::Green, color);
		emitterAttrs.SetEnvelope(EmitterAttrs::Blue, color);

		// set attributes
		attrs->SetEmitterAttrs(node.path, emitterAttrs);
		attrs->SetEmitterMesh(node.path, emitterMesh);

		// set physics stuff
		phys->SetName(name);
		phys->SetPhysicsMesh(mesh);

		// create stream for constants
		Ptr<Stream> stream = IoServer::Instance()->CreateStream(constants);

		// save constants
		consts->Save(stream);

		// create stream for attributes
		stream = IoServer::Instance()->CreateStream(attributes);

		// save attributes
		attrs->Save(stream);

		// save physics
		stream = IoServer::Instance()->CreateStream(physics);

		// save physics
		phys->Save(stream);

		// ok, now write
		Ptr<ModelBuilder> builder = ModelBuilder::Create();
		builder->SetConstants(consts);
		builder->SetAttributes(attrs);
		builder->SetPhysics(phys);
		builder->SaveN3(res, Platform::Win32);
		builder->SaveN3Physics(physRes, Platform::Win32);

		// finally update library
		this->UpdateLibrary();

		// then select the resource
		this->SelectItem("models", cat, name + ".n3", true);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnBrowseEmitterMesh()
{
	// LOL, implement!
}

//------------------------------------------------------------------------------
/**
	Move to specific item!
*/
void 
ContentBrowserWindow::TreeRightClicked( const QPoint& point )
{
	// get item at position
	QTreeWidgetItem* item = this->ui.libraryTree->itemAt(point);

	// cast to base item
	BaseItem* baseItem = dynamic_cast<BaseItem*>(item);

	// if we have an item here, perform the context menu
	if (baseItem)
	{
		// call base item function
		baseItem->OnRightClicked(point);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnConnectToLevelEditor()
{
	// connect remote client
	if (!QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->IsOpen())
	{
		bool b = QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Open();

		// update text
		if (b)
		{
			this->ui.connectionStatus->setText("<b><span style=\"color:#5bac26\">Connected with Level Editor</span><\b>");
		}
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnDisconnectFromLevelEditor()
{
	this->ui.connectionStatus->setText("<b><span style=\"color:#FF7272\">Not connected with Level Editor</span><\b>");
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnAbout()
{
    QString txt = "Nebula Trifid - Contentbrowser\n";
    txt += NEBULA_TOOLKIT_VERSION;
    QMessageBox::about(this,"About",txt);
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserWindow::OnDebugPage()
{    
    QDesktopServices::openUrl(QUrl("http://127.0.0.1:2101"));
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::OnFrame()
{
	if (this->animationHandler.isvalid() && this->animationHandler->IsSetup()) this->animationHandler->OnFrame();
	if (this->audioHandler.isvalid() && this->audioHandler->IsSetup()) this->audioHandler->OnFrame();
	if (this->uiHandler.isvalid() && this->uiHandler->IsSetup()) this->uiHandler->OnFrame();
	if (this->meshHandler.isvalid() && this->meshHandler->IsSetup()) this->meshHandler->OnFrame();
	if (this->modelHandler.isvalid() && this->modelHandler->IsSetup()) this->modelHandler->OnFrame();
	if (this->textureHandler.isvalid() && this->textureHandler->IsSetup()) this->textureHandler->OnFrame();
	if (this->materialHandler.isvalid() && this->materialHandler->IsSetup()) this->materialHandler->OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
ContentBrowserWindow::ResetLayout()
{
	// restore the state of the window and all dock widgets
	IO::URI path = String::Sprintf("toolkit:data/cbdefault.ini");
	QSettings settings(path.LocalPath().AsCharPtr(), QSettings::IniFormat);
	this->restoreGeometry(settings.value("geometry").toByteArray());
	this->restoreState(settings.value("windowState").toByteArray());
	this->assetBrowserWindow->raise();
	this->assetBrowserWindow->show();
}

} // namespace ContentBrowser
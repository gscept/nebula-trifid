//------------------------------------------------------------------------------
//  leveleditor2window.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "leveleditor2window.h"
#include "leveleditor2app.h"
#include "io/ioserver.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include <QPlastiqueStyle>
#include <QApplication>
#include <QDesktopServices>
#include <QSettings>
#include <QtGui/QGroupBox>
#include "qinputdialog.h"
#include "leveleditor2entitymanager.h"
#include "qfiledialog.h"
#include "level.h"
#include "levelpropertieshandler.h"
#include "blueprinthandler.h"
#include "applauncher.h"
#include "qmessagebox.h"
#include "remoteinterface/qtremoteclient.h"
#include "remoteinterface/qtremoteprotocol.h"
#include "toolkitversion.h"
#include "qurl.h"
#include "scripteditor.h"
#include "grid/gridaddon.h"
#include "lighting/lightprobemanager.h"
#include "qcolordialog.h"
#include "uidialoghandler.h"
#include "properties/editorproperty.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "game/templateexporter.h"
#include "db/dbfactory.h"
#include "game/gameexporter.h"
#include "entityutils/placementutil.h"

using namespace Util;
using namespace IO;
using namespace Graphics;
using namespace Toolkit;
using namespace Attr;

namespace LevelEditor2
{
//------------------------------------------------------------------------------
/**
*/
LevelEditor2Window::LevelEditor2Window():
	ctrlPressed(false),
	gridVisible(true)
{
    this->ui.setupUi(this);
    QApplication::setStyle(new QPlastiqueStyle);
    this->setMouseTracking(true);
	this->setAcceptDrops(true);

    this->createEntityDialog = new CreateEntityDialogHandler(this);
    this->levelPropertiesDialog = new LevelPropertiesHandler(this);
    this->bluePrintsDialog = new BlueprintsHandler(this);
	this->audioDialog = new AudioDialogHandler(this);	
	this->uiDialog = new UIDialogHandler(this);
	this->layerHandler = Layers::LayerHandler::Create();
	this->layerHandler->SetUI(&this->ui);
	this->layerHandler->Setup();

    gridSizeDialog = new QDialog((QWidget*)this);
    gridSizeUi.setupUi(gridSizeDialog);

    this->attributeControllerVLayout = new QVBoxLayout(this->ui.scrollAreaWidgetContents);
    this->scriptEditor = new QtAttributeControllerAddon::ScriptEditor;

	// create texture browser window
	this->assetBrowserWindow = ResourceBrowser::AssetBrowser::Create();
	this->assetBrowserWindow->setParent(this);
	this->assetBrowserWindow->Open();

    this->ui.scrollAreaWidgetContents->setLayout(this->attributeControllerVLayout);    

	connect(this->ui.actionAdd_Environment_Entity, SIGNAL(triggered()), this, SLOT(OnNewEnvironmentEntity()));
	connect(this->ui.actionAdd_Light_Entity, SIGNAL(triggered()), this, SLOT(OnNewLightEntity()));
	connect(this->ui.actionAdd_Game_Entity, SIGNAL(triggered()), this, SLOT(OnNewGameEntity()));
	connect(this->ui.actionLight_probes_2, SIGNAL(triggered()), LightProbeManager::Instance(), SLOT(Show()));
    connect(this->ui.actionUndo, SIGNAL(triggered()), this, SLOT(OnUndo()));
    connect(this->ui.actionDelete, SIGNAL(triggered()), this, SLOT(OnDelete()));
    connect(this->ui.actionSave, SIGNAL(triggered()), this, SLOT(OnSave()));
    connect(this->ui.actionSave_As, SIGNAL(triggered()), this, SLOT(OnSaveAs()));
    connect(this->ui.actionLoad, SIGNAL(triggered()), this, SLOT(OnLoad()));
    connect(this->ui.actionNew, SIGNAL(triggered()), this, SLOT(OnNew()));
    connect(this->ui.actionShow_PostEffect_Controls, SIGNAL(triggered()), this, SLOT(OnShowPostEffect()));
    connect(this->ui.actionShow_Level_Properties, SIGNAL(triggered()), this, SLOT(OnShowLevelProperties()));
    connect(this->ui.actionGrid, SIGNAL(triggered()), this, SLOT(OnShowGridProperties()));
    connect(this->ui.actionShow_Blueprint_editor, SIGNAL(triggered()), this, SLOT(OnShowBlueprintEditor()));
	connect(this->ui.actionSelection_Colour, SIGNAL(triggered()), this, SLOT(OnChangeSelectionColour()));
    connect(this->ui.actionTest_in_Level_Viewer, SIGNAL(triggered()), this, SLOT(OnTestLevelViewer()));
	connect(this->ui.actionHide_grid, SIGNAL(triggered()), this, SLOT(OnGridVisible()));
	connect(this->ui.actionReset_window_layout, SIGNAL(triggered()), this, SLOT(OnResetWindows()));
	connect(this->ui.actionPerformance_overlay, SIGNAL(triggered()), this, SLOT(OnTogglePerformanceUI()));
	connect(this->ui.actionShow_global_light_probe, SIGNAL(triggered()), this, SLOT(OnShowEnvironmentProbeSettings()));
    connect(this->ui.actionPhysics_Materials, SIGNAL(triggered()), this, SLOT(OnShowPhysicsMaterials()));

    connect(this->gridSizeUi.GridSize, SIGNAL(valueChanged(double)),this, SLOT(OnGridSizeChanged(double)));


    connect(this->ui.actionDuplicate, SIGNAL(triggered()), LevelEditor2App::Instance(), SLOT(DuplicateCurrentSelection()));
    connect(this->ui.actionGroup, SIGNAL(triggered()), LevelEditor2App::Instance(), SLOT(GroupSelection()));

    connect(this->ui.actionTranslate, SIGNAL(triggered()), LevelEditor2App::Instance(), SLOT(ToggleTranslateFeature()));
    connect(this->ui.actionRotate, SIGNAL(triggered()), LevelEditor2App::Instance(), SLOT(ToggleRotateFeature()));
    connect(this->ui.actionScale, SIGNAL(triggered()), LevelEditor2App::Instance(), SLOT(ToggleScaleFeature()));

    connect(this->ui.actionFocus, SIGNAL(triggered()), LevelEditor2App::Instance(), SLOT(FocusOnSelection()));
    connect(this->ui.actionCenter, SIGNAL(triggered()), LevelEditor2App::Instance(), SLOT(CenterOnSelection()));

    connect(this->ui.actionCreateNavMesh, SIGNAL(triggered()), LevelEditor2App::Instance(), SLOT(CreateNavMesh()));
    connect(this->ui.actionUpdateNavMesh, SIGNAL(triggered()), LevelEditor2App::Instance(), SLOT(UpdateNavMesh()));
    connect(this->ui.actionAddNavArea, SIGNAL(triggered()), LevelEditor2App::Instance(), SLOT(AddNavArea()));

    connect(this->ui.actionAbout, SIGNAL(triggered()), this, SLOT(OnAbout()));
    connect(this->ui.actionDebug_Page, SIGNAL(triggered()), this, SLOT(OnDebugPage()));    
    connect(this->ui.actionNebulaApplicationPage, SIGNAL(triggered()), this, SLOT(OnApplicationDebugPage()));    

    connect(this->ui.actionShow_Script_Editor, SIGNAL(triggered()), this->scriptEditor, SLOT(show()));
	connect(this->ui.actionAudio_Properties, SIGNAL(triggered()), this, SLOT(OnShowAudioProperties()));
	connect(this->ui.actionUI_Properties, SIGNAL(triggered()), this, SLOT(OnShowUIProperties()));
	connect(this->ui.actionCreate_template_from_selection, SIGNAL(triggered()), this, SLOT(OnCreateTemplate()));
	connect(this->ui.actionBatch_game_data, SIGNAL(triggered()), this, SLOT(OnBatchGame()));
	connect(this->ui.actionCenter_Group_Pivot, SIGNAL(triggered()), this, SLOT(OnCenterPivot()));

	connect(this->ui.actionExport_Selection, SIGNAL(triggered()), this, SLOT(OnExportSelection()));
	connect(this->ui.actionImport, SIGNAL(triggered()), this, SLOT(OnImport()));
	connect(this->ui.actionImport_Level, SIGNAL(triggered()), this, SLOT(OnImportLevel()));
	connect(this->ui.actionExport_Selection_as_Level, SIGNAL(triggered()), this, SLOT(OnExportSelectionLevel()));
    connect(this->ui.actionAdd_Level_Reference, SIGNAL(triggered()), this, SLOT(OnAddReference()));
    connect(this->ui.actionBatch_on_save, SIGNAL(toggled(bool)), this, SLOT(OnBatchToggle(bool)));

    

    this->addAction(this->ui.actionDuplicate);
    this->addAction(this->ui.actionGroup);
    this->addAction(this->ui.actionTranslate);
    this->addAction(this->ui.actionRotate);
    this->addAction(this->ui.actionScale);
    this->addAction(this->ui.actionFocus);
    this->addAction(this->ui.actionCenter);
    this->addAction(this->ui.actionCreateNavMesh);
    this->addAction(this->ui.actionUpdateNavMesh);

    this->ui.centralwidget->update();
    this->SetWindowTitle("Untitled");

    this->progressBar = new QProgressBar();
    this->progressBar->setFixedSize(300, 15);    
    this->statusBar()->addPermanentWidget(this->progressBar);

	connect(this->ui.menu_Scripts, SIGNAL(triggered(QAction*)), LevelEditor2App::Instance(), SLOT(ScriptAction(QAction*)));

}

//------------------------------------------------------------------------------
/**
*/
LevelEditor2Window::~LevelEditor2Window()
{
	this->layerHandler->Discard();
	this->layerHandler = 0;
    delete this->scriptEditor;
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::showEvent(QShowEvent* e)
{
	QMainWindow::showEvent(e);
	if (this->defaultState.size() == 0)
	{
		defaultState = this->saveState(1);
	}		
	// restore the state of the window and all dock widgets
	QSettings settings("gscept", "Level editor");
	this->restoreGeometry(settings.value("geometry").toByteArray());
	this->restoreState(settings.value("windowState").toByteArray(), 0);
	this->assetBrowserWindow->Open();
    this->assetBrowserWindow->close();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::closeEvent(QCloseEvent *e)
{
	LevelEditor2App::Instance()->RequestState("Exit");

	this->environmentProbeWindow->close();
	delete this->environmentProbeWindow;

	this->assetBrowserWindow->close();
	this->assetBrowserWindow = 0;

	// restore the state of the window and all dock widgets
	QSettings settings("gscept", "Level editor");
	settings.setValue("geometry", this->saveGeometry());
	settings.setValue("windowState", this->saveState(0));
	QMainWindow::closeEvent(e);
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::Setup()
{
	// create environment probe window, this must happen after we setup the graphics subsystem
	this->environmentProbeWindow = new Lighting::EnvironmentProbeWindow;
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnUndo()
{
	if (ActionManager::Instance()->CanUndo())
	{
		ActionManager::Instance()->UndoLatestAction();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnSave()
{		
	Level::Instance()->SaveLevel();
}


//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnSaveAs()
{
	IO::URI path;
	if (this->PickLevelFile("Save Level as", "work:levels", path, true))
	{
		Util::String full = path.GetTail().ExtractFileName();
		
		full.StripFileExtension();

		Level::Instance()->SaveLevelAs(full);
		this->ui.actionSave->setEnabled(true);
		this->SetWindowTitle(full);
	}		
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnExportSelectionLevel()
{
	IO::URI path;
	if (this->PickLevelFile("Export Selection as", "work:levels", path, true))
	{		
		Util::String full = path.GetTail().ExtractFileName();

		full.StripFileExtension();

		Level::Instance()->SaveSelection(full);		
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnDelete()
{
	this->GetEntityTreeWidget()->SelectAllChildren();
	ActionManager::Instance()->RemoveCurrentSelection();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnNew()
{	
	this->layerHandler->Discard();
	LevelEditor2EntityManager::Instance()->RemoveAllEntities();		
	ActionManager::Instance()->ClearStack();
	this->ui.actionSave->setEnabled(false);
	LevelEditor2EntityManager::Instance()->CreateLightEntity("GlobalLight");
	this->postEffectController.ActivatePrefix("Default");
	this->SetWindowTitle("Untitled");
	this->layerHandler->Setup();
	Level::Instance()->Clear();
    LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->ClearReferences();

}

//------------------------------------------------------------------------------
/**
*/
bool
LevelEditor2Window::PickLevelFile(const Util::String & title, const Util::String & folder, IO::URI & file, bool save)
{
	
	QFileDialog fileDialog(this, title.AsCharPtr(), IO::URI(folder).GetHostAndLocalPath().AsCharPtr(), "*.xml");
	if (save)
	{
		fileDialog.setAcceptMode(QFileDialog::AcceptSave);
		fileDialog.setOptions(QFileDialog::HideNameFilterDetails);
	}
	else
	{
		fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
		fileDialog.setOptions(QFileDialog::HideNameFilterDetails | QFileDialog::ReadOnly);
	}
	fileDialog.setNameFilterDetailsVisible(false);
	fileDialog.setDefaultSuffix("xml");
	if (fileDialog.exec() == QDialog::Accepted)
	{
		QString fullPath = fileDialog.selectedFiles()[0];
		file.Set(fullPath.toUtf8().constData());
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnLoad()
{
	IO::URI path;
	if(this->PickLevelFile("Load Level", "work:levels", path, false))
	{
		Util::String full = path.GetTail().ExtractFileName();
		full.StripFileExtension();

		LevelEditor2EntityManager::Instance()->LoadLevel(full, Level::Replace);		
		this->ui.actionSave->setEnabled(true);
		this->SetWindowTitle(full);
		ActionManager::Instance()->ClearStack();
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnImportLevel()
{
	IO::URI path;
	if (this->PickLevelFile("Import Level", "work:level", path, false))
	{
		Util::String full = path.GetTail().ExtractFileName();
		full.StripFileExtension();

		Level::Instance()->LoadLevel(full, Level::Merge);
		this->ui.actionSave->setEnabled(true);		
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnAddReference()
{
    IO::URI path;
    if (this->PickLevelFile("Import Level", "work:levels", path, false))
    {
        Util::String full = path.GetTail().ExtractFileName();
        full.StripFileExtension();

        Level::Instance()->LoadLevel(full, Level::Reference);
        this->ui.actionSave->setEnabled(true);        
    }
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnBatchToggle(bool batch)
{
    Level::Instance()->SetAutoBatch(batch);
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnImport()
{
	IO::URI path;
	if (this->PickLevelFile("Import Fragment", "work:levelfragments", path, false))
	{
		Level::Instance()->LoadEntities(path);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::keyPressEvent(QKeyEvent* e)
{
	if (Qt::Key_Control == e->key())
	{
		ctrlPressed = true;
	}
	this->ui.nebulaFrame->keyPressEvent(e);
	QMainWindow::keyPressEvent(e);
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::keyReleaseEvent(QKeyEvent* e)
{
	if (Qt::Key_Control == e->key())
	{
		ctrlPressed = false;
	}
	this->ui.nebulaFrame->keyReleaseEvent(e);
	QMainWindow::keyReleaseEvent(e);
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::dragEnterEvent(QDragEnterEvent* e)
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
LevelEditor2Window::dropEvent(QDropEvent* e)
{
	// get mime data
	const QMimeData* data = e->mimeData();
	if (data->hasFormat("nebula/resourceid"))
	{
		Util::String res = data->data("nebula/resourceid").constData();
		Util::Array<Util::String> toks = res.Tokenize("/");
		EntityGuid guid;
		Util::String errorMsg;
		ActionManager::Instance()->CreateEntity(Environment, toks[0], toks[1], guid, errorMsg);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnExportSelection()
{
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	// make sure the directory exists
	ioServer->CreateDirectory("work:levelfragments");
	IO::URI path;
	if(this->PickLevelFile("Export Fragment as", "work:levelfragments", path, true))
	{				
		Util::Array<Ptr<Game::Entity>> selected = SelectionUtil::Instance()->GetSelectedEntities();
		Level::Instance()->SaveEntityArray(selected, path);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnNewEnvironmentEntity()
{
	if (this->ctrlPressed)
	{
		this->ForceReleaseCtrlKey();
	}

	this->createEntityDialog->NewEnvironmentEntity();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnNewLightEntity()
{
	if (this->ctrlPressed)
	{
		this->ForceReleaseCtrlKey();
	}

	this->createEntityDialog->NewLightEntity();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnShowPostEffect()
{		
	this->postEffectController.show();
	this->postEffectController.raise();
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnShowEnvironmentProbeSettings()
{
	this->environmentProbeWindow->show();
	this->environmentProbeWindow->raise();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnNewGameEntity()
{
	if (this->ctrlPressed)
	{
		this->ForceReleaseCtrlKey();
	}

	this->createEntityDialog->NewGameEntity();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::ForceReleaseCtrlKey()
{
	n_assert(this->ctrlPressed);
	
	//TODO: must be fixed, the following does not work
	QApplication::postEvent(this, 
		new QKeyEvent(QEvent::KeyRelease, Qt::Key_Control, Qt::NoModifier));
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnShowLevelProperties()
{
	this->levelPropertiesDialog->UpdateValues();
	this->levelPropertiesDialog->show();
	this->levelPropertiesDialog->raise();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnShowGridProperties()
{
	this->gridSizeDialog->show();
	this->gridSizeDialog->raise();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnGridSizeChanged(double val)
{
	Grid::GridAddon::Instance()->SetGridSize((float)val);
	PlacementUtil::Instance()->GetPlacementFeature(PlacementUtil::TRANSLATE).cast<TranslateFeature>()->SetSnapOffset(val);
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnGridVisible()
{
	this->gridVisible = !this->gridVisible;
	Grid::GridAddon::Instance()->SetVisible(this->gridVisible);
}

//------------------------------------------------------------------------------
/** LevelEditor2Window::OnShowBlueprintEditor
*/
void
LevelEditor2Window::OnShowBlueprintEditor()
{
	this->bluePrintsDialog->SetupDialog();
	this->bluePrintsDialog->show();
	this->bluePrintsDialog->raise();
}

//------------------------------------------------------------------------------
/** 
*/
void
LevelEditor2Window::OnShowAudioProperties()
{
	this->audioDialog->SetupDialog();
	this->audioDialog->show();
	this->audioDialog->raise();
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnShowUIProperties()
{
	this->uiDialog->SetupDialog();
	this->uiDialog->show();
	this->uiDialog->raise();
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnShowPhysicsMaterials()
{
    this->materialEditor.show();
    this->materialEditor.raise();
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnCenterPivot()
{
	PlacementUtil::Instance()->CenterPivot();
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnChangeSelectionColour()
{		
	QColor qcolor(this->selectionColour.x()*255.0f, this->selectionColour.y()*255.0f, this->selectionColour.z()*255.0f, this->selectionColour.w()*255.0f);
	QColorDialog qc(qcolor);
	qc.setOption(QColorDialog::ShowAlphaChannel,true);
	int val = qc.exec();
	if (val == QDialog::Accepted)
	{
		QColor diaColor = qc.currentColor();
		float r = diaColor.red() / 255.0f;
		float g = diaColor.green() / 255.0f;
		float b = diaColor.blue() / 255.0f;
		float a = diaColor.alpha() / 255.0f;
		this->selectionColour.set(r, g, b, a);
		BaseGameFeature::UserProfile* userProfile = BaseGameFeature::LoaderServer::Instance()->GetUserProfile();
		userProfile->SetFloat4("SelectionColour", this->selectionColour);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnResetWindows()
{
	this->restoreState(this->defaultState, 1); 
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnTogglePerformanceUI()
{
	LevelEditor2App::Instance()->GetEditorState()->TogglePerformanceOverlay();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnTestLevelViewer()
{
	if(Level::Instance()->GetName().IsEmpty())
	{
		QMessageBox::warning(this, tr("Level Editor"),
			tr("Your level name is empty, save it first!."),
			QMessageBox::Ok);
		return;
	}

	int ret = QMessageBox::question(this, tr("Level Editor"),
		tr("Do you want to save the level first?"),
		QMessageBox::Save,
		QMessageBox::Cancel);
	if(ret == QMessageBox::Save)
	{
		Level::Instance()->SaveLevel();
	}
#if NEBULA3_DEBUG
	viewerLauncher.SetExecutable(IO::URI("toolkit:bin/win32/levelviewer.debug.exe"));
#else
	viewerLauncher.SetExecutable(IO::URI("toolkit:bin/win32/levelviewer.exe"));
#endif
	viewerLauncher.SetWorkingDirectory(IO::URI("toolkit:bin/win32"));

	if(viewerLauncher.IsRunning() || viewerLauncher.CheckIfExists())
	{		
		if(!QtRemoteInterfaceAddon::QtRemoteClient::GetClient("viewer")->IsOpen())
		{
			QtRemoteInterfaceAddon::QtRemoteClient::GetClient("viewer")->Open();
		}
		Ptr<QtRemoteInterfaceAddon::LoadLevel> msg = QtRemoteInterfaceAddon::LoadLevel::Create();
		msg->SetLevel(Level::Instance()->GetName());
		QtRemoteInterfaceAddon::QtRemoteClient::GetClient("viewer")->Send(msg.cast<Messaging::Message>());

	}
	else
	{
		Util::String argstring;
		argstring.Format("-level %s",Level::Instance()->GetName().AsCharPtr());
		viewerLauncher.SetArguments(argstring);
		viewerLauncher.Launch();
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::SetWindowTitle(const Util::String & levelName)
{
	Util::String title;
	title.Format("Level Editor - %s",levelName.AsCharPtr());
	this->setWindowTitle(title.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnAbout()
{
    QString txt = "Nebula Trifid - Leveleditor\n";
    txt += NEBULA_TOOLKIT_VERSION;
    QMessageBox::about(this,"About",txt);
}


//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnDebugPage()
{    
    QDesktopServices::openUrl(QUrl("http://127.0.0.1:2102"));
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnApplicationDebugPage()
{    
    QDesktopServices::openUrl(QUrl("http://127.0.0.1:2100"));
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2Window::OnCollapseProperty(bool state)
{
    QGroupBox * box = (QGroupBox*)QObject::sender();
    if(state)
    {
        for(int i = 0 ; i < box->layout()->count();i++)
        {
            box->layout()->itemAt(i)->widget()->show();            
        }
    }
    else
    {
        for(int i = 0 ; i < box->layout()->count();i++)
        {
            box->layout()->itemAt(i)->widget()->hide();            
        }        
    }
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnCreateTemplate()
{
	Util::Array<Ptr<Game::Entity>> selected = SelectionUtil::Instance()->GetSelectedEntities();
	if (selected.Size() != 1)
	{
		n_warning("Exactly one entity has to be selected\n");
		return;
	}
	Ptr<Game::Entity> entity = selected[0];
	if (entity->GetInt(Attr::EntityType) != Game)
	{
		n_warning("Selected entity must be of a game entity\n");
		return;
	}
	Ptr<GetEntityValues> gmsg = GetEntityValues::Create();
	entity->SendSync(gmsg.cast<Messaging::Message>());	
	Attr::AttributeContainer attrs = gmsg->GetAttrs();	
	Util::String id = attrs.GetString(Id);

	Util::String cat = entity->GetString(Attr::EntityCategory);

	bool isDone = false;
	do 
	{
		bool ok;
		QString name = QInputDialog::getText(this, "Enter template name", "Template name", QLineEdit::Normal, id.AsCharPtr(),&ok);
		if (ok)
		{
			if (Toolkit::EditorBlueprintManager::Instance()->HasTemplate(name.toLatin1().constData(), cat))
			{
				QMessageBox::critical(this, "Template name already taken", "Template name is already in use");
			}
			else if (name.length() == 0)
			{
				QMessageBox::critical(this, "Template name can't be empty", "Template name can't be empty");
			}
			else
			{
				id = name.toLatin1().constData();
				isDone = true;
			}
		}
		else
		{
			return;
		}

	} while (!isDone);	

	
	Attr::AttributeContainer cont = Toolkit::EditorBlueprintManager::Instance()->GetCategoryAttributes(cat);
	const Util::Dictionary<AttrId, Attribute>& catattrs = cont.GetAttrs();
	
	Attr::AttributeContainer newtemplate;
	for (IndexT i = 0; i < catattrs.Size(); i++)
	{ 
		if (attrs.HasAttr(catattrs.KeyAtIndex(i)))
		{
			newtemplate.AddAttr(attrs.GetAttr(catattrs.KeyAtIndex(i)));
		}		
	}
	newtemplate.SetString(Attr::Id, id);
	Toolkit::EditorBlueprintManager::Instance()->AddTemplate(id, cat, newtemplate);
	Toolkit::EditorBlueprintManager::Instance()->SaveCategoryTemplates(cat);
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2Window::OnBatchGame()
{
	ToolkitUtil::Logger logger;
	Ptr<ToolkitUtil::GameExporter> exporter = ToolkitUtil::GameExporter::Create();	
	exporter->SetLogger(&logger);
	exporter->Open();
	exporter->ExportAll();
	exporter->Close();	
}

//------------------------------------------------------------------------------
/**
*/
void
n_status(const char *fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);
    Util::String str;
    str.FormatArgList(fmt, argList);
    n_printf(str.AsCharPtr());
    LevelEditor2App::Instance()->GetWindow()->statusBar()->showMessage(str.AsCharPtr(), 5000);
    va_end(argList);
}

} // namespace LevelEditor2
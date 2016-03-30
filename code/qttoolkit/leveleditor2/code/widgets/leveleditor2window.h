#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::LevelEditor2Window
    
    Nebula/qt application used as the main window for the level editor.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include <QtGui/QMainWindow>
#include <QtGui/QMouseEvent>
#include "ui_leveleditor2window.h"
#include "actions/actionmanager.h"
#include "createentitydialoghandler.h"
#include "posteffectcontroller.h"
#include "physicsmaterialcontroller.h"
#include "levelpropertieshandler.h"
#include "ui_gridsizedialog.h"
#include "qscrollarea.h"
#include "qprogressbar.h"
#include "blueprinthandler.h"
#include "applauncher.h"
#include "resourcestringcontroller.h"
#include "audiodialoghandler.h"
#include "uidialoghandler.h"
#include "layers/layerhandler.h"
#include "globalprobecontroller/code/environmentprobewindow.h"
#include "resourcebrowser/code/assetbrowser.h"
#include "physicsmaterialeditor/code/physicsmaterialeditor.h"

namespace LevelEditor2
{

//------------------------------------------------------------------------------
/**
    writes a message to the status bar and to n_printfs it as well
*/
void n_status(const char *fmt, ...);

class LevelEditor2App;

class LevelEditor2Window : public QMainWindow
{
	Q_OBJECT
public:
	/// constructor
	LevelEditor2Window();
	/// destructor
	virtual ~LevelEditor2Window();

	/// returns the window Id for the nebula frame
	Util::Blob GetNebulaWindowData();
	/// returns nebula widget
	ViewerWidget* GetNebulaWidget();
	/// returns the entity treeview
	EntityTreeWidget* GetEntityTreeWidget();
    ///
    QProgressBar* GetProgressBar();    
	/// returns the container for entity attribute widgets
	QLayout* GetAttributeWidgetContainer();
	/// returns the posteffect controller
	QtPostEffectAddon::PostEffectController* GetPostEffectController();
	/// handle opening the window which restores the state
	void showEvent(QShowEvent* e);
	/// handle close event to terminate nebula application when frame closes
	void closeEvent(QCloseEvent *e);

	/// sets up content browser window stuff which is dependent on nebula
	void Setup();

	/// access to ui object
	Ui::LevelEditor2Window& GetUi();

	/// get layer handler
	Ptr<Layers::LayerHandler> GetLayerHandler();	

public slots:
    /// batch all game data
    void OnBatchGame();
private slots:

	/// override key pressed
	void keyPressEvent(QKeyEvent *event);
	/// override key released
	void keyReleaseEvent(QKeyEvent* event);

	/// when the menu-item for creating a new environment entity is triggered
	void OnNewEnvironmentEntity();
	/// when the menu-item for creating a new light entity is triggered
	void OnNewLightEntity();
	/// when the menu-item for creating a new game entity is triggered
	void OnNewGameEntity();

	/// when the undo-menuitem is activated
	void OnUndo();
	/// when the delete-menuitem is activated
	void OnDelete();
	/// Save
	void OnSave();
	/// Save as
	void OnSaveAs();
	/// load
	void OnLoad();
	/// new
	void OnNew();
	/// show posteffect dialog
	void OnShowPostEffect();
	/// displays environment probe window
	void OnShowEnvironmentProbeSettings();
	/// show level properties dialog
	void OnShowLevelProperties();
	/// show grid properties dialog
	void OnShowGridProperties();
	/// change grid size
	void OnGridSizeChanged(double);
	/// change grid visibility
	void OnGridVisible();
	/// change selection colour
	void OnChangeSelectionColour();
	/// reset dockable widgets
	void OnResetWindows();
	/// show performance ui
	void OnTogglePerformanceUI();
	/// show blueprint editor
	void OnShowBlueprintEditor();
	/// start level viewer
	void OnTestLevelViewer();
	/// show audio properties
	void OnShowAudioProperties();
	/// show ui properties
	void OnShowUIProperties();
    /// about
    void OnAbout();
    /// debug page
    void OnDebugPage();
    /// default debug page
    void OnApplicationDebugPage();
	/// create new template from selected entity
	void OnCreateTemplate();
    /// show physics materials editor
    void OnShowPhysicsMaterials();
	/// center pivot
	void OnCenterPivot();
	///
	void OnImport();
	///
	void OnImportLevel();
	///
	void OnExportSelectionLevel();
    ///
    void OnAddReference();
    ///
    void OnBatchToggle(bool);

    public slots:
    /// collapse property page
    void OnCollapseProperty(bool);

	/// get selection colour
	const Math::float4 & GetSelectionColour();
	/// set selection colour
	void SetSelectionColour(const Math::float4 & colour);
	/// check for valid drag data
	void dragEnterEvent(QDragEnterEvent* e);
	/// drop event
	void dropEvent(QDropEvent* e);
	///
	void OnExportSelection();

private:

	///
	bool PickLevelFile(const Util::String & title, const Util::String & folder, IO::URI & file, bool save);	

	/// forcibly release the ctrl key
	void ForceReleaseCtrlKey();
	/// set level name in window title 
	void SetWindowTitle(const Util::String & levelName);

	Ui::LevelEditor2Window ui;
	bool ctrlPressed;	//< flag for forcibly release the ctrl-key when opening an add-entity dialog

	bool gridVisible;
	CreateEntityDialogHandler* createEntityDialog;
	LevelPropertiesHandler* levelPropertiesDialog;
	BlueprintsHandler* bluePrintsDialog;
	QtPostEffectAddon::PostEffectController postEffectController;
    QtPhysics::MaterialsEditor materialEditor;
	Lighting::EnvironmentProbeWindow* environmentProbeWindow;
	QVBoxLayout *attributeControllerVLayout;	
	QDialog * gridSizeDialog;
	QByteArray defaultState;
    QProgressBar * progressBar;
    QtAttributeControllerAddon::ScriptEditor* scriptEditor;
	Ui::GridSizeDialog gridSizeUi;
	AudioDialogHandler * audioDialog;
	UIDialogHandler * uiDialog;
	Ptr<Layers::LayerHandler> layerHandler;	
	Ptr<ResourceBrowser::AssetBrowser> assetBrowserWindow;
	ToolkitUtil::AppLauncher viewerLauncher;
	Math::float4 selectionColour;
}; 

//------------------------------------------------------------------------------
/**
*/
inline Util::Blob
LevelEditor2Window::GetNebulaWindowData()
{
    Util::Blob data;
    this->ui.nebulaFrame->GenerateWindowData();
    data.Set(this->ui.nebulaFrame->GetWindowData(), this->ui.nebulaFrame->GetWindowDataSize());
    return data;
}

//------------------------------------------------------------------------------
/**
*/
inline ViewerWidget* 
LevelEditor2Window::GetNebulaWidget()
{
    return this->ui.nebulaFrame;
}

//------------------------------------------------------------------------------
/**
*/
inline QtPostEffectAddon::PostEffectController * 
LevelEditor2Window::GetPostEffectController()
{
	return &this->postEffectController;
}

//------------------------------------------------------------------------------
/**
*/
inline EntityTreeWidget* 
LevelEditor2Window::GetEntityTreeWidget()
{
    return this->ui.treeWidget;
}

//------------------------------------------------------------------------------
/**
*/
inline QProgressBar*
LevelEditor2Window::GetProgressBar()
{
    return this->progressBar;
}

//------------------------------------------------------------------------------
/**
*/
inline QLayout* 
LevelEditor2Window::GetAttributeWidgetContainer()
{
    return this->attributeControllerVLayout;
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::LevelEditor2Window&
LevelEditor2Window::GetUi()
{
	return this->ui;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4&
LevelEditor2Window::GetSelectionColour()
{
	return this->selectionColour;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LevelEditor2Window::SetSelectionColour(const Math::float4& colour)
{
	this->selectionColour = colour;
}

//------------------------------------------------------------------------------
/**
*/
inline Ptr<Layers::LayerHandler>
LevelEditor2Window::GetLayerHandler()
{
	return this->layerHandler;
}

} // namespace LevelEditor2
//------------------------------------------------------------------------------
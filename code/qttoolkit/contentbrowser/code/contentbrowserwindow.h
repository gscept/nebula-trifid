#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::ContentBrowserApp
    
    Nebula/qt application used as the main window for the content browser.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QtGui/QMainWindow>
#include <QtGui/QMouseEvent>
#include "ui_contentbrowserwindow.h"
#include "ui_particleeffectwizard.h"
#include "widgets/animations/animationhandler.h"
#include "widgets/audio/audiohandler.h"
#include "widgets/meshes/meshhandler.h"
#include "widgets/models/modelhandler.h"
#include "widgets/textures/texturehandler.h"
#include "widgets/materials/materialhandler.h"
#include "widgets/baseitem.h"
#include "widgets/models/modelitem.h"
#include "widgets/animations/animationitem.h"
#include "widgets/audio/audioitem.h"
#include "widgets/ui/layoutitem.h"
#include "widgets/ui/fontitem.h"
#include "widgets/meshes/meshitem.h"
#include "widgets/textures/textureitem.h"
#include "modelimporter/modelimporterwindow.h"
#include "textureimporter/textureimporterwindow.h"
#include "globalprobecontroller/code/environmentprobewindow.h"
#include "posteffectcontroller.h"
#include "util/array.h"
#include "nody/shady/code/shadywindow.h"
#include "resourcebrowser/code/assetbrowser.h"
#include "code/tiledgraphicsview.h"

namespace Widgets
{
	class ModelNodeHandler;
}
namespace ContentBrowser
{
class ContentBrowserApp;
class ContentBrowserWindow : public QMainWindow
{
	Q_OBJECT
public:
	/// constructor
	ContentBrowserWindow();
	/// destructor
	virtual ~ContentBrowserWindow();

	/// returns the window Id for the nebula frame
	Util::Blob GetNebulaWindowData();
	/// returns nebula widget
	PreviewWidget* GetNebulaWidget() const;
	/// returns model importer
	ModelImporter::ModelImporterWindow* GetModelImporter() const;
	/// returns texture importer
	TextureImporter::TextureImporterWindow* GetTextureImporter() const;
	/// returns posteffectcontroller
	QtPostEffectAddon::PostEffectController * GetPostEffectWindow() const;

	/// handle showing the window
	void showEvent(QShowEvent* e);
	/// handle close event to terminate nebula application when frame closes
	void closeEvent(QCloseEvent* e);
	/// handles drag enter events
	void dragEnterEvent(QDragEnterEvent* e);
	/// handles drop events
	void dropEvent(QDropEvent* e);

	/// sets up content browser window stuff which is dependent on nebula
	void Setup();

	/// handles animation specific item clicks
	bool OnAnimationItemActivated(Widgets::BaseItem* item);
	/// handles audio specific item clicks
	bool OnAudioItemActivated(Widgets::BaseItem* item);
	/// handles ui specific item clicks
	bool OnUIItemActivated(Widgets::BaseItem* item);
	/// handles font specific item clicks
	bool OnFontItemActivated(Widgets::BaseItem* item);	
	/// handles mesh specific item clicks
	bool OnMeshItemActivated(Widgets::BaseItem* item);
	/// handles model specific item clicks
	bool OnModelItemActivated(Widgets::BaseItem* item);
	/// handles texture specific item clicks
	bool OnTextureItemActivated(Widgets::BaseItem* item);

	/// call when removing an animation item
	void RemoveAnimationItem(Widgets::AnimationItem* item);
	/// call when removing an audio item
	void RemoveAudioItem(Widgets::AudioItem* item);
	/// call when removing an ui item
	void RemoveUIItem(Widgets::LayoutItem* item);
	/// call when removing a mesh item
	void RemoveMeshItem(Widgets::MeshItem* item);
	/// call when removing a model item
	void RemoveModelItem(Widgets::ModelItem* item);
	/// call when removing a model item together with it's related resources (mesh, animation)
	void RemoveModelItemAndAssociated(Widgets::ModelItem* item);
	/// call when removing a texture item
	void RemoveTextureItem(Widgets::TextureItem* item);

	/// called from the current state to update onframe events
	void OnFrame();

public slots:
	/// updates library utilities, gets the resource which as been updated
	void UpdateLibrary();

private slots:

	/// call when you wish to import an FBX
	void ImportModel();
	/// call when you wish to import a texture
	void ImportTexture();

	/// updates animation libraryz
	void UpdateAnimationLibrary(QTreeWidgetItem* animationItem);
	/// updates audio library
	void UpdateAudioLibrary(QTreeWidgetItem* audioItem);
	/// updates ui library
	void UpdateUILibrary(QTreeWidgetItem* audioItem);
	/// updates mesh library
	void UpdateMeshLibrary(QTreeWidgetItem* meshItem);
	/// updates model library
	void UpdateModelLibrary(QTreeWidgetItem* modelItem);
	/// updates texture library
	void UpdateTextureLibrary(QTreeWidgetItem* textureItem);

    /// called whenever a model has been imported
    void OnModelImported(const Util::String& res);
    /// called whenever a texture has been imported
    void OnTextureImported(const Util::String& res);

	/// called whenever a model has been saved with a new name
	void ModelSavedWithNewName(const Util::String& res);

    /// called whenever we click on a texture in the asset browser
    void OnTextureSelected(const QString& tex);
    /// called whenever we click on a model in the asset browser
    void OnModelSelected(const QString& mdl);
    /// called whenever we click on a surface in the asset browser
    void OnSurfaceSelected(const QString& sur);
	/// called when a right click occurs in the asset browser
	void OnAssetBrowserRightClick(QContextMenuEvent* event);
	/// called when a right click occurs over an item in the asset browser
	void OnAssetBrowserItemRightClick(ResourceBrowser::TiledGraphicsItem* item, QGraphicsSceneContextMenuEvent* event);

	/// handles item activated (double-click)
    void ItemActivated(QTreeWidgetItem* item);
	/// handles item clicked
    void ItemClicked(QTreeWidgetItem* item);
	/// handles item right click
    void TreeRightClicked(const QPoint& point);
	/// called whenever an item gets changed
	void ItemChanged();

	/// displays model info if its pressed
	void OnShowModelInfo();
	/// displays texture info if its pressed
	void OnShowTextureInfo();
	/// displays mesh info if its pressed
	void OnShowMeshInfo();
	/// displays audio info if its pressed
	void OnShowAudioInfo();
	/// displays ui info if its pressed
	void OnShowUIInfo();
	/// displays animation info if its pressed
	void OnShowAnimationInfo();
	
	/// displays post effect controller
	void OnShowPostEffectController();
	/// called when the texture browser should be opened
	void OnShowAssetBrowser();
	/// displays environment probe window
	void OnShowEnvironmentProbeSettings();
	/// displays the grid
	void OnShowGrid();

	/// called when the save all button is clicked
	void OnSaveAll();

	/// called whenever ctrl+z is pressed
	void OnUndo();
	/// called whenever shift+ctrl+z is pressed
	void OnRedo();

	/// called whenever the particle effect wizard is opened
	void OnCreateParticleEffect();
	/// called whenever the mesh browse button of the particle effect wizard is pressed
	void OnBrowseEmitterMesh();

	/// called whenever we try to connect to the level editor
	void OnConnectToLevelEditor();
	/// called whenever we disconnect from the level editor
	void OnDisconnectFromLevelEditor();

    /// called when about is clicked
    void OnAbout();
    /// called when Debug Page is clicked
    void OnDebugPage();


	/// called when the user presses the reset geometry button
	void ResetLayout();

private:
	friend class Widgets::ModelNodeHandler;

	/// saves the current library state
	void SaveLibraryState();
	/// restores the expansion state of the library ui
	void RestoreLibraryState();
	/// searches for an item and selects it if possible
	void SelectItem(const Util::String& topLevel, const Util::String& category, const Util::String& file, bool expand = false);
    /// searches for a category and expands if possible
    void SelectFolder(const Util::String& topLevel, const Util::String& category, bool expand = false);

	Ptr<Widgets::AnimationHandler> animationHandler;
	Ptr<Widgets::AudioHandler> audioHandler;
	Ptr<Widgets::UIHandler> uiHandler;
	Ptr<Widgets::MeshHandler> meshHandler;
	Ptr<Widgets::ModelHandler> modelHandler;
	Ptr<Widgets::TextureHandler> textureHandler;
    Ptr<Widgets::MaterialHandler> materialHandler;
	Util::Array<Widgets::BaseItem*> items;
	Widgets::ModelItem* modelItem;
	Widgets::AnimationItem* animItem;
	Widgets::AudioItem* audioItem;
	Widgets::LayoutItem* layoutItem;
	Widgets::FontItem* fontItem;
	Widgets::MeshItem* meshItem;
	Widgets::TextureItem* textureItem;

	bool wireFrame;
	bool showAmbientOcclusion;
	bool showPhysics;
	bool showControls;
	bool showSky;
	bool lockLight;

	QList<bool> libraryState;

	Ui::ContentBrowserWindow ui;
	ModelImporter::ModelImporterWindow* modelImporterWindow;
	TextureImporter::TextureImporterWindow* textureImporterWindow;
	Lighting::EnvironmentProbeWindow* environmentProbeWindow;
    Ptr<Shady::ShadyWindow> shadyWindow;
	Ptr<ResourceBrowser::AssetBrowser> assetBrowserWindow;
	Ptr<ContentBrowser::ProgressReporter> progressReporter;
	QtPostEffectAddon::PostEffectController* postEffectController;
    
	Ui::ModelInfoWidget modelInfoUi;
	Ui::TextureInfoWidget textureInfoUi;
	Ui::ParticleEffectWizard particleWizardUi;
	Ui::AudioInfoWidget audioInfoUi;
	Ui::UIInfoWidget uiInfoUi;
    Ui::MaterialInfoWidget materialInfoUi;
    LibraryTreeWidget* libraryTree;

	QDockWidget* modelInfoWindow;
	QDockWidget* textureInfoWindow;
	QDockWidget* meshInfoWindow;
	QDockWidget* audioInfoWindow;
	QDockWidget* uiInfoWindow;
	QDockWidget* animationInfoWindow;
    QDockWidget* materialInfoWindow;

	QDialog particleEffectWizard;
}; 

//------------------------------------------------------------------------------
/**
*/
inline Util::Blob 
ContentBrowserWindow::GetNebulaWindowData()
{
    Util::Blob data;
    this->ui.nebulaFrame->GenerateWindowData();
    data.Set(this->ui.nebulaFrame->GetWindowData(), this->ui.nebulaFrame->GetWindowDataSize());
    return data;
}

//------------------------------------------------------------------------------
/**
*/
inline PreviewWidget* 
ContentBrowserWindow::GetNebulaWidget() const
{
    return this->ui.nebulaFrame;
}

//------------------------------------------------------------------------------
/**
*/
inline ModelImporter::ModelImporterWindow* 
ContentBrowserWindow::GetModelImporter() const
{
    return this->modelImporterWindow;
}

//------------------------------------------------------------------------------
/**
*/
inline TextureImporter::TextureImporterWindow* 
ContentBrowserWindow::GetTextureImporter() const
{
	return this->textureImporterWindow;
}

//------------------------------------------------------------------------------
/**
*/
inline QtPostEffectAddon::PostEffectController*
ContentBrowserWindow::GetPostEffectWindow() const
{
	return this->postEffectController;
}
} // namespace ContentBrowser
//------------------------------------------------------------------------------
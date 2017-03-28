#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadyWindow
    
    Shady main window. Inherits both main window and refcounted
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QMainWindow>
#include <QDialog>
#include "core/refcounted.h"
#include "util/dictionary.h"
#include "variation/variation.h"
#include "scene/nodescene.h"
#include "variation/variationdatabase.h"
#include "variation/supervariation.h"
#include "node/shadynode.h"
#include "project/shadyproject.h"
#include "shadydefs.h"
#include "codehighlighter.h"
#include "rendering/materialwindow.h"


#define SHADY_MESSAGE(x) ShadyWindow::ConsoleMessage(x);
#define SHADY_ERROR(x) ShadyWindow::ConsoleError(x);
#define SHADY_WARNING(x) ShadyWindow::ConsoleWarning(x);
#define SHADY_MESSAGE_FORMAT(x, ...) ShadyWindow::ConsoleMessage(x, __VA_ARGS__);
#define SHADY_ERROR_FORMAT(x, ...) ShadyWindow::ConsoleError(x, __VA_ARGS__);
#define SHADY_WARNING_FORMAT(x, ...) ShadyWindow::ConsoleWarning(x, __VA_ARGS__);

namespace Ui
{
    class BuildSettings;
    class Validation;
    class ShadyMainWindow;
}
namespace Shady
{
class ShadyWindow : 
	public QMainWindow, 
	public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(ShadyWindow);
    __DeclareSingleton(ShadyWindow);
public:
	/// constructor
	ShadyWindow();
	/// destructor
	virtual ~ShadyWindow();

	/// sets up window
	void Setup();
	/// closes window
	void Close();

	/// setup variation tree
	void SetupVariationTree();
	/// get access to scene
	const Ptr<Nody::NodeScene>& GetScene() const;

    /// handle on-show actions
    void showEvent(QShowEvent* e);
    /// handle on-close actions
    void closeEvent(QCloseEvent* e);

    /// output message in message field
    static void ConsoleMessage(const char* format, ...);
    /// output error in message field
    static void ConsoleError(const char* format, ...);
    /// output warning in message field
    static void ConsoleWarning(const char* format, ...);

	// grant global access to ui
	static Ui::ShadyMainWindow* ui;	

private slots:

	/// invoked when a node should be created
	void OnCreateNode(const Ptr<Nody::Variation>& variation);
    /// invoked when the user saves
    void OnSave();
    /// invoked when the user saves as
    void OnSaveAs();
    /// invoked when we open a file
    void OnOpen();
    /// invoked when we create a new project
    void OnNew();

	/// invoked whenever the real-time visualization button gets clicked
	void OnRealtimeVisualizeClicked();

    /// invoked when we build
    void OnBuild();
    
    /// invoked when a node gets clicked
    void OnNodeClicked(const Ptr<Nody::Node>& node);

    /// invoked when a node value gets changed
    void OnNodeValueChanged();

signals:
    /// invoked when the build is done
    void OnBuildDone(const Util::Blob& data);
	
private:
    /// sets up a new scene with a single node containing the default super variation
    void SetupNewScene();

    IO::URI currentPath;
    Ui::BuildSettings* buildSettingsUi;
    Ui::Validation* validationUi;

    QDialog settingsDialog;
    QDialog validationDialog;
	Ptr<Nody::NodeScene> nodeScene;
	Ptr<Nody::VariationDatabase> variationDatabase;
    Ptr<Nody::SuperVariation> superVariation;    
    Ptr<Shady::ShadyNode> mainNode;
    Ptr<Nody::Node> currentNode;
    Ptr<Shady::ShadyProject> project;
	Ptr<Shady::MaterialWindow> materialEditor;
    Highlighter* highlighter;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Nody::NodeScene>&
ShadyWindow::GetScene() const
{
	return this->nodeScene;
}

} // namespace Shady
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
/**
    @class Importer::ImporterWindow
    
    Importer window handler, handles signals from GUI.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QProcess>
#include <QtGui/QMainWindow>
#include "ui_modelimporterwindow.h"
#include "clipcontroller.h"
#include "fbx/nfbxreader.h"
#include "tools/commandlineargs.h"
#include "modelcontroller.h"
#include "io/uri.h"
#include "fbx/nfbxexporter.h"
#include "progressreporter.h"
#include "toolkitutil/modelutil/modelattributes.h"
#include "toolkitutil/modelutil/modeldatabase.h"
#include "physicscontroller.h"
#include "importerwindow.h"
#include "messaging/message.h"

namespace ModelImporter
{
class ModelImporterWindow : public ContentBrowser::ImporterWindow
{
	Q_OBJECT
public:
	/// constructor
	ModelImporterWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	/// destructor
	~ModelImporterWindow();

	/// sets the file URI
	void SetUri(const IO::URI& path);
	/// opens the app
	void Open();
public slots:
	/// notifies change has taken place
	void NotifyChange();
private slots:
	/// reader has loaded
	void ReaderLoaded(int status, const IO::URI& path);
	/// opens a file browser
	void Browse();
	/// sets the window to show it's been changed
	void SetChanged(bool state);
	/// imports loaded FBX to nebula
	void Import();
	/// called when reader is done
	void UpdateInformation(int status, const IO::URI& path);
	/// called whenever the viewer is closed;
	void ViewerClosed();
	/// called whenever the viewer is opened
	void ViewerOpened();

private:


	/// called when the model update is done
	void OnModelUpdateDone( const Ptr<Messaging::Message>& msg );

	/// sets whether or not the GUI controllers should be enabled or not
	void SetControllersEnabled(bool state);

	Ui::ModelImporterWindow ui;
	ClipController* clipController;
	ModelController* modelController;
	PhysicsController* physicsController;
	NFbxReader* reader;
	IO::URI path;
	Ptr<ToolkitUtil::ModelDatabase> database;
	Ptr<ToolkitUtil::ModelAttributes> currentOptions;
	Ptr<ToolkitUtil::ModelPhysics> currentPhysics;
	Ptr<ToolkitUtil::NFbxExporter> fbxExporter;

};
}
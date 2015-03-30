#pragma once
#include <QProcess>
#include <QtGui/QMainWindow>
#include "ui_importerqt.h"
#include "clipcontroller.h"
#include "fbx/fbxreader.h"
#include "tools/commandlineargs.h"
#include "modelcontroller.h"

namespace Importer
{
class Importer : public QObject
{
	Q_OBJECT
public:
	/// constructor
	Importer();
	/// destructor
	~Importer();

	/// sets the command line args
	void SetCommandLineArgs(const QtTools::CommandLineArgs& args);
	/// opens the app
	void Open();
public slots:
	/// notifies change has taken place
	void NotifyChange();
private slots:
	/// reader has loaded
	void ReaderLoaded(int status, const QString& file, const QString& category);
	/// opens a file browser
	void Browse();
	/// sets the window to show it's been changed
	void SetChanged(bool state);
	/// imports loaded FBX to nebula
	void Import();
	/// called whenever the exporter is done
	void ExporterDone(int, QProcess::ExitStatus);
	/// called when reader is done
	void UpdateInformation(int status, const QString& file, const QString& category);
	/// called whenever the viewer is closed;
	void ViewerClosed();
	/// called whenever the viewer is opened
	void ViewerOpened();

private:
	/// sets whether or not the GUI controllers should be enabled or not
	void SetControllersEnabled(bool state);

	QString file;
	QString cat;
	ClipController* clipController;
	ModelController* modelController;
	FbxReader* reader;
	QtTools::CommandLineArgs args;
	ImportOptions* currentOptions;
};
}
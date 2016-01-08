#pragma once
//------------------------------------------------------------------------------
/**
	@class BatchExporter::BatchExporterApp
    
    Batch exporter application

    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QProcess>
#include <QMainWindow>
#include <QQueue>
#include <QPair>
#include "app/consoleapplication.h"
#include "ui_batchexporterqt.h"
#include "tools/commandlineargs.h"
#include "toolkitutil/projectinfo.h"
#include "toolkitconsolehandler.h"

namespace BatchExporter
{
class BatchExporterApp : public QMainWindow
{
	Q_OBJECT

	enum ExportBits
	{
		Graphics = 1 << 0,
		Models = 1 << 1,
		Physics = 1 << 2,
		Textures = 1 << 3,
		Shaders = 1 << 4,
		GameData = 1 << 5,		
		Audio = 1 << 7,

		All = (1 << 8) - 1,

		Levels = (1 << 9),		// special bit for exporting levels
		Templates = (1 << 10)	// special bit for exporting templates
	};
public:

	/// constructor
	BatchExporterApp(const QtTools::CommandLineArgs& args);
	/// destructor
	~BatchExporterApp();

private slots:

	/// is this the first time this application has been opened?
	bool IsFirstTime() const;
	/// show a dialog explaining what needs to be done the first time this is run
	void ShowFirstTimeHelpDialog();

	/// stops export
	void StopExports();
	/// assembles the execution queue
	void GatherExports();
	/// executes the queue
	bool Export();
	/// callback when exportation succeeds
	void ExporterDone(int exitCode, QProcess::ExitStatus status);

	/// sets the force parameter
	void SetForce(bool force);
	/// sets the Graphics bit
	void SetExportGraphics(bool state);
	/// sets the Model bit
	void SetExportModels(bool state);	
	/// sets the Physics bit
	void SetExportPhysics(bool state);
	/// sets the Textures bit
	void SetExportTextures(bool state);
	/// sets the Shaders bit
	void SetExportShaders(bool state);
	/// sets the Templates bit
	void SetExportGameData(bool state);
	/// sets the export Audio bit
	void SetExportAudio(bool state);


	///
	void StartContentbrowser();
	///
	void StartLeveleditor();

	/// outputs a message
	void OutputMessage();

	/// receive stderr from child processes
	void OutputStderr();

	/// opens a file browser to pick out the working directory
	void PickWorkingDir();
	/// opens a file browser to pick out the toolkit directory		
	void PickToolkitDir();
	/// shows the about dialog
	void ShowAbout();

	/// converts export bit to string
	const QString FromBit(ExportBits bit);
signals:
	void ExportFailed(const QString&);
private:    

    /// insert row feed
    void SetDefaultMessageStyle();
    /// helper function to output start/end of export
    void OutputMarkerMessage(const QString& message);
    /// helper function to output exporter message
    void OutputExportStatus(const QString& message);
    /// helper function to output error message
    void OutputErrorMessage(const QString& message);
    /// helper function to output standard message
    void OutputStandardMessage(const QString& message);
	/// update title string with current project directory
	void UpdateTitle();

	QQueue<QPair<ExportBits, QString> > executionQueue;
	QProcess remoteProcess;
	QtTools::CommandLineArgs args;
	bool force;
	int exportBits;

	Ui::BatchExporterQtClass ui;
    App::ConsoleApplication* nebApp;
    ToolkitUtil::ProjectInfo projInfo;

    QString workDir;
    QString toolDir;
};
}

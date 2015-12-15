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
#include <QThread>
#include <QSemaphore>
#include <QMutex>
#include "app/consoleapplication.h"
#include "ui_batchexporterqt.h"
#include "tools/commandlineargs.h"
#include "toolkitutil/projectinfo.h"
#include "modelutil/modeldatabase.h"
#include "toolkitapp.h"
#include "asset/assetexporter.h"
#include "io/consolehandler.h"


// FIXME, all of this feels horribly clunky, especially the thread handling

namespace BatchExporter
{
class BatchExporterApp;



class WorkerThread : public QThread, public IO::ConsoleHandler
{
	Q_OBJECT
		__DeclareAbstractClass(WorkerThread);
public:

	BatchExporterApp* app;
	Threading::ThreadId myId;

	void Print(const Util::String& s);
	/// called by console with serious error
	void Error(const Util::String& s);
	/// called by console 
	void Warning(const Util::String& s);
	/// called by console 
	void DebugOut(const Util::String& s);

	/// thread main function
	void run() {}

signals:
	void Message(unsigned char level, const QString&);
};

class AssetWorkerThread : public WorkerThread
{
	Q_OBJECT
	__DeclareClass(AssetWorkerThread);
public:
		
	/// set assetfolders to work on
	void SetWorkAssets(const Util::Array<Util::String> & assets);
	/// batch system data on this thread
	void BatchSystem(bool enable);
	/// thread main function
	void run();

private:
	Ptr<ToolkitUtil::ModelDatabase> modelDatabase;
	Util::Array<Util::String> workPackage;
	
	bool system;
};

class ShaderWorkerThread : public WorkerThread
{
	Q_OBJECT
		__DeclareClass(ShaderWorkerThread);
public:

	/// thread main function
	void run();
private:
};


class GameWorkerThread : public WorkerThread
{
	Q_OBJECT
		__DeclareClass(GameWorkerThread);
public:

	/// thread main function
	void run();
private:
};

class BatchExporterApp : public QMainWindow, public ToolkitUtil::ToolkitApp
{
	Q_OBJECT
	
	
	enum ExportBits
	{
		Assets = 1 << 0,
		Shaders = 1 << 1,
		GameData = 1 << 2,
		Audio = 1 << 3,		

		All = (1 << 8) - 1,

		Levels = (1 << 9),		// special bit for exporting levels
		Templates = (1 << 10)	// special bit for exporting templates
	};
public:

	/// constructor
	BatchExporterApp();
	/// destructor
	~BatchExporterApp();

	///
	void Open(const QtTools::CommandLineArgs& args);


	

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
	void Export();
	
	/// callback when exportation succeeds
	void ExporterDone(int exitCode, QProcess::ExitStatus status);
	/// callback when workerthread has finished
	void ThreadDone();
	
	/// opens a file browser to pick out the working directory
	void PickWorkingDir();
	/// opens a file browser to pick out the toolkit directory		
	void PickToolkitDir();
	/// shows the about dialog
	void ShowAbout();

	/// 
	void UpdateOutputWindow();

	/// selection in list changed
	void SelectionChanged();
	/// converts export bit to string
	const QString FromBit(ExportBits bit);
	///
	void OutputMessage(unsigned char level, const QString& msg);
signals:
	void ExportFailed(const QString&);	
private:    
   
	/// update title string with current project directory
	void UpdateTitle();

	///
	void AddMessages(const Util::Array<ToolkitUtil::AssetExporter::AssetLog>&messages);
	

	QtTools::CommandLineArgs args;
	bool force;
	int exportBits;

	Ui::BatchExporterQtClass ui;
	
	

	Util::Array<ToolkitUtil::AssetExporter::AssetLog> messages;
    
    QString workDir;
    QString toolDir;
		
	Util::Array<Ptr<AssetWorkerThread>> workerThreads;
	Ptr<ShaderWorkerThread> shaderThread;
	Ptr<GameWorkerThread> gameThread;
	QSemaphore runningThreads;
	QMutex messageMutex;
};

}

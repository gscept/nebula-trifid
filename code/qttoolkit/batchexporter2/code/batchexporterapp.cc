//------------------------------------------------------------------------------
//  batchexporterapp.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "stdneb.h"
#include <QFileDialog>
#include <QPlastiqueStyle>
#include <QMessageBox>
#include <QScrollBar>
#include <QLibrary>
#include <QKeyEvent>
#include "batchexporterapp.h"
#include "ui_about.h"
#include "tools/progressnotifier.h"
#include "style/nebulastyletool.h"
#include "util/string.h"
#include "system/nebulasettings.h"
#include "io/fswrapper.h"
#include "io/assignregistry.h"
#include "toolkitconsolehandler.h"
#ifdef WIN32
#include "io/win32/win32consolehandler.h"
#else
#include "io/posix/posixconsolehandler.h"
#endif
#include "asset/assetexporter.h"
#include "base/exporterbase.h"
#include "QTreeWidget"
#include "QColor"
#include "QtConcurrentRun"
#include "util/localstringatomtable.h"
#include "system/systeminfo.h"
#include "game/gameexporter.h"
#include "io/memorystream.h"


#if WIN32
#ifdef _DEBUG
#define EXESUFFIX ".debug.exe"
#else
#define EXESUFFIX ".exe"
#endif
#define DEFAULTPLATFORM ToolkitUtil::Platform::Win32
#elif __LINUX__
#define EXESUFFIX ""
#define DEFAULTPLATFORM ToolkitUtil::Platform::Linux
#else
#error "Platform not supported!"
#endif

using namespace QtTools;
using namespace ToolkitUtil;

__ImplementAbstractClass(BatchExporter::WorkerThread, 'BEWP', IO::ConsoleHandler);
__ImplementClass(BatchExporter::AssetWorkerThread, 'BEAT', BatchExporter::WorkerThread);
__ImplementClass(BatchExporter::ShaderWorkerThread, 'SHWT', BatchExporter::WorkerThread);
__ImplementClass(BatchExporter::GameWorkerThread, 'GWWT', BatchExporter::WorkerThread);
__ImplementClass(BatchExporter::SystemWorkerThread, 'SWWT', BatchExporter::WorkerThread);

class QLogTreeItem : public QTreeWidgetItem
{
public:
	QLogTreeItem() :QTreeWidgetItem(){}
	QLogTreeItem(const QStringList &strings) : QTreeWidgetItem(strings)
	{}

	Util::Array<ToolkitConsoleHandler::LogEntry> logs;
};

QColor errorColor(200, 50, 0);
QColor warningColor(255, 153, 0);
QColor defaultColour(90, 90, 90);

QColor LogLevelToBackgroundColour(unsigned char level)
{
	if (level & ToolkitConsoleHandler::LogError)
	{
		return errorColor;
	}
	else if (level & ToolkitConsoleHandler::LogWarning)
	{
		return warningColor;
	}
	return defaultColour;
}

QColor LogLevelToTextColour(unsigned char level)
{
	if (level & ToolkitConsoleHandler::LogError)
	{
		return Qt::white;
	}
	else if (level & ToolkitConsoleHandler::LogWarning)
	{
		return Qt::black;
	}
	return Qt::lightGray;
}

QFont LogLevelToFont(unsigned char level)
{
	QFont font;
	if (level & ToolkitConsoleHandler::LogError)
	{
		font.setPointSize(10);
		font.setWeight(QFont::DemiBold);
		return font;
	}
	else if (level & ToolkitConsoleHandler::LogWarning)
	{
		font.setBold(true);
		return font;
	}
	return font;
}

namespace BatchExporter
{

//------------------------------------------------------------------------------
/**
*/
BatchExporterApp::BatchExporterApp() : 
	exportBits(All), force(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void BatchExporterApp::Open(const CommandLineArgs& args)
{
	ToolkitApp::Open();
	
	Ptr<IO::Console> console = IO::Console::Instance();
#ifdef WIN32
	const Util::Array<Ptr<IO::ConsoleHandler>> & handlers = console->GetHandlers();
	for (int i = 0; i < handlers.Size(); i++)
	{
		if (handlers[i]->IsA(Win32::Win32ConsoleHandler::RTTI))
		{
			console->RemoveHandler(handlers[i]);
		}
	}
#endif
	this->SetupProjectInfo();

	System::SystemInfo sysInfo;
	int cores = sysInfo.GetNumCpuCores();
	// we need at least two threads
	cores = Math::n_max(2, cores);	
	// FIXME, having more than 3 will endanger memory limits quickly due to nvtt being VERY memory hungry
	cores = Math::n_max(3, cores);
	for (int i = 0; i < cores; i++)
	{
		Ptr<AssetWorkerThread> thread = AssetWorkerThread::Create();

		// thread 0 is the dedicated fbx, model, surface batcher thread (includes system)		
		// thread 1 will batch system textures, threads 2-x will batch textures
		thread->app = this;
		if (i == 0)
		{			
			thread->BatchGraphics(true);
		}		
		
		this->workerThreads.Append(thread);
		connect(thread.get_unsafe(), SIGNAL(Message(unsigned char, const QString&)),
			this, SLOT(OutputMessage(unsigned char, const QString&)));
		connect(thread.get_unsafe(), SIGNAL(finished()), this, SLOT(ThreadDone()));
	}

	this->systemThread = SystemWorkerThread::Create();
	this->systemThread->app = this;

	this->shaderThread = ShaderWorkerThread::Create();
	this->shaderThread->app = this;
	
	connect(shaderThread.get_unsafe(), SIGNAL(Message(unsigned char, const QString&)),
		this, SLOT(OutputMessage(unsigned char, const QString&)));
	connect(shaderThread.get_unsafe(), SIGNAL(finished()), this, SLOT(ThreadDone()));

	gameThread = GameWorkerThread::Create();
	gameThread->app = this;

	connect(gameThread.get_unsafe(), SIGNAL(Message(unsigned char, const QString&)),
		this, SLOT(OutputMessage(unsigned char, const QString&)));
	connect(gameThread.get_unsafe(), SIGNAL(finished()), this, SLOT(ThreadDone()));

	this->runningThreads.release(sysInfo.GetNumCpuCores());
	
	ui.setupUi(this);
	

	this->args = args;
	
	// create palette
	QtToolkitUtil::NebulaStyleTool palette;
	QApplication::setPalette(palette);
	this->ui.progress->setStyle(new QPlastiqueStyle);
	this->ui.progress->setVisible(false);
	this->ui.status->setVisible(false);

	ProgressNotifier::Instance()->SetProgressBar(this->ui.progress);
	ProgressNotifier::Instance()->SetStatusLabel(this->ui.status);

    if(System::NebulaSettings::Exists("gscept","ToolkitShared", "path"))
    {
        this->toolDir = System::NebulaSettings::ReadString("gscept","ToolkitShared", "path").AsCharPtr();
    }
    if(System::NebulaSettings::Exists("gscept","ToolkitShared", "workdir"))
    {
        this->workDir = System::NebulaSettings::ReadString("gscept","ToolkitShared", "workdir").AsCharPtr();
    }

	if (this->IsFirstTime())
	{
		this->ShowFirstTimeHelpDialog();
	}
	
	this->UpdateTitle();

	
	QMainWindow::connect(ui.exportButton, SIGNAL(clicked()), reinterpret_cast<QMainWindow*>(this), SLOT(Export()));

	// if the exporter failed to start, run export again, but not recursively
	connect(this, SIGNAL(ExportFailed(const QString&)), this, SLOT(Export()));
	connect(ui.actionSetWorkDir, SIGNAL(triggered(bool)), this, SLOT(PickWorkingDir()));
	connect(ui.actionContentbrowser, SIGNAL(triggered(bool)), this, SLOT(StartContentbrowser()));
	connect(ui.actionLeveleditor, SIGNAL(triggered(bool)), this, SLOT(StartLeveleditor()));
	
	connect(ui.actionSetToolkitDirectory, SIGNAL(triggered(bool)), this, SLOT(PickToolkitDir()));
	connect(ui.actionAbout, SIGNAL(triggered(bool)), this, SLOT(ShowAbout()));
	connect(ui.messageList, SIGNAL(itemSelectionChanged()), this, SLOT(SelectionChanged()));
	connect(ui.errorFilter, SIGNAL(clicked()), this, SLOT(UpdateOutputWindow()));
	connect(ui.warningsFilter, SIGNAL(clicked()), this, SLOT(UpdateOutputWindow()));
	connect(ui.infoFilter, SIGNAL(clicked()), this, SLOT(UpdateOutputWindow()));
	
	connect(ui.batchGraphicsButton, SIGNAL(clicked()), this, SLOT(ExportGraphics()));
	connect(ui.batchTexturesButton, SIGNAL(clicked()), this, SLOT(ExportTextures()));
	connect(ui.batchShaderButton, SIGNAL(clicked()), this, SLOT(ExportShaders()));
	connect(ui.batchGameButton, SIGNAL(clicked()), this, SLOT(ExportGameData()));
	
	this->exportBits = All;	
}

//------------------------------------------------------------------------------
/**
*/
BatchExporterApp::~BatchExporterApp()
{
	ToolkitApp::Close();
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::GatherExports()
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::StopExports()
{

	ProgressNotifier::Instance()->End();
}


//------------------------------------------------------------------------------

void
BatchExporterApp::Export()
{	
	this->ClearLogs();
	
	this->ExportGraphics(false);

	this->ExportTextures(false);

	this->ExportShaders(false);

	this->ExportGameData(false);	
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::ExporterDone(int exitCode, QProcess::ExitStatus status)
{
	/*
	if (this->executionQueue.size() > 0)
	{
		// we assume the running app is in the beginning of the stack
		QString appWithArgs = this->executionQueue.front().second;
		QString app = appWithArgs.split(" ")[0];
		this->executionQueue.pop_front();
		this->ui.progress->setValue(0);
		this->ui.status->setText("");
		if (status == QProcess::NormalExit)
		{
            this->OutputExportStatus("--------------------Exporter: " + app + " exited without errors!--------------------");
		}
		else
		{
            this->OutputErrorMessage("--------------------Exporter: " + app + " exited with errors!--------------------");
			this->executionQueue.clear();
		}	
		ProgressNotifier::Instance()->End();
	}
	*/
	// continues the exporting
	this->Export();
}


//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::PickWorkingDir()
{
    Util::String currentDir = IO::FSWrapper::GetUserDirectory();
	
    if(System::NebulaSettings::Exists("gscept","ToolkitShared", "workdir"))
    {
        currentDir = System::NebulaSettings::ReadString("gscept","ToolkitShared", "workdir").AsCharPtr();
    }
    	
	QFileDialog fileBrowser(NULL, "Pick a working directory", currentDir.AsCharPtr());
	fileBrowser.setAcceptMode(QFileDialog::AcceptOpen);
	fileBrowser.setFileMode(QFileDialog::Directory);
	fileBrowser.setOption(QFileDialog::ShowDirsOnly);

	if (fileBrowser.exec() == QDialog::Accepted)
	{
		QDir directory = fileBrowser.directory();
        if(QFile::exists(directory.path() + "/projectinfo.xml"))
        {
            this->workDir = directory.absolutePath();
            System::NebulaSettings::WriteString("gscept","ToolkitShared","workdir",this->workDir.toAscii().constData());    
			this->projectInfo.Discard();
			this->projectInfo.Setup();
        }
        else
        {
            QMessageBox box;
            box.setWindowTitle("Invalid Directory");
            box.setText("Invalid working directory chosen, has to contain projectinfo.xml");
            box.exec();
        }
	}
	this->UpdateTitle();
}


//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::PickToolkitDir()
{
    Util::String currentDir = IO::FSWrapper::GetUserDirectory();
    
    if(System::NebulaSettings::Exists("gscept","ToolkitShared", "path"))
    {
        currentDir = System::NebulaSettings::ReadString("gscept","ToolkitShared", "path").AsCharPtr();
    }
    
	QFileDialog fileBrowser(NULL, "Locate the toolkit directory", currentDir.AsCharPtr());
    
	fileBrowser.setAcceptMode(QFileDialog::AcceptOpen);
	fileBrowser.setFileMode(QFileDialog::Directory);
	fileBrowser.setOption(QFileDialog::ShowDirsOnly);

	if (fileBrowser.exec() == QDialog::Accepted)
	{
		QDir directory = fileBrowser.directory();
        if(QFile::exists(directory.path() + "/projectinfo.xml"))
        {
            this->toolDir = directory.absolutePath();
            System::NebulaSettings::WriteString("gscept","ToolkitShared","path",this->toolDir.toAscii().constData());  
			this->projectInfo.Discard();
			this->projectInfo.Setup();
        }
        else
        {
            QMessageBox box;
            box.setWindowTitle("Invalid Directory");
            box.setText("Invalid directory chosen for toolkit, has to contain projectinfo.xml and tools");
            box.exec();
        }        
	}
	this->UpdateTitle();
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::ShowAbout()
{
	QDialog aboutDialog;
	Ui::AboutDialog aboutUi;
	aboutUi.setupUi(&aboutDialog);
	aboutDialog.exec();
}

//------------------------------------------------------------------------------
/**
*/
const QString
BatchExporterApp::FromBit( ExportBits bit )
{
	switch (bit)
	{
	case Assets:
		return "Assets";
	case Shaders:
		return "Shaders";
	case GameData:
		return "Game Data";	
	case Audio:
		return "Audio";
	default:
		return "";
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
BatchExporterApp::IsFirstTime() const
{
    if(this->workDir.isEmpty() || this->toolDir.isEmpty())
    {
        return true;
    }
    return false;    
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::ShowFirstTimeHelpDialog()
{
	//FIXME: Optimally this should ask the user if setup.bat should be run, and then do so if pressed 'Yes'
	QMessageBox messageBox;
	messageBox.setWindowTitle("Configuration might be required");
	messageBox.setText("It looks like you are running the batch exporter for the first time!\n"
		"You must configure directory paths, but this can be done easily by running setup.bat in the nebula folder.");
	messageBox.exec();
}


//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::UpdateTitle()
{
	QString title = "Nebula Batch Exporter - ";
	this->window()->setWindowTitle(title + this->workDir);
	this->ui.toolkitLabel->setText("Using toolkit: '" + this->toolDir + "'");	
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::SelectionChanged()
{	
	this->ui.messageText->clear();
	QLogTreeItem * item = dynamic_cast<QLogTreeItem*>(this->ui.messageList->currentItem());
	if (item)
	{		
		for (int j = 0; j <  item->logs.Size(); j++)
		{
			this->ui.messageText->setTextBackgroundColor(LogLevelToBackgroundColour(item->logs[j].level));
			this->ui.messageText->setTextColor(LogLevelToTextColour(item->logs[j].level));
			this->ui.messageText->setCurrentFont(LogLevelToFont(item->logs[j].level));
			this->ui.messageText->insertPlainText(item->logs[j].message.AsCharPtr());
		}		
	}			
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::UpdateOutputWindow()
{
	this->ui.messageList->clear();
	this->treeItems.Clear();
	
	Util::String lastAsset;
	QLogTreeItem * currentItem;
	unsigned char displayLevel = 0;
	if (this->ui.errorFilter->isChecked())
	{
		displayLevel |= ToolkitConsoleHandler::LogError;
	}
	if (this->ui.warningsFilter->isChecked())
	{
		displayLevel |= ToolkitConsoleHandler::LogWarning;
	}
	if (this->ui.infoFilter->isChecked())
	{
		displayLevel |= ToolkitConsoleHandler::LogInfo | ToolkitConsoleHandler::LogDebug;
	}
	for (int i = 0; i < this->messages.Size(); i++)
	{
		if ((this->messages[i].logLevels & displayLevel) > 0)
		{
			if (this->treeItems.Contains(this->messages[i].asset))
			{
				currentItem = this->treeItems[this->messages[i].asset];
			}
			else
			{
				currentItem = new QLogTreeItem();
				currentItem->setData(0, Qt::DisplayRole, this->messages[i].asset.AsCharPtr());
				Util::Array<ToolkitConsoleHandler::LogEntry> assetLogs;
				currentItem->logs = assetLogs;
				this->ui.messageList->addTopLevelItem(currentItem);
				this->treeItems.Add(this->messages[i].asset, currentItem);
			}
			
			const Util::Array<ToolkitUtil::ToolLogEntry> & logs = this->messages[i].logs;
			for (int j = 0; j < logs.Size(); j++)
			{
				if (logs[j].logs.Size() && (logs[j].logLevels & displayLevel) > 0)
				{
					QStringList fields;
					fields.append("");
					fields.append(logs[j].tool.AsCharPtr());
					fields.append(logs[j].source.AsCharPtr());
					QLogTreeItem * newItem = new QLogTreeItem(fields);
					currentItem->addChild(newItem);
					newItem->logs = logs[j].logs;
					currentItem->logs.AppendArray(logs[j].logs);
					if (logs[j].logLevels > 0x02)
					{
						for (int k = 0; k < 3; k++)
						{
							newItem->setBackgroundColor(k, LogLevelToBackgroundColour(logs[j].logLevels));
							newItem->setTextColor(k, LogLevelToTextColour(logs[j].logLevels));
							newItem->setFont(k, LogLevelToFont(this->messages[i].logLevels));
						}
					}
				}
			}
			if (this->messages[i].logLevels > 0x02)
			{
				for (int k = 0; k < 3; k++)
				{
					currentItem->setBackgroundColor(k, LogLevelToBackgroundColour(this->messages[i].logLevels));
					currentItem->setTextColor(k, LogLevelToTextColour(this->messages[i].logLevels));
					currentItem->setFont(k, LogLevelToFont(this->messages[i].logLevels));
				}
			}
			if (this->messages[i].logLevels & ToolkitConsoleHandler::LogError)
			{
				currentItem->setExpanded(true);
			}			
		}
	}

}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::OutputMessage(unsigned char level, const QString& msg)
{
	this->ui.consoleOut->setTextColor(LogLevelToTextColour(level));
	this->ui.consoleOut->setTextBackgroundColor(LogLevelToBackgroundColour(level));
	this->ui.consoleOut->setCurrentFont(LogLevelToFont(level));
	this->ui.consoleOut->insertPlainText(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::ThreadDone()
{
	this->messageMutex.lock();
	this->UpdateOutputWindow();
	this->messageMutex.unlock();
	if (this->runningThreads.available() == this->workerThreads.Size())
	{
		this->ui.exportButton->setEnabled(true);		
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::AddMessages(const Util::Array<ToolkitUtil::ToolLog>&messages)
{
	this->messageMutex.lock();
	this->messages.AppendArray(messages);
	this->messageMutex.unlock();
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::ExportGraphics(bool clear)
{
	if (clear)
	{
		this->ClearLogs();
	}
	this->systemThread->SetForce(this->force);
	// lock system mutex to delay depending threads
	this->systemBatchMutex.lock();
	this->systemThread->start();
	auto fileList = IO::IoServer::Instance()->ListDirectories("src:assets/", "*");
	int files = fileList.Size();

	// the dedicated fbx thread needs all fbx files
	this->workerThreads[0]->SetWorkAssets(fileList);
	this->workerThreads[0]->SetForce(this->force);
	this->workerThreads[0]->start();	
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::ExportTextures(bool clear)
{
	if (clear)
	{
		this->ClearLogs();
	}
	auto fileList = IO::IoServer::Instance()->ListDirectories("src:assets/", "*");
	int files = fileList.Size();

	int cores = this->workerThreads.Size() - 1;
	Util::FixedArray<Util::Array<Util::String>> jobs;
	jobs.SetSize(cores);
	int current = 0;
	for (int i = 0; i < files; i++)
	{
		jobs[current++].Append(fileList[i]);
		current = current % cores;
	}

	for (int i = 0; i < cores; i++)
	{

		{
			this->workerThreads[i + 1]->SetWorkAssets(jobs[i]);

		}
		this->workerThreads[i + 1]->SetForce(this->force);
		this->workerThreads[i + 1]->start();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::ExportShaders(bool clear)
{
	if (clear)
	{
		this->ClearLogs();
	}
	this->shaderThread->SetForce(this->force);
	this->shaderThread->start();
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::ExportGameData(bool clear)
{
	if (clear)
	{
		this->ClearLogs();
	}	
	this->gameThread->start();
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::ClearLogs()
{
	this->ui.consoleOut->clear();
	this->ui.consoleOut->setTextBackgroundColor(defaultColour);
	this->messages.Clear();
	this->ui.exportButton->setEnabled(false);
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Shift)
	{
		this->SetForce(true);
	}
	QMainWindow::keyPressEvent(e);
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::keyReleaseEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Shift)
	{
		this->SetForce(false);
	}
	QMainWindow::keyReleaseEvent(e);
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::leaveEvent(QEvent* e)
{
	this->SetForce(false);
	QMainWindow::leaveEvent(e);
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::SetForce(bool inForce)
{
	if (inForce)
	{
		this->ui.exportButton->setText("Force Export All");
		this->ui.batchGraphicsButton->setText("Force Batch Graphics");
		this->ui.batchTexturesButton->setText("Force Batch Textures");
		this->ui.batchShaderButton->setText("Force Batch Shaders");
	}
	else
	{
		this->ui.exportButton->setText("Export All");
		this->ui.batchGraphicsButton->setText("Batch Graphics");
		this->ui.batchTexturesButton->setText("Batch Textures");
		this->ui.batchShaderButton->setText("Batch Shaders");
	}
	this->force = inForce;
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::StartContentbrowser()
{
#ifdef DEBUG
	Util::String exe = "bin:contentbrowser.debug";
#else
	Util::String exe = "bin:contentbrowser";
#endif
#ifdef WIN32
	exe += ".exe";
#endif
	IO::URI exeUri(exe);
	if (!ToolkitUtil::AppLauncher::CheckIfExists(exeUri))
	{
		QProcess content;
		content.startDetached(exeUri.LocalPath().AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::StartLeveleditor()
{

#ifdef DEBUG
	Util::String exe = "bin:leveleditor2.debug";
#else
	Util::String exe = "bin:leveleditor2";
#endif
#ifdef WIN32
	exe += ".exe";
#endif
	IO::URI exeUri(exe);
	if (!ToolkitUtil::AppLauncher::CheckIfExists(exeUri))
	{
		QProcess content;
		content.startDetached(exeUri.LocalPath().AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AssetWorkerThread::run()
{
	// check if system batcher is done
	this->app->SystemMutex().lock();
	// unlock right away since we dont actually need it
	this->app->SystemMutex().unlock();

	this->myId = Threading::Thread::GetMyThreadId();
	this->app->WorkerSemaphore().acquire(1);
	IO::Console::Instance()->AttachHandler(this);
	
	Ptr<IO::IoServer> io = IO::IoServer::Create();
	Util::LocalStringAtomTable localStringAtomTable;
	
	this->modelDatabase = ToolkitUtil::ModelDatabase::Create();
	this->modelDatabase->Open();

	Ptr<AssetExporter> exporter = AssetExporter::Create();


	IO::AssignRegistry::Instance()->SetAssign(IO::Assign("home", "proj:"));
	exporter->Open();
	exporter->SetForce(this->force);
	exporter->SetExportFlag(Base::ExporterBase::All);
	exporter->SetPlatform(this->app->GetProjectInfo().GetCurrentPlatform());
	exporter->SetProgressPrecision(1000000);
	if (this->graphics)
	{
		exporter->SetExportMode(AssetExporter::All - AssetExporter::Textures);
	}
	else
	{
		exporter->SetExportMode(AssetExporter::Textures);
	}
	
		
	for (int i = 0; i < this->workPackage.Size(); i++)
	{
		exporter->ExportDir(this->workPackage[i]);		
	}
	//exporter->ExportAll();
	if (this->modelDatabase.isvalid())
	{
		this->modelDatabase->Close();
		this->modelDatabase = 0;
	}
	this->app->AddMessages(exporter->GetMessages());	
	IO::Console::Instance()->RemoveHandler(this);
	this->app->WorkerSemaphore().release(1);	
}

//------------------------------------------------------------------------------
/**
*/
void
WorkerThread::Error(const Util::String& s)
{	
	if (this->myId == Threading::Thread::GetMyThreadId())
	{
		emit Message(ToolkitConsoleHandler::LogError, s.AsCharPtr());
	}
	
}

//------------------------------------------------------------------------------
/**
*/
void
WorkerThread::Warning(const Util::String& s)
{
	if (this->myId == Threading::Thread::GetMyThreadId())
	{
		emit Message(ToolkitConsoleHandler::LogWarning, s.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
WorkerThread::Print(const Util::String& s)
{
	if (this->myId == Threading::Thread::GetMyThreadId())
	{
		emit Message(ToolkitConsoleHandler::LogInfo, s.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
WorkerThread::DebugOut(const Util::String& s)
{
	if (this->myId == Threading::Thread::GetMyThreadId())
	{
		emit Message(ToolkitConsoleHandler::LogDebug, s.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
WorkerThread::SetForce(bool force)
{
	this->force = force;
}

//------------------------------------------------------------------------------
/**
*/
AssetWorkerThread::AssetWorkerThread() : graphics(false)
{
	// empty
}
//------------------------------------------------------------------------------
/**
*/
void
AssetWorkerThread::SetWorkAssets(const Util::Array<Util::String> & assets)
{
	this->workPackage = assets;
}

//------------------------------------------------------------------------------
/**
*/
void
AssetWorkerThread::BatchGraphics(bool enable)
{
	this->graphics = enable;
}

//------------------------------------------------------------------------------
/**
*/
bool
AssetWorkerThread::GetBatchGraphics(void) const
{
	return this->graphics;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderWorkerThread::run()
{
	this->app->SystemMutex().lock();
	this->myId = Threading::Thread::GetMyThreadId();
	this->app->WorkerSemaphore().acquire(1);
	IO::Console::Instance()->AttachHandler(this);

	Ptr<IO::IoServer> io = IO::IoServer::Create();
	Util::LocalStringAtomTable localStringAtomTable;

	n_printf("------------- Starting Shaderbatcher -------------\n");

	const ProjectInfo& projectInfo = this->app->GetProjectInfo();
	this->shaderCompiler.SetPlatform(projectInfo.GetCurrentPlatform());
	if (projectInfo.HasAttr("ShaderToolParams"))
	{
		this->shaderCompiler.SetAdditionalParams(projectInfo.GetAttr("ShaderToolParams"));
	}

	// setup required stuff in compiler
	this->shaderCompiler.SetLanguage(projectInfo.GetAttr("ShaderLanguage"));
	this->shaderCompiler.SetSrcShaderBaseDir(projectInfo.GetAttr("ShaderSrcDir"));
	this->shaderCompiler.SetDstShaderDir(projectInfo.GetAttr("ShaderDstDir"));
	this->shaderCompiler.SetSrcFrameShaderBaseDir(projectInfo.GetAttr("FrameShaderSrcDir"));
	this->shaderCompiler.SetDstFrameShaderDir(projectInfo.GetAttr("FrameShaderDstDir"));
	this->shaderCompiler.SetSrcMaterialBaseDir(projectInfo.GetAttr("MaterialsSrcDir"));
	this->shaderCompiler.SetDstMaterialsDir(projectInfo.GetAttr("MaterialsDstDir"));
	this->shaderCompiler.SetForceFlag(this->force);

	// setup custom stuff
	if (projectInfo.HasAttr("ShaderSrcCustomDir")) this->shaderCompiler.SetSrcShaderCustomDir(projectInfo.GetAttr("ShaderSrcCustomDir"));
	if (projectInfo.HasAttr("FrameShaderSrcCustomDir")) this->shaderCompiler.SetSrcFrameShaderCustomDir(projectInfo.GetAttr("FrameShaderSrcCustomDir"));
	if (projectInfo.HasAttr("MaterialsSrcCustomDir")) this->shaderCompiler.SetSrcMaterialCustomDir(projectInfo.GetAttr("MaterialsSrcCustomDir"));

	Util::Array<ToolLog> messages;
	Ptr<ToolkitUtil::ToolkitConsoleHandler> console = ToolkitUtil::ToolkitConsoleHandler::Instance();

	// FIXME, this is too basic, should have per file
	// call the shader compiler tool
	ToolLog log("Shaders");
	this->shaderCompiler.CompileShaders();
	log.AddEntry(console, "Shader Compiler", "...");
	console->Clear();
	this->shaderCompiler.CompileFrameShaders();
	log.AddEntry(console, "Frame Shader Compiler", "...");
	console->Clear();
	this->shaderCompiler.CompileMaterials();
	log.AddEntry(console, "Materials Compiler", "...");
	messages.Append(log);
	this->app->AddMessages(messages);
	IO::Console::Instance()->RemoveHandler(this);
	this->app->WorkerSemaphore().release(1);	
	this->app->SystemMutex().unlock();
}

#ifdef WIN32
typedef
const char* (WINAPI *BatchFunc)(void);
#else
typedef const char* (*BatchFunc)(void);
#endif

//------------------------------------------------------------------------------
/**
*/
void
GameWorkerThread::run()
{
 	this->myId = Threading::Thread::GetMyThreadId();
 	this->app->WorkerSemaphore().acquire(1);
 	IO::Console::Instance()->AttachHandler(this);
 
 	Ptr<IO::IoServer> io = IO::IoServer::Create();
 	Util::LocalStringAtomTable localStringAtomTable;
 
 	n_printf("------------- Starting Game batcher -------------\n");


#ifdef DEBUG
	QString libFile = "gamebatch.debug";
#else
	QString libFile = "gamebatch";
#endif
	QLibrary batcherLib(libFile);
	if (batcherLib.load())
	{
			
		void * funcPtr = batcherLib.resolve("BatchGameData");

		if (funcPtr != NULL)
		{
			BatchFunc func;
			func = (BatchFunc)funcPtr;
			const char * log = func();
			if (log)
			{
				Util::String logString(log);
				Ptr<IO::MemoryStream> stream = IO::MemoryStream::Create();
				stream->Open();
				stream->SetSize(logString.Length());
				void * data = stream->Map();
				Memory::Copy(logString.AsCharPtr(), data, logString.Length());
				stream->Close();
				stream->SetAccessMode(IO::Stream::ReadAccess);
				Ptr<IO::XmlReader> reader = IO::XmlReader::Create();
				reader->SetStream(stream.cast<IO::Stream>());
				reader->Open();
				Util::Array<ToolkitUtil::ToolLog> logs;

				if (reader->SetToFirstChild("Log"))
				{
					do
					{
						logs.Append(ToolkitUtil::ToolLog::FromString(reader));
					} while (reader->SetToNextChild("Log"));
				}

				reader->Close();
				this->app->AddMessages(logs);
#ifdef WIN32
				CoTaskMemFree((LPVOID)log);
#else
				Memory::Free(Memory::DefaultHeap, log);
#endif
			}
			n_printf("------------- Game batcher Done -----------------\n");
		}
		else
		{
			n_error("Unable to locate function in gamebatch library, is everything installed/compiled correctly?");
		}
		batcherLib.unload();
	}
	else
	{
		n_error("Unable to load gamebatch library, is everything installed/compiled correctly?");
	}

 	IO::Console::Instance()->RemoveHandler(this);
 	this->app->WorkerSemaphore().release(1);
}

//------------------------------------------------------------------------------
/**
*/
void
SystemWorkerThread::run()
{
	this->myId = Threading::Thread::GetMyThreadId();
	this->app->WorkerSemaphore().acquire(1);
	IO::Console::Instance()->AttachHandler(this);

	Ptr<IO::IoServer> io = IO::IoServer::Create();
	Util::LocalStringAtomTable localStringAtomTable;

	this->modelDatabase = ToolkitUtil::ModelDatabase::Create();
	this->modelDatabase->Open();

	Ptr<AssetExporter> exporter = AssetExporter::Create();


	IO::AssignRegistry::Instance()->SetAssign(IO::Assign("home", "proj:"));
	exporter->Open();
	exporter->SetForce(this->force);
	exporter->SetExportFlag(Base::ExporterBase::All);
	exporter->SetPlatform(this->app->GetProjectInfo().GetCurrentPlatform());
	exporter->SetProgressPrecision(1000000);
	exporter->SetExportMode(AssetExporter::All);
	exporter->ExportSystem();

	//exporter->ExportAll();
	if (this->modelDatabase.isvalid())
	{
		this->modelDatabase->Close();
		this->modelDatabase = 0;
	}
	this->app->AddMessages(exporter->GetMessages());
	IO::Console::Instance()->RemoveHandler(this);
	this->app->WorkerSemaphore().release(1);
	// system batcher is done, we are free to start other threads using src: assigns
	this->app->SystemMutex().unlock();
}

} // namespace BatchExporter

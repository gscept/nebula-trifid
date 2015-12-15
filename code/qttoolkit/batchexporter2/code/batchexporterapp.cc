//------------------------------------------------------------------------------
//  batchexporterapp.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "stdneb.h"
#include <QFileDialog>
#include <QPlastiqueStyle>
#include <QMessageBox>
#include <QScrollBar>
#include "batchexporterapp.h"
#include "ui_about.h"
#include "tools/progressnotifier.h"
#include "style/graypalette.h"
#include "util/string.h"
#include "system/nebulasettings.h"
#include "io/fswrapper.h"
#include "io/assignregistry.h"
#include "toolkitconsolehandler.h"
#include "io/win32/win32consolehandler.h"
#include "asset/assetexporter.h"
#include "base/exporterbase.h"
#include "QTreeWidget"
#include "QColor"
#include "QtConcurrentRun"
#include "util/localstringatomtable.h"
#include "system/systeminfo.h"


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

class QLogTreeItem : public QTreeWidgetItem
{
public:
	QLogTreeItem() :QTreeWidgetItem(){}
	QLogTreeItem(const QStringList &strings) : QTreeWidgetItem(strings)
	{}

	Util::Array<ToolkitConsoleHandler::LogEntry> logs;
};

QColor errorColor(255, 0, 0);
QColor warningColor(255, 255, 0);
QColor defaultColour(90, 90, 90);

QColor LogLevelToColour(unsigned char level)
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



namespace BatchExporter
{

//------------------------------------------------------------------------------
/**
*/
BatchExporterApp::BatchExporterApp() : 
	exportBits(All)
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
	const Util::Array<Ptr<IO::ConsoleHandler>> & handlers = console->GetHandlers();
	for (int i = 0; i < handlers.Size(); i++)
	{
		if (handlers[i]->IsA(Win32::Win32ConsoleHandler::RTTI))
		{
			console->RemoveHandler(handlers[i]);
		}
	}
	Ptr<ToolkitUtil::ToolkitConsoleHandler> handler = ToolkitUtil::ToolkitConsoleHandler::Create();
	console->AttachHandler(handler.cast<IO::ConsoleHandler>());
	
	this->SetupProjectInfo();

	System::SystemInfo sysInfo;
	for (int i = 0; i < sysInfo.GetNumCpuCores(); i++)
	{
		Ptr<AssetWorkerThread> thread = AssetWorkerThread::Create();
		thread->app = this;
		if (i == 0)
		{
			thread->BatchSystem(true);
		}
		else
		{
			thread->BatchSystem(false);
		}
		this->workerThreads.Append(thread);
		connect(thread.get_unsafe(), SIGNAL(Message(unsigned char, const QString&)),
			this, SLOT(OutputMessage(unsigned char, const QString&)));
		connect(thread.get_unsafe(), SIGNAL(finished()), this, SLOT(ThreadDone()));
	}
	this->runningThreads.release(sysInfo.GetNumCpuCores());

	ui.setupUi(this);
	

	this->args = args;
	
	// create palette
	QtToolkitUtil::GrayPalette palette;
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
	
	connect(ui.actionSetToolkitDirectory, SIGNAL(triggered(bool)), this, SLOT(PickToolkitDir()));
	connect(ui.actionAbout, SIGNAL(triggered(bool)), this, SLOT(ShowAbout()));
	connect(ui.messageList, SIGNAL(itemSelectionChanged()), this, SLOT(SelectionChanged()));
	connect(ui.errorFilter, SIGNAL(clicked()), this, SLOT(UpdateOutputWindow()));
	connect(ui.warningsFilter, SIGNAL(clicked()), this, SLOT(UpdateOutputWindow()));
	connect(ui.infoFilter, SIGNAL(clicked()), this, SLOT(UpdateOutputWindow()));
	
	
	
	/*
    if(System::NebulaSettings::Exists("gscept", "ToolkitShared.batchexporter", "force"))
    {
        Util::String forceSetting = System::NebulaSettings::ReadString("gscept", "ToolkitShared.batchexporter", "force");
        ui.forceExport->setChecked(forceSetting.AsBool());
    }

    if(System::NebulaSettings::Exists("gscept", "ToolkitShared.batchexporter", "exportFlags"))	
	{
        int flag = System::NebulaSettings::ReadString("gscept", "ToolkitShared.batchexporter", "exportFlags").AsInt();		
		this->exportBits = flag;
		ui.exportGraphics->setChecked(flag & Graphics);
		ui.exportModels->setChecked(flag & Models);			
		ui.exportTextures->setChecked(flag & Textures);
		ui.exportShaders->setChecked(flag & Shaders);
		ui.exportGameData->setChecked(flag & GameData);
		ui.exportAudio->setChecked(flag & Audio);
	}
	else
	*/
	{
		this->exportBits = All;
	}
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
	this->ui.consoleOut->clear();
	this->ui.consoleOut->setTextBackgroundColor(defaultColour);
	this->messages.Clear();
	this->ui.exportButton->setEnabled(false);
	auto fileList = IO::IoServer::Instance()->ListDirectories("src:assets/", "*");
	int files = fileList.Size();
	
	int cores = this->workerThreads.Size();
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
		this->workerThreads[i]->SetWorkAssets(jobs[i]);
		this->workerThreads[i]->start();
	}	
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
			this->ui.messageText->setTextBackgroundColor(LogLevelToColour(item->logs[j].level));
			this->ui.messageText->append(item->logs[j].message.AsCharPtr());
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
			currentItem = new QLogTreeItem();
			currentItem->setData(0, Qt::DisplayRole, this->messages[i].asset.AsCharPtr());
			Util::Array<ToolkitConsoleHandler::LogEntry> assetLogs;
			this->ui.messageList->addTopLevelItem(currentItem);

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
					assetLogs.AppendArray(logs[j].logs);
					if (logs[j].logLevels > 0x02)
					{
						for (int k = 0; k < 3; k++)
						{
							newItem->setBackgroundColor(k, LogLevelToColour(logs[j].logLevels));
						}
					}
				}
			}
			if (this->messages[i].logLevels > 0x02)
			{
				for (int k = 0; k < 3; k++)
				{
					currentItem->setBackgroundColor(k, LogLevelToColour(this->messages[i].logLevels));
				}
			}
			if (this->messages[i].logLevels & ToolkitConsoleHandler::LogError)
			{
				currentItem->setExpanded(true);
			}

			currentItem->logs = assetLogs;		
		}
	}

}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::OutputMessage(unsigned char level, const QString& msg)
{
	switch (level)
	{
	case ToolkitConsoleHandler::LogError:
		this->ui.consoleOut->setTextBackgroundColor(errorColor);
		break;
	case ToolkitConsoleHandler::LogWarning:
		this->ui.consoleOut->setTextBackgroundColor(warningColor);
		break;
	case ToolkitConsoleHandler::LogInfo:
	case ToolkitConsoleHandler::LogDebug:
		this->ui.consoleOut->setTextBackgroundColor(defaultColour);
	}
	this->ui.consoleOut->append(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::ThreadDone()
{
	if (this->runningThreads.available() == this->workerThreads.Size())
	{
		this->ui.exportButton->setEnabled(true);
		this->UpdateOutputWindow();
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
AssetWorkerThread::run()
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
	exporter->SetForce(false);
	exporter->SetExportFlag(Base::ExporterBase::All);
	exporter->SetPlatform(this->app->GetProjectInfo().GetCurrentPlatform());
	exporter->SetProgressPrecision(1000000);
	if (this->system)
	{
		exporter->ExportSystem();
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
AssetWorkerThread::SetWorkAssets(const Util::Array<Util::String> & assets)
{
	this->workPackage = assets;
}

//------------------------------------------------------------------------------
/**
*/
void
AssetWorkerThread::BatchSystem(bool enable)
{
	this->system = enable;
}

} // namespace BatchExporter
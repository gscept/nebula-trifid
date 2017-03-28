//------------------------------------------------------------------------------
//  batchexporterapp.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "stdneb.h"
#include <QFileDialog>
#ifdef __USE_QT4
#include <QPlastiqueStyle>
#endif
#include <QMessageBox>
#include <QScrollBar>
#include <QTreeWidget>
#include <QColor>
#include "batchexporterapp.h"
#include "ui_about.h"
#include "tools/progressnotifier.h"
#include "style/nebulastyletool.h"
#include "util/string.h"
#include "system/nebulasettings.h"
#include "io/fswrapper.h"
#include "io/assignregistry.h"
#include "io/memorystream.h"
#include "io/xmlreader.h"
#include "toolkitconsolehandler.h"
#include "applauncher.h"


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

namespace BatchExporter
{

class QLogTreeItem : public QTreeWidgetItem
{
public:
	QLogTreeItem() :QTreeWidgetItem(){}
	QLogTreeItem(const QStringList &strings) : QTreeWidgetItem(strings)
	{}

	Util::Array<ToolkitUtil::ToolkitConsoleHandler::LogEntry> logs;
};

QColor errorColor(255, 0, 0);
QColor warningColor(255, 255, 0);
QColor defaultColour(90, 90, 90);

QColor LogLevelToColour(unsigned char level)
{
	if (level & ToolkitUtil::ToolkitConsoleHandler::LogError)
	{
		return errorColor;
	}
	else if (level & ToolkitUtil::ToolkitConsoleHandler::LogWarning)
	{
		return warningColor;
	}
	return defaultColour;
}


//------------------------------------------------------------------------------
/**
*/
BatchExporterApp::BatchExporterApp(const CommandLineArgs& args) : 
	exportBits(All)
{
    
    this->nebApp = new App::ConsoleApplication();
    this->nebApp->Open();
	ui.setupUi(this);

	this->args = args;
	
	// create palette
	QtToolkitUtil::NebulaStyleTool palette;
	QApplication::setPalette(palette);
#ifdef __USE_QT4
	this->ui.progress->setStyle(new QPlastiqueStyle);
#endif
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

	this->projInfo.SetCurrentPlatform(DEFAULTPLATFORM);
	this->projInfo.Setup();
	
	connect(ui.exportGraphics, SIGNAL(toggled(bool)), this, SLOT(SetExportGraphics(bool)));	
	connect(ui.exportShaders, SIGNAL(toggled(bool)), this, SLOT(SetExportShaders(bool)));
	connect(ui.exportGameData, SIGNAL(toggled(bool)), this, SLOT(SetExportGameData(bool)));	
	connect(ui.exportAudio, SIGNAL(toggled(bool)), this, SLOT(SetExportAudio(bool)));
	connect(ui.forceExport, SIGNAL(toggled(bool)), this, SLOT(SetForce(bool)));

	connect(ui.actionContentbrowser, SIGNAL(triggered(bool)), this, SLOT(StartContentbrowser()));
	connect(ui.actionLeveleditor, SIGNAL(triggered(bool)), this, SLOT(StartLeveleditor()));


	connect(ui.exportButton, SIGNAL(clicked()), this, SLOT(GatherExports()));
	connect(&this->remoteProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(OutputMessage()));
	connect(&this->remoteProcess, SIGNAL(readyReadStandardError()), this, SLOT(OutputStderr()));
	connect(&this->remoteProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(ExporterDone(int, QProcess::ExitStatus)));

	// if the exporter failed to start, run export again, but not recursively
	connect(this, SIGNAL(ExportFailed(const QString&)), this, SLOT(Export()));
	connect(ui.actionSetWorkDir, SIGNAL(triggered(bool)), this, SLOT(PickWorkingDir()));
	//connect(ui.actionSetNodyDirectory, SIGNAL(triggered(bool)), this, SLOT(PickNodyDir()));
	connect(ui.actionSetToolkitDirectory, SIGNAL(triggered(bool)), this, SLOT(PickToolkitDir()));
	connect(ui.actionAbout, SIGNAL(triggered(bool)), this, SLOT(ShowAbout()));

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
		ui.exportShaders->setChecked(flag & Shaders);
		ui.exportGameData->setChecked(flag & GameData);
		ui.exportAudio->setChecked(flag & Audio);
	}
	else
	{
		this->exportBits = All;
	}
}

//------------------------------------------------------------------------------
/**
*/
BatchExporterApp::~BatchExporterApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::GatherExports()
{
	/// clears the output window
	this->ui.outputText->clear();
	QString jobsString;
	if(ui.jobs->value()>1)
	{			
		jobsString.sprintf("-jobs %d -processors %d",ui.jobs->value(),ui.jobs->value());
	}
	this->executionQueue.clear();
	QString force = ui.forceExport->isChecked() ? "true" : "false";
	if (this->exportBits & Graphics)
	{
		QStringList processArgs;
		if (ui.forceExport->isChecked())
		{
			processArgs.append("-force");
		}

		Util::String gb = IO::URI(IO::AssignRegistry::Instance()->ResolveAssignsInString(this->projInfo.GetPathAttr("AssetBatcher") + EXESUFFIX)).LocalPath();
		gb = "\"" + gb + "\"";
		QString graphicsBatcherPath = gb.AsCharPtr();

		// batch system first
		QStringList systemArgs = processArgs;
		systemArgs.append("-system");
		 
		QString batcherPath = graphicsBatcherPath;
		for (int argIndex = 0; argIndex < systemArgs.size(); argIndex++)
		{
			batcherPath += " " + systemArgs[argIndex];
		}
		executionQueue.append(QPair<ExportBits, QString>(Graphics, batcherPath));

		processArgs.append(jobsString);
        
		for (int argIndex = 0; argIndex < processArgs.size(); argIndex++)
		{
			graphicsBatcherPath += " " + processArgs[argIndex];
		}
		executionQueue.append(QPair<ExportBits, QString>(Graphics, graphicsBatcherPath));
	}	
	if (this->exportBits & Shaders)
	{
		QStringList processArgs;
		if (ui.forceExport->isChecked())
		{
			processArgs.append("-force");
		}
		Util::String gb = IO::URI(IO::AssignRegistry::Instance()->ResolveAssignsInString(this->projInfo.GetPathAttr("ShaderBatcher") + EXESUFFIX)).LocalPath();        		
		gb = "\"" + gb + "\"";
		QString nodyPath = gb.AsCharPtr();    		
		for (int argIndex = 0; argIndex < processArgs.size(); argIndex++)
		{
			nodyPath += " " + processArgs[argIndex];
		}
		executionQueue.append(QPair<ExportBits, QString>(Shaders, nodyPath));
	}
	if (this->exportBits & GameData)
	{
        Util::String gb = IO::URI(IO::AssignRegistry::Instance()->ResolveAssignsInString(this->projInfo.GetPathAttr("GameBatcher") + EXESUFFIX)).LocalPath();        
		gb = "\"" + gb + "\"";
		QString templateBatcherPath = gb.AsCharPtr();    		
		executionQueue.append(QPair<ExportBits, QString>(GameData, templateBatcherPath));
	}
	if (this->exportBits & Audio)
	{
		if (this->projInfo.HasAttr("AudioBatcher"))
		{
			Util::String gb = IO::URI(IO::AssignRegistry::Instance()->ResolveAssignsInString(this->projInfo.GetPathAttr("AudioBatcher") + EXESUFFIX)).LocalPath();
			gb = "\"" + gb + "\"";
			QString audioBatcherPath = gb.AsCharPtr();
			executionQueue.append(QPair<ExportBits, QString>(Audio, audioBatcherPath));
		}
		else
		{
			this->OutputErrorMessage("-------------------- No AudioBatcher defined in projectinfo.xml --------------------");
		}
        
	}

	this->ui.exportButton->setText("Abort");
	disconnect(this->ui.exportButton, SIGNAL(clicked()), this, SLOT(GatherExports()));
	connect(this->ui.exportButton, SIGNAL(clicked()), this, SLOT(StopExports()));

    this->OutputMarkerMessage("--------------------Starting export--------------------");
	this->Export();
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::StopExports()
{
	this->remoteProcess.kill();
	this->executionQueue.clear();
	ProgressNotifier::Instance()->End();
}

//------------------------------------------------------------------------------
/**
	Exports using a single application
*/
bool 
BatchExporterApp::Export()
{
	if (!this->executionQueue.isEmpty())
	{
		QString appWithArgs = this->executionQueue.front().second;
		QString stringBit = this->FromBit(this->executionQueue.front().first);		
		this->remoteProcess.start(appWithArgs);
		bool started = this->remoteProcess.waitForStarted(1000);
		if (!started)
		{
            this->OutputErrorMessage("--------------------Failed to start exporter: " + appWithArgs + "--------------------");
			this->executionQueue.pop_front();
			emit ExportFailed(appWithArgs);
			return false;
		}
		else
		{
			ProgressNotifier::Instance()->Start("Exporting " + stringBit + " ...");
            this->OutputExportStatus("--------------------Starting exporter: " + appWithArgs + "--------------------<br>");
            this->SetDefaultMessageStyle();
		}
	}
	else
	{
		disconnect(this->ui.exportButton, SIGNAL(clicked()), this, SLOT(StopExports()));
		connect(this->ui.exportButton, SIGNAL(clicked()), this, SLOT(GatherExports()));
		this->ui.exportButton->setText("Export");
        this->OutputMarkerMessage("--------------------Export done!--------------------");
	}

	// exporter executed nicely
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::ExporterDone(int exitCode, QProcess::ExitStatus status)
{
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

	// continues the exporting
	this->Export();
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::OutputMessage()
{
    QByteArray data = this->remoteProcess.readAllStandardOutput();
    QString outputString(data);
    outputString.replace('\n', "<br>");    
    this->OutputStandardMessage(outputString);
}

//------------------------------------------------------------------------------
/**
*/
void
BatchExporterApp::OutputStderr()
{
	QByteArray dataArray = this->remoteProcess.readAllStandardError();
	
	
	{
		Ptr<IO::MemoryStream> stream = IO::MemoryStream::Create();
		stream->Open();
		stream->SetSize(dataArray.count());
		void * data = stream->Map();		
		Memory::Copy(dataArray.data(), data, dataArray.length());
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
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::SetForce( bool force )
{
	this->force = force;
 
    Util::String forceString = Util::String::FromBool(this->force);
    System::NebulaSettings::WriteString("gscept", "ToolkitShared.batchexporter", "force", forceString);    	
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::SetExportGraphics( bool state )
{
	if (state)
	{
		this->exportBits |= Graphics;
	}
	else
	{
		this->exportBits &= ~Graphics;
	}
    Util::String intString = Util::String::FromInt(this->exportBits);
	System::NebulaSettings::WriteString("gscept", "ToolkitShared.batchexporter", "exportFlags", intString);	
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::SetExportModels( bool state )
{
	if (state)
	{
		this->exportBits |= Models;
	}
	else
	{
		this->exportBits &= ~Models;
	}

	Util::String intString = Util::String::FromInt(this->exportBits);
    System::NebulaSettings::WriteString("gscept", "ToolkitShared.batchexporter", "exportFlags", intString);    
}


//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::SetExportPhysics( bool state )
{
	if (state)
	{
		this->exportBits |= Physics;
	}
	else
	{
		this->exportBits &= ~Physics;
	}

	Util::String intString = Util::String::FromInt(this->exportBits);
    System::NebulaSettings::WriteString("gscept", "ToolkitShared.batchexporter", "exportFlags", intString);    
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::SetExportTextures( bool state )
{
	if (state)
	{
		this->exportBits |= Textures;
	}
	else
	{
		this->exportBits &= ~Textures;
	}

	Util::String intString = Util::String::FromInt(this->exportBits);
    System::NebulaSettings::WriteString("gscept", "ToolkitShared.batchexporter", "exportFlags", intString);    
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::SetExportShaders( bool state )
{
	if (state)
	{
		this->exportBits |= Shaders;
	}
	else
	{
		this->exportBits &= ~Shaders;
	}

	Util::String intString = Util::String::FromInt(this->exportBits);
    System::NebulaSettings::WriteString("gscept", "ToolkitShared.batchexporter", "exportFlags", intString);    
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::SetExportGameData( bool state )
{
	if (state)
	{
		this->exportBits |= GameData;
	}
	else
	{
		this->exportBits &= ~GameData;
	}

	Util::String intString = Util::String::FromInt(this->exportBits);
    System::NebulaSettings::WriteString("gscept", "ToolkitShared.batchexporter", "exportFlags", intString);    
}


//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::SetExportAudio( bool state )
{
	if (state)
	{
		this->exportBits |= Audio;
	}
	else
	{
		this->exportBits &= ~Audio;
	}
	Util::String intString = Util::String::FromInt(this->exportBits);
    System::NebulaSettings::WriteString("gscept", "ToolkitShared.batchexporter", "exportFlags", intString);    
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
            System::NebulaSettings::WriteString("gscept","ToolkitShared","workdir",this->workDir.toLatin1().constData());    
			this->projInfo.Discard();
			this->projInfo.Setup();
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
            System::NebulaSettings::WriteString("gscept","ToolkitShared","path",this->toolDir.toLatin1().constData());  
			this->projInfo.Discard();
			this->projInfo.Setup();
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
	case Graphics:
		return "Graphics";
	case Models:
		return "Models";
	case Physics:
		return "Physics";
	case Textures:
		return "Textures";
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
BatchExporterApp::SetDefaultMessageStyle()
{
    QTextBlockFormat block;
    QTextCursor cursor(this->ui.outputText->document());
    block.setAlignment(Qt::AlignLeft);
    block.setBackground(Qt::transparent);
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(block);
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::OutputMarkerMessage( const QString& message )
{
    QTextCursor cursor(this->ui.outputText->document());
    QTextBlockFormat block;
    block.setAlignment(Qt::AlignCenter);
    block.setBackground(Qt::darkGreen);
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(block);
    QString html;
    html.sprintf("<font color=\"white\">%s</font>", message.toUtf8().constData());
    cursor.insertHtml(html);

    QScrollBar* sb = this->ui.outputText->verticalScrollBar();
    sb->setSliderPosition(sb->maximum());
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::OutputErrorMessage( const QString& message )
{
    QTextCursor cursor(this->ui.outputText->document());
    QTextBlockFormat block;
    block.setAlignment(Qt::AlignHCenter);
    block.setBackground(Qt::red);
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(block);
    QString html;
    html.sprintf("<font color=\"white\">%s</font>", message.toUtf8().constData());
    cursor.insertHtml(html);

    QScrollBar* sb = this->ui.outputText->verticalScrollBar();
    sb->setSliderPosition(sb->maximum());
}

//------------------------------------------------------------------------------
/**
*/
void 
BatchExporterApp::OutputStandardMessage( const QString& message )
{
    QTextCursor cursor(this->ui.outputText->document());
    QString html;
    html.sprintf("<font color=\"lightGray\">%s</font>", message.toUtf8().constData());
    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml(html);

    QScrollBar* sb = this->ui.outputText->verticalScrollBar();
    sb->setSliderPosition(sb->maximum());
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
BatchExporterApp::OutputExportStatus( const QString& message )
{
    QTextCursor cursor(this->ui.outputText->document());
    QTextBlockFormat block;
    block.setAlignment(Qt::AlignHCenter);
    block.setBackground(Qt::transparent);
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(block);
    QString html;
    html.sprintf("<font color=\"yellow\">%s</font>", message.toUtf8().constData());
    cursor.insertHtml(html);

    QScrollBar* sb = this->ui.outputText->verticalScrollBar();
    sb->setSliderPosition(sb->maximum());
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
		Util::String path = exeUri.LocalPath();
		path = "\"" + path + "\"";
		content.startDetached(path.AsCharPtr());
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
		Util::String path = exeUri.LocalPath();
		path = "\"" + path + "\"";
		content.startDetached(path.AsCharPtr());
	}
}
	

} // namespace BatchExporter
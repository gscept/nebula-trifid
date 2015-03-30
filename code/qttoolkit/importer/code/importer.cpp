#include "importerqt.h"
#include <QFileDialog>
#include <QPlastiqueStyle>
#include <QMessageBox>
#include "tools/pathresolver.h"
#include "tools/optionsmanager.h"
#include "tools/progressnotifier.h"
#include "importdatabase.h"

#if _DEBUG
#define EXPORTERPATH "bin:fbxbatcher3.debug.exe"
#define VIEWERPATH "bin:n3viewer.debug.exe"
#else
#define EXPORTERPATH "bin:fbxbatcher3.exe"
#define VIEWERPATH "bin:n3viewer.exe"
#endif

using namespace QtTools;

namespace Importer
{
//------------------------------------------------------------------------------
/**
*/
Importer::Importer()
{
	this->ui.setupUi(this);

	this->ui.tabWidget->setTabEnabled(2, false);
	this->ui.tabWidget->setTabEnabled(3, false);
	this->ui.tabWidget->setTabEnabled(4, false);
	this->ui.tabWidget->setTabEnabled(5, false);
	
	ProgressNotifier::Instance()->Open(ProgressNotifier::Receive);
	ProgressNotifier::Instance()->SetProgressBar(this->ui.loadProgress);
	ProgressNotifier::Instance()->SetStatusLabel(this->ui.status);

	ImportDatabase* database = ImportDatabase::Instance();
	
	this->reader = new FbxReader();
	this->reader->SetParent(this);

	this->clipController = new ClipController(&this->ui);
	this->clipController->SetReader(this->reader);

	this->modelController = new ModelController(&this->ui);

	this->ui.loadProgress->setStyle(new QPlastiqueStyle);
	this->ui.loadProgress->setVisible(false);
	this->ui.status->setVisible(false);

	this->SetControllersEnabled(false);

	connect(ui.staticImport, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(ui.skeletalImport, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(ui.merge, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(ui.removeRedudant, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(ui.addClip, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(ui.removeClip, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));

	connect(ui.actionOpen, SIGNAL(triggered(bool)), this, SLOT(Browse()));
	connect(ui.actionSave, SIGNAL(triggered(bool)), database, SLOT(SaveToDefault()));
	connect(ui.importButton, SIGNAL(clicked()), this, SLOT(Import()));

	connect(this->reader, SIGNAL(Loaded(int, const QString&, const QString&)), this, SLOT(ReaderLoaded(int, const QString&, const QString&)));
	connect(this->reader, SIGNAL(Loaded(int, const QString&, const QString&)), this->modelController, SLOT(ReaderLoaded(int, const QString&, const QString&)));
	connect(this->reader, SIGNAL(Loaded(int, const QString&, const QString&)), this->clipController, SLOT(ReaderLoaded(int, const QString&, const QString&)));
	connect(this->reader, SIGNAL(Loaded(int, const QString&, const QString&)), this, SLOT(UpdateInformation(int, const QString&, const QString&)));

	connect(ui.actionSave, SIGNAL(triggered(bool)), this, SLOT(SetChanged(bool)));
	connect(this->clipController, SIGNAL(Changed(bool)), this, SLOT(SetChanged(bool)));

	PathResolver::Instance()->SetupResources();
	OptionsManager::Instance()->SetApplication("nody");
	OptionsManager::Instance()->Open();
	PathResolver::Instance()->SetWorkingDirectory(OptionsManager::Instance()->GetSharedSetting(WorkingDirectory).toString());
	PathResolver::Instance()->SetToolkitDirectory(OptionsManager::Instance()->GetSharedSetting(ToolkitDirectory).toString());
	database->LoadFromDefault();
}

//------------------------------------------------------------------------------
/**
*/
Importer::~ImporterQt()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Importer::Open()
{
	/// load file if args are supplied
	if (this->args.HasArgument("-dir") && this->args.HasArgument("-file"))
	{
		QString dir = this->args.GetArgument("-dir");
		QString file = this->args.GetArgument("-file");
		this->reader->Open(file, dir);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
Importer::Browse()
{
	QFileDialog fileDialog(this, "Import FBX model", PathResolver::Instance()->ResolvePath("workgfx:"), "*.fbx");
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);

	if (fileDialog.exec() == QDialog::Accepted)
	{
		QString fullPath = fileDialog.selectedFiles()[0];
		QString file = fullPath.split("/").back().split(".").front();
		QString cat = *(fullPath.split("/").end()-2);

		this->ui.loadProgress->setVisible(true);
		this->ui.status->setVisible(true);
		if (this->reader->IsOpen())
		{
			this->reader->Close();
		}
		this->reader->Open(file, cat);
		this->ui.loadProgress->setVisible(false);
		this->ui.status->setVisible(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
Importer::SetChanged( bool state )
{
	this->setWindowModified(state);
}


//------------------------------------------------------------------------------
/**
*/
void 
Importer::NotifyChange()
{
	this->setWindowModified(true);
}


//------------------------------------------------------------------------------
/**
*/
void 
Importer::UpdateInformation(int status, const QString& file, const QString& category)
{
	if (status == FbxReader::Success)
	{
		QString fbxInfo = this->reader->GetInfo();
		this->ui.infoText->setHtml(fbxInfo);
		this->file = file;
		this->cat = category;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
Importer::SetCommandLineArgs( const CommandLineArgs& args )
{
	this->args = args;
}

//------------------------------------------------------------------------------
/**
*/
void 
Importer::Import()
{
	QString exporterPath = PathResolver::Instance()->ResolvePath(EXPORTERPATH);
	QStringList args;

	this->ui.actionSave->trigger();

	args.append("-dir");
	args.append(this->cat);
	args.append("-file");
	args.append(this->file + ".fbx");
	args.append("-force");
	args.append("true");

	this->exporter.start(exporterPath, args);
	bool started = this->exporter.waitForStarted(1000);
	if (!started)
	{
		QMessageBox::critical(this, "Failed to open exporter!", "The FBX exporter at: " + exporterPath + " failed to start!", QMessageBox::Ok);
	}
	else
	{
		ProgressNotifier::Instance()->Start("Exporting: " + this->file + ".fbx");
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
Importer::ExporterDone(int code, QProcess::ExitStatus status)
{
	if (status == QProcess::NormalExit)
	{
		int result = QMessageBox::question(this, "Exporter done!", "Would you like to preview?", QMessageBox::Yes, QMessageBox::No);
		ProgressNotifier::Instance()->End();
		if (result == QMessageBox::Yes)
		{
			QString viewerPath = PathResolver::Instance()->ResolvePath(VIEWERPATH);
			QStringList args;
			args.append("-view");
			args.append("mdl:" + this->cat + "/" + this->file + ".n3");
			viewer.start(viewerPath, args);
			bool started = viewer.waitForStarted(1000);
			if (!started)
			{
				QMessageBox::critical(this, "Failed to open viewer!", "The Nebula 3 viewer at: " + viewerPath + " failed to start!", QMessageBox::Ok);
			}
		}
	}
}


//------------------------------------------------------------------------------
/**
*/
void 
Importer::ViewerClosed()
{
	this->ui.importButton->setEnabled(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
Importer::ViewerOpened()
{
	this->ui.importButton->setEnabled(false);
}

//------------------------------------------------------------------------------
/**
*/
void 
Importer::ReaderLoaded( int status, const QString& file, const QString& category )
{
	if (status == FbxReader::Success)
	{
		this->setWindowTitle("Nebula 3 FBX Importer[*] - " + file);
		this->SetControllersEnabled(true);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
Importer::SetControllersEnabled( bool state )
{

	ui.importButton->setEnabled(state);
	ui.status->setEnabled(state);
}
}
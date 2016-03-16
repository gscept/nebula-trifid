//------------------------------------------------------------------------------
//  importerwindow.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelimporterwindow.h"
#include <QtGui/QFileDialog>
#include <QPlastiqueStyle>
#include <QtGui/QMessageBox>
#include <QtGui/QMessageBox>
#include "tools/progressnotifier.h"
#include "toolkitutil/modelutil/modeldatabase.h"
#include "progressreporter.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "io/stream.h"
#include "io/ioserver.h"
#include "qtaddons/remoteinterface/qtremoteclient.h"
#include "messaging/messagecallbackhandler.h"
#include "contentbrowserapp.h"

using namespace Graphics;
using namespace ContentBrowser;
using namespace ToolkitUtil;
using namespace Util;
using namespace IO;
namespace ModelImporter
{

//------------------------------------------------------------------------------
/**
	Global function called from KFbxImporter when loading progress is returned
*/
bool
LoadProgress(void* arguments, float progress, const char* status)
{
	ContentBrowser::ProgressReporter::ReportProgress(progress, status);
	return true;
}

//------------------------------------------------------------------------------
/**
*/
ModelImporterWindow::ModelImporterWindow( QWidget *parent /*= 0*/, Qt::WFlags flags /*= 0*/ )
{
	this->ui.setupUi(this);

	this->database = ModelDatabase::Create();
	this->database->Open();
	
	this->reader = new NFbxReader();
	this->reader->SetParent(this);

	this->fbxExporter = NFbxExporter::Create();
	this->fbxExporter->SetProgressCallback(ProgressReporter::ReportProgress);
	this->fbxExporter->SetMinMaxCallback(ProgressReporter::SetupProgress);
	this->fbxExporter->SetFbxProgressCallback(LoadProgress);
	this->fbxExporter->SetForce(true);
	this->fbxExporter->SetRemote(false);
	this->fbxExporter->SetForce(true);
	this->fbxExporter->Open();

	this->clipController = new ClipController(&this->ui);
	this->clipController->SetReader(this->reader);
	this->modelController = new ModelController(&this->ui);
	this->physicsController = new PhysicsController(&this->ui);

	this->ui.loadProgress->setVisible(false);
	this->ui.status->setVisible(false);
	this->resourceFolder = "assets";

	this->SetControllersEnabled(false);
	this->SetChanged(false);

	connect(this->ui.staticImport, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(this->ui.skeletalImport, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(this->ui.removeRedundant, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(this->ui.flipUVs, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(this->ui.vertexColors, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(this->ui.calcNormals, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));
	connect(this->ui.calcBinormAndTangents, SIGNAL(toggled(bool)), this, SLOT(NotifyChange()));

	connect(ui.importButton, SIGNAL(clicked()), this, SLOT(Import()));

	connect(this->reader, SIGNAL(Loaded(int, const IO::URI&)), this->modelController, SLOT(ReaderLoaded(int, const IO::URI&)));
	connect(this->reader, SIGNAL(Loaded(int, const IO::URI&)), this->clipController, SLOT(ReaderLoaded(int, const IO::URI&)));
	connect(this->reader, SIGNAL(Loaded(int, const IO::URI&)), this->physicsController, SLOT(ReaderLoaded(int, const IO::URI&)));
	connect(this->reader, SIGNAL(Loaded(int, const IO::URI&)), this, SLOT(UpdateInformation(int, const IO::URI&)));
	connect(this->reader, SIGNAL(Loaded(int, const IO::URI&)), this, SLOT(ReaderLoaded(int, const IO::URI&)));
	
	connect(this->clipController, SIGNAL(Changed(bool)), this, SLOT(SetChanged(bool)));

	// fix size
	this->setFixedSize(this->width(), this->height());

	// spawn import-to-nebula window
	this->dialogUi.setupUi(&this->dialog);
	this->dialog.setFixedSize(this->dialog.width(), this->dialog.height());

	// connect button slot
	connect(this->dialogUi.newCategory, SIGNAL(pressed()), this, SLOT(OnNewCategory()));
}

//------------------------------------------------------------------------------
/**
*/
ModelImporterWindow::~ModelImporterWindow()
{
	this->database->Close();
	this->database = 0;
	this->fbxExporter->Close();
	this->fbxExporter = 0;

    delete this->clipController;
    delete this->modelController;
    delete this->physicsController;

    this->currentOptions = 0;
    this->currentPhysics = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::Open()
{
	if (this->path.IsValid())
	{
		String category = this->path.AsString().ExtractLastDirName();
		String file = this->path.AsString().ExtractFileName();
		file.StripFileExtension();

		String workFile;
		workFile.Format("src:assets/%s/%s.fbx", category.AsCharPtr(), file.AsCharPtr());
		URI workPath(workFile);

		// check if file is in work
		if (workPath != this->path)
		{
			// setup combo box
			this->SetupCategories(this->path.GetHostAndLocalPath().ExtractLastDirName().AsCharPtr());

			String model = this->path.GetHostAndLocalPath().ExtractFileName();
			model.StripFileExtension();

			// setup name
			this->dialogUi.nameEdit->setText(model.AsCharPtr());

			// open dialog
			int result = this->dialog.exec();

			// if dialog was accepted
			if (result == QDialog::Accepted)
			{
				// reformat category, texture and entry
				category = this->dialogUi.categoryBox->currentText().toUtf8().constData();
				file = this->dialogUi.nameEdit->text().toUtf8().constData();

				// reformat work file
				workFile.Format("src:assets/%s/%s.fbx", 
					category.AsCharPtr(), 
					file.AsCharPtr());

				// now check if this file already exists
				if (IoServer::Instance()->FileExists(workFile))
				{
					// ask if one wants to overwrite the file
					QMessageBox box;
					box.setText("The file: " + QString(file.AsCharPtr()) + ".fbx already exists, do you want to overwrite it?");
					box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
					box.setDefaultButton(QMessageBox::Yes);
					result = box.exec();

					if (result == QMessageBox::Yes)
					{
						// copy file to nebula
						IoServer::Instance()->DeleteFile(workFile);
						IoServer::Instance()->CopyFile(this->path, workFile);
						this->path = workFile;
					}
					else
					{
						// stop importing
						this->close();
						return;
					}
				}
				else
				{
					// copy file to nebula
					IoServer::Instance()->CopyFile(this->path, workFile);
					this->path = workFile;
				}
			}
			else
			{
				// exit importation
				this->close();
				return; 
			}
		}

		// tell progress reporter to use our load progress
		ProgressReporter::Instance()->SetProgressBar(this->ui.loadProgress);
		ProgressReporter::Instance()->SetStatusLabel(this->ui.status);

		// reset title
		String title = "NebulaT FBX Importer[*]";
		this->setWindowTitle(title.AsCharPtr());
		this->SetChanged(false);
    
		this->currentOptions = database->LookupAttributes(category + "/" + file, true);
		this->currentPhysics = database->LookupPhysics(category + "/" + file, true);

		ProgressReporter::Instance()->BeginProgressReporting();
		this->reader->Open(this->path);
		ProgressReporter::Instance()->EndProgressReporting();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::Browse()
{
	QFileDialog fileDialog(this, "Import FBX model", IO::URI("src:assets").GetHostAndLocalPath().AsCharPtr(), "*.fbx");
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);

	if (fileDialog.exec() == QDialog::Accepted)
	{
		QString fullPath = fileDialog.selectedFiles()[0];
		this->path = IO::URI(fullPath.toLatin1().constData());

		this->ui.loadProgress->setVisible(true);
		this->ui.status->setVisible(true);
		if (this->reader->IsOpen())
		{
			this->reader->Close();
		}
		this->Open();
		this->ui.loadProgress->setVisible(false);
		this->ui.status->setVisible(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::SetChanged( bool state )
{
	this->setWindowModified(state);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::NotifyChange()
{
	this->setWindowModified(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::UpdateInformation( int status, const IO::URI& path )
{
	if (status == NFbxReader::Success)
	{
		String fbxInfo = this->reader->GetInfo();
		this->ui.infoText->setHtml(fbxInfo.AsCharPtr());
		this->path = path;
		this->physicsController->SetHasPhysics(reader->HasPhysics());		
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::Import()
{
	// tell progress reporter to use our load progress
	ProgressReporter::Instance()->SetProgressBar(this->ui.loadProgress);
	ProgressReporter::Instance()->SetStatusLabel(this->ui.status);

	this->SetChanged(false);

	// set options for exporter
	this->fbxExporter->SetForce(true);
	
	// start progress reporting, export then end progress reporting
	ProgressReporter::Instance()->BeginProgressReporting();

	// create string for fbx
	String fbx;
	fbx.Format("src:assets/%s.fbx", this->currentOptions->GetName().AsCharPtr());

	// export file
	this->fbxExporter->ExportFile(fbx);

	// stop progress reporting
	ProgressReporter::Instance()->EndProgressReporting();

	// create mesh name
	String mesh;
	mesh.Format("msh:%s.nvx2", this->currentOptions->GetName().AsCharPtr());

	// send mesh update
	Ptr<ReloadResourceIfExists> meshMsg = ReloadResourceIfExists::Create();
	meshMsg->SetResourceName(mesh);
	GraphicsInterface::Instance()->Send(meshMsg.upcast<Messaging::Message>());

	// send modification message to remote end
	QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Send(meshMsg.upcast<Messaging::Message>());

	// do the same for animation
	String anim;
	anim.Format("ani:%s.nax3", this->currentOptions->GetName().AsCharPtr());

	// send anim update
	Ptr<ReloadResourceIfExists> animMsg = ReloadResourceIfExists::Create();
	animMsg->SetResourceName(anim);
	GraphicsInterface::Instance()->Send(animMsg.upcast<Messaging::Message>());

	// send modification message to remote end
	QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Send(animMsg.upcast<Messaging::Message>());

	// lastly do it for the model
	String model;
	model.Format("mdl:%s.n3", this->currentOptions->GetName().AsCharPtr());

	// send model update, wait for it to finish
	Ptr<ReloadResourceIfExists> modelMsg = ReloadResourceIfExists::Create();
	modelMsg->SetResourceName(model);
	GraphicsInterface::Instance()->Send(modelMsg.upcast<Messaging::Message>());

	Ptr<ReloadModelByResource> rmMsg = ReloadModelByResource::Create();
	rmMsg->SetResourceName(model);
	GraphicsInterface::Instance()->Send(rmMsg.upcast<Messaging::Message>());
	QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Send(rmMsg.upcast<Messaging::Message>());

	// now copy to intermediate if it exists
	String tempModel;
	tempModel.Format("int:models/%s_temp.n3", this->currentOptions->GetName().AsCharPtr());
	if (IoServer::Instance()->FileExists(tempModel))
	{
		// copy to temp model and reload
		IoServer::Instance()->CopyFile(model, tempModel);

		Ptr<ReloadResourceIfExists> tempModelMsg = ReloadResourceIfExists::Create();
		tempModelMsg->SetResourceName(tempModel);
		GraphicsInterface::Instance()->Send(tempModelMsg.upcast<Messaging::Message>());	

		// since this is our last message, we should wait for this one  to be done before we update
		__SingleFireCallback(ModelImporterWindow, OnModelUpdateDone, this, tempModelMsg.upcast<Messaging::Message>());

		// invalidate the previewer model by attaching it again, if we have a character and this is the model we are modifying, this should resolve any invalid characters
		ContentBrowserApp::Instance()->GetPreviewState()->SetModel(model);
	}
	else
	{
		// wait for model to reload, since we send them in order, they will be handled in the same order
		__SingleFireCallback(ModelImporterWindow, OnModelUpdateDone, this, modelMsg.upcast<Messaging::Message>());
	}

	// send modification message to remote end
	QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Send(modelMsg.upcast<Messaging::Message>());

	// close window
	this->close();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::ViewerClosed()
{
	this->ui.importButton->setEnabled(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::ViewerOpened()
{
	this->ui.importButton->setEnabled(false);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::ReaderLoaded( int status, const IO::URI& path )
{
	if (status == NFbxReader::Success)
	{
		String file = path.GetHostAndLocalPath().ExtractFileName();
		file.StripFileExtension();
		String title;
		title.Format("NebulaT FBX Importer[*] - %s", file.AsCharPtr());
		this->setWindowTitle(title.AsCharPtr());
		this->SetControllersEnabled(true);

		// if we don't have any joints, disable the skeletal import option and set the export mode to be static
		if (this->reader->HasJoints())
		{
			this->ui.skeletalImport->setEnabled(true);
			this->ui.skeletalImport->setChecked(true);	
			this->currentOptions->SetExportMode(ToolkitUtil::Skeletal);
		}
		else
		{
			this->ui.skeletalImport->setEnabled(false);			
            this->ui.staticImport->setChecked(true);
            this->currentOptions->SetExportMode(ToolkitUtil::Static);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::SetControllersEnabled( bool state )
{
	ui.importButton->setEnabled(state);
	ui.status->setEnabled(state);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::SetUri( const IO::URI& path )
{
	this->path = path;
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelImporterWindow::OnModelUpdateDone( const Ptr<Messaging::Message>& msg )
{
	// emit that importing is done
	emit ImportDone(this->currentOptions->GetName());

	// reload clip controller
	this->clipController->Load();
}
}
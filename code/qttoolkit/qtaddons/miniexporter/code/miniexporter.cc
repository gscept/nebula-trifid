//------------------------------------------------------------------------------
//  miniexporter.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "miniexporter.h"
#include "fbx/nfbxexporter.h"
#include "applauncher.h"
#include "platform.h"
#include "base/exporterbase.h"
#include "ui_miniexporterdialog.h"
#include "modelutil/modeldatabase.h"


using namespace ToolkitUtil;
namespace MiniExporterAddon
{
__ImplementClass(MiniExporterAddon::MiniExporter, 'MIEX', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
MiniExporter::MiniExporter() :
	force(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
MiniExporter::~MiniExporter()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
MiniExporter::Run()
{
	// open dialog	
	this->ui = new Ui::MiniExporterDialog();
	this->ui->setupUi(&this->dialog);
	this->dialog.setWindowFlags(Qt::FramelessWindowHint);
	this->dialog.setModal(true);
	this->dialog.setWindowModality(Qt::ApplicationModal);
	this->dialog.show();
	QApplication::processEvents();

	// create model database
	Ptr<ModelDatabase> database = ModelDatabase::Create();

	this->ui->statusProgress->setValue(0);
	this->ui->statusLabel->setText("Exporting models...");
	QApplication::processEvents();

	// export models
	Ptr<ToolkitUtil::NFbxExporter> fbxExporter = ToolkitUtil::NFbxExporter::Create();
	fbxExporter->Open();
	fbxExporter->SetPlatform(Platform::Win32);
	fbxExporter->SetExportFlag(Base::ExporterBase::Dir);
	fbxExporter->ExportDir("system");
	fbxExporter->Close();

	this->ui->statusProgress->setValue(33);
	this->ui->statusLabel->setText("Exporting textures...");
	QApplication::processEvents();

	// export textures
	AppLauncher launcher;

	// setup arguments
	Util::String arguments;
	arguments.Format("-dir %s", "system");
	if (this->force)
	{
		arguments.Append(" -force");
	}
	launcher.SetArguments(arguments);
	launcher.SetWorkingDirectory("src:");
	launcher.SetNoConsoleWindow(true);
	
	// export textures
#if _DEBUG
	launcher.SetExecutable("bin:texturebatcher3.debug.exe");
#else
	launcher.SetExecutable("bin:texturebatcher3.exe");
#endif
	launcher.LaunchWait();

	this->ui->statusProgress->setValue(66);
	this->ui->statusLabel->setText("Exporting shaders...");
	QApplication::processEvents();

	// export shaders
#if _DEBUG 
	launcher.SetExecutable("bin:shaderbatcher.debug.exe");
#else
	launcher.SetExecutable("bin:shaderbatcher.exe");
#endif
	launcher.LaunchWait();	

	this->ui->statusProgress->setValue(100);
	QApplication::processEvents();	

	// close dialog
	this->dialog.close();

	delete this->ui;
}

} // namespace MiniExporterAddon
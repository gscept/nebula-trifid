//------------------------------------------------------------------------------
//  fbxbatcher3app.cc
//  (C) 2011-2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "gamebatcher3app.h"
#include "io/assignregistry.h"
#include "core/coreserver.h"
#include "fbx/helpers/animsplitterhelper.h"
#include "fbx/helpers/batchattributes.h"
#include "game/gameexporter.h"


using namespace IO;
using namespace Util;
using namespace ToolkitUtil;
using namespace Base;

namespace Toolkit
{
//------------------------------------------------------------------------------
/**
*/
GameBatcher3App::GameBatcher3App()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GameBatcher3App::~GameBatcher3App()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
GameBatcher3App::Open()
{
	bool retval = DistributedToolkitApp::Open();
	return retval;
}

//------------------------------------------------------------------------------
/**
*/
void 
GameBatcher3App::DoWork()
{
	Ptr<GameExporter> exporter = GameExporter::Create();
	exporter->SetLogger(&this->logger);
	String dir = "";
	String file = "";
	String projectFolder = "proj:";
	ExporterBase::ExportFlag exportFlag = ExporterBase::All;
	bool force = false;
	if (this->args.HasArg("-dir"))
	{
		exportFlag = ExporterBase::Dir;
		dir = this->args.GetString("-dir");
	}
	if (this->args.HasArg("-file"))
	{
		exportFlag = ExporterBase::File;
		file = this->args.GetString("-file");
	}
	if (this->args.HasArg("-projectdir"))
	{
		projectFolder = this->args.GetString("-projectdir") + "/";
	}	 
	if (this->args.HasArg("-force"))
	{
		force = this->args.GetBool("-force");
	}
	IO::AssignRegistry::Instance()->SetAssign(Assign("home","proj:"));
	exporter->Open();
	exporter->SetForce(force);
	exporter->SetExportFlag(exportFlag);
	exporter->SetPlatform(this->platform);
	switch (exportFlag)
	{
	case ExporterBase::All:
		exporter->ExportAll();
		break;
	case ExporterBase::Dir:
		exporter->ExportDir(dir);
		break;
	case ExporterBase::File:
		exporter->SetCategory(dir);
		exporter->SetFile(file);
		exporter->ExportFile(projectFolder + "work/gfxlib/" + dir + "/" + file);
		break;	
	}
	exporter->Close();

	// if we have any errors, set the return code to be errornous
	if (exporter->HasErrors()) this->SetReturnCode(-1);
}

//------------------------------------------------------------------------------
/**
*/
bool 
GameBatcher3App::ParseCmdLineArgs()
{
	return DistributedToolkitApp::ParseCmdLineArgs();
}

//------------------------------------------------------------------------------
/**
*/
bool 
GameBatcher3App::SetupProjectInfo()
{
	if (DistributedToolkitApp::SetupProjectInfo())
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
GameBatcher3App::ShowHelp()
{
	n_printf("Nebula Trifid Game Batcher.\n"
		"(C) 2012-2015 Individual contributors, see AUTHORS file.\n");
	//n_printf(this->GetArgumentDescriptionString().AsCharPtr());
	n_printf("-help         --display this help\n"
			 "-force        --ignores time stamps\n"
			 "-dir          --category name\n"
			 "-file         --file name (if empty, dir will be parsed)\n"
			 "-projectdir   --nebula project trunk (if empty, attempts to use registry)\n"
			 "-platform     --export platform");
	//DistributedToolkitApp::ShowHelp();
}



} // namespace Toolkit


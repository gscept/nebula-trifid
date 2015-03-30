//------------------------------------------------------------------------------
//  fbxbatcher3app.cc
//  (C) 2011-2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "guibatcher3app.h"
#include "io/assignregistry.h"
#include "core/coreserver.h"
#include "gui/guiexporter.h"


using namespace IO;
using namespace Util;
using namespace ToolkitUtil;
using namespace Base;

namespace Toolkit
{
//------------------------------------------------------------------------------
/**
*/
GuiBatcher3App::GuiBatcher3App()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GuiBatcher3App::~GuiBatcher3App()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
GuiBatcher3App::Open()
{
	bool retval = DistributedToolkitApp::Open();
	return retval;
}

//------------------------------------------------------------------------------
/**
*/
void 
GuiBatcher3App::DoWork()
{

	Ptr<CEGuiExporter> exporter = CEGuiExporter::Create();
	String projectFolder = "proj:";
	ExporterBase::ExportFlag exportFlag = ExporterBase::All;
	bool force = false;
	if (this->args.HasArg("-projectdir"))
	{
		projectFolder = this->args.GetString("-projectdir") + "/";
	}	 
	if (this->args.HasArg("-force"))
	{
		force = this->args.GetBool("-force");
		this->textureConverter.SetForceFlag(force);
	}
	IO::AssignRegistry::Instance()->SetAssign(Assign("home","proj:"));	

	exporter->Open();
	exporter->SetForce(force);
	exporter->SetExportFlag(exportFlag);
	exporter->SetPlatform(this->platform);
	exporter->ExportAll(this->textureConverter, this->logger);
	exporter->Close();

	// if we have any errors, set the return code to be negative
	if (exporter->HasErrors()) this->SetReturnCode(-1);
}

//------------------------------------------------------------------------------
/**
*/
bool 
GuiBatcher3App::ParseCmdLineArgs()
{
	return DistributedToolkitApp::ParseCmdLineArgs();
}

//------------------------------------------------------------------------------
/**
*/
bool 
GuiBatcher3App::SetupProjectInfo()
{
	if (DistributedToolkitApp::SetupProjectInfo())
	{
		this->textureConverter.SetPlatform(this->platform);
		this->textureConverter.SetToolPath(this->projectInfo.GetPathAttr("TextureTool"));
		this->textureConverter.SetTexAttrTablePath(this->projectInfo.GetAttr("TextureAttrTable"));
		//this->textureConverter.SetSrcDir(this->projectInfo.GetAttr("TextureSrcDir"));
		//this->textureConverter.SetDstDir(this->projectInfo.GetAttr("TextureDstDir"));

		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
GuiBatcher3App::ShowHelp()
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


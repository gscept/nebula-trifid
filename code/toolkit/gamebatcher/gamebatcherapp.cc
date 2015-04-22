//------------------------------------------------------------------------------
//  gamebatcherapp.cc
//  (C) 2011-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "gamebatcherapp.h"
#include "io/assignregistry.h"
#include "core/coreserver.h"
#include "fbx/helpers/animsplitterhelper.h"
#include "fbx/helpers/batchattributes.h"
#include "game/gameexporter.h"
#include "sqlite3/sqlite3factory.h"
#include "game/leveldbwriter.h"


using namespace IO;
using namespace Util;
using namespace ToolkitUtil;
using namespace Base;

namespace Toolkit
{
//------------------------------------------------------------------------------
/**
*/
GameBatcherApp::GameBatcherApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GameBatcherApp::~GameBatcherApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
GameBatcherApp::Open()
{
	bool retval = DistributedToolkitApp::Open();
	return retval;
}

//------------------------------------------------------------------------------
/**
*/
void 
GameBatcherApp::DoWork()
{
;
    String projectFolder = "proj:";
 	
	if (this->args.HasArg("-projectdir"))
	{
		projectFolder = this->args.GetString("-projectdir") + "/";
        IO::AssignRegistry::Instance()->SetAssign(Assign("proj", projectFolder));
	}	 
	
	IO::AssignRegistry::Instance()->SetAssign(Assign("home","proj:"));
	
	Ptr<EditorBlueprintManager> bm = EditorBlueprintManager::Create();
    bm->SetLogger(&this->logger);
    Ptr<Db::DbFactory> sqlite3Factory = Db::Sqlite3Factory::Create();
           
    bm->ParseProjectInfo("proj:projectinfo.xml");
    bm->ParseProjectInfo("toolkit:projectinfo.xml");
            
    bm->ParseBlueprint("proj:data/tables/blueprints.xml");    
    bm->ParseBlueprint("toolkit:data/tables/blueprints.xml");
    // templates need to be parsed from tookit first to add virtual templates with their attributes
    bm->ParseTemplates("toolkit:data/tables/db");
    bm->ParseTemplates("proj:data/tables/db");
    

    bm->UpdateAttributeProperties();
    bm->CreateMissingTemplates();
    bm->CreateDatabases("proj:foo/");

    Ptr<Db::Database> gamedb = Db::DbFactory::Instance()->CreateDatabase();
    gamedb->SetURI("proj:foo/game.db4");
    gamedb->SetAccessMode(Db::Database::ReadWriteExisting);
    gamedb->Open();
    Ptr<Db::Database> staticdb = Db::DbFactory::Instance()->CreateDatabase();
    staticdb->SetURI("proj:foo/static.db4");
    staticdb->SetAccessMode(Db::Database::ReadWriteExisting);
    staticdb->Open();
   
    Ptr<ToolkitUtil::LevelDbWriter> dbwriter = ToolkitUtil::LevelDbWriter::Create();
    dbwriter->Open(gamedb,staticdb);
    String levelDir = "proj:work/levels";
    Array<String> files = IoServer::Instance()->ListFiles(IO::URI(levelDir), "*.xml", true);
    for (int fileIndex = 0; fileIndex < files.Size(); fileIndex++)
    {        
        Ptr<IO::Stream> levelStream = IoServer::Instance()->CreateStream(files[fileIndex]);
        Ptr<XmlReader> xmlReader = XmlReader::Create();
        levelStream->Open();
        xmlReader->SetStream(levelStream);
        xmlReader->Open();
        dbwriter->LoadXmlLevel(xmlReader);
        xmlReader->Close();
        levelStream->Close();        
    }
    dbwriter->Close();    
    gamedb->Close();
    staticdb->Close();
	// if we have any errors, set the return code to be errornous
	//if (exporter->HasErrors()) this->SetReturnCode(-1);
}

//------------------------------------------------------------------------------
/**
*/
bool 
GameBatcherApp::ParseCmdLineArgs()
{
	return DistributedToolkitApp::ParseCmdLineArgs();
}

//------------------------------------------------------------------------------
/**
*/
bool 
GameBatcherApp::SetupProjectInfo()
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
GameBatcherApp::ShowHelp()
{
	n_printf("Nebula Trifid Game Batcher.\n"
		"(C) 2012-2015 Individual contributors, see AUTHORS file.\n");	
	n_printf("-help         --display this help\n"			 
			 "-dir          --category name\n"
			 "-file         --file name (if empty, dir will be parsed)\n"
			 "-projectdir   --nebula project trunk (if empty, attempts to use registry)\n"
			 "-platform     --export platform");	
}



} // namespace Toolkit


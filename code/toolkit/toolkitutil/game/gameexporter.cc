//------------------------------------------------------------------------------
//  gameexporter.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "game/gameexporter.h"
#include "io/ioserver.h"
#include "db/sqlite3/sqlite3factory.h"
#include "db/writer.h"
#include "attr/attributedefinitionbase.h"
#include "scriptfeature/scriptattr/scriptattributes.h"
#include "leveldbwriter.h"

using namespace IO;
using namespace Db;
using namespace Attr;
using namespace Util;

namespace ToolkitUtil
{
__ImplementClass(ToolkitUtil::GameExporter, 'GAEX', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
GameExporter::GameExporter()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GameExporter::~GameExporter()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
GameExporter::Open()
{
	ExporterBase::Open();

}

//------------------------------------------------------------------------------
/**
*/
void 
GameExporter::Close()
{
	ExporterBase::Close();
}

//------------------------------------------------------------------------------
/**
	Exports both templates and levels with a clean database (removes and creates backup of previous)
*/
void 
GameExporter::ExportAll()
{

    String projectFolder = "proj:";
    

    IO::AssignRegistry::Instance()->SetAssign(Assign("home","proj:"));

    Ptr<Db::DbFactory> sqlite3Factory;
    if(Db::DbFactory::HasInstance())
    {
        sqlite3Factory = Db::Sqlite3Factory::Instance();
    }
    else
    {
        sqlite3Factory = Db::Sqlite3Factory::Create();
    }
	
	if (!IoServer::Instance()->DirectoryExists("export:data/tables"))
	{
		IoServer::Instance()->CreateDirectory("export:data/tables");
	}

    Ptr<Toolkit::EditorBlueprintManager> bm;
    if(Toolkit::EditorBlueprintManager::HasInstance())
    {
        bm = Toolkit::EditorBlueprintManager::Instance();
    }
    else
    {
        bm = Toolkit::EditorBlueprintManager::Create();
        bm->SetLogger(this->logger);

        bm->ParseProjectInfo("proj:projectinfo.xml");
        bm->ParseProjectInfo("toolkit:projectinfo.xml");

        bm->ParseBlueprint("proj:data/tables/blueprints.xml");    
        bm->ParseBlueprint("toolkit:data/tables/blueprints.xml");
        // templates need to be parsed from tookit first to add virtual templates with their attributes
        bm->ParseTemplates("toolkit:data/tables/db");
        bm->ParseTemplates("proj:data/tables/db");


        bm->UpdateAttributeProperties();
        bm->CreateMissingTemplates();
    }
    
   
    bm->CreateDatabases("export:/db/");
	bm->SaveBlueprint("export:data/tables/blueprints.xml");

    Ptr<Db::Database> gamedb = Db::DbFactory::Instance()->CreateDatabase();
    gamedb->SetURI("export:db/game.db4");
    gamedb->SetAccessMode(Db::Database::ReadWriteExisting);
    gamedb->Open();
    Ptr<Db::Database> staticdb = Db::DbFactory::Instance()->CreateDatabase();
    staticdb->SetURI("export:db/static.db4");
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
    bm = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
GameExporter::ExportTables()
{
    String projectFolder = "proj:";


    IO::AssignRegistry::Instance()->SetAssign(Assign("home","proj:"));

    Ptr<Db::DbFactory> sqlite3Factory;
    if(Db::DbFactory::HasInstance())
    {
        sqlite3Factory = Db::Sqlite3Factory::Instance();
    }
    else
    {
        sqlite3Factory = Db::Sqlite3Factory::Create();
    }


    Ptr<Toolkit::EditorBlueprintManager> bm;
    if(Toolkit::EditorBlueprintManager::HasInstance())
    {
        bm = Toolkit::EditorBlueprintManager::Instance();
    }
    else
    {
        bm = Toolkit::EditorBlueprintManager::Create();
        bm->SetLogger(this->logger);

        bm->ParseProjectInfo("proj:projectinfo.xml");
        bm->ParseProjectInfo("toolkit:projectinfo.xml");

        bm->ParseBlueprint("proj:data/tables/blueprints.xml");    
        bm->ParseBlueprint("toolkit:data/tables/blueprints.xml");
        // templates need to be parsed from tookit first to add virtual templates with their attributes
        bm->ParseTemplates("toolkit:data/tables/db");
        bm->ParseTemplates("proj:data/tables/db");


        bm->UpdateAttributeProperties();
        bm->CreateMissingTemplates();
    }
    bm->CreateDatabases("export:/db/");
    bm = 0;
}

} // namespace ToolkitUtil
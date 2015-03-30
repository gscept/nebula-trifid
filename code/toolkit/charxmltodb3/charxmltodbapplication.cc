//------------------------------------------------------------------------------
//  charxmltodbapplication.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "charxmltodbapplication.h"
#include "system/nebulasettings.h"
#include "timing/time.h"
#include "tinyxml/tinyxml.h"
#include "io/ioserver.h"
#include "io/filestream.h"
#include "attributes.h"

using namespace System;
using namespace Util;
using namespace IO;
using namespace Db;

namespace Toolkit
{
//------------------------------------------------------------------------------
/**
*   Constructor
*/
CharXmlToDbApplication::CharXmlToDbApplication() :
    waitForKey(false)
{
    loopTypes.Clear();
    loopTypes.Append("Clamp");
    loopTypes.Append("Repeat");

    sampleRates.Clear();
    sampleRates.Append("every frame");
    sampleRates.Append("every 2nd frame");
    sampleRates.Append("every 4th frame");
    sampleRates.Append("every 8th frame");
}

//------------------------------------------------------------------------------
/**
*   Destructor
*/
CharXmlToDbApplication::~CharXmlToDbApplication()
{
    loopTypes.Clear();
    sampleRates.Clear();
}

//------------------------------------------------------------------------------
/**
*   Create a table on given database if it does not exist.
*/
Ptr<Table> CharXmlToDbApplication::CreateTable(const Ptr<Database> & db, const String & tableName)
{
    Ptr<Table> characterTable = 0;
    if(true == db->HasTable(tableName))
    {
        characterTable = db->GetTableByName(tableName);
    }
    else
    {
        characterTable = DbFactory::Instance()->CreateTable();
        characterTable->SetName(tableName);
    }
    return characterTable;
    
}

//------------------------------------------------------------------------------
/**
*   Create column on given table if it does not exist.
*/
void CharXmlToDbApplication::CreateColumn(const Ptr<Db::Table> & table, Column::Type t, Attr::AttrId attr)
{
    if(false == table->HasColumn(attr))
    {
        Column column;
        column.SetType(t);
        column.SetAttrId(attr);
        table->AddColumn(column);
    }
}

//------------------------------------------------------------------------------
/**
*   Setup the database for the animations. Creates necessary tables and columns.
*/
void CharXmlToDbApplication::SetupTables(const Ptr<Db::Database> & db)
{
    // Clear old tables
    this->ClearDatabase(db);
    
    Ptr<Dataset> dataset;
    Ptr<Table> table;

    // Create animation table
    table = this->CreateTable(db,"_CharacterAnims");
    
    this->CreateColumn(table,Db::Column::Primary,Attr::ID);
    this->CreateColumn(table,Db::Column::Indexed,Attr::Character);
    this->CreateColumn(table,Db::Column::Indexed,Attr::Animation);
    this->CreateColumn(table,Db::Column::Default,Attr::SampleRate);
    this->CreateColumn(table,Db::Column::Default,Attr::LoopType);
    this->CreateColumn(table,Db::Column::Default,Attr::FadenInFrames);

    if(false == db->HasTable(table->GetName()))
    {
        db->AddTable(table);
    }
    dataset = table->CreateDataset();
    dataset->AddAllTableColumns();
    dataset->CommitChanges();

    // Create hotspot table
    table = this->CreateTable(db,"_CharacterHotSpots");
    
    this->CreateColumn(table,Db::Column::Primary,Attr::ID);
    this->CreateColumn(table,Db::Column::Indexed,Attr::AnimationID);
    this->CreateColumn(table,Db::Column::Indexed,Attr::HotSpot);
    this->CreateColumn(table,Db::Column::Default,Attr::HotSpotPos);

    if(false == db->HasTable(table->GetName()))
    {
        db->AddTable(table);
    }
    dataset = table->CreateDataset();
    dataset->AddAllTableColumns();
    dataset->CommitChanges();
}

//------------------------------------------------------------------------------
/**
*   Deletes all CharEditor relevant tables:
*   charcters and hotspots.
*/
void CharXmlToDbApplication::ClearDatabase(const Ptr<Db::Database> & db)
{
    if (db->HasTable("_CharacterAnims"))
    {
        db->DeleteTable("_CharacterAnims");
    }
    if (db->HasTable("_CharacterHotSpots"))
    {
        db->DeleteTable("_CharacterHotSpots");
    }
}

//------------------------------------------------------------------------------
/**
*   Loads a xml file from the given path and connects to given database path.
*   If connection could be etablished, this analyses the xml and writes
*   the parsed data to the database.
*/
bool CharXmlToDbApplication::ParseXml(URI xmlpath, URI dbpath)
{
    n_printf("Open xml file...\n");
    // setup stream
    Ptr<Stream> stream = FileStream::Create();
    stream->SetURI(xmlpath);
    if(false == stream->Open())
    {
        n_error("Could not open xml file.\n");
        return false;
    }
    
    // setup reader
    Ptr<XmlReader> reader = XmlReader::Create();
    reader->SetStream(stream);
    if(false == reader->Open())
    {
        n_error("Could not open xml file.\n");
        return false;
    }

    // setup database
    n_printf("Open database...\n");
    this->dbFactory = Sqlite3Factory::Create();
    Ptr<Database> db = DbFactory::Instance()->CreateDatabase();
    db->SetURI(dbpath);
    db->SetAccessMode(Database::ReadWriteCreate);
    db->SetIgnoreUnknownColumns(true);
    bool dbOpened = db->Open();
    if(false == dbOpened)
    {
        n_error("Could not open database.\n");
        return false;
    }

    // add new tables if needed
    this->SetupTables(db);
    // create datasets
    this->CreateDatasets(db);

    // browse to Animation node
    reader->SetToNode("/Animations");

    // Search for first xml element with the name "Characater".
    // Parses all animation elements until no one is left.
    if(true == reader->SetToFirstChild("Character"))
    {
        do
        {
            if(false == this->ParseAnimation(reader,db))
            {
                n_printf("Found invalid element.\n");
            }    
        }
        while(true == reader->SetToNextChild("Character"));
    }
    else
    {
        // If no animation element was found abort the process...
        n_printf("Xml does not contain any animation.\n");
        this->DisposeDatasets();
        reader->Close();
        stream->Close();
        db->Close();
        return false;
    }

    n_printf("Commiting animation values to database...\n");
    this->animation_dataset->CommitChanges();
    n_printf("Commiting hotspot values to database...\n");
    this->hotspot_dataset->CommitChanges();

    // cleanup
    this->DisposeDatasets();

    reader->Close();
    stream->Close();
    db->Close();

    this->dbFactory = 0;

    return true;
}

//------------------------------------------------------------------------------
/**
*   Creates datasets from tables and fills a value table with all data of the tables.
*/
void CharXmlToDbApplication::CreateDatasets(const Ptr<Database> & db)
{
    Ptr <Table> table;

    // get animation table from db
    table = db->GetTableByName("_CharacterAnims");
    this->animation_dataset = table->CreateDataset();
    this->animation_dataset->AddAllTableColumns();
    this->animation_dataset->PerformQuery();
    this->animation_values = this->animation_dataset->Values();

    // get hotspot table from db
    table = db->GetTableByName("_CharacterHotSpots");
    this->hotspot_dataset = table->CreateDataset();
    this->hotspot_dataset->AddAllTableColumns();
    this->hotspot_dataset->PerformQuery();
    this->hotspot_values = this->hotspot_dataset->Values();
}

//------------------------------------------------------------------------------
/**
*   Sets all datasets and value tables to 0
*/
void CharXmlToDbApplication::DisposeDatasets()
{
    this->animation_dataset = 0;
    this->hotspot_dataset = 0;

    this->animation_values = 0;
    this->hotspot_values = 0;
}

//------------------------------------------------------------------------------
/**
*   Parses current node of the given xmlreader
*   and adds it to the given database.
*/
bool CharXmlToDbApplication::ParseAnimation(const Ptr<XmlReader> & reader,const Ptr<Database> & db)
{
    if(
        true == reader->HasAttr("pattern") &&
        true == reader->HasAttr("fadeInFrames") &&
        true == reader->HasAttr("loopType") &&
        true == reader->HasAttr("sampleRate"))
    {
        n_printf("Parse animation %s\n",reader->GetString("pattern"));

        // get pattern and name from string
        Util::String pattern = reader->GetString("pattern");
        pattern.StripFileExtension();
        Util::Array<Util::String> strArray = pattern.Tokenize("/");

        // get new rowindex
        IndexT animation_index = this->animation_values->AddRow();
        this->animation_values->SetInt      (Attr::ID,              animation_index, animation_index);
        this->animation_values->SetString   (Attr::Character,       animation_index, strArray[0]);
        this->animation_values->SetString   (Attr::Animation,       animation_index, strArray[1]);
        this->animation_values->SetInt      (Attr::SampleRate,      animation_index, sampleRates.FindIndex(reader->GetString("sampleRate")));
        this->animation_values->SetInt      (Attr::LoopType,        animation_index, loopTypes.FindIndex(reader->GetString("loopType")));
        this->animation_values->SetFloat    (Attr::FadenInFrames,   animation_index, reader->GetFloat("fadeInFrames"));

        // add hotspots if they exist
        if(true == reader->SetToFirstChild("Hotspot"))
        {             
            IndexT hotspot_index;
            do
            {
                if(
                    true == reader->HasAttr("name") &&
                    true == reader->HasAttr("position"))
                {
                    // get new rowindex
                    hotspot_index = hotspot_values->AddRow();
                    this->hotspot_values->SetInt        (Attr::ID,          hotspot_index, hotspot_index);
                    this->hotspot_values->SetInt        (Attr::AnimationID, hotspot_index, animation_index);
                    this->hotspot_values->SetString     (Attr::HotSpot,     hotspot_index, reader->GetString("name"));
                    this->hotspot_values->SetFloat      (Attr::HotSpotPos,  hotspot_index, reader->GetFloat("position"));
                }
            }
            while(reader->SetToNextChild("Hotspot")); // go to next hotspot if existing
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*   gets the directory of working project (read from registry)
*/
IO::URI CharXmlToDbApplication::GetProjDir(void)
{
    String projDirectory = "home:";
    if(System::NebulaSettings::Exists("gscept.ToolkitShared","workdir"))
    {    
        projDirectory = System::NebulaSettings::ReadString("gscept.ToolkitShared","workdir");
    }    
    else
    {
        projDirectory = "home:";
    }    
    return projDirectory;
}

//------------------------------------------------------------------------------
/**
*   Parses the given commandline args and check for a valid format.
*/
bool CharXmlToDbApplication::ParseCommandLineArgs()
{
    // show help
    if(true == this->args.GetBoolFlag("-help"))
    {
        this->waitForKey = true;
        return false;
    }
    // set wait for key
    this->waitForKey = this->args.GetBoolFlag("-waitforkey");

    // parse source path
    if(this->args.HasArg("-source"))
    {
        this->xmlUri = URI(args.GetString("-source"));
    }
    else
    {
        this->xmlUri = GetProjDir();
        this->xmlUri.AppendLocalPath(String("\\work\\gfxlib\\characters\\batchattributes.xml"));
    }
    // parse database path
    if(this->args.HasArg("-database"))
    {
        this->dbUri = URI(args.GetString("-database"));
    }
    else
    {
        this->dbUri = GetProjDir();
        this->dbUri.AppendLocalPath(String("\\export\\db\\characters.db4"));
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*   Prints a little help text how to use this program.
*/
void CharXmlToDbApplication::ShowHelp()
{
    
    n_printf("CharEditor XML to DB tool\n\
             Arguments:\n\
             [-source]\t\tPath to a XML file created with the N2 Character Tool.\n\t\tIf not specified, the projects default path will be used.\n\
             [-database]\t\tPath to a SQLite database.\n\t\tIf not specified, the projects default path will be used.\n\
             [-waitforkey]\t(optional) Waits for user input before exit\n\
             [-help]\t\t(optional) Shows this Help\n\
             Example:\n\
             charxmltodb.exe -source C:/test.xml -database C:/database.db4 -waitforkey\n");
    
}

//------------------------------------------------------------------------------
/**
*   Opens application.
*/
bool CharXmlToDbApplication::Open()
{
    return ConsoleApplication::Open();
}

//------------------------------------------------------------------------------
/**
*   Runs the application and parses commandline args.
*/
void CharXmlToDbApplication::Run()
{
    if(true == this->ParseCommandLineArgs())
    {

        this->ParseXml(this->xmlUri,this->dbUri);

    }
    else
    {
        this->ShowHelp();
    }
    // wait for user input
    if (this->waitForKey)
    {
        n_printf("Press <Enter> to continue!\n");
        while (!IO::Console::Instance()->HasInput())
        {
            Timing::Sleep(0.01);
        }
    }
}

//------------------------------------------------------------------------------
/**
*   Closes program
*/
void CharXmlToDbApplication::Close()
{
    
    ConsoleApplication::Close();
}

} // namespace Toolkit
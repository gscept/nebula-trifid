#pragma once

//------------------------------------------------------------------------------
/**
    @class Tools::CharXmlToDbApplication
    
    CharXmlToDb Console application.

    (C) 2009 Radon Labs GmbH
    (C) 2013 Individual contributors, see AUTHORS file
*/
#include "app/consoleapplication.h"
#include "io/xmlreader.h"
#include "io/uri.h"
#include "addons/db/database.h"
#include "addons/db/sqlite3/sqlite3factory.h"
#include "addons/db/dataset.h"

namespace Toolkit
{
    class CharXmlToDbApplication : public App::ConsoleApplication
    {
    
    public:
        /// constructor
        CharXmlToDbApplication();
        /// destructor
        virtual ~CharXmlToDbApplication();
        /// open the application
        virtual bool Open();
        /// run the application
        virtual void Run();
        /// close the application
        virtual void Close();
    private:
        /// create tables on the given database
        void SetupTables(const Ptr<Db::Database> & db);

        /// create table if it does not exist on the given database
        Ptr<Db::Table> CreateTable(const Ptr<Db::Database> & db, const Util::String & tableName);
        /// create column on given table if it does not exist
        void CreateColumn(const Ptr<Db::Table> & table, Db::Column::Type t, Attr::AttrId attr);
        /// clears the database, drops all tables
        void ClearDatabase(const Ptr<Db::Database> & db);
        /// parses the given xml file for animations
        bool ParseXml(IO::URI xmlpath, IO::URI dbpath);
        /// gets project path from nebula toolkit
        IO::URI GetProjDir(void);
        /// parses an animation from the reader to the database
        bool ParseAnimation(const Ptr<IO::XmlReader> & reader, const Ptr<Db::Database> & db);
        /// creates datasets from db tables
        void CreateDatasets(const Ptr<Db::Database> & db);
        /// disposes datasets
        void DisposeDatasets();
        /// shows a simple help
        void ShowHelp();
        /// set up the program from the command line args
        bool ParseCommandLineArgs();
        /// if true, the program waits for user input before closing.
        bool waitForKey;
        /// path to the xml file that should parsed
        IO::URI xmlUri;
        /// path to the database file where the data should commited to
        IO::URI dbUri;

        // db factory
        Ptr<Db::DbFactory> dbFactory;

        // datasets
        Ptr<Db::Dataset> animation_dataset;
        Ptr<Db::Dataset> hotspot_dataset;

        // valueTables
        Ptr<Db::ValueTable> animation_values;
        Ptr<Db::ValueTable> hotspot_values;

        // lookup tables for xml-string to db-int
        Util::Array<Util::String> loopTypes;
        Util::Array<Util::String> sampleRates;
    };
}

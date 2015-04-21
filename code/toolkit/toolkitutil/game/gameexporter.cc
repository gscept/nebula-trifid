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

using namespace IO;
using namespace Db;
using namespace Attr;

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

	this->levelExporter = LevelExporter::Create();
	this->levelExporter->SetLogger(this->logger);
	this->templateExporter = TemplateExporter::Create();
	this->templateExporter->SetLogger(this->logger);
	this->postEffectExporter = PostEffectExporter::Create();
	this->postEffectExporter->SetLogger(this->logger);
	
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

	IoServer* ioServer = IoServer::Instance();

	if (ioServer->FileExists("db:static.db4") && ioServer->FileExists("db:game.db4"))
	{
		ioServer->DeleteFile("db:static.bak.db4");
		ioServer->DeleteFile("db:game.bak.db4");
		ioServer->CopyFile("db:static.db4", "db:static.bak.db4");
		ioServer->CopyFile("db:game.db4", "db:game.bak.db4");
		ioServer->DeleteFile("db:static.db4");
		ioServer->DeleteFile("db:game.db4");
	}
	if(!ioServer->DirectoryExists("export:data/tables"))
	{
		ioServer->CreateDirectory("export:data/tables");
	}

	Util::Array<Util::String> xmlfiles = ioServer->ListFiles("proj:data/tables/","*.xml",false);
	for(Util::Array<Util::String>::Iterator iter = xmlfiles.Begin();iter!=xmlfiles.End();iter++)
	{
		ioServer->CopyFile("proj:data/tables/"+*iter,"data:tables/"+*iter);

	}

	Ptr<Db::DbFactory> sqlite3Factory;

	if (!Db::DbFactory::HasInstance())
	{
		sqlite3Factory = Db::Sqlite3Factory::Create();
	}
	
	this->templateExporter->SetDbFactory(Db::DbFactory::Instance());
	this->levelExporter->SetDbFactory(Db::DbFactory::Instance());

    this->blueprintManager = Toolkit::EditorBlueprintManager::Create();
    this->blueprintManager->SetLogger(this->logger);
    this->blueprintManager->Open();		

	n_printf("---- Exporting Templates ----\n");
	this->templateExporter->Open();
	this->templateExporter->ExportAll();
	if (this->templateExporter->HasErrors())
	{
		this->SetHasErrors(true);
	}	
    this->templateExporter->Close();

	n_printf("---- Exporting Levels ----\n");
	this->levelExporter->Open();
	this->levelExporter->ExportAll();
	if (this->levelExporter->HasErrors())
	{
		this->SetHasErrors(true);
	}
	this->levelExporter->Close();
	
	n_printf("---- Exporting post effect presets ----\n");
	this->postEffectExporter->Open();
	this->postEffectExporter->ExportAll();
	if (this->postEffectExporter->HasErrors())
	{
		this->SetHasErrors(true);
	}
	this->postEffectExporter->Close();

	n_printf("---- Exporting global attributes ----\n");
	this->ExportGlobals();

	n_printf("---- batching scriptfeature tables ----\n");
	this->ExportScriptFeature();
    
    this->blueprintManager->SaveBlueprint("data:blueprint.xml");
    this->blueprintManager = 0;
	sqlite3Factory = 0;
}


//------------------------------------------------------------------------------
/**
	Exports global attributes found in data/globals.xml
*/
void 
GameExporter::ExportGlobals()
{

	IoServer* ioServer = IoServer::Instance();

	if(ioServer->FileExists("data:tables/globals.xml"))
	{
		n_printf("Found globals.xml, exporting...\n");
		Ptr<Db::Database> gameDb = Db::DbFactory::Instance()->CreateDatabase();
		gameDb->SetURI(URI("db:game.db4"));
		gameDb->SetAccessMode(Db::Database::ReadWriteCreate);
		gameDb->SetIgnoreUnknownColumns(true);
		if(gameDb->Open())
		{						
			Ptr<Stream> stream = IoServer::Instance()->CreateStream("data:tables/globals.xml");
			Ptr<XmlReader> xmlReader = XmlReader::Create();
			xmlReader->SetStream(stream);
			if(xmlReader->Open())
			{
				if(gameDb->HasTable("_Globals"))
				{
					gameDb->DeleteTable("_Globals");
				}
				// initialize a database writer
				Ptr<Db::Writer> dbWriter = Db::Writer::Create();
				dbWriter->SetDatabase(gameDb);
				dbWriter->SetTableName("_Globals");

				if(xmlReader->SetToFirstChild("Attribute"))
				{
					Util::Array<Attr::Attribute> attrs;
					do
					{
						Util::String name = xmlReader->GetString("name");
						Attr::ValueType vtype = Attr::Attribute::StringToValueType(xmlReader->GetString("type"));
						Util::String val = xmlReader->GetString("value");
						if(!Attr::AttributeDefinitionBase::FindByName(name))
						{
							Attr::AttributeDefinitionBase::RegisterDynamicAttribute(name, Util::FourCC(), vtype, Attr::ReadWrite);
						}						
						Attr::Attribute atr;
						atr.SetAttrId(Attr::AttrId(name));						
						atr.SetValueFromString(val);
						dbWriter->AddColumn(Db::Column(atr.GetAttrId()));
						attrs.Append(atr);
					}
					while(xmlReader->SetToNextChild("Attribute"));

					dbWriter->Open();
					dbWriter->BeginRow();

					for(IndexT i = 0 ; i < attrs.Size() ; i++)
					{
						const Attr::Attribute& value = attrs[i];
						const Attr::AttrId& attrId = value.GetAttrId();

						switch (value.GetValueType())
						{
						case Attr::IntType:
							dbWriter->SetInt(attrId, value.GetInt());
							break;
						case Attr::FloatType:
							dbWriter->SetFloat(attrId, value.GetFloat());
							break;
						case Attr::BoolType:
							dbWriter->SetBool(attrId, value.GetBool());
							break;
						case Attr::Float4Type:
							dbWriter->SetFloat4(attrId, value.GetFloat4());
							break;
						case Attr::StringType:
							dbWriter->SetString(attrId, value.GetString());
							break;
						case Attr::Matrix44Type:
							dbWriter->SetMatrix44(attrId, value.GetMatrix44());
							break;
						case Attr::GuidType:
							dbWriter->SetGuid(attrId, value.GetGuid());
							break;
						case Attr::BlobType:
							dbWriter->SetBlob(attrId, value.GetBlob());
							break;
						default:
							break;
						}							
					}
					dbWriter->EndRow();
					dbWriter->Close();
				}
				xmlReader->Close();				
			}
			gameDb->Close();

		}
		


	}
	else
	{
		n_printf("No globals.xml");
	}
	n_printf("---- Done exporting global attributes ----\n");

}

//------------------------------------------------------------------------------
/**
*/
void
GameExporter::ExportScriptFeature()
{

	Ptr<Database> staticDb = DbFactory::Instance()->CreateDatabase();
	staticDb->SetURI(URI("db:static.db4"));
	staticDb->SetAccessMode(Database::ReadWriteCreate);
	staticDb->SetIgnoreUnknownColumns(true);

	if (staticDb->Open())
	{
		Ptr<Table> table = TemplateExporter::CreateTable(staticDb, "_Scripts_ConditionScripts");
		TemplateExporter::CreateColumn(table, Column::Primary, AttrId("ScriptName"));				
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("ConditionBlock"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("ConditionRef"));		
		staticDb->AddTable(table);
		table->CommitChanges();		
		
		table = TemplateExporter::CreateTable(staticDb, "_Scripts_Conditions");		
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("Id"));
		TemplateExporter::CreateColumn(table, Column::Primary, AttrId("ConditionGUID"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("ConditionContent"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("ConditionType"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("ConditionBlockGUID"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("ConditionStatementGUID"));
		staticDb->AddTable(table);
		table->CommitChanges();

		table = TemplateExporter::CreateTable(staticDb, "_Scripts_Statements");
		TemplateExporter::CreateColumn(table, Column::Primary, AttrId("StatementGUID"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StatementContent"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StatementType"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StatementBlock"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StatementBlockGUID"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StatementRef"));
		staticDb->AddTable(table);
		table->CommitChanges();

		table = TemplateExporter::CreateTable(staticDb, "_Scripts_ActionScripts");
		TemplateExporter::CreateColumn(table, Column::Primary, AttrId("ScriptName"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StatementRef"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StatementBlock"));
		staticDb->AddTable(table);
		table->CommitChanges();
				
		table = TemplateExporter::CreateTable(staticDb, "_Script_StateMachines");
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("Id"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StartState"));
		staticDb->AddTable(table);
		table->CommitChanges();

		table = TemplateExporter::CreateTable(staticDb, "_Script_StateMachineStates");
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("Id"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("MachineName"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StateName"));		
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("OnEntryStatementRef"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("OnEntryStatementBlock"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("OnFrameStatementRef"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("OnFrameStatementBlock"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("OnExitStatementRef"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("OnExitStatementBlock"));
		staticDb->AddTable(table);
		table->CommitChanges(); 

		table = TemplateExporter::CreateTable(staticDb, "_Script_StateTransitions");
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("Id"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("MachineName"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StateName"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("ToState"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("ConditionRef"));		
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("ConditionBlock"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StatementRef"));
		TemplateExporter::CreateColumn(table, Column::Default, AttrId("StatementBlock"));	
		staticDb->AddTable(table);
		table->CommitChanges();

		staticDb->Close();
	}
}

} // namespace ToolkitUtil
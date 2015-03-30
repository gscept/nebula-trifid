//------------------------------------------------------------------------------
//  globalattrsmanager.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "managers/globalattrsmanager.h"
#include "addons/db/dbserver.h"
#include "addons/db/reader.h"
#include "addons/db/writer.h"

namespace BaseGameFeature
{
__ImplementClass(GlobalAttrsManager, 'MGAT', Game::Manager);
__ImplementSingleton(GlobalAttrsManager);

//------------------------------------------------------------------------------
/**
*/
GlobalAttrsManager::GlobalAttrsManager()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
GlobalAttrsManager::~GlobalAttrsManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
GlobalAttrsManager::OnActivate()
{
    Manager::OnActivate();
    this->attrs.Clear();

    // check if world database is open
    if (Db::DbServer::Instance()->IsGameDatabaseOpen())
    {
        this->LoadAttributes();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GlobalAttrsManager::LoadAttributes()
{
    this->attrs.Clear();
    const Ptr<Db::Database>& db = Db::DbServer::Instance()->GetGameDatabase();
    if (db->HasTable("_Globals"))
    {
        Ptr<Db::Reader> dbReader = Db::Reader::Create();
        dbReader->SetDatabase(db);
        dbReader->SetTableName("_Globals");
        if (dbReader->Open())
        {
            Db::ValueTable* values = dbReader->GetValueTable();
            n_assert(1 == values->GetNumRows());
            IndexT colIndex;
            SizeT numColumns = values->GetNumColumns();
            for (colIndex = 0; colIndex < numColumns; colIndex++)
            {
                const Attr::AttrId& attrId = values->GetColumnId(colIndex);
                switch (attrId.GetValueType())
                {
                    case Attr::IntType:
                        this->SetInt(attrId, values->GetInt(attrId, 0));
                        break;
                    case Attr::FloatType:
                        this->SetFloat(attrId, values->GetFloat(attrId, 0));
                        break;
                    case Attr::BoolType:
                        this->SetBool(attrId, values->GetBool(attrId, 0));
                        break;
                    case Attr::Float4Type:
                        this->SetFloat4(attrId, values->GetFloat4(attrId, 0));
                        break;
                    case Attr::StringType:
                        this->SetString(attrId, values->GetString(attrId, 0));
                        break;
                    case Attr::Matrix44Type:
                        this->SetMatrix44(attrId, values->GetMatrix44(attrId, 0));
                        break;
                    case Attr::GuidType:
                        this->SetGuid(attrId, values->GetGuid(attrId, 0));
                        break;
                    case Attr::BlobType:
                        this->SetBlob(attrId, values->GetBlob(attrId, 0));
                        break;
                    case Attr::VoidType:
                        n_error("GlobalAttrsManager::LoadAttributes() void type!");
                        break;
                }
            }
            dbReader->Close();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GlobalAttrsManager::SaveAttributes()
{
    // delete table if exists
    const Ptr<Db::Database>& db = Db::DbServer::Instance()->GetGameDatabase();
    if (db->HasTable("_Globals"))
    {
        db->DeleteTable("_Globals");
    }

    // initialize a database writer
    Ptr<Db::Writer> dbWriter = Db::Writer::Create();
    dbWriter->SetDatabase(db);
    dbWriter->SetTableName("_Globals");

    // add columns to writer...
    IndexT i;
    SizeT num = this->attrs.GetAttrs().Size();
    if (num > 0)
    {
        for (i = 0; i < num; i++)
        {
            dbWriter->AddColumn(Db::Column(this->attrs.GetAttrs().KeyAtIndex(i)));
        }

        // set values in writer, note: there is only one row in the table!
        bool success = dbWriter->Open();
        dbWriter->BeginRow();
        n_assert(success);
        for (i = 0; i < num; i++)
        {
            const Attr::Attribute& value = this->attrs.GetAttrs().ValueAtIndex(i);
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
                case Attr::VoidType:
                    n_error("GlobalAttrsManager::SaveAttributes() void type!");
                    break;
            }
        }
        dbWriter->EndRow();
        dbWriter->Close();
    }
}

}
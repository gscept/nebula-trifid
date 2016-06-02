//------------------------------------------------------------------------------
//  attrcontainerdbstorage.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attr/io/attrcontainerdbstorage.h"
#include "db/dbserver.h"
#include "db/reader.h"
#include "db/writer.h"

namespace Attr
{
__ImplementClass(AttrContainerDBStorage, 'ACDB', Attr::AttrContainerStorageBase);    

//------------------------------------------------------------------------------
/**
*/
AttrContainerDBStorage::AttrContainerDBStorage()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AttrContainerDBStorage::~AttrContainerDBStorage()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
void
AttrContainerDBStorage::Load()
{
    n_assert(this->db.isvalid() && this->db->IsOpen());
    this->attrs.Clear();
    
    if (db->HasTable(this->table))
    {
        Ptr<Db::Reader> dbReader = Db::Reader::Create();
        dbReader->SetDatabase(this->db);
        dbReader->SetTableName(this->table);
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
				case Attr::Transform44Type:
					this->SetTransform44(attrId, values->GetTransform44(attrId, 0));
					break;
                case Attr::GuidType:
                    this->SetGuid(attrId, values->GetGuid(attrId, 0));
                    break;
                case Attr::BlobType:
                    this->SetBlob(attrId, values->GetBlob(attrId, 0));
                    break;
                case Attr::VoidType:
                    n_error("AttrContainerDBStorage::Load() void type!");
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
AttrContainerDBStorage::Save()
{
    n_assert(this->db.isvalid() && this->db->IsOpen());    

    if (this->db->HasTable(this->table))
    {
        this->db->DeleteTable(this->table);
    }

    // initialize a database writer
    Ptr<Db::Writer> dbWriter = Db::Writer::Create();
    dbWriter->SetDatabase(this->db);
    dbWriter->SetTableName(this->table);

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
			case Attr::Transform44Type:
				dbWriter->SetTransform44(attrId, value.GetTransform44());
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
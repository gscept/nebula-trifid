//------------------------------------------------------------------------------
//  scriptfeature/scripttable.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/scripttable.h"
#include "db/reader.h"
#include "db/dbserver.h"

//#include "kernel/nprofiler.h"

namespace Script
{
__ImplementClass(Script::ScriptTable, 'SCTA', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ScriptTable::ScriptTable() :
    isOpen(false),
	blockAttrId(Attr::AttrId()) // THIS IS DUMMY STYLE
{
    this->reader = Db::Reader::Create();
}

//------------------------------------------------------------------------------
/**
*/
ScriptTable::~ScriptTable()
{
    if (this->isOpen)
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/    
const Util::Array<int>& 
ScriptTable::GetBlockIndices(const Util::Guid& guid) const
{
    n_assert(this->blockIndexMap.Contains(guid));
    return this->blockIndexArrays[this->blockIndexMap[guid]];
}

//------------------------------------------------------------------------------
/**
*/      
void 
ScriptTable::SetGuidAttrId(Attr::GuidAttrId id)
{
    this->blockAttrId = id;
}

//------------------------------------------------------------------------------
/**
*/      
void 
ScriptTable::SetTableName(const Util::String& tableName)
{
    n_assert(!this->reader->IsOpen());
    this->reader->SetTableName(tableName);
}

//------------------------------------------------------------------------------
/**
*/   
const Ptr<Db::Reader>&
ScriptTable::GetReader()
{
    return this->reader;
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptTable::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->reader->IsOpen());   
    this->reader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
    this->reader->Open();  // db access
    n_assert(this->reader->IsOpen());    
    n_assert(this->blockAttrId.IsValid());
    n_assert(this->reader->HasAttr(this->blockAttrId));

    // build a sorted array of GUID/rowIndex key-value-pairs
    // so that identical GUIDs are next to each other in the array
    IndexT rowIndex;
    SizeT numRows = this->reader->GetNumRows();
	Util::Array<Util::KeyValuePair<Util::Guid,int> > sortedGuidArray(numRows, 0);
    if (numRows > 0)
    {
        // build a sorted key/value pair array of GUIDs with their row indices
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            this->reader->SetToRow(rowIndex);
            const Util::Guid& guid = this->reader->GetGuid(this->blockAttrId);
			sortedGuidArray.Append(Util::KeyValuePair<Util::Guid,int>(guid, rowIndex));
        }
        sortedGuidArray.Sort();

        // identical Guids now next to each other in array
        IndexT i = 0;
        while (i < sortedGuidArray.Size())
        {
            Util::Array<IndexT> newArray;
            this->blockIndexArrays.Append(newArray);
            const Util::Guid& curGuid = sortedGuidArray[i].Key();

            // gather all identical GUIDs
            while ((i < sortedGuidArray.Size()) && (sortedGuidArray[i] == curGuid))
            {
                this->blockIndexArrays.Back().Append(sortedGuidArray[i].Value());
                i++;
            }       
            // need to restore original table order!
            this->blockIndexArrays.Back().Sort();
            this->blockIndexMap.Add(curGuid, this->blockIndexArrays.Size() - 1);
        }
    }
    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptTable::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
    if (this->reader->IsOpen())
    {
        this->reader->Close();
    }
    this->blockIndexArrays.Clear();
    this->blockIndexMap.Clear();
}

} // namespace Script
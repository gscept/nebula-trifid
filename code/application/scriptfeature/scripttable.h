#pragma once
//------------------------------------------------------------------------------
/**
    @class Managers::ScriptTable

    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "util/guid.h"
#include "attr/guidattrid.h"

//------------------------------------------------------------------------------
namespace Db
{
    class Reader;
};

//------------------------------------------------------------------------------
namespace Script
{
class ScriptTable : public Core::RefCounted
{
    __DeclareClass(ScriptTable);
public:
    /// constructor
    ScriptTable();
    /// destructor
    virtual ~ScriptTable();
    /// set table name
    void SetTableName(const Util::String& tableName);
    /// set GuidAttrId
    void SetGuidAttrId(Attr::GuidAttrId id);
    /// open reader and init blocks
    void Open();
    /// cleanup (close reader etc)
    void Close();
    /// get reader
    const Ptr<Db::Reader>& GetReader();
    /// return true if the script table has data for a given guid
    bool ContainsGuid(const Util::Guid& guid);
    /// get array of indices for a block 
    const Util::Array<IndexT>& GetBlockIndices(const Util::Guid& guid) const;

private:
    bool isOpen;
    Ptr<Db::Reader> reader;
    Attr::GuidAttrId blockAttrId;
    Util::Array<Util::Array<IndexT> > blockIndexArrays;    // array of arrays of indices into Db2::Reader
    Util::Dictionary<Util::Guid, IndexT> blockIndexMap;   // map GUIDs to block index arrays
};

//------------------------------------------------------------------------------
/**
*/
inline bool
ScriptTable::ContainsGuid(const Util::Guid& guid)
{
    return this->blockIndexMap.Contains(guid);
}

} // namespace Managers
//------------------------------------------------------------------------------

#pragma once
//------------------------------------------------------------------------------
/**
    @class Attr::AttrContainerDBStorage
  
    Provides read/write access to an attribute container using a DB
        
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "attr/io/attrcontainerstoragebase.h"
#include "db/database.h"

//------------------------------------------------------------------------------
namespace Attr
{
class AttrContainerDBStorage : public Attr::AttrContainerStorageBase
{
    __DeclareClass(AttrContainerDBStorage);

public:
    /// constructor
    AttrContainerDBStorage();
    /// destructor
    virtual ~AttrContainerDBStorage();

    /// set db to use
    void SetDB(const Ptr<Db::Database>& db, const Util::String & table);
    /// save attributes to stream
    virtual void Save();
    /// load attributes from stream
    virtual void Load();
private:
    Ptr<Db::Database> db;    
};


/**
*/
inline void
AttrContainerDBStorage::SetDB(const Ptr<Db::Database>& istream, const Util::String & itable)
{
    this->db = istream;
    this->table = itable;
}

}
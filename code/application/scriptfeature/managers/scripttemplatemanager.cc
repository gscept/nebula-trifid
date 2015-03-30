//------------------------------------------------------------------------------
//  scripttemplatemanager.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/managers/scripttemplatemanager.h"
#include "db/reader.h"
#include "db/dbserver.h"
#include "scriptfeature/scriptattr/scriptattributes.h"
#include "scriptfeature/scripttable.h"

namespace Script
{
__ImplementClass(Script::ScriptTemplateManager, 'SCTM', Game::Manager);
__ImplementSingleton(Script::ScriptTemplateManager);

//------------------------------------------------------------------------------
/**
*/
ScriptTemplateManager::ScriptTemplateManager()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ScriptTemplateManager::~ScriptTemplateManager()
{
    n_assert(!this->IsActive());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptTemplateManager::OnActivate()
{
    this->RegisterTables();
    Manager::OnActivate();
}

//------------------------------------------------------------------------------
/**
    This unloads everything.
*/
void
ScriptTemplateManager::OnDeactivate()
{
    this->CleanupReaders();
    Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    This unloads everything.
*/
void
ScriptTemplateManager::CleanupReaders()
{
    int i;
    int numReader = this->reader.Size();
    for(i = 0; i < numReader; i++)
    {
        this->reader.ValueAtIndex(i)->Close();
    }
    this->reader.Clear();
}

//------------------------------------------------------------------------------
/**
    This unloads everything.
*/
void
ScriptTemplateManager::ReloadScriptTemplates()
{
    this->CleanupReaders();
    this->RegisterTables();
}

//------------------------------------------------------------------------------
/**
*/    
const Util::Array<IndexT>& 
ScriptTemplateManager::GetBlockIndices(const Util::String& tableName, const Util::Guid& guid) const
{
    n_assert(this->reader.Contains(tableName));
    n_assert(this->reader[tableName]->ContainsGuid(guid));
    return this->reader[tableName]->GetBlockIndices(guid);
}

//------------------------------------------------------------------------------
/**
*/    
bool
ScriptTemplateManager::HasBlockIndices(const Util::String& tableName, const Util::Guid& guid) const
{
    n_assert(this->reader.Contains(tableName));
    return this->reader[tableName]->ContainsGuid(guid);
}

//------------------------------------------------------------------------------
/**
*/   
Ptr<Db::Reader>
ScriptTemplateManager::GetReader(const Util::String& tableName)
{
    if (this->reader.Contains(tableName))
    {
        return this->reader[tableName]->GetReader();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/    
bool 
ScriptTemplateManager::HasTable(const Util::String& tableName)
{
    return this->reader.Contains(tableName);
}

//------------------------------------------------------------------------------
/**
    override by Subclass
*/
void 
ScriptTemplateManager::RegisterTables()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    register table on activation of our manager or override RegisterTables
*/
void 
ScriptTemplateManager::RegisterTable(const Util::String& tableName, Attr::GuidAttrId id)
{
    /// add table if reader list doesn't contain it
    if (!this->reader.Contains(tableName))
    {
        Ptr<Db::Database> db = Db::DbServer::Instance()->GetStaticDatabase();
        n_assert(db->HasTable(tableName));

        // setup a new script table
        Ptr<Script::ScriptTable> newTable = Script::ScriptTable::Create();
        newTable->SetTableName(tableName);
        newTable->SetGuidAttrId(id);
        newTable->Open();  

        // attach table
        this->reader.Add(tableName, newTable);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptTemplateManager::ReloadReader(const Util::String& tableName)
{
    n_assert(this->reader.Contains(tableName));
    this->reader[tableName]->Close();
    this->reader[tableName]->Open();  
}

} // namespace Script
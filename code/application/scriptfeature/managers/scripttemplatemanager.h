#pragma once
//------------------------------------------------------------------------------
/**
    @class Managers::ScriptTemplateManager
  
    Wraps entity categories and provides access to category template and
    instance tables.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "game/manager.h"
#include "core/singleton.h"
#include "util/guid.h"
#include "util/stringatom.h"

//------------------------------------------------------------------------------
namespace Attr
{
    class GuidAttrId;
};
namespace Db
{
    class Reader;
};
namespace Script
{
    class ScriptTable;
};

//------------------------------------------------------------------------------
namespace Script
{
class ScriptTemplateManager : public Game::Manager
{
    __DeclareClass(ScriptTemplateManager);
    __DeclareSingleton(ScriptTemplateManager);
public:
    /// constructor
    ScriptTemplateManager();
    /// destructor
    virtual ~ScriptTemplateManager();

    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();

    /// check if a guid exist in table
    virtual bool HasBlockIndices(const Util::String& tableName, const Util::Guid& guid) const;
    /// get array of indices for a block
    virtual const Util::Array<IndexT>& GetBlockIndices(const Util::String& tableName, const Util::Guid& guid) const;
    /// get reader for table xy, if table not exist return 0
    virtual Ptr<Db::Reader> GetReader(const Util::String& tableName);
    /// check if table exist
    virtual bool HasTable(const Util::String& tableName);
    /// register a table 
    virtual void RegisterTable(const Util::String& tableName, Attr::GuidAttrId id);
    /// reload table from db (slow)
    virtual void ReloadReader(const Util::String& tableName);
    /// reload script templates
    void ReloadScriptTemplates();

protected:
    /// (override) register all tables, to be loaded in your project
    virtual void RegisterTables();   
    /// cleanup script template readers
    void CleanupReaders();

    Util::Dictionary<Util::StringAtom, Ptr<Script::ScriptTable> > reader;
};

} // namespace Script
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  factory.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "addons/db/dbfactory.h"
#include "addons/db/database.h"
#include "addons/db/command.h"
#include "addons/db/table.h"
#include "addons/db/dataset.h"
#include "addons/db/reader.h"
#include "addons/db/writer.h"
#include "addons/db/relation.h"
#include "addons/db/valuetable.h"

namespace Db
{
__ImplementClass(Db::DbFactory, 'DBFC', Core::RefCounted);
__ImplementSingleton(Db::DbFactory);

//------------------------------------------------------------------------------
/**
*/
DbFactory::DbFactory()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
DbFactory::~DbFactory()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Database>
DbFactory::CreateDatabase() const
{
    n_error("Db::DbFactory::CreateDatabase() called!");
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Command>
DbFactory::CreateCommand() const
{
    n_error("Db::DbFactory::CreateCommand() called!");
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Table>
DbFactory::CreateTable() const
{
    n_error("Db::DbFactory::CreateTable() called!");
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Dataset>
DbFactory::CreateDataset() const
{
    n_error("Db::DbFactory::CreateDataset() called!");
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<FilterSet>
DbFactory::CreateFilterSet() const
{
    n_error("Db::DbFactory::CreateFilterSet() called!");
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Relation>
DbFactory::CreateRelation() const
{
    return Relation::Create();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ValueTable>
DbFactory::CreateValueTable() const
{
    return ValueTable::Create();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Reader>
DbFactory::CreateReader() const
{
    return Reader::Create();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Writer>
DbFactory::CreateWriter() const
{
    return Writer::Create();
}

} // namespace Db

//------------------------------------------------------------------------------
//  categorymanager.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "managers/categorymanager.h"
#include "addons/db/dbserver.h"
#include "addons/db/reader.h"
#include "addons/db/dbfactory.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "basegamefeature/managers/factorymanager.h"

namespace Attr
{
    DefineString(CategoryName, 'CANA', ReadWrite);
    DefineBool(IsVirtualCategory, 'ISVC', ReadWrite);
    DefineBool(IsSpecialCategory, 'ISSC', ReadWrite);
    DefineString(CategoryTemplateTable, 'CATT', ReadWrite);
    DefineString(CategoryInstanceTable, 'CAIT', ReadWrite);
};

namespace BaseGameFeature
{
__ImplementClass(CategoryManager, 'MCAT', Game::Manager);
__ImplementSingleton(CategoryManager);

using namespace Db;
using namespace Attr;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
CategoryManager::CategoryManager() :
    inBeginAddCategoryAttrs(false),
    addAttrCategoryIndex(InvalidIndex),
	addAttrCategorySync(NULL)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
CategoryManager::~CategoryManager()
{
    n_assert(!this->IsActive());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    NOTE: The category manager must be re-activated when the world database 
    is opened so that it may read the categories from the world database.
*/
void
CategoryManager::OnActivate()
{
    Manager::OnActivate();

    // load categories from database
    this->LoadCategories();

    // load template tables from database
    this->LoadTemplates();

    // create a dummy category
    this->dummyInstTable = ValueTable::Create();
}

//------------------------------------------------------------------------------
/**
    This unloads everything.
*/
void
CategoryManager::OnDeactivate()
{
    this->categoryArray.Clear();
    this->catIndexMap.Clear();
    this->attrCategoryMap.Clear();
    Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    This commits changes to the instance data back into the database.
    Call this method right before creating a save game.
*/
void
CategoryManager::CommitChangesToDatabase()
{
    IndexT catIndex;
    for (catIndex = 0; catIndex < this->categoryArray.Size(); catIndex++)
    {
        const Category& category = this->categoryArray[catIndex];
        
        // commit changes to instances
        if (category.HasInstanceDataset())
        {
            category.GetInstanceDataset()->CommitChanges();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Populates the internal categories array by loading the _Categories
    table from the database.
*/
void
CategoryManager::LoadCategories()
{
    n_assert(this->categoryArray.IsEmpty());
    Ptr<Reader> dbReader = DbFactory::Instance()->CreateReader();
    dbReader->SetDatabase(DbServer::Instance()->GetStaticDatabase());
    dbReader->SetTableName("_Categories");
    if (dbReader->Open())
    {
        IndexT i;
        SizeT num = dbReader->GetNumRows();
        for (i = 0; i < num; i++)
        {
            dbReader->SetToRow(i);
            Category cat;
            cat.name           = dbReader->GetString(Attr::CategoryName);
            cat.isVirtual      = dbReader->GetBool(Attr::IsVirtualCategory);
            cat.isSpecial      = dbReader->GetBool(Attr::IsSpecialCategory);
            cat.templTableName = dbReader->GetString(Attr::CategoryTemplateTable);
            cat.instTableName  = dbReader->GetString(Attr::CategoryInstanceTable);
            this->categoryArray.Append(cat);
            this->catIndexMap.Add(cat.name, this->categoryArray.Size() - 1);
        }
        dbReader->Close();
    }
    else
    {
        n_error("CategoryManager::LoadCategories(): error opening _Category table!");
    }
}

//------------------------------------------------------------------------------
/**
    For each category that has a template table, create a dataset and load
    it into memory. Also, setup empty instance tables.
*/
void
CategoryManager::LoadTemplates()
{
    const Ptr<Db::Database>& staticDatabase = DbServer::Instance()->GetStaticDatabase();

    // for each category...
    IndexT catIndex;
    for (catIndex = 0; catIndex < this->categoryArray.Size(); catIndex++)
    {
        Category& cat = this->categoryArray[catIndex];
        if (cat.GetTemplateTableName().IsValid())
        {
			n_assert(staticDatabase->HasTable(cat.GetTemplateTableName()));
            Table* dbTable = staticDatabase->GetTableByName(cat.GetTemplateTableName());
            cat.templDataset = dbTable->CreateDataset();
            cat.templDataset->AddAllTableColumns();
            cat.templDataset->PerformQuery();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Begin adding category attributes.
*/
void
CategoryManager::BeginAddCategoryAttrs(const String& categoryName)
{
    n_assert(!this->inBeginAddCategoryAttrs);
    n_assert(categoryName.IsValid());
    n_assert(this->HasCategory(categoryName));
    this->inBeginAddCategoryAttrs = true;
    this->addAttrCategoryIndex = this->catIndexMap[categoryName];

    const Category& cat = this->GetCategoryByName(categoryName);
    if (cat.HasInstanceDataset())
    {
        ValueTable* values = cat.GetInstanceDataset()->Values();
        values->BeginAddColumns();
    }
	if (!this->syncedAttrs.Contains(categoryName))
	{
		Util::Array<Attr::AttrId> arr;
		this->syncedAttrs.Add(categoryName, arr);
		this->addAttrCategorySync = &(this->syncedAttrs[categoryName]);
	}
	else
	{
		this->addAttrCategorySync = NULL;
	}
	
}

//------------------------------------------------------------------------------
/**
    This adds an attribute to a category. Will extend the template and
    instance table of the category by the new attribute and update
    the attr/category mapping table.
*/
void
CategoryManager::AddCategoryAttr(const Attr::AttrId& attrId, bool serialize)
{
    n_assert(this->inBeginAddCategoryAttrs);
    const Category& cat = this->categoryArray[this->addAttrCategoryIndex];
    
    // update the instance table
    if (cat.HasInstanceDataset())
    {
        ValueTable* values = cat.GetInstanceDataset()->Values();
        Table* table = cat.GetInstanceDataset()->GetTable();
        if(!values->HasColumn(attrId))
        {
            values->AddColumn(attrId);
        }
        if(!table->HasColumn(attrId))
        {
            table->AddColumn(attrId);
        }
    }

    // update attr/category mapping, make sure there are no duplicates!
    if (!this->attrCategoryMap.Contains(attrId))
    {
        Array<IndexT> newArray;
        this->attrCategoryMap.Add(attrId, newArray);
    }
    if (InvalidIndex == this->attrCategoryMap[attrId].FindIndex(this->addAttrCategoryIndex))
    {
        this->attrCategoryMap[attrId].Append(this->addAttrCategoryIndex);
    }

	if (this->addAttrCategorySync)
	{
		this->addAttrCategorySync->Append(attrId);
	}	
}

//------------------------------------------------------------------------------
/**
    End adding attributes to a category.
*/
void
CategoryManager::EndAddCategoryAttrs()
{
    n_assert(this->inBeginAddCategoryAttrs);
    const Category& cat = this->categoryArray[this->addAttrCategoryIndex];
    if (cat.HasInstanceDataset())
    {
        ValueTable* values = cat.GetInstanceDataset()->Values();
        values->EndAddColumns();
    }
    this->inBeginAddCategoryAttrs = false;

	this->addAttrCategorySync = NULL;
}

//------------------------------------------------------------------------------
/**
    This creates the mapping table which maps attributes to categories,
    so that it is very fast to check which attributes are used by
    which categories.
*/
void
CategoryManager::UpdateAttrCategoryMapping()
{
    const Ptr<Db::Database>& db = DbServer::Instance()->GetGameDatabase();

    // for each category...
    IndexT catIndex;
    for (catIndex = 0; catIndex < this->categoryArray.Size(); catIndex++)
    {
        Category& cat = this->categoryArray[catIndex];
        if (cat.GetInstanceTableName().IsValid())
        {
            Table* dbTable = db->GetTableByName(cat.GetInstanceTableName());

            // for each attribute of that category
            IndexT colIndex;
            SizeT numColumns = dbTable->GetNumColumns();
            for (colIndex = 0; colIndex < numColumns; colIndex++)
            {
                const Attr::AttrId& attrId = dbTable->GetColumn(colIndex).GetAttrId();
                if (!this->attrCategoryMap.Contains(attrId))
                {
                    Util::Array<IndexT> newArray;
                    this->attrCategoryMap.Add(attrId, newArray);
                }
                if (InvalidIndex == this->attrCategoryMap[attrId].FindIndex(catIndex))
                {
                    this->attrCategoryMap[attrId].Append(catIndex);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    This creates the mapping table which maps attributes to categories,
    so that it is very fast to check which attributes are used by
    which categories.

    Note: this version is suitable for live updates, as it uses the category datasets
    for the update. only mapping for a single category is updated.
*/
void
CategoryManager::UpdateAttrCategoryMappingForCategory(const Util::String& categoryName)
{
    int catIndex = this->catIndexMap[categoryName];
    Category& category = this->categoryArray[catIndex];
    IndexT colIndex;
    SizeT numColumns = category.GetInstanceDataset()->GetTable()->GetNumColumns();
    for (colIndex = 0; colIndex < numColumns; colIndex++)
    {
        const Attr::AttrId& attrId = category.GetInstanceDataset()->GetTable()->GetColumn(colIndex).GetAttrId();
        n_assert(InvalidIndex != catIndex);
        if (!this->attrCategoryMap.Contains(attrId))
        {
            Util::Array<IndexT> newArray;
            this->attrCategoryMap.Add(attrId, newArray);
        }
        if (InvalidIndex == this->attrCategoryMap[attrId].FindIndex(catIndex))
        {
            this->attrCategoryMap[attrId].Append(catIndex);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CategoryManager::LoadInstances(const Util::String& levelName)
{
    const Ptr<Db::Database>& db = DbServer::Instance()->GetGameDatabase();

    // for each category...
    IndexT catIndex;
    SizeT numCategories = this->categoryArray.Size();
    for (catIndex = 0; catIndex < numCategories; catIndex++)
    {
        Category& category = this->categoryArray[catIndex];
        const Util::String& instTableName = category.GetInstanceTableName();
        if (instTableName.IsValid())
        {
            // if the category has an instance table, load
            // all rows with a matching _Level attribute 
            Ptr<Dataset> dataset = db->GetTableByName(instTableName)->CreateDataset();
            dataset->AddAllTableColumns();
            dataset->Filter()->AddEqualCheck(Attribute(Attr::_Level, levelName));
            dataset->PerformQuery();
            category.instDataset = dataset;
        }
    }
    
    // setup attributes for instance table
    FactoryManager::Instance()->SetupAttributes();

    // create an attribute/category mapping
    this->UpdateAttrCategoryMapping();
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Dataset> >
CategoryManager::FindInstances(const Util::String& levelName)
{
    const Ptr<Db::Database>& db = DbServer::Instance()->GetGameDatabase();
    Util::Array<Ptr<Dataset> > result; 

    // for each category...
    IndexT catIndex;
    SizeT numCategories = this->categoryArray.Size();
    for (catIndex = 0; catIndex < numCategories; catIndex++)
    {
        const Category& category = this->categoryArray[catIndex];
        const Util::String& instTableName = category.GetInstanceTableName();
        if (instTableName.IsValid())
        {
            // if the category has an instance table, load
            // all rows with a matching _Level attribute 
            Ptr<Dataset> dataset = db->GetTableByName(instTableName)->CreateDataset();
            dataset->AddAllTableColumns();
            dataset->Filter()->AddEqualCheck(Attribute(Attr::_Level, levelName));
            dataset->PerformQuery();
            result.Append(dataset);
        }
    }

    return result;
}


//------------------------------------------------------------------------------
/**
    This returns the overall number of loaded instances.
*/
int
CategoryManager::GetNumInstances() const
{
    int numInstances = 0;
    IndexT catIndex;
    SizeT numCategories = this->categoryArray.Size();
    for (catIndex = 0; catIndex < numCategories; catIndex++)
    {
        if (this->categoryArray[catIndex].HasInstanceDataset())
        {
            numInstances += this->categoryArray[catIndex].GetInstanceDataset()->Values()->GetNumRows();
        }
    }
    return numInstances;
}

//------------------------------------------------------------------------------
/**
    Associate a game entity pointer with an instance. This uses the
    ValueTable's per-row user data field to store the pointer.
*/
void
CategoryManager::SetInstanceEntity(const Entry& instance, const Ptr<Game::Entity>& entity)
{
    instance.Values()->SetRowUserData(instance.RowIndex(), entity.get_unsafe());
}

//------------------------------------------------------------------------------
/**
    Get the game entity pointer associated with an instance.
*/
Ptr<Game::Entity>
CategoryManager::GetInstanceEntity(const Entry& instance) const
{
    return Ptr<Game::Entity>((Game::Entity*) instance.Values()->GetRowUserData(instance.RowIndex()));
}

//------------------------------------------------------------------------------
/**
    Create an instance in the dummy category, this is for entities
    which are created as helper entities and should never show up in the
    database (i.e. the global environment entity).
*/
CategoryManager::Entry
CategoryManager::CreateDummyInstance()
{
    IndexT rowIndex = this->dummyInstTable->AddRow();
    return Entry("_Dummy", this->dummyInstTable, rowIndex);
}

//------------------------------------------------------------------------------
/**
    This creates a new instance from a category name and id and returns
    its location.
*/
CategoryManager::Entry
CategoryManager::CreateInstanceFromTemplate(const Util::String& categoryName, const Util::String& id)
{
    n_assert(categoryName.IsValid());
    n_assert(id.IsValid());

    // need to copy a row from the template table to the instance table
    const Category& cat = this->GetCategoryByName(categoryName);
    n_assert(cat.HasTemplateDataset());
    n_assert(cat.HasInstanceDataset());
    ValueTable* templValues = cat.GetTemplateDataset()->Values();
    ValueTable* instValues  = cat.GetInstanceDataset()->Values();
    n_assert(templValues->HasColumn(Attr::Id));

    // find the template row and copy it into the instance table
    Util::Array<IndexT> templRowIndices = templValues->FindRowIndicesByAttr(Attribute(Attr::Id, id), true);
    if (templRowIndices.IsEmpty())
    {
        n_error("CategoryManager::CreateInstance(): template id '%s' not found in category '%s'!\n", 
            id.AsCharPtr(), categoryName.AsCharPtr());
    }
    IndexT instRowIndex = instValues->CopyExtRow(templValues, templRowIndices[0]);
    return Entry(categoryName, instValues, instRowIndex);
}

//------------------------------------------------------------------------------
/**
    This creates a new instance from scratch and fills it with the
    provided attributes.
*/
CategoryManager::Entry
CategoryManager::CreateInstanceFromAttrs(const Util::String& categoryName, const Util::Array<Attribute>& attrs)
{
    n_assert(categoryName.IsValid());
    const Category& cat = this->GetCategoryByName(categoryName);
    n_assert(cat.HasInstanceDataset());
    ValueTable* instValues  = cat.GetInstanceDataset()->Values();
    IndexT rowIndex = instValues->AddRow();
    IndexT attrIndex;
    for (attrIndex = 0; attrIndex < attrs.Size(); attrIndex++)
    {
        if (!instValues->HasColumn(attrs[attrIndex].GetAttrId()))
        {
            instValues->AddColumn(attrs[attrIndex].GetAttrId());
        }
        instValues->SetAttr(attrs[attrIndex], rowIndex);
    }
    return Entry(categoryName, instValues, rowIndex);
}

//------------------------------------------------------------------------------
/**
    This creates a new instance from a category name and id, but puts the
    instance into a different category's instance table.
*/
CategoryManager::Entry
CategoryManager::CreateInstanceFromTemplateAsCategory(const Util::String& categoryName, const Util::String& id, const Util::String& targetCategory)
{
    n_assert(categoryName.IsValid());
    n_assert(id.IsValid());
    n_assert(targetCategory.IsValid());

    // need to copy a row from the template table to a different instance table
    const Category& srcCat = this->GetCategoryByName(categoryName);
    const Category& dstCat = this->GetCategoryByName(targetCategory);
    n_assert(srcCat.HasTemplateDataset());
    n_assert(dstCat.HasInstanceDataset());
    ValueTable* templValues = srcCat.GetTemplateDataset()->Values();
    ValueTable* instValues = dstCat.GetInstanceDataset()->Values();
    n_assert(templValues->HasColumn(Attr::Id));

    // find the template row and copy it into the instance table
    Util::Array<IndexT> templRowIndices = templValues->FindRowIndicesByAttr(Attribute(Attr::Id, id), true);
    if (templRowIndices.IsEmpty())
    {
        n_error("CategoryManager::CreateInstance(): template id '%s' not found in category '%s'!\n", 
            categoryName.AsCharPtr(), id.AsCharPtr());
    }
    IndexT instRowIndex = instValues->CopyExtRow(templValues, templRowIndices[0]);
    return Entry(categoryName, instValues, instRowIndex);
}

//------------------------------------------------------------------------------
/**
    This creates a new instance as a copy of another instance of the same
    category.
*/
CategoryManager::Entry
CategoryManager::CreateInstanceFromInstance(const Entry& source)
{
    n_assert(source.IsValid());
    IndexT newRowIndex = source.Values()->CopyRow(source.RowIndex());
    Entry result(source.Category(), source.Values(), newRowIndex);
    return result;
}

//------------------------------------------------------------------------------
/**
    This creates a new instance in a different category as a copy of another 
    instance.
*/
CategoryManager::Entry
CategoryManager::CreateInstanceFromInstanceAsCategory(const Entry& source, const Util::String& targetCategory, bool createMissingAttributes)
{
    n_assert(source.IsValid());
    n_assert(targetCategory.IsValid());
    const Category& dstCat = this->GetCategoryByName(targetCategory);
    n_assert(dstCat.HasInstanceDataset());
    ValueTable* instValues = dstCat.GetInstanceDataset()->Values();

    IndexT newRowIndex = instValues->CopyExtRow(source.Values(), source.RowIndex(), createMissingAttributes);
    Entry result(targetCategory, instValues, newRowIndex);
    if (createMissingAttributes) 
    {
        this->UpdateAttrCategoryMappingForCategory(targetCategory);
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Create new instances by a matching attribute. This method
    can do several database access, and thus can be slow if the requested
    attribute doesn't map to an indexed database column.

    If you know that there is only one instance in the database, set the
    firstMatchOnly parameter to true to avoid unessecary database accesses.

    Matching instances which already loaded will not be correctly ignored,
    this is done by checking against the primary attribute (Attr::Guid)
    of the table.
*/
Util::Array<CategoryManager::Entry>
CategoryManager::GetInstancesByAttr(const Attribute& attr, bool loadedOnly, bool firstMatchOnly)
{
    Util::Array<Entry> result;

    // if no category has this attribute, silently return with an empty result
    if (!this->attrCategoryMap.Contains(attr.GetAttrId()))
    {
        return result;
    }

    // first check the already existing instances
    Util::Array<Util::Array<IndexT> > existInstRowIndices;
    const Util::Array<IndexT>& catIndices = this->attrCategoryMap[attr.GetAttrId()];
    IndexT i;
    for (i = 0; i < catIndices.Size(); i++)
    {
        const Category& cat = this->categoryArray[catIndices[i]];
        n_assert(cat.HasInstanceDataset());
        n_assert(cat.GetInstanceDataset()->Values()->HasColumn(attr.GetAttrId()));
        Dataset* instDataset = cat.GetInstanceDataset();
        ValueTable* instValues = instDataset->Values();
        Util::Array<IndexT> emptyArray;
        existInstRowIndices.Append(emptyArray);

        // first find instances which are already loaded
        existInstRowIndices.Back() = instValues->FindRowIndicesByAttr(attr, firstMatchOnly);
        IndexT ii;
        for (ii = 0; ii < existInstRowIndices.Back().Size(); ii++)
        {
            result.Append(Entry(cat.GetName(), instValues, existInstRowIndices.Back()[ii]));
            if (firstMatchOnly)
            {
                return result;
            }
        }
    }

    // no check database if needed
    if (!loadedOnly)
    {
        for (i = 0; i < catIndices.Size(); i++)
        {
            const Category& cat = this->categoryArray[catIndices[i]];
            n_assert(cat.HasInstanceDataset());
            n_assert(cat.GetInstanceDataset()->Values()->HasColumn(attr.GetAttrId()));
            Dataset* instDataset = cat.GetInstanceDataset();
            ValueTable* instValues = instDataset->Values();

            // first check if the filter attribute is actually committed to the
            // database, otherwise a database access doesn't make sense
            // (since there can't be any instances in the database we're looking for)
            Table* dbTable = DbServer::Instance()->GetGameDatabase()->GetTableByName(cat.GetInstanceTableName());
            if (!dbTable->GetColumn(attr.GetAttrId()).IsCommitted())
            {
                continue;
            }

            // construct a new filter on the instance dataset, and
            // perform a database query which just appends the result (if any)
            // to the dataset... to find out if new rows have been added
            // we check the row count before and after the operation
            SizeT numRowsBefore = instValues->GetNumRows();
            instDataset->Filter()->Clear();
            instDataset->Filter()->AddEqualCheck(attr);
            if (existInstRowIndices[i].Size() > 0)
            {
                // remove already loaded instances from the query
                instDataset->Filter()->AddAnd();
                instDataset->Filter()->AddNot();
                instDataset->Filter()->BeginBlock();
                IndexT notIndex;
                for (notIndex = 0; notIndex < existInstRowIndices[i].Size(); notIndex++)
                {
                    IndexT instRowIndex = existInstRowIndices[i][notIndex];
                    Attribute guidAttr(Attr::Guid, instValues->GetGuid(Attr::Guid, instRowIndex));
                    instDataset->Filter()->AddEqualCheck(guidAttr);
                    if (notIndex < (existInstRowIndices[i].Size() - 1))
                    {
                        instDataset->Filter()->AddOr();
                    }
                }
                instDataset->Filter()->EndBlock();
            }
            instDataset->PerformQuery(true);
            SizeT numRowsAfter = instValues->GetNumRows();

            // check if we got results
            IndexT newRowIndex;
            for (newRowIndex = numRowsBefore; newRowIndex < numRowsAfter; newRowIndex++)
            {
                result.Append(Entry(cat.GetName(), instValues, newRowIndex));
                if (firstMatchOnly)
                {
                    return result;
                }
            }
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Create new instances by a multiple matching attributes.
*/
Util::Array<CategoryManager::Entry>
CategoryManager::GetInstancesByAttrs(const Util::Array<Attribute>& attrs, bool loadedOnly, bool firstMatchOnly)
{
    Util::Array<Entry> result;

    // first find categories which have all required attributes
    Util::Array<IndexT> catIndices;
    IndexT catIndex;
    for (catIndex = 0; catIndex < this->categoryArray.Size(); catIndex++)
    {
        const Category& cat = this->categoryArray[catIndex];
        if (!cat.HasInstanceDataset())
        {
            continue;
        }
        bool hasAllAttrs = true;
        ValueTable* instValues = cat.GetInstanceDataset()->Values();
        IndexT attrIndex;
        for (attrIndex = 0; attrIndex < attrs.Size(); attrIndex++)
        {
            if (!instValues->HasColumn(attrs[attrIndex].GetAttrId()))
            {
                hasAllAttrs = false;
                break;
            }
        }

        // ok category has all required attributes
        if (hasAllAttrs)
        {
            catIndices.Append(catIndex);
        }
    }

    // for each category which has all attributes...
    Util::Array<Util::Array<IndexT> > existInstRowIndices;
    IndexT i;
    for (i = 0; i < catIndices.Size(); i++)
    {
        const Category& cat = this->categoryArray[catIndices[i]];
        n_assert(cat.HasInstanceDataset());
        Dataset* instDataset = cat.GetInstanceDataset();
        ValueTable* instValues = instDataset->Values();
        Util::Array<IndexT> emptyArray;
        existInstRowIndices.Append(emptyArray);

        // first find instances which are already loaded
        existInstRowIndices.Back() = instValues->FindRowIndicesByAttrs(attrs, firstMatchOnly);
        IndexT ii;
        for (ii = 0; ii < existInstRowIndices.Back().Size(); ii++)
        {
            result.Append(Entry(cat.GetName(), instValues, existInstRowIndices.Back()[ii]));
            if (firstMatchOnly)
            {
                return result;
            }
        }
    }

    if (!loadedOnly)
    {
        for (i = 0; i < catIndices.Size(); i++)
        {
            const Category& cat = this->categoryArray[catIndices[i]];
            n_assert(cat.HasInstanceDataset());
            Dataset* instDataset = cat.GetInstanceDataset();
            ValueTable* instValues = instDataset->Values();

            // first check if all filter attribute is actually committed to the
            // database, otherwise a database access doesn't make sense
            // (since there can't be any instances in the database we're looking for)
            bool allAttrColumnsCommitted = true;
            Table* dbTable = DbServer::Instance()->GetGameDatabase()->GetTableByName(cat.GetInstanceTableName());
            IndexT attrIndex;
            for (attrIndex = 0; attrIndex < attrs.Size(); attrIndex++)
            {
                if (!dbTable->GetColumn(attrs[attrIndex].GetAttrId()).IsCommitted())
                {
                    allAttrColumnsCommitted = false;
                    break;
                }
            }
            if (!allAttrColumnsCommitted)
            {
                continue;
            }

            // construct a new filter on the instance dataset, and
            // perform a database query which just appends the result (if any)
            // to the dataset... to find out if new rows have been added
            // we check the row count before and after the operation
            SizeT numRowsBefore = instValues->GetNumRows();
            instDataset->Filter()->Clear();
            SizeT numAttrs = attrs.Size();
            for (attrIndex = 0; attrIndex < numAttrs; attrIndex++)
            {
                instDataset->Filter()->AddEqualCheck(attrs[attrIndex]);
                if (attrIndex < (numAttrs - 1))
                {
                    instDataset->Filter()->AddAnd();
                }
            }
            if (existInstRowIndices[i].Size() > 0)
            {
                // remove already loaded instances from the query
                instDataset->Filter()->AddAnd();
                instDataset->Filter()->AddNot();
                instDataset->Filter()->BeginBlock();
                IndexT notIndex;
                for (notIndex = 0; notIndex < existInstRowIndices[i].Size(); notIndex++)
                {
                    IndexT instRowIndex = existInstRowIndices[i][notIndex];
                    Attribute guidAttr(Attr::Guid, instValues->GetGuid(Attr::Guid, instRowIndex));
                    instDataset->Filter()->AddEqualCheck(guidAttr);
                    if (notIndex < (existInstRowIndices[i].Size() - 1))
                    {
                        instDataset->Filter()->AddOr();
                    }
                }
                instDataset->Filter()->EndBlock();
            }
            instDataset->PerformQuery(true);
            SizeT numRowsAfter = instValues->GetNumRows();

            // check if we got results
            IndexT newRowIndex;
            for (newRowIndex = numRowsBefore; newRowIndex < numRowsAfter; newRowIndex++)
            {
                result.Append(Entry(cat.GetName(), instValues, newRowIndex));
                if (firstMatchOnly)
                {
                    return result;
                }
            }
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    This deletes an instance from the database.
*/
void
CategoryManager::DeleteInstance(const Entry& entry)
{
    if(!entry.Values()->IsRowDeleted(entry.RowIndex()))
    {
        entry.Values()->DeleteRow(entry.RowIndex());
    }
    this->GetCategoryByName(entry.Category()).GetInstanceDataset()->CommitDeletedRows();
}

//------------------------------------------------------------------------------
/**
*/
CategoryManager::Entry
CategoryManager::FindTemplate(const Util::String& categoryName, const Util::String& id) const
{
    ValueTable* table = this->GetTemplateTable(categoryName);
    Util::Array<IndexT> rowIndices = table->FindRowIndicesByAttr(Attribute(Attr::Id, id), true);
    if (rowIndices.Size() > 0)
    {
        return Entry(categoryName, table, rowIndices[0]);
    }
    else
    {
        Entry invalidEntry;
        return invalidEntry;
    }
}

//------------------------------------------------------------------------------
/**
    Find the first template which has the matching attribute. If 
    categoryName is given, only search in this category, otherwise
    in all categories which have the attribute.

    WARNING: this method does linear searches on the value tables
    and thus can be slow.
*/
CategoryManager::Entry
CategoryManager::FindTemplateByAttr(const Attr::Attribute& attr, const Util::String& categoryName) const
{
    Util::Array<IndexT> catIndices;
    if (categoryName.IsValid())
    {
        // if category name given, only search in this category
        catIndices.Append(this->catIndexMap[categoryName]);
    }
    else
    {
        // else search in all categories which have this attribute
        catIndices = this->attrCategoryMap[attr.GetAttrId()];
    }
    IndexT i;
    for (i = 0; i < catIndices.Size(); i++)
    {
        const Category& cat = this->categoryArray[catIndices[i]];
        Util::Array<IndexT> rowIndices = cat.GetTemplateDataset()->Values()->FindRowIndicesByAttr(attr, true);
        if (!rowIndices.IsEmpty())
        {
            return Entry(cat.GetName(), cat.GetTemplateDataset()->Values(), rowIndices[0]);
        }
    }

    // fallthrough: no result found
    Entry invalidEntry;
    return invalidEntry;
}

//------------------------------------------------------------------------------
/**
*/
void
CategoryManager::DuplicateLevel(const Util::String& levelName, const Util::String& duplicateName)
{
    n_assert(levelName.IsValid());
    n_assert(duplicateName.IsValid());

    // get all instances from source level
    Array<Ptr<Dataset> > instances = this->FindInstances(levelName);
    Util::Guid guid;

    IndexT idxDataset;
    for (idxDataset = 0; idxDataset < instances.Size(); idxDataset++)
    {
        ValueTable* valueTable = instances[idxDataset]->Values();
        
        SizeT numRows = valueTable->GetNumRows();
        IndexT idxRow;
        for (idxRow = 0; idxRow < numRows; idxRow++)
        {
            // copy the row
            int duplicateRow = valueTable->CopyRow(idxRow);
            // set the duplicate level name
            valueTable->SetString(Attr::_Level, duplicateRow, duplicateName);
            // assign a new guid
            guid.Generate();
            valueTable->SetGuid(Attr::Guid, duplicateRow, guid);
        }

        instances[idxDataset]->CommitChanges();
    }

    // duplicate _Instance_Levels table entry
    const Ptr<Db::Database>& db = DbServer::Instance()->GetGameDatabase();
    Ptr<Dataset> dataset = db->GetTableByName("_Instance_Levels")->CreateDataset();
    dataset->AddAllTableColumns();
    dataset->Filter()->AddEqualCheck(Attribute(Attr::Id, levelName));
    dataset->PerformQuery();

    ValueTable* valueTable = dataset->Values();
    n_assert(valueTable->GetNumRows());
    int duplicateRow = valueTable->CopyRow(0);
    valueTable->SetString(Attr::Id, duplicateRow, duplicateName);
    dataset->CommitChanges();
}

//------------------------------------------------------------------------------
/**
*/
void
CategoryManager::DeleteLevel(const Util::String& levelName)
{
    n_assert(levelName.IsValid());

    // get all instances from source level
    Array<Ptr<Dataset> > instances = this->FindInstances(levelName);

    // delete all instances of all categorys
    IndexT idxDataset;
    for (idxDataset = 0; idxDataset < instances.Size(); idxDataset++)
    {
        Ptr<Dataset> dataset = instances[idxDataset];
        dataset->Values()->DeleteAllRows();
        dataset->CommitChanges();
    }

    const Ptr<Db::Database>& db = DbServer::Instance()->GetGameDatabase();
    Ptr<Dataset> dataset = db->GetTableByName("_Instance_Levels")->CreateDataset();
    dataset->AddColumn(Attr::Id);
    dataset->Filter()->AddEqualCheck(Attribute(Attr::Id, levelName));
    dataset->PerformQuery();

    ValueTable* valueTable = dataset->Values();
    if (valueTable->GetNumRows())
    {
        valueTable->DeleteAllRows();
        dataset->CommitDeletedRows();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CategoryManager::RenameLevel(const Util::String& levelName, const Util::String& newLevelName)
{
    n_assert(levelName.IsValid());
    n_assert(newLevelName.IsValid());

    // get all instances from source level
    Util::Array<Ptr<Dataset> > instances = this->FindInstances(levelName);
    Util::Guid guid;

    IndexT idxDataset;
    for (idxDataset = 0; idxDataset < instances.Size(); idxDataset++)
    {
        ValueTable* valueTable = instances[idxDataset]->Values();
        
        SizeT numRows = valueTable->GetNumRows();
        IndexT idxRow;
        for (idxRow = 0; idxRow < numRows; idxRow++)
        {
            // set the duplicate level name
            valueTable->SetString(Attr::_Level, idxRow, newLevelName);
        }

        instances[idxDataset]->CommitChanges();
    }

    // duplicate _Instance_Levels table entry
    const Ptr<Db::Database>& db = DbServer::Instance()->GetGameDatabase();
    Ptr<Dataset> dataset = db->GetTableByName("_Instance_Levels")->CreateDataset();
    dataset->AddAllTableColumns();
    dataset->Filter()->AddEqualCheck(Attribute(Attr::Id, levelName));
    dataset->PerformQuery();

    ValueTable* valueTable = dataset->Values();
    n_assert(valueTable->GetNumRows());
    valueTable->SetString(Attr::Id, 0, newLevelName);
    dataset->CommitChanges();
}

//------------------------------------------------------------------------------
/**
*/
bool
CategoryManager::LevelExists(const Util::String& levelName)
{
    const Ptr<Db::Database>& db = DbServer::Instance()->GetGameDatabase();
    Ptr<Dataset> dataset = db->GetTableByName("_Instance_Levels")->CreateDataset();
    dataset->AddColumn(Attr::Id);
    dataset->Filter()->AddEqualCheck(Attribute(Attr::Id, levelName));
    dataset->PerformQuery();

    ValueTable* valueTable = dataset->Values();
    return (valueTable->GetNumRows() > 0);
}


//------------------------------------------------------------------------------
/**
*/
void
CategoryManager::AddCategory(const Category& cat)
{
	this->categoryArray.Append(cat);
	this->catIndexMap.Add(cat.name, this->categoryArray.Size() - 1);
}

} // namespace Managers
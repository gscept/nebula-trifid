#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::CategoryManager
  
    Wraps entity categories and provides access to category template and
    instance tables.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "game/manager.h"
#include "core/singleton.h"
#include "db/dataset.h"
#include "db/valuetable.h"
#include "game/entity.h"
#include "util/hashtable.h"

#define SetupAttr(ATTRID,SERIALIZE) BaseGameFeature::CategoryManager::Instance()->AddCategoryAttr(ATTRID,SERIALIZE)

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareString(CategoryName, 'CANA', ReadWrite);
    DeclareBool(IsVirtualCategory, 'ISVC', ReadWrite);
    DeclareBool(IsSpecialCategory, 'ISSC', ReadWrite);
    DeclareString(CategoryTemplateTable, 'CATT', ReadWrite);
    DeclareString(CategoryInstanceTable, 'CAIT', ReadWrite);
};
//------------------------------------------------------------------------------

namespace BaseGameFeature
{
	
class FactoryManager;
	
class CategoryManager : public Game::Manager
{
    __DeclareClass(CategoryManager);
    __DeclareSingleton(CategoryManager);
public:
    /// describes a category
    class Category
    {
    public:
        /// constructor
        Category();
        /// get category name
        const Util::String& GetName() const;
        /// return true if virtual category (starting with a . in the db.xml)
        bool IsVirtual() const;
        /// return true if this is a special category (e.g. _Environment) 
        bool IsSpecial() const;
        /// get template table name
        const Util::String& GetTemplateTableName() const;
        /// get instance table name
        const Util::String& GetInstanceTableName() const;
        /// return true if the category has a template dataset
        bool HasTemplateDataset() const;
        /// return true if the category has an instance dataset
        bool HasInstanceDataset() const;
        /// get pointer to template dataset
        const Ptr<Db::Dataset>& GetTemplateDataset() const;
        /// get pointer to instance dataset
        const Ptr<Db::Dataset>& GetInstanceDataset() const;

    private:
        friend class CategoryManager;
		friend class FactoryManager;
        Util::String name;
        bool isVirtual;
        bool isSpecial;
        Util::String templTableName;
        Util::String instTableName;
        Ptr<Db::Dataset> templDataset;
        Ptr<Db::Dataset> instDataset;
    };

    /// describes an entry in a value table
    class Entry
    {
    public:
        /// default constructor
        Entry();
        /// constructor with value table and row index
        Entry(const Util::String& categoryName, const Ptr<Db::ValueTable>& valueTable, IndexT rowIndex);
        /// return true if valid
        bool IsValid() const;
        /// get category name
        const Util::String& Category() const;
        /// get pointer to value table
        const Ptr<Db::ValueTable>& Values() const;
        /// get row index in value table
        IndexT RowIndex() const;
    private:
        Util::String categoryName;
        Ptr<Db::ValueTable> valueTable;
        IndexT rowIndex;
    };

    /// constructor
    CategoryManager();
    /// destructor
    virtual ~CategoryManager();

    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();
    /// commit changes back into database
    void CommitChangesToDatabase();
    /// load all instances with the given level attribute
    void LoadInstances(const Util::String& levelName);
    /// find all instances with the given level attribute
    Util::Array<Ptr<Db::Dataset> > FindInstances(const Util::String& levelName);
    /// create a dummy instance which will never be saved to the database
    Entry CreateDummyInstance();
    /// create a new instance from a template
    Entry CreateInstanceFromTemplate(const Util::String& categoryName, const Util::String& id);
    /// create a new instance from a template as a different category
    Entry CreateInstanceFromTemplateAsCategory(const Util::String& categoryName, const Util::String& id, const Util::String& targetCategory);
    /// create a new instance from a template and override attributes
    Entry CreateInstanceFromAttrs(const Util::String& categoryName, const Util::Array<Attr::Attribute>& attrs);
    /// create a new instance as a copy from another instance of the same category
    Entry CreateInstanceFromInstance(const Entry& source);
    /// create a new instance as a copy from another instance, but in a different category
    Entry CreateInstanceFromInstanceAsCategory(const Entry& source, const Util::String& targetCategory, bool createMissingAttributes = false);
    /// find and/or create new instance by matching a key attribute, this method can be quite slow!
    Util::Array<Entry> GetInstancesByAttr(const Attr::Attribute& attrs, bool loadedOnly, bool firstMatchOnly);
    /// find and/or create new instance by matching a key attribute, this method can be quite slow!
    Util::Array<Entry> GetInstancesByAttrs(const Util::Array<Attr::Attribute>& attrs, bool loadedOnly, bool firstMatchOnly);
    /// delete an instance identified by GUID
    void DeleteInstance(const Entry& source);
    /// get current overall number of instances
    int GetNumInstances() const;
    /// bind entity pointer to instance
    void SetInstanceEntity(const Entry& instance, const Ptr<Game::Entity>& entity);
    /// get instance entity pointer (can be 0!)
    Ptr<Game::Entity> GetInstanceEntity(const Entry& instance) const;

    /// return true if a category exists
    bool HasCategory(const Util::String& categoryName) const;
    /// get number of categories
    SizeT GetNumCategories() const;
    /// access to category attributes by index
    const Category& GetCategoryByIndex(IndexT index) const;
    /// access to category attributes by name
    const Category& GetCategoryByName(const Util::String& categoryName) const;
    /// begin adding category attributes
    void BeginAddCategoryAttrs(const Util::String& categoryName);
    /// add a category attribute
	void AddCategoryAttr(const Attr::AttrId& attrId, bool serialize);
    /// end adding category attributes
    void EndAddCategoryAttrs();

    /// return true if a template table for a category exists
    bool HasTemplateTable(const Util::String& categoryName) const;
    /// get template value table by category name
    const Ptr<Db::ValueTable>& GetTemplateTable(const Util::String& categoryName) const;
    /// find a template entry by category name and id
    Entry FindTemplate(const Util::String& categoryName, const Util::String& id) const;
    /// find a single template by attribute
    Entry FindTemplateByAttr(const Attr::Attribute& attr, const Util::String& categoryName="") const;

	/// get synced attributes for category
	const Util::Array<Attr::AttrId> & GetSyncedAttributes(const Util::String & category);

    /// return true if an instance table for a category exists
    bool HasInstanceTable(const Util::String& categoryName) const;
    /// get instance value table by category name
    const Ptr<Db::ValueTable>& GetInstanceTable(const Util::String& categoryName) const;

    /// duplicates a complete level in the databes
    void DuplicateLevel(const Util::String& levelName, const Util::String& duplicateName);
    /// deletes a complete level from database
    void DeleteLevel(const Util::String& levelName);
    /// renames a complete level in the database
    void RenameLevel(const Util::String& levelName, const Util::String& newLevelName);
    /// return true if level is found in _Instance_Levels table
    bool LevelExists(const Util::String& levelName);

private:
	friend class BaseGameFeature::FactoryManager;

    /// load categories from database
    void LoadCategories();
    /// load template tables from database
    void LoadTemplates();
    /// create the attr/category mapping table
    void UpdateAttrCategoryMapping();
    /// create the attr/category mapping table for a single category, uses live data
    void UpdateAttrCategoryMappingForCategory(const Util::String& categoryName);
	/// add another category that was not stored in the database
	void AddCategory(const Category& cat);


    Util::Array<Category> categoryArray;
    Util::Dictionary<Util::String,IndexT> catIndexMap;
    Util::Dictionary<Attr::AttrId,Util::Array<IndexT> > attrCategoryMap;   // what attributes are used by what categories?
    Ptr<Db::ValueTable> dummyInstTable;
    bool inBeginAddCategoryAttrs;
    IndexT addAttrCategoryIndex;
	Util::HashTable<Util::String, Util::Array<Attr::AttrId>> syncedAttrs;
	Util::Array<Attr::AttrId> * addAttrCategorySync;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CategoryManager::HasCategory(const Util::String& categoryName) const
{
    return this->catIndexMap.Contains(categoryName);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
CategoryManager::GetNumCategories() const
{
    return this->categoryArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const CategoryManager::Category&
CategoryManager::GetCategoryByIndex(IndexT index) const
{
    return this->categoryArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline const CategoryManager::Category&
CategoryManager::GetCategoryByName(const Util::String& categoryName) const
{
    return this->categoryArray[this->catIndexMap[categoryName]];
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CategoryManager::HasTemplateTable(const Util::String& categoryName) const
{
    n_assert(categoryName.IsValid());
    if (this->HasCategory(categoryName))
    {
        return this->GetCategoryByName(categoryName).HasTemplateDataset();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Db::ValueTable>&
CategoryManager::GetTemplateTable(const Util::String& categoryName) const
{
    n_assert(this->HasTemplateTable(categoryName));
    return this->GetCategoryByName(categoryName).GetTemplateDataset()->Values();
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CategoryManager::HasInstanceTable(const Util::String& categoryName) const
{
    n_assert(categoryName.IsValid());
    if (this->HasCategory(categoryName))
    {
        return this->GetCategoryByName(categoryName).HasInstanceDataset();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Db::ValueTable>&
CategoryManager::GetInstanceTable(const Util::String& categoryName) const
{
    n_assert(this->HasInstanceTable(categoryName));
    return this->GetCategoryByName(categoryName).GetInstanceDataset()->Values();
}

//------------------------------------------------------------------------------
/**
*/
inline CategoryManager::Category::Category() :
    isVirtual(false),
    isSpecial(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
CategoryManager::Category::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CategoryManager::Category::IsVirtual() const
{
    return this->isVirtual;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CategoryManager::Category::IsSpecial() const
{
    return this->isSpecial;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
CategoryManager::Category::GetTemplateTableName() const
{
    return this->templTableName;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
CategoryManager::Category::GetInstanceTableName() const
{
    return this->instTableName;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CategoryManager::Category::HasTemplateDataset() const
{
    return this->templDataset.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CategoryManager::Category::HasInstanceDataset() const
{
    return this->instDataset.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Db::Dataset>&
CategoryManager::Category::GetTemplateDataset() const
{
    return this->templDataset;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Db::Dataset>&
CategoryManager::Category::GetInstanceDataset() const
{
    n_assert(this->HasInstanceDataset());
    return this->instDataset;
}

//------------------------------------------------------------------------------
/**
*/
inline 
CategoryManager::Entry::Entry() :
    rowIndex(InvalidIndex)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline CategoryManager::Entry::Entry(const Util::String& catName, const Ptr<Db::ValueTable>& t, IndexT i) :
    categoryName(catName),
    valueTable(t),
    rowIndex(i)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CategoryManager::Entry::IsValid() const
{
    return this->valueTable.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
CategoryManager::Entry::Category() const
{
    return this->categoryName;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Db::ValueTable>&
CategoryManager::Entry::Values() const
{
    return this->valueTable;
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
CategoryManager::Entry::RowIndex() const
{
    return this->rowIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<Attr::AttrId> & 
CategoryManager::GetSyncedAttributes(const Util::String & category)
{
	return this->syncedAttrs[category];
}

} // namespace Managers
//------------------------------------------------------------------------------

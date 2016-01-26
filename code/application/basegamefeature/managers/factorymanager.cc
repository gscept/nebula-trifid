//------------------------------------------------------------------------------
//  managers/factorymanager.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "core/factory.h"
#include "util/guid.h"
#include "managers/factorymanager.h"
#include "game/property.h"
#include "game/entity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "io/ioserver.h"
#include "io/xmlreader.h"
#include "basegamefeatureunit.h"
#include "game/gameserver.h"


namespace BaseGameFeature
{
__ImplementClass(FactoryManager, 'MFAM', Game::Manager);
__ImplementSingleton(FactoryManager);

using namespace Game;
using namespace Util;

Util::String FactoryManager::blueprintFilename("blueprints.xml");
Util::String FactoryManager::blueprintFolder("data:tables/");
//------------------------------------------------------------------------------
/**
*/
FactoryManager::FactoryManager()
{
    __ConstructSingleton;
    if (!this->ParseBluePrints())
    {
        n_error("Managers::FactoryManager: Error parsing data:tables/blueprints.xml!");
    }
}

//------------------------------------------------------------------------------
/**
*/
FactoryManager::~FactoryManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Create a raw entity by its C++ class name name. This method should be 
    extended by subclasses if a Mangalore application implements new
    Game::Entity subclasses.
*/
Ptr<Entity>
FactoryManager::CreateEntityByClassName(const Util::String& cppClassName) const
{
    if ("Entity" == cppClassName) 
    {
        return Entity::Create();
    }
    else
    {
        n_error("Managers::FactoryManager::CreateEntity(): unknown entity class name '%s'!", cppClassName.AsCharPtr());
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Create an entity from its category name. The category name is looked
    up in the blueprints.xml file to check what properties must be attached
    to the entity. All required properties will be attached, and all
    attributes will be initialised according to the attribute table.
*/
Ptr<Entity> 
FactoryManager::CreateEntityByCategory(const Util::String& categoryName, 
                                       const Ptr<Db::ValueTable>& attrTable, 
                                       IndexT attrTableRowIndex, 
									   bool isMaster /*= true*/) const
{
    // find blueprint index
    IndexT i = this->FindBluePrint(categoryName);
    if (InvalidIndex != i)
    {
        // create raw entity
        Ptr<Entity> entity = this->CreateEntityByClassName(this->bluePrints[i].cppClass);
        entity->SetAttrTable(attrTable);
        entity->SetAttrTableRowIndex(attrTableRowIndex);
        entity->SetCategory(categoryName);
		this->AddProperties(entity, categoryName, isMaster);
        return entity;
    }
    else
    {
        n_error("Managers::FactoryManager::CreateEntityByCategory(%s): category not found in blueprints.xml!", categoryName.AsCharPtr());
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Create a new entity from scratch and initialize it with the provided
    attributes.
*/
Ptr<Entity>
FactoryManager::CreateEntityByAttrs(const Util::String& categoryName, const Util::Array<Attr::Attribute>& attrs, bool isMaster /*= true */) const
{
    // find blueprint index
    IndexT i = this->FindBluePrint(categoryName);
    if (InvalidIndex != i)
    {
        // create a new category instance
        CategoryManager::Entry newEntry = CategoryManager::Instance()->CreateInstanceFromAttrs(categoryName, attrs);

        // create raw entity
        Ptr<Entity> entity = this->CreateEntityByClassName(this->bluePrints[i].cppClass);
        entity->SetAttrTable(newEntry.Values());
        entity->SetAttrTableRowIndex(newEntry.RowIndex());
        entity->SetCategory(categoryName);
		this->AddProperties(entity, categoryName, isMaster);

        Util::Guid guid;
        guid.Generate();
        entity->SetGuid(Attr::Guid, guid);
        entity->SetString(Attr::_Level, BaseGameFeatureUnit::Instance()->GetCurrentLevel());
		if (entity->GetString(Attr::_Layers).Length() == 0) entity->SetString(Attr::_Layers, "Default");
        return entity;
    }
    else
    {
        n_error("Managers::FactoryManager::CreateEntityByAttrs(%s): category not found in blueprints.xml!", categoryName.AsCharPtr());
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Create an entity from a template in the database. The template is
    defined by category name and the template name defined by the 
    the Attr::Id attribute. This will create a complete entity with properties 
    and attributes initialized to the values from the template.
    A new GUID will be assigned to the entity.
*/
Ptr<Entity>
FactoryManager::CreateEntityByTemplate(const Util::String& categoryName, const Util::String& templateName, bool isMaster/* = true*/) const
{
	n_assert(categoryName.IsValid());
	n_assert(templateName.IsValid());
    CategoryManager* catManager = CategoryManager::Instance();

    // ask category manager to create a new instance from template
    CategoryManager::Entry catEntry = catManager->CreateInstanceFromTemplate(categoryName, templateName);
    n_assert(catEntry.IsValid());

    // create a new entity from the new category instance
	Ptr<Entity> entity = this->CreateEntityByCategory(categoryName, catEntry.Values(), catEntry.RowIndex(), isMaster);

    // need to initialize the GUID and _Level attribute
    Util::Guid guid;
    guid.Generate();
    entity->SetGuid(Attr::Guid, guid);
    entity->SetString(Attr::_Level, BaseGameFeatureUnit::Instance()->GetCurrentLevel());
    return entity;
}

//------------------------------------------------------------------------------
/**
    Create a new entity from a template, but put the entity into a different
    category.
*/
Ptr<Entity>
FactoryManager::CreateEntityByTemplateAsCategory(const Util::String& categoryName, const Util::String& templateName, const Util::String& targetCategory) const
{
	n_assert(categoryName.IsValid());
	n_assert(templateName.IsValid());
    n_assert(targetCategory.IsValid());
    CategoryManager* catManager = CategoryManager::Instance();

    // ask category manager to create a new instance from template
    CategoryManager::Entry catEntry = catManager->CreateInstanceFromTemplateAsCategory(categoryName, templateName, targetCategory);
    n_assert(catEntry.IsValid());

    // create a new entity from the new category instance
    Ptr<Entity> entity = this->CreateEntityByCategory(targetCategory, catEntry.Values(), catEntry.RowIndex());

    // need to initialize the GUID and _Level attribute
    Util::Guid guid;
    guid.Generate();
    entity->SetGuid(Attr::Guid, guid);
    entity->SetString(Attr::_Level, BaseGameFeatureUnit::Instance()->GetCurrentLevel());
    return entity;
}

//------------------------------------------------------------------------------
/**
    Create a entity as a clone of a existing one. A new GUID will be assigned.
*/
Ptr<Entity>
FactoryManager::CreateEntityByEntity(const Ptr<Entity>& srcEntity) const
{
    n_assert(0 != srcEntity);
    CategoryManager* catManager = CategoryManager::Instance();

    // ask category manager to duplicate the original instance
    CategoryManager::Entry srcCatEntry(srcEntity->GetCategory(), srcEntity->GetAttrTable(), srcEntity->GetAttrTableRowIndex());
    CategoryManager::Entry newCatEntry = catManager->CreateInstanceFromInstance(srcCatEntry);
    n_assert(newCatEntry.IsValid());

    // create a new entity from the new category instance
    Ptr<Entity> entity = this->CreateEntityByCategory(newCatEntry.Category(), newCatEntry.Values(), newCatEntry.RowIndex());

    // need to assign a new guid
    Util::Guid guid;
    guid.Generate();
    entity->SetGuid(Attr::Guid, guid);
    return entity;
}

//------------------------------------------------------------------------------
/**
    Create a entity as a clone of a existing one but in a different category
    as the original entity. A new GUID will be assigned.
*/
Ptr<Entity>
FactoryManager::CreateEntityByEntityAsCategory(const Ptr<Entity>& srcEntity, const Util::String& targetCategory, bool createMissingAttributes) const
{
    n_assert(0 != srcEntity);
    CategoryManager* catManager = CategoryManager::Instance();

    // ask category manager to duplicate the original instance
    CategoryManager::Entry srcCatEntry(srcEntity->GetCategory(), srcEntity->GetAttrTable(), srcEntity->GetAttrTableRowIndex());
    CategoryManager::Entry newCatEntry = catManager->CreateInstanceFromInstanceAsCategory(srcCatEntry, targetCategory, createMissingAttributes);
    n_assert(newCatEntry.IsValid());

    // create a new entity from the new category instance
    Ptr<Entity> entity = this->CreateEntityByCategory(newCatEntry.Category(), newCatEntry.Values(), newCatEntry.RowIndex());

    // need to assign a new guid
    Util::Guid guid;
    guid.Generate();
    entity->SetGuid(Attr::Guid, guid);
    return entity;
}

//------------------------------------------------------------------------------
/**
    This will 'load' a new entity from the world database.    
    This will create a new entity, attach properties as described by 
    blueprints.xml, and update the entity attributes from the database. 
    Changes to attributes can later be written back to the
    database by calling the Entity::Save() method.

    NOTE: this method will not call the Entity::OnLoad() method, which may be
    required to finally initialize the entity. The OnLoad() method expects
    that all other entities in the level have already been loaded, so this
    must be done after loading in a separate pass.

    NOTE: use this method only if you know there's only one matching entity
    in the database (for instance by Guid attribute), otherwise, use the
    CreateEntitiesByKeyAttr() method, which checks all matches.
*/
Ptr<Entity>
FactoryManager::CreateEntityByKeyAttr(const Attr::Attribute& key) const
{    
    // ask category manager to create an entity by matching attribute
    CategoryManager* catManager = CategoryManager::Instance();
    Util::Array<CategoryManager::Entry> catEntries;
    catEntries = catManager->GetInstancesByAttr(key, false, true);
    if (catEntries.Size() == 1)
    {
        const Util::String& catName = catEntries[0].Category();
        Db::ValueTable* valueTable = catEntries[0].Values();
        IndexT tableRowIndex = catEntries[0].RowIndex();
        Ptr<Entity> entity = this->CreateEntityByCategory(catName, valueTable, tableRowIndex);
        return entity;
    }
    else
    {
        // no match, this is a hard error
        n_error("FactoryManager::CreateEntityByKeyAttr(): failed to create entity by attr(%s,%s)",
            key.GetName().AsCharPtr(), key.ValueAsString().AsCharPtr());
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Creates a new entity from the world database using a GUID as key.
    Simply calls CreateEntityByKeyAttr().
*/
Ptr<Entity>
FactoryManager::CreateEntityByGuid(const Util::Guid& guid) const
{
    return this->CreateEntityByKeyAttr(Attr::Attribute(Attr::Guid, guid));
}

//------------------------------------------------------------------------------
/**
    Create a property by its type name. This method should be extended by
    subclasses if a Mangalore application implements new properties.
*/
Ptr<Property>
FactoryManager::CreateProperty(const Util::String& type) const
{
    Game::Property* result = (Game::Property*) Core::Factory::Instance()->Create(type);
    n_assert(result != 0);
    return result;
}

//------------------------------------------------------------------------------
/**
    This method parses the file data:tables/blueprints.xml into
    the bluePrints array.
*/
bool
FactoryManager::ParseBluePrints()
{
    // it is not an error here if blueprints.xml doesn't exist
	Util::String blueprintsPath = FactoryManager::blueprintFolder;
    blueprintsPath.Append(FactoryManager::blueprintFilename);

    if (IO::IoServer::Instance()->FileExists(blueprintsPath))
    {        
        Ptr<IO::XmlReader> xmlReader = IO::XmlReader::Create();
        xmlReader->SetStream(IO::IoServer::Instance()->CreateStream(blueprintsPath));
        if (xmlReader->Open())
        {
            // make sure it's a BluePrints file
            if (xmlReader->GetCurrentNodeName() != "BluePrints")
            {
                n_error("FactoryManager::ParseBluePrints(): not a valid blueprints file!");
                return false;
            }
            if (xmlReader->SetToFirstChild("Entity")) do
            {
                BluePrint bluePrint;
                bluePrint.type = xmlReader->GetString("type");
                bluePrint.cppClass = xmlReader->GetString("cppclass");
                if (xmlReader->SetToFirstChild("Property")) do
                {
                    PropertyEntry newProp;
                    newProp.propertyName = xmlReader->GetString("type");
					newProp.masterOnly = xmlReader->GetOptBool("masterOnly", false);

                    bluePrint.properties.Append(newProp);
                }
                while (xmlReader->SetToNextChild("Property"));
                this->bluePrints.Append(bluePrint);
            }
            while (xmlReader->SetToNextChild("Entity"));
            xmlReader->Close();
            return true;
        }
        else
        {
            n_error("Managers::FactoryManager::ParseBluePrints(): could not open '%s'!", blueprintsPath.AsCharPtr());
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    This method finds a blueprint index by entity type name. Returns
    InvalidIndex if blueprint doesn't exist.
*/
IndexT
FactoryManager::FindBluePrint(const Util::String& entityType) const
{
    IndexT i;
    SizeT num = this->bluePrints.Size();
    for (i = 0;  i < num; i++)
    {
        if (this->bluePrints[i].type == entityType)
        {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
/**
    This method checks if a blueprint for the provided entity exists,
    and adds the properties defined in the blue print to the entity. If
    no matching blueprint exists, the entity will not be altered.
*/
void
FactoryManager::AddProperties(const Ptr<Entity>& entity, const Util::String& categoryName, bool isMaster /*= true*/) const
{
    n_assert(entity);
    
    IndexT index = this->FindBluePrint(categoryName);
    if (InvalidIndex != index)
    {
        const BluePrint& bluePrint = this->bluePrints[index];
        int i;
        int num = bluePrint.properties.Size();
        for (i = 0; i < num; i++)
        {
            // check if only remote property has to be created
			if (!(!isMaster && bluePrint.properties[i].masterOnly))
            {
                Ptr<Property> prop = this->CreateProperty(bluePrint.properties[i].propertyName);
                entity->AttachProperty(prop);
            }                                            
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
FactoryManager::SetBlueprintsFilename(const Util::String& name, const Util::String& folder)
{
    n_assert(name.IsValid());
    blueprintFilename = name;
	blueprintFolder = folder;
}

//------------------------------------------------------------------------------
/**
    Create the properties of every category and call SetupDefaultAttributes on it.
*/
void
FactoryManager::SetupAttributes()
{
    // create a instance of every property and call SetupDefaultAttributes()
    IndexT idxBluePrint;
    for(idxBluePrint = 0; idxBluePrint < this->bluePrints.Size(); idxBluePrint++)
    {
        const BluePrint& bluePrint = this->bluePrints[idxBluePrint];

        // category for blueprint type not found
        if (!CategoryManager::Instance()->HasCategory(bluePrint.type))
        {
            n_printf("Obsolete Category '%s' in blueprints.xml", bluePrint.type.AsCharPtr());
            continue;
        }

        // begin add category attrs
        CategoryManager::Instance()->BeginAddCategoryAttrs(bluePrint.type);

        const Util::Array<PropertyEntry>& catProperties = bluePrint.properties;
        IndexT idxCatProperty;
        for(idxCatProperty = 0; idxCatProperty < catProperties.Size(); idxCatProperty++)
        {
            const Util::String& propertyName = catProperties[idxCatProperty].propertyName;
            if (Core::Factory::Instance()->ClassExists(propertyName))
            {
                Ptr<Game::Property> newProperty = this->CreateProperty(propertyName);
				newProperty->SetupExternalAttributes();
                newProperty->SetupDefaultAttributes();
            }
        }
		CategoryManager::Instance()->EndAddCategoryAttrs();
    }		
}

//------------------------------------------------------------------------------
/**
	Adds a new blueprint type at runtime. use with care, the types end up in the database but wont be usable unless you add them to the
	blueprints.xml as well. Meant for temporary types.
*/
void 
FactoryManager::AddBlueprint(const Util::String& type, const Util::String &cppClass, const Util::Array<Util::String> &properties, bool special, bool isVirtual)
{
	BluePrint bluePrint;
	bluePrint.type = type;
	bluePrint.cppClass = cppClass;
	n_assert2(this->FindBluePrint(type) == InvalidIndex,"Entityclass already exists!");

	for(Util::Array<Util::String>::Iterator iter = properties.Begin();iter != properties.End();iter++)
	{
		PropertyEntry newProp;
		newProp.propertyName = *iter;
		newProp.masterOnly = false;
		bluePrint.properties.Append(newProp);
	}		
	this->bluePrints.Append(bluePrint);

	CategoryManager::Category cat;
	cat.name = type;
	cat.isSpecial = special;
	cat.isVirtual = isVirtual;
	Ptr<Db::Table> table = Db::DbFactory::Instance()->CreateTable();
	Util::String tablename;
	tablename.Format("_Instance_%s",type.AsCharPtr());
	table->SetName(tablename);
	table->AddColumn(Db::Column(Attr::Id));
	table->AddColumn(Db::Column(Attr::Guid));
	table->AddColumn(Db::Column(Attr::_Level));
	table->AddColumn(Db::Column(Attr::_Layers));
	Db::DbServer::Instance()->GetGameDatabase()->AddTable(table);
	cat.instTableName = tablename;
	cat.instDataset = table->CreateDataset();
	cat.instDataset->AddAllTableColumns();
	CategoryManager::Instance()->AddCategory(cat);
	FactoryManager::Instance()->SetupAttributes();
	CategoryManager::Instance()->UpdateAttrCategoryMapping();

}


} // namespace Managers

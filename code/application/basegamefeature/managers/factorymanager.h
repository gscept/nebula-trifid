#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::FactoryManager

    The FactoryManager is responsible for creating new game entities. 
    FactoryManager must be extended by Mangalore applications if the
    application needs new game entity classes.

    The FactoryManager loads the file

    data:tables/blueprints.xml

    on creation, which contains the construction blueprints for the
    entity types of your application. This file defines entity types
    by the properties which are added to them.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file  
*/
#include "game/manager.h"
#include "game/property.h"
#include "game/entity.h"
#include "addons/db/valuetable.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class FactoryManager : public Game::Manager
{
    __DeclareClass(FactoryManager);
    __DeclareSingleton(FactoryManager);
public:
    /// constructor
    FactoryManager();
    /// destructor
    virtual ~FactoryManager();
    /// create a new raw game entity by type name, extend this method in subclasses for new types
    virtual Ptr<Game::Entity> CreateEntityByClassName(const Util::String& cppClassName) const;
    /// create a new entity from its category name
	virtual Ptr<Game::Entity> CreateEntityByCategory(const Util::String& categoryName, const Ptr<Db::ValueTable>& attrTable, IndexT attrTableRowIndex, bool isMaster = true) const;
    /// create a new entity from scratch and initialize it with the provided attributes
    virtual Ptr<Game::Entity> CreateEntityByAttrs(const Util::String& categoryName, const Util::Array<Attr::Attribute>& attrs, bool isMaster = true) const;;
    /// create a new entity from a database template entry
	virtual Ptr<Game::Entity> CreateEntityByTemplate(const Util::String& categoryName, const Util::String& templateName, bool isMaster = true) const;
    /// create a new entity from a database template entry, and add it into a different category
    virtual Ptr<Game::Entity> CreateEntityByTemplateAsCategory(const Util::String& categoryName, const Util::String& templateName, const Util::String& targetCategory) const;
    /// create a new entity cloning an existing one
    virtual Ptr<Game::Entity> CreateEntityByEntity(const Ptr<Game::Entity>& sourceEntity) const;
    /// create a new entity in a different category cloning an existing one 
    virtual Ptr<Game::Entity> CreateEntityByEntityAsCategory(const Ptr<Game::Entity>& sourceEntity, const Util::String& targetCategory, bool createMissingAttributes = false) const;
    /// create new entity from world database using any key attribute
    virtual Ptr<Game::Entity> CreateEntityByKeyAttr(const Attr::Attribute& key) const;
    /// create new entity from world database using GUID as key attribute
    virtual Ptr<Game::Entity> CreateEntityByGuid(const Util::Guid& guid) const;
    /// create a new property by type name, extend in subclass for new types
    virtual Ptr<Game::Property> CreateProperty(const Util::String& type) const;
    /// add properties to entity according to blue print
	void AddProperties(const Ptr<Game::Entity>& entity, const Util::String& categoryName, bool isMaster) const;
    /// set a optional blueprints.xml, which is used instead of standard blueprint.xml
	static void SetBlueprintsFilename(const Util::String& name, const Util::String& folder);
    /// setup attributes on properties
    virtual void SetupAttributes();

	/// Adds a new blueprint entry at runtime
	void AddBlueprint(const Util::String& type, const Util::String &cppClass, const Util::Array<Util::String> &properties, bool special = false, bool isVirtual = false);


protected:
    /// parse entity blueprints file
    virtual bool ParseBluePrints();
    /// find blueprint index by property type
    IndexT FindBluePrint(const Util::String& entityType) const;

    /// an entity blueprint, these are created by ParseBlueprints()
    struct PropertyEntry
    {
        Util::String propertyName;
        bool masterOnly;
    };

    struct BluePrint
    {
        Util::String type;
        Util::String cppClass;
        Util::Array<PropertyEntry> properties;
    };
    Util::Array<BluePrint> bluePrints;
    static Util::String blueprintFilename;
	static Util::String blueprintFolder;
};

}; // namespace BaseGameFeature
//------------------------------------------------------------------------------

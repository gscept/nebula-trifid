#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::EditorBlueprintManager

    Based on normal factory manager but allows handling of blueprint files
	and access to them

    (C) 2012 Johannes Hirche
*/

#include "basegamefeature/managers/factorymanager.h"
#include "toolkitutil/idldocument/idlattribute.h"
#include "toolkitutil/idldocument/idlproperty.h"
#include "attr/attributecontainer.h"
#include "toolkitutil/logger.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class EditorBlueprintManager : public Core::RefCounted
{
    __DeclareClass(EditorBlueprintManager);
    __DeclareSingleton(EditorBlueprintManager);
public:

	struct TemplateEntry
	{		
		Util::String id;
		Util::String category;		
		Attr::AttributeContainer attrs;
	};
	struct BluePrint
	{
		Util::String type;
		Util::String cppClass;
		Util::String description;
		Util::Array<Ptr<Tools::IDLProperty>> properties;
		Util::Dictionary<Util::String,bool> remoteTable;
	};
	/// constructor
	EditorBlueprintManager();
	/// destructor
	virtual ~EditorBlueprintManager();
	/// Open blueprintmanager, load projectinfo and all blueprints, nidl files and templates
	void Open();
	
	/// is the AttrID with the fourcc a system attribute that should not be shown to the user
	bool IsSystemAttribute(Util::FourCC);
	/// is the attribute known to the blueprintmanager
	bool HasAttributeByName(const Util::String & attr) const;
	/// find an IDLAttribute by its name
	const Ptr<Tools::IDLAttribute> & GetAttributeByName(const Util::String & attr) const;


	/// get number of known categories
	const IndexT GetNumCategories() const;
	/// get the name of category by index
	const Util::String & GetCategoryByIndex(IndexT i) const;
	
	/// get all known templates for given category, can be empty
	const Util::Array<TemplateEntry> & GetTemplates(const Util::String & category) const;
	/// get all attributes for a category/template
	const Attr::AttributeContainer & GetTemplate(const Util::String & category, const Util::String & etemplate) const;
	/// get all attributes used by a given category
	Attr::AttributeContainer GetCategoryAttributes(const Util::String & category);	
	/// get all properties used by a category
	const Util::Array<Util::String> & GetCategoryProperties(const Util::String & category) const;

	/// add a template
	void AddTemplate(const Util::String & id, const Util::String & category, const Attr::AttributeContainer & container);
	/// save category templates
	void SaveCategoryTemplates(const Util::String & category);
	/// does template exist
	bool HasTemplate(const Util::String & id, const Util::String & category);

	/// is category known to manager, aka defined in any of the used blueprint files
	const bool HasCategory(const Util::String&templ) const;
	/// gets a category blueprint object
	const BluePrint& GetCategory(const Util::String & category); 
	/// adds or updates a category
	void SetCategory(const Util::String & category, const Util::Array<Util::String> properties );
	/// remove a category
	void RemoveCategory(const Util::String & category);

	/// property is known to system
	const bool HasProperty(const Util::String & propertyName) const;
	/// get property by name
	const Ptr<Tools::IDLProperty> & GetProperty(const Util::String & propertyName);

	/// get all known properties
	Util::Array<Ptr<Tools::IDLProperty>>  GetAllProperties();
	/// get all known attributes
	Util::Array<Ptr<Tools::IDLAttribute>> GetAllAttributes();

	/// get properties containing attribute
	Util::Array<Util::String> GetAttributeProperties(const Attr::AttrId& id);

	/// use templates from sdk directory instead of project
	void SetUseSDK(bool enable);

	/// set logger object
	void SetLogger(ToolkitUtil::Logger * log);

	/// save blueprint
	void SaveProjectBlueprint();
protected:
	
	/// parse project info file for nidls
	void ParseProjectInfo(const Util::String & path);
	/// add nidl file to attribute/property database
	void AddNIDLFile(const IO::URI & filename);
	/// parse specific blueprint file
	void ParseBlueprint(const IO::URI & filename);
	/// parse category templates from folder
	void ParseTemplates(const Util::String & folder);
	/// adds empty template tables for all categories
	void CreateMissingTemplates();
	/// update attributes used by a category
	void UpdateCategoryAttributes(const Util::String & category);	
	/// update properties owning an attribute
	void UpdateAttributeProperties();

	Util::Dictionary<Util::String, Ptr<Tools::IDLProperty>> properties;
	Util::Dictionary<Util::String,BluePrint> bluePrints;
	Util::Dictionary<Util::FourCC,bool> systemAttributeDictionary;
	Util::Dictionary<Util::String, Util::Array<TemplateEntry>> templates;
	Util::Dictionary<Util::String, Util::String> templateFiles;
	Util::Dictionary<Util::String, Attr::AttributeContainer> categoryAttributes;
	Util::Dictionary<Util::String, Ptr<Tools::IDLAttribute>> attributes;
	Util::Dictionary<Attr::AttrId, Util::Array<Util::String>> attributeProperties;
	Util::Dictionary<Util::String, Util::Array<Util::String>> categoryProperties;

	ToolkitUtil::Logger * logger;
	IO::URI blueprintPath;
	Util::String templateDir;
	bool useSDKDir;
};

//------------------------------------------------------------------------------
/**
*/
inline 
const IndexT
EditorBlueprintManager::GetNumCategories() const
{
	return bluePrints.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Util::String &
EditorBlueprintManager::GetCategoryByIndex(IndexT i) const
{
	return bluePrints.KeyAtIndex(i);
}
//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<Toolkit::EditorBlueprintManager::TemplateEntry> & 
EditorBlueprintManager::GetTemplates(const Util::String & category) const
{
	return this->templates[category];
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
EditorBlueprintManager::HasAttributeByName(const Util::String & attr) const
{
	return this->attributes.Contains(attr);
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Ptr<Tools::IDLAttribute> &
EditorBlueprintManager::GetAttributeByName(const Util::String & attr) const
{
	return this->attributes[attr];
}
//------------------------------------------------------------------------------
/**
*/
inline 
const bool
EditorBlueprintManager::HasCategory(const Util::String & category) const
{
	return this->templates.Contains(category);
}

//------------------------------------------------------------------------------
/**
*/
inline 
const EditorBlueprintManager::BluePrint& EditorBlueprintManager::GetCategory(const Util::String & category)
{
	return this->bluePrints[category];
}

//------------------------------------------------------------------------------
/**
*/
inline
const bool
EditorBlueprintManager::HasProperty(const Util::String & propertyName) const
{
	return this->properties.Contains(propertyName);
}

//------------------------------------------------------------------------------
/**
*/
inline
const Ptr<Tools::IDLProperty> &
EditorBlueprintManager::GetProperty(const Util::String & propertyName)
{
	return this->properties[propertyName];
}

//------------------------------------------------------------------------------
/**
*/
inline 
Util::Array<Ptr<Tools::IDLProperty>>
EditorBlueprintManager::GetAllProperties()
{
	return this->properties.ValuesAsArray();
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
EditorBlueprintManager::SetUseSDK(bool enable)
{
	this->useSDKDir = enable;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Util::Array<Util::String> &
EditorBlueprintManager::GetCategoryProperties(const Util::String & category) const
{
	return this->categoryProperties[category];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
EditorBlueprintManager::SetLogger(ToolkitUtil::Logger * log)
{
	this->logger = log;
}
}
#include "stdneb.h"

#include "io/ioserver.h"
#include "io/xmlreader.h"
#include "util/guid.h"
#include "attr/attrid.h"
#include "editorblueprintmanager.h"
#include "attr/attributecontainer.h"
#include "managers/factorymanager.h"
#include "idldocument/idldocument.h"
#include "idldocument/idlattributelib.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "projectinfo.h"
#include "system/nebulasettings.h"
#include "io/xmlwriter.h"
#include "logger.h"
#include "game/templateexporter.h"
#include "db/dbfactory.h"

using namespace Util;
using namespace Attr;
using namespace Tools;

namespace Toolkit
{
__ImplementClass(EditorBlueprintManager, 'EBPM', Core::RefCounted);
__ImplementSingleton(EditorBlueprintManager);

//------------------------------------------------------------------------------
/**
*/

EditorBlueprintManager::EditorBlueprintManager() :useSDKDir(false), logger(NULL)
{
	__ConstructSingleton;		
}

//------------------------------------------------------------------------------
/**
*/
EditorBlueprintManager::~EditorBlueprintManager()
{
	__DestructSingleton;		
	this->properties.Clear();
	this->bluePrints.Clear();
	this->systemAttributeDictionary.Clear();
	this->categoryAttributes.Clear();
	this->attributes.Clear();
	this->attributeProperties.Clear();
	this->categoryProperties.Clear();
}


//------------------------------------------------------------------------------
/**
*/
void
EditorBlueprintManager::Open()
{
	String toolkitdir;
	if (System::NebulaSettings::Exists("gscept","ToolkitShared", "path"))
	{
		 toolkitdir = System::NebulaSettings::ReadString("gscept","ToolkitShared", "path");
		if (IO::IoServer::Instance()->FileExists(toolkitdir + "/projectinfo.xml"))
		{
			this->ParseProjectInfo(toolkitdir + "/projectinfo.xml");
		}		
		IO::AssignRegistry::Instance()->SetAssign(IO::Assign("toolkit",toolkitdir));
	}
	String workdir;
	if (System::NebulaSettings::Exists("gscept","ToolkitShared", "workdir"))
	{
		workdir = System::NebulaSettings::ReadString("gscept","ToolkitShared", "workdir");
		if (IO::IoServer::Instance()->FileExists(workdir + "/projectinfo.xml"))
		{
			this->ParseProjectInfo(workdir + "/projectinfo.xml");
		}
	}
	if(this->useSDKDir)
	{
		workdir = toolkitdir;
	}
	String blueprintpath = workdir + "/data/tables/blueprints.xml";
	if (IO::IoServer::Instance()->FileExists(blueprintpath))
	{
		this->ParseBlueprint(blueprintpath);
	}
	this->UpdateAttributeProperties();
	this->blueprintPath = blueprintpath;
	templateDir = workdir + "/data/tables/db";
	this->ParseTemplates(templateDir);
	this->CreateMissingTemplates();
}

//------------------------------------------------------------------------------
/**
*/
void 
EditorBlueprintManager::AddNIDLFile(const IO::URI & uri)
{
	Ptr<Tools::IDLDocument> doc = Tools::IDLDocument::Create();
	// no source file given?
	if (uri.IsEmpty())
	{
		return;
	}

	// parse the source file into a C++ object tree
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(uri);
	stream->SetAccessMode(IO::Stream::ReadAccess);
	if (stream->Open())
	{
		Ptr<IO::XmlReader> xmlReader = IO::XmlReader::Create();
		xmlReader->SetStream(stream);
		if (xmlReader->Open())
		{
			// check if it is a valid IDL file
			if (xmlReader->GetCurrentNodeName() == "Nebula3")
			{				

				if (doc->Parse(xmlReader))
				{
					const Util::Array<Ptr<Tools::IDLAttributeLib>>& libs = doc->GetAttributeLibs();
					IndexT i;
					for(i=0;i<libs.Size();i++)
					{
						const Util::Array<Ptr<Tools::IDLAttribute>>& attrs =  libs[i]->GetAttributes();
						IndexT a;
						for(a=0 ; a < attrs.Size();a++)
						{	
							if(!this->attributes.Contains(attrs[a]->GetName()))
							{								
								this->attributes.Add(attrs[a]->GetName(),attrs[a]);
							}
							this->systemAttributeDictionary.Add(FourCC(attrs[a]->GetFourCC()),attrs[a]->IsSystem());							

							if(!AttributeDefinitionBase::FindByName(attrs[a]->GetName()))
							{
								// attribute is unknown to nebula, add it to the dynamic attributes
								Util::String typestring = attrs[a]->GetType();
								typestring.ToLower();
								AttributeDefinitionBase::RegisterDynamicAttribute(attrs[a]->GetName(),attrs[a]->GetFourCC(),Attribute::StringToValueType(typestring),ReadWrite);
							}							
						}
					}
					const Util::Array<Ptr<Tools::IDLProperty>>& props = doc->GetProperties();
					for(i=0 ; i<props.Size();i++)
					{
						if(!this->properties.Contains(props[i]->GetName()))
						{
							this->properties.Add(props[i]->GetName(),props[i]);
						}
					}
				}
			}			
			xmlReader->Close();
		}		
		stream->Close();
	}
	else
	{
		this->logger->Error("Can't open NIDL file %s\n", uri.AsString().AsCharPtr());
	}
}


//------------------------------------------------------------------------------
/**
*/
void 
EditorBlueprintManager::ParseBlueprint(const IO::URI & filename)
{
	if (IO::IoServer::Instance()->FileExists(filename))
	{        
		Ptr<IO::XmlReader> xmlReader = IO::XmlReader::Create();
		Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(filename);
		stream->SetAccessMode(IO::Stream::ReadAccess);
		xmlReader->SetStream(stream);
		if (xmlReader->Open())
		{
			// make sure it's a BluePrints file
			if (xmlReader->GetCurrentNodeName() != "BluePrints")
			{
				this->logger->Error("FactoryManager::ParseBluePrints(): not a valid blueprints file!");
			}
			if (xmlReader->SetToFirstChild("Entity")) do
			{
				BluePrint bluePrint;
				bluePrint.type = xmlReader->GetString("type");
				bluePrint.cppClass = xmlReader->GetString("cppclass");
				if(xmlReader->HasAttr("desc"))
				{
					bluePrint.description = xmlReader->GetString("desc");
				}
				if (xmlReader->SetToFirstChild("Property")) do
				{
				
					Util::String name = xmlReader->GetString("type");
					if(this->properties.Contains(name))
					{
						bluePrint.properties.Append(this->properties[name]);
					}
					if(xmlReader->HasAttr("createRemote"))
					{
						bool remote = xmlReader->GetBool("createRemote");
						bluePrint.remoteTable.Add(name,remote);
					}
				}
				while (xmlReader->SetToNextChild("Property"));
				this->bluePrints.Add(bluePrint.type,bluePrint);

				this->UpdateCategoryAttributes(bluePrint.type);				
			}
			while (xmlReader->SetToNextChild("Entity"));
			xmlReader->Close();			
		}
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
EditorBlueprintManager::UpdateCategoryAttributes(const Util::String & category)
{
	const BluePrint& bluePrint = this->bluePrints[category];
	/// create an attribute container with all the attributes used by this entity type based
	/// on its properties
	AttributeContainer cont;
	IndexT i;
	/// make sure Id and Name exist
	cont.AddAttr(Attribute(Attr::Id,""));
	cont.AddAttr(Attribute(Attr::Name,""));

	/// create container for property list
	Array<String> propArray;

	for(i=0;i<bluePrint.properties.Size();i++)
	{
		Ptr<IDLProperty> curprop = bluePrint.properties[i];
		propArray.Append(curprop->GetName());
		bool done = false;
		do 
		{						
			const Util::Array<Util::String>& propattrs = curprop->GetAttributes();
			IndexT j;
			for(j=0;j<propattrs.Size();j++)
			{
				if(!cont.HasAttr(AttrId(propattrs[j])))
				{
					/// FUGLY WTH

					Ptr<IDLAttribute> idlattr = this->attributes[propattrs[j]];
					AttrId newid(propattrs[j]);
					switch(newid.GetValueType())
					{
					case IntType:	
						{
							Attribute newAttr(newid,0);
							if(idlattr->HasDefault())
							{
								newAttr.SetValueFromString(idlattr->GetDefault());
							}
							cont.AddAttr(newAttr);
						}									
						break;
					case FloatType:
						{
							Attribute newAttr(newid,0.0f);
							if(idlattr->HasDefault())
							{
								newAttr.SetValueFromString(idlattr->GetDefault());
							}
							cont.AddAttr(newAttr);
						}									
						break;
					case BoolType:
						{
							Attribute newAttr(newid,false);
							if(idlattr->HasDefault())
							{
								newAttr.SetValueFromString(idlattr->GetDefault());
							}
							cont.AddAttr(newAttr);
						}									
						break;
					case Float4Type:
						{
							Attribute newAttr(newid,Math::float4(0,0,0,0));
							if(idlattr->HasDefault())
							{
								newAttr.SetValueFromString(idlattr->GetDefaultRaw());
							}
							cont.AddAttr(newAttr);
						}									
						break;
					case StringType:
						{
							Attribute newAttr(newid,Util::String(""));
							if(idlattr->HasDefault())
							{
								newAttr.SetValueFromString(idlattr->GetDefaultRaw());
							}
							cont.AddAttr(newAttr);
						}									
						break;
					case Matrix44Type:
						{
							Attribute newAttr(newid,Math::matrix44::identity());
							if(idlattr->HasDefault())
							{
								newAttr.SetValueFromString(idlattr->GetDefault());
							}
							cont.AddAttr(newAttr);
						}									
						break;
					case GuidType:
						{
							Attribute newAttr(newid,Util::Guid());
							if(idlattr->HasDefault())
							{
								newAttr.SetValueFromString(idlattr->GetDefault());
							}
							cont.AddAttr(newAttr);
						}									
						break;
					default:
						cont.AddAttr(Attribute(newid));
					}

				}
			}
			if(curprop->GetParentClass().IsEmpty() || curprop->GetParentClass() == "Game::Property" || !this->properties.Contains(curprop->GetParentClass()))
			{
				done = true;
			}
			else
			{
				curprop = this->properties[curprop->GetParentClass()];
			}

		} while (!done);

	}
	IndexT old = this->categoryAttributes.FindIndex(category);
	if(old != InvalidIndex )
	{
		this->categoryAttributes.EraseAtIndex(old);
	}
	this->categoryAttributes.Add(bluePrint.type,cont);	

	old = this->categoryProperties.FindIndex(category);
	if(old != InvalidIndex )
	{
		this->categoryProperties.EraseAtIndex(old);
	}
	this->categoryProperties.Add(bluePrint.type,propArray);	

}

//------------------------------------------------------------------------------
/**
*/
void
EditorBlueprintManager::UpdateAttributeProperties()
{

	// slow 
	const Array<String>& attrs = this->attributes.KeysAsArray();
	const Array<Ptr<IDLProperty>> proparray = properties.ValuesAsArray();		

	for(IndexT i = 0 ; i < attrs.Size();i++)
	{
		String id = attrs[i];
		Array<String> props;
		for(IndexT j = 0; j < proparray.Size(); j++)
		{
			bool done = false;
			Ptr<IDLProperty> curprop = proparray[j];
			do
			{
				IndexT found = curprop->GetAttributes().FindIndex(attrs[i]);
				if(found != InvalidIndex)
				{
					if(props.FindIndex(proparray[j]->GetName()) == InvalidIndex)
					{
						props.Append(proparray[j]->GetName());
					}					
				}
				if(curprop->GetParentClass().IsEmpty() || curprop->GetParentClass() == "Game::Property" || !this->properties.Contains(curprop->GetParentClass()))
				{
					done = true;
				}
				else
				{
					curprop = this->properties[curprop->GetParentClass()];
				}
			}
			while(!done);					
		}
		this->attributeProperties.Add(Attr::AttrId(attrs[i]),props);
	}	
}

//------------------------------------------------------------------------------
/**
*/
Attr::AttributeContainer
EditorBlueprintManager::GetCategoryAttributes(const Util::String & _template)
{
	n_assert2(this->categoryAttributes.Contains(_template), "unknown category");
	return this->categoryAttributes[_template];
}



//------------------------------------------------------------------------------
/**
	Creates empty template entries for all categories so that new templates
	can be added
*/
void 
EditorBlueprintManager::CreateMissingTemplates()
{
	Util::Array<Util::String> cats = this->bluePrints.KeysAsArray();
	IndexT i;
	for(i=0 ; i< cats.Size();i++)
	{
		if(!this->templates.Contains(cats[i]))
		{
			Util::Array<TemplateEntry> emptyTemplates;
			this->templates.Add(cats[i],emptyTemplates);
		}
	}
}


//------------------------------------------------------------------------------
/**
	parse all template files in projects data folder
*/
void 
EditorBlueprintManager::ParseTemplates(const Util::String & folder)
{
	Array<String> templates = IO::IoServer::Instance()->ListFiles(folder, "*.xml", true);
	
	IndexT i;
	for(i = 0 ; i<templates.Size() ; i++)
	{
		if (IO::IoServer::Instance()->FileExists(templates[i]))
		{
			Ptr<IO::XmlReader> xmlReader = IO::XmlReader::Create();
			Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(templates[i]);
			stream->SetAccessMode(IO::Stream::ReadAccess);
			xmlReader->SetStream(stream);
			if (xmlReader->Open())
			{
				String category = xmlReader->GetCurrentNodeName();
				if(this->bluePrints.Contains(category))
				{
					this->templateFiles.Add(category, templates[i]);
					Util::Array<TemplateEntry> newTemplates;
					
					if (xmlReader->SetToFirstChild("Item")) do
					{
						AttributeContainer cont = this->GetCategoryAttributes(category);
						Array<String> attrs = xmlReader->GetAttrs();
						IndexT j;
						for(j=0;j<attrs.Size();j++)
						{
							Util::String attval = xmlReader->GetString(attrs[j].AsCharPtr());
							if (Attr::AttrId::IsValidName(attrs[j]))
							{
								AttrId newId(attrs[j]);
								Attribute newAttr(newId);
								newAttr.SetValueFromString(attval);
								cont.SetAttr(newAttr);
							}
							else
							{
								this->logger->Warning("Unknown attribute %s in template %s\n", attrs[j].AsCharPtr(), templates[i].AsCharPtr());
							}
						}
						TemplateEntry newTemp;						
						newTemp.attrs = cont;
						newTemp.id = cont.GetString(Attr::Id);
						newTemp.category = category;
						newTemplates.Append(newTemp);
					}
					while (xmlReader->SetToNextChild("Item"));
					this->templates.Add(category,newTemplates);
				}
				xmlReader->Close();			
			}
		}

	}
}


//------------------------------------------------------------------------------
/**
	
*/
const Attr::AttributeContainer  &  
EditorBlueprintManager::GetTemplate(const Util::String & category, const Util::String & etemplate) const
{
	const Util::Array<TemplateEntry> & templs = this->templates[category];
	/// FIXME use a dictionary
	for(IndexT i =0;i<templs.Size();i++)
	{
		if(templs[i].id == etemplate)
		{
			return templs[i].attrs;
		}
	}
	this->logger->Error("template not found in category");
	return templs[0].attrs;
}

//------------------------------------------------------------------------------
/**
*/
bool 
EditorBlueprintManager::IsSystemAttribute(Util::FourCC fourCC)
{
	if (!this->systemAttributeDictionary.Contains(fourCC))
	{
		return true;
	}

	return  this->systemAttributeDictionary[fourCC];
}

//------------------------------------------------------------------------------
/**
*/
void 
EditorBlueprintManager::ParseProjectInfo(const Util::String & path)
{
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(IO::URI(path));
	Ptr<IO::XmlReader> xmlReader = IO::XmlReader::Create();
	xmlReader->SetStream(stream);
	if (xmlReader->Open())
	{
		// check if it's a valid project info file
		if (!xmlReader->HasNode("/Nebula3/Project"))
		{
			return;
		}
		xmlReader->SetToNode("/Nebula3/Project");		
		// parse nidl entries if any
		if (xmlReader->SetToFirstChild("NIDL"))
		{
			// load files
			if (xmlReader->SetToFirstChild("File")) do
			{
				Util::String nidlfile = path.ExtractDirName() + "/" + xmlReader->GetString("name");

				this->AddNIDLFile(IO::URI(nidlfile));				
			}
			while(xmlReader->SetToNextChild("File"));
		}
		xmlReader->Close();
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
EditorBlueprintManager::SetCategory(const Util::String & category, const Util::Array<Util::String> newproperties )
{
	
	IndexT old = this->bluePrints.FindIndex(category);
	if(old != InvalidIndex)
	{
		this->bluePrints.EraseAtIndex(old);
	}
	else
	{
		Util::Array<TemplateEntry> emptyTemplate;
		this->templates.Add(category,emptyTemplate);
	}

	BluePrint newPrint;
	newPrint.type = category;
	newPrint.cppClass = "Entity";
	for(IndexT i = 0; i < newproperties.Size();i++)
	{
		newPrint.properties.Append(this->properties[newproperties[i]]);
	}
	this->bluePrints.Add(category,newPrint);
	this->UpdateCategoryAttributes(category);
}

//------------------------------------------------------------------------------
/**
*/
void 
EditorBlueprintManager::RemoveCategory(const Util::String & category)
{
	IndexT old = this->bluePrints.FindIndex(category);
	if(old != InvalidIndex)
	{
		this->bluePrints.EraseAtIndex(old);
	}
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<IDLAttribute>>
EditorBlueprintManager::GetAllAttributes()
{
	return this->attributes.ValuesAsArray();
}
//------------------------------------------------------------------------------
/** EditorBlueprintManager::SaveProjectBlueprint
*/
void
EditorBlueprintManager::SaveProjectBlueprint()
{	
	Ptr<IO::XmlWriter> xmlWriter = IO::XmlWriter::Create();
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(this->blueprintPath);
	stream->SetAccessMode(IO::Stream::WriteAccess);
	xmlWriter->SetStream(stream);
	if (xmlWriter->Open())
	{
		xmlWriter->BeginNode("BluePrints");
		Util::Array<BluePrint> prints = this->bluePrints.ValuesAsArray();
		IndexT i;
		for(i=0 ; i<prints.Size();i++)
		{
			xmlWriter->BeginNode("Entity");
			xmlWriter->SetString("cppclass","Entity");
			xmlWriter->SetString("type",prints[i].type);
			if(!prints[i].description.IsEmpty())
			{
				xmlWriter->SetString("desc",prints[i].description);
			}
			IndexT j;
			for(j=0;j<prints[i].properties.Size();j++)
			{
				xmlWriter->BeginNode("Property");
				xmlWriter->SetString("type",prints[i].properties[j]->GetName());
				
				if(prints[i].remoteTable.Contains(prints[i].properties[j]->GetName()))
				{
					xmlWriter->SetBool("createRemote",prints[i].remoteTable[prints[i].properties[j]->GetName()]);
				}
				xmlWriter->EndNode();
			}
			xmlWriter->EndNode();
		}
		xmlWriter->EndNode();
		xmlWriter->Close();	
	}
}

//------------------------------------------------------------------------------
/** EditorBluePrintManager::GetAttributeProperties
*/
Util::Array<Util::String> 
EditorBlueprintManager::GetAttributeProperties(const Attr::AttrId& id)
{
	return this->attributeProperties[id];
}

//------------------------------------------------------------------------------
/**
*/
void
EditorBlueprintManager::AddTemplate(const Util::String & id, const Util::String & category, const Attr::AttributeContainer & container)
{
	TemplateEntry newTemp;
	newTemp.attrs = container;
	newTemp.id = id;
	newTemp.category = category;	
	this->templates[category].Append(newTemp);
}

//------------------------------------------------------------------------------
/**
*/
void
EditorBlueprintManager::SaveCategoryTemplates(const Util::String & category)
{
	n_assert(this->templates.Contains(category));
	Util::String filename;
	if (!this->templateFiles.Contains(category))
	{
		filename = this->templateDir + "/" + category + ".xml";
		this->templateFiles.Add(category, filename);
	}
	else
	{
		filename = this->templateFiles[category];
	}

	Ptr<IO::XmlWriter> xmlWriter = IO::XmlWriter::Create();
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(filename);
	stream->SetAccessMode(IO::Stream::WriteAccess);
	xmlWriter->SetStream(stream);
	if (xmlWriter->Open())
	{
		xmlWriter->BeginNode(category);
		const Util::Array<TemplateEntry> & entries = this->templates[category];
		for (int i = 0; i < entries.Size(); i++)
		{
			xmlWriter->BeginNode("Item");			
			const Util::Dictionary<AttrId, Attribute>& entryDic = entries[i].attrs.GetAttrs();
			for (int j = 0; j < entryDic.Size(); j++)
			{
				// skip transform
				if (entryDic.KeyAtIndex(j) != Attr::Transform)
				{
					xmlWriter->SetString(entryDic.KeyAtIndex(j).GetName(), entryDic.ValueAtIndex(j).ValueAsString());
				}				
			}
			xmlWriter->SetString("Id", entries[i].id);
			xmlWriter->EndNode();
		}
		xmlWriter->EndNode();
		xmlWriter->Close();
	}	
	ToolkitUtil::Logger logger;
	Ptr<ToolkitUtil::TemplateExporter> exporter = ToolkitUtil::TemplateExporter::Create();
	exporter->SetLogger(&logger);
	exporter->SetDbFactory(Db::DbFactory::Instance());
	exporter->Open();
	exporter->ExportFile(IO::URI(filename));
	exporter->Close();
}

//------------------------------------------------------------------------------
/**
*/
bool
EditorBlueprintManager::HasTemplate(const Util::String & id, const Util::String & category)
{
	const Util::Array<TemplateEntry> & entries = this->templates[category];
	for (int i = 0; i < entries.Size(); i++)
	{
		if (entries[i].id == id)
		{
			return true;
		}
	}
	return false;
}
}
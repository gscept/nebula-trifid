//------------------------------------------------------------------------------
//  level.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "level.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "io/xmlreader.h"
#include "attr/attributecontainer.h"
#include "posteffect/posteffectentity.h"
#include "util/string.h"
#include "leveleditor2entitymanager.h"
#include "io/xmlwriter.h"
#include "managers/entitymanager.h"
#include "properties/editorproperty.h"
#include "entityguid.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "game/levelexporter.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "posteffect/posteffectmanager.h"
#include "posteffect/posteffectentity.h"
#include "properties/graphicsproperty.h"
#include "graphicsfeatureunit.h"
#include "math/polar.h"
#include "leveleditor2app.h"
#include "posteffect/posteffectfeatureunit.h"
#include "posteffect/posteffectparser.h"
#include "posteffect/posteffectexporter.h"
#include "basegamefeature/basegameprotocol.h"

using namespace Attr;
using namespace Util;
using namespace Math;

namespace LevelEditor2
{

__ImplementClass(LevelEditor2::Level, 'LEVL', ToolkitUtil::LevelParser);
__ImplementSingleton(LevelEditor2::Level);


//------------------------------------------------------------------------------
/**
*/
Level::Level(): startLevel(false), autoBatch(true)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
Level::~Level()
{
	__DestructSingleton;
}


//------------------------------------------------------------------------------------
/**
*/
bool
Level::LoadLevel(const Util::String& level)
{
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	Util::String fileName;
	fileName.Format("work:levels/%s.xml",level.AsCharPtr());
	if(!ioServer->FileExists(fileName))
	{
		return false;
	}
	LevelEditor2EntityManager::Instance()->RemoveAllEntities();

	LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->Discard();
	LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->Setup();

	Ptr<IO::Stream> stream = ioServer->CreateStream(fileName);
	stream->SetAccessMode(IO::Stream::ReadAccess);	
	if(!stream->Open())
	{
		return false;
	}

	bool result = false;

	Ptr<IO::XmlReader> reader = IO::XmlReader::Create();
	reader->SetStream(stream);
	if(reader->Open())
	{
		result = this->LoadXmlLevel(reader);		
		reader->Close();
		stream->Close();
	}

	
	if(result)
	{
		// reparent all items
		LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->RebuildTree();
        this->startLevel = false;
        if(LevelEditor2App::Instance()->GetGlobalAttrs()->HasAttr(Attr::_DefaultLevel))
        {
            if(LevelEditor2App::Instance()->GetGlobalAttrs()->GetString(Attr::_DefaultLevel) == this->name )
            {
                this->startLevel = true;
            }
        }
	}
		
	// add wrapper entity for global light
	LevelEditor2EntityManager::Instance()->CreateLightEntity("GlobalLight");
	return result;	
}


//------------------------------------------------------------------------------------
/**
*/
bool
Level::LoadVersionedLevel(const Ptr<IO::XmlReader>& reader)
{
	reader->SetToRoot();

	if (reader->HasNode("/Level") && reader->HasNode("/Level/NebulaLevel"))
	{
		reader->SetToFirstChild();
		
		String levelName = reader->GetString("name");
		String levelId = reader->GetString("id");		
		int levelversion = reader->GetInt("Version");
		if (levelversion < 1)
		{
			n_warning("Unsupport level version, must be very, very old, sorry cant continue");
			return false;
		}
				
		this->name = levelName;

		if (reader->SetToFirstChild("Layers"))
		{
			if (reader->SetToFirstChild("Layer"))
			{
				do
				{
					Util::String name = reader->GetString("name");
					bool autoload= reader->GetBool("autoload");
					bool visible = reader->GetBool("visible");					
					bool locked = reader->GetBool("locked");										
					const Ptr<Layers::LayerHandler>& handler = LevelEditor2App::Instance()->GetWindow()->GetLayerHandler();						
												
					if (!handler->HasLayer(name))
					{
						Ptr<Layers::Layer> layer = Layers::Layer::Create();
						layer->SetName(name);
						layer->SetAutoLoad(autoload);
						layer->SetVisible(visible);
						layer->SetLocked(locked);
						handler->AddLayer(layer);
					}
					else
					{
						Ptr<Layers::Layer> layer = handler->GetLayer(name);
						int row = handler->layerToRow[layer];
						((QPushButton*)(handler->tableWidget->cellWidget(row, 0)))->setChecked(visible);
						((QPushButton*)(handler->tableWidget->cellWidget(row, 1)))->setChecked(autoload);						
						layer->SetAutoLoad(autoload);
						layer->SetVisible(visible);
						layer->SetLocked(locked);

					}
				}while (reader->SetToNextChild());								
			}
			reader->SetToNextChild("Entities");
		}
		else
		{
			reader->SetToFirstChild("Entities");
		}

		bool foundInvalidAttr = false;
		Util::Array<Util::String> invalidAttrs;

		
		reader->SetToFirstChild();
		do 
		{
			Util::String name = reader->GetCurrentNodeName();
			if(name == "Object")
			{
				AttributeContainer entAttrs;
				String category = reader->GetString("category");
				if (reader->SetToFirstChild("Attributes"))
				{
										
					reader->SetToFirstChild();
					do
					{
						String name = reader->GetCurrentNodeName();
						String val;
						if(reader->HasContent())
						{
							val = reader->GetContent();
						}						
						
						Attr::AttrId id(Attr::AttributeDefinitionBase::FindByName(name));

						if (!id.IsValid())
						{
							foundInvalidAttr = true;

							if (InvalidIndex == invalidAttrs.FindIndex(name))	invalidAttrs.Append(name);

							continue;
						}

						switch(id.GetValueType())
						{
						case IntType:
							entAttrs.SetAttr(Attribute(id,val.AsInt()));
							break;
						case FloatType:
							entAttrs.SetAttr(Attribute(id,val.AsFloat()));
							break;
						case BoolType:
							entAttrs.SetAttr(Attribute(id,val.AsBool()));
							break;
						case Float4Type:
							entAttrs.SetAttr(Attribute(id,val.AsFloat4()));
							break;
						case StringType:
							entAttrs.SetAttr(Attribute(id,val));
							break;
						case Matrix44Type:
							entAttrs.SetAttr(Attribute(id,val.AsMatrix44()));
							break;
						case GuidType:
							entAttrs.SetAttr(Attribute(id,Util::Guid::FromString(val)));
							break;
                        case BlobType:
                            entAttrs.SetAttr(Attribute(id,val.AsBlob()));
                            break;
						default:
							break;
						}						
					}
					while(reader->SetToNextChild());				
					reader->SetToParent();
				}
				LevelEditor2EntityManager::Instance()->CreateEntityFromAttrContainer(category,entAttrs);
			}
		}while(reader->SetToNextChild("Object"));

		if (foundInvalidAttr)
		{
			// throw an error message telling which attributes that are missing
			Util::String errorMessage;
			errorMessage.Format("\n\nInvalid attributes (have they been removed since last level save?) has been found in level '%s':\n\n", this->name.AsCharPtr());

			for (IndexT i = 0; i < invalidAttrs.Size(); i++)
			{
				errorMessage.Append("\t" + invalidAttrs[i] + "\n");
			}

			n_warning(errorMessage.AsCharPtr());
		}
				
		if(reader->SetToNextChild("Global"))
		{
			if (levelversion == 1)
			{
				if (reader->SetToFirstChild("Object"))
				{
					if (reader->HasAttr("type") && reader->GetString("type") == "PostEffect")
					{
						reader->SetToFirstChild("Attributes");
						const Ptr<PostEffect::PostEffectEntity>& peEntity = PostEffect::PostEffectFeatureUnit::Instance()->GetDefaultPostEffect();

						if (reader->SetToFirstChild("")) do
						{
							this->ReadPostEffectAttribute(reader->GetCurrentNodeName(), reader->GetContent(), peEntity);
						} while (reader->SetToNextChild(""));
						peEntity->SetDirty(true);
						// create a new preset from the settings
						PostEffect::PostEffectEntity::ParamSet params;
						params = PostEffect::PostEffectFeatureUnit::Instance()->GetDefaultPostEffect()->Params();
						params.common->SetName(levelName);
						params.common->SetBlendTime(2.0f);
						PostEffect::PostEffectRegistry::Instance()->SetPreset(levelName, params);
						PostEffect::PostEffectFeatureUnit::Instance()->ApplyPreset(levelName);
						ToolkitUtil::PostEffectParser::Save("root:data/tables/posteffect/" + levelName + ".xml", params);	
						LevelEditor2App::Instance()->GetWindow()->GetPostEffectController()->LoadPresets();
						LevelEditor2App::Instance()->GetWindow()->GetPostEffectController()->ActivatePrefix(levelName);
					}
				}
			}
			else if (levelversion == 2)
			{
				Util::String preset = reader->GetOptString("PostEffectPreset", "Default");				
				if (PostEffect::PostEffectRegistry::Instance()->HasPreset(preset))
				{
					LevelEditor2App::Instance()->GetWindow()->GetPostEffectController()->ActivatePrefix(preset);					
				}
				else
				{
					Util::String msg;
					msg.Format("Unkown posteffect preset %s in level, missing file from data/tables/posteffect ?",preset.AsCharPtr());
					n_warning(msg.AsCharPtr());
					LevelEditor2App::Instance()->GetWindow()->GetPostEffectController()->ActivatePrefix("Default");					
				}
				if (reader->HasAttr("GlobalLightTransform"))
				{
					Util::String transString = reader->GetString("GlobalLightTransform");
					Math::matrix44 trans = transString.AsMatrix44();
					Ptr<Game::Entity> light = LevelEditor2EntityManager::Instance()->GetGlobalLight();
					Ptr<BaseGameFeature::UpdateTransform> update = BaseGameFeature::UpdateTransform::Create();
					update->SetMatrix(trans);
					__SendSync(light, update);
				}				
			}			
		}
        this->startLevel = false;
        if(LevelEditor2App::Instance()->GetGlobalAttrs()->HasAttr(Attr::_DefaultLevel))
        {
            if(LevelEditor2App::Instance()->GetGlobalAttrs()->GetString(Attr::_DefaultLevel) == this->name )
            {
                this->startLevel = true;
            }
        }
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::SaveLevelAs(const Util::String& newName)
{
	this->name = newName;
	this->SaveLevel();
}

//------------------------------------------------------------------------------
/**
*/
Util::String
Level::AllocateID(const Util::String& objType, const Util::String&category, const Util::String &name)
{
	
	Util::String base = Level::Instance()->GetName() + "|" + objType + "|";
	if(!category.IsEmpty())
	{
		base += category + "|";
	}
	base += name;
	int count = 0;
	if(this->objectCounters.Contains(base))
	{
		count = ++this->objectCounters[base];
	}
	else
	{
		this->objectCounters.Add(base,0);
	}
	
	base += "_" + Util::String::FromInt(count);
	return base;
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::WriteString(const Ptr<IO::XmlWriter> & xmlWriter, const Util::String & node, const Util::String& content )
{
	xmlWriter->BeginNode(node);
	xmlWriter->WriteContent(content);
	xmlWriter->EndNode();
}

//------------------------------------------------------------------------------
/**
	returns a sorted version of the entity array, kind of slow
	sortby has to be global unique in the entitymanager
*/
static Array<Ptr<Game::Entity>> SortEntityArray(const Array<Ptr<Game::Entity>> & entities, const Attr::GuidAttrId & sortBy)
{
	Array<Util::Guid> toSort;
	for(IndexT i = 0 ; i < entities.Size() ; i++)
	{
		toSort.Append(entities[i]->GetGuid(sortBy));
	}
	toSort.Sort();
	Array<Ptr<Game::Entity>> sorted;
	for(IndexT i = 0 ; i < toSort.Size() ; i++)
	{
		Ptr<Game::Entity> ent = BaseGameFeature::EntityManager::Instance()->GetEntityByAttr(Attribute(sortBy,toSort[i]));
		sorted.Append(ent);
	}
	return sorted;
}

//------------------------------------------------------------------------------
/**
*/
Math::bbox
Level::GetBoundingBox()
{
	Math::bbox box;
	box.set(Math::point(0,0,0),Math::vector(1,1,1));
	Util::Array<Ptr<Game::Entity>> ents = BaseGameFeature::EntityManager::Instance()->GetEntities();
	for(IndexT i = 0 ; i < ents.Size() ; i++)
	{
		if(ents[i]->HasAttr(Attr::EntityType))
		{
			Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
			ents[i]->SendSync(msg.cast<Messaging::Message>());
			const Ptr<Graphics::ModelEntity>& model = msg->GetEntity();
            if(model.isvalid())
            {
                box.extend(model->GetGlobalBoundingBox());			
            }            
		}
	}
	return box;
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::SaveLevel()
{
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	// make sure the levels directory exists
	ioServer->CreateDirectory("work:levels");

	// build a filename for the level file
	Util::String fileName;
	fileName.Format("work:levels/%s.xml",this->name.AsCharPtr());

	Ptr<IO::XmlWriter> xmlWriter = IO::XmlWriter::Create();
	Ptr<IO::Stream> stream;

	stream = ioServer->CreateStream(IO::URI(fileName));
	stream->SetAccessMode(IO::Stream::WriteAccess);
	stream->Open();

	if (!stream->IsOpen())
	{
		return;
	}	

	Math::bbox box;
	box.set(Math::point(0,0,0),Math::vector(10,10,10));

	this->objectCounters.Clear();

	xmlWriter->SetStream(stream);
	xmlWriter->Open();
	xmlWriter->BeginNode("Level"); //Root node
	xmlWriter->BeginNode("NebulaLevel"); //Nebula level node
	xmlWriter->SetInt("Version",2);
	xmlWriter->SetString("name", this->name);
	xmlWriter->SetString("id", this->name);

	// layers
	xmlWriter->BeginNode("Layers");
	Util::Array<Ptr<Layers::Layer>> layerarray = LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->GetLayers();
	for (int i = 0; i < layerarray.Size(); i++)
	{
		xmlWriter->BeginNode("Layer");
		xmlWriter->SetString("name", layerarray[i]->GetName());
		xmlWriter->SetBool("autoload", layerarray[i]->GetAutoLoad());
		xmlWriter->SetBool("visible", layerarray[i]->GetVisible());
		xmlWriter->SetBool("locked", layerarray[i]->GetLocked());
		xmlWriter->EndNode();
	}
	xmlWriter->EndNode();
	
	xmlWriter->BeginNode("Entities"); //Entities node	
	
	// get environment entities and sort the stuff
	
	Array<Ptr<Game::Entity>> envEntities = SortEntityArray(BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Environment)),Attr::EntityGuid);
	for(int i=0;i<envEntities.Size();i++)
	{
		xmlWriter->BeginNode("Object"); 
		xmlWriter->SetString("category","_Environment");		
		xmlWriter->BeginNode("Attributes");
		// get graphics
		String gfx = envEntities[i]->GetString(Attr::Graphics);
		this->WriteString(xmlWriter, "_ID", this->AllocateID("environment","",gfx.ExtractFileName()));		
		this->WriteString(xmlWriter, "Id",envEntities[i]->GetString(Attr::Id));		
		this->WriteString(xmlWriter, "_Level", this->name.AsCharPtr());
		this->WriteString(xmlWriter, "_Layers", envEntities[i]->GetString(Attr::_Layers));
		this->WriteString(xmlWriter, "Guid",envEntities[i]->GetGuid(Attr::EntityGuid).AsString());				
		this->WriteString(xmlWriter, "ParentGuid",envEntities[i]->GetGuid(Attr::ParentGuid).AsString());		
		this->WriteString(xmlWriter, "Transform",Util::String::FromMatrix44(envEntities[i]->GetMatrix44(Attr::Transform)));
		this->WriteString(xmlWriter, "CastShadows", Util::String::FromBool(envEntities[i]->GetBool(Attr::CastShadows)));
		this->WriteString(xmlWriter, "PhysicMaterial", envEntities[i]->GetString(Attr::PhysicMaterial));
		this->WriteString(xmlWriter, "Graphics",gfx);
		this->WriteString(xmlWriter, "StartAnimation", envEntities[i]->GetString(Attr::StartAnimation));
		this->WriteString(xmlWriter, "LoadSynced", Util::String::FromBool(envEntities[i]->GetBool(Attr::LoadSynced)));
		this->WriteString(xmlWriter, "CollisionEnabled",Util::String::FromBool(envEntities[i]->GetBool(Attr::CollisionEnabled)));
		this->WriteString(xmlWriter, "DynamicObject",Util::String::FromBool(envEntities[i]->GetBool(Attr::DynamicObject)));
		this->WriteString(xmlWriter, "VelocityVector",Util::String::FromFloat4(envEntities[i]->GetFloat4(Attr::VelocityVector)));
		this->WriteString(xmlWriter, "Instanced",Util::String::FromBool(envEntities[i]->GetBool(Attr::Instanced)));
		xmlWriter->EndNode();
		xmlWriter->EndNode();					

		Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
		envEntities[i]->SendSync(msg.cast<Messaging::Message>());
		const Ptr<Graphics::ModelEntity> & model = msg->GetEntity();
        box.extend(model->GetGlobalBoundingBox());
	}

	// real game entities
	Array<Ptr<Game::Entity>> gameEntities = SortEntityArray(BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Game)),Attr::EntityGuid);
	for(int i=0;i<gameEntities.Size();i++)
	{
		xmlWriter->BeginNode("Object"); 
		xmlWriter->SetString("category",gameEntities[i]->GetString(Attr::EntityCategory));		
		xmlWriter->BeginNode("Attributes");

		this->WriteString(xmlWriter,"_ID",this->AllocateID("object",gameEntities[i]->GetString(Attr::EntityCategory),gameEntities[i]->GetString(Attr::Id)));
		this->WriteString(xmlWriter,"Id",gameEntities[i]->GetString(Attr::Id));
		this->WriteString(xmlWriter, "_Level", this->name.AsCharPtr());
		this->WriteString(xmlWriter, "_Layers", gameEntities[i]->GetString(Attr::_Layers));
		this->WriteString(xmlWriter,"Guid",gameEntities[i]->GetGuid(Attr::EntityGuid).AsString());
		this->WriteString(xmlWriter,"ParentGuid",gameEntities[i]->GetGuid(Attr::ParentGuid).AsString());		
		this->WriteString(xmlWriter,"Transform",Util::String::FromMatrix44(gameEntities[i]->GetMatrix44(Attr::Transform)));		

		Ptr<GetEntityValues> gmsg = GetEntityValues::Create();
		gameEntities[i]->SendSync(gmsg.cast<Messaging::Message>());
		const Array<Attribute>& attrs = gmsg->GetAttrs().GetAttrs().ValuesAsArray();
		for(IndexT idx = 0;idx<attrs.Size();idx++)
		{
			if (attrs[idx].GetAttrId() != Attr::Guid && attrs[idx].GetAttrId() != Attr::_Level && attrs[idx].GetAttrId() != Attr::_ID && attrs[idx].GetAttrId() != Attr::Id)
			{
				this->WriteString(xmlWriter,attrs[idx].GetName(), attrs[idx].ValueAsString());
			}			
		}
				
		Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
		gameEntities[i]->SendSync(msg.cast<Messaging::Message>());
		const Ptr<Graphics::ModelEntity> & model = msg->GetEntity();
        box.extend(model->GetGlobalBoundingBox());

		xmlWriter->EndNode();
		xmlWriter->EndNode();					
	}
	/// lights
	Array<Ptr<Game::Entity>> lightEntities = SortEntityArray(BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Light)),Attr::EntityGuid);
	for(int i=0;i<lightEntities.Size();i++)
	{	
		// ignore the global light, it gets handled by posteffect
		if(lightEntities[i]->GetInt(Attr::LightType) == Lighting::LightType::Global)
		{
			continue;
		}
		xmlWriter->BeginNode("Object"); 
		xmlWriter->SetString("category","Light");		
		xmlWriter->BeginNode("Attributes");
		
		this->WriteString(xmlWriter,"_ID",this->AllocateID("light",Util::String::FromInt(lightEntities[i]->GetInt(Attr::LightType)),lightEntities[i]->GetString(Attr::Id)));	
		this->WriteString(xmlWriter,"Id",lightEntities[i]->GetString(Attr::Id));
		this->WriteString(xmlWriter, "_Level", this->name.AsCharPtr());
		this->WriteString(xmlWriter, "_Layers", lightEntities[i]->GetString(Attr::_Layers));
		this->WriteString(xmlWriter,"Guid",lightEntities[i]->GetGuid(Attr::EntityGuid).AsString());
		this->WriteString(xmlWriter,"ParentGuid",lightEntities[i]->GetGuid(Attr::ParentGuid).AsString());		
		this->WriteString(xmlWriter,"Transform",Util::String::FromMatrix44(lightEntities[i]->GetMatrix44(Attr::Transform)));		

		this->WriteString(xmlWriter,"LightType",lightEntities[i]->GetAttr(Attr::LightType).ValueAsString());
		this->WriteString(xmlWriter,"LightColor",lightEntities[i]->GetAttr(Attr::LightColor).ValueAsString());
		this->WriteString(xmlWriter,"LightOppositeColor",lightEntities[i]->GetAttr(Attr::LightOppositeColor).ValueAsString());
		this->WriteString(xmlWriter,"LightIntensity",lightEntities[i]->GetAttr(Attr::LightIntensity).ValueAsString());
		this->WriteString(xmlWriter,"LightRange",lightEntities[i]->GetAttr(Attr::LightRange).ValueAsString());
		this->WriteString(xmlWriter,"LightAmbient",lightEntities[i]->GetAttr(Attr::LightAmbient).ValueAsString());
		this->WriteString(xmlWriter,"LightCastShadows",lightEntities[i]->GetAttr(Attr::LightCastShadows).ValueAsString());
		this->WriteString(xmlWriter,"LightFlickerEnable",lightEntities[i]->GetAttr(Attr::LightFlickerEnable).ValueAsString());
		this->WriteString(xmlWriter,"LightFlickerFrequency",lightEntities[i]->GetAttr(Attr::LightFlickerFrequency).ValueAsString());
		this->WriteString(xmlWriter,"LightFlickerIntensity",lightEntities[i]->GetAttr(Attr::LightFlickerIntensity).ValueAsString());
		this->WriteString(xmlWriter,"LightFlickerPosition",lightEntities[i]->GetAttr(Attr::LightFlickerPosition).ValueAsString());
		this->WriteString(xmlWriter,"LightConeAngle",lightEntities[i]->GetAttr(Attr::LightConeAngle).ValueAsString());
		this->WriteString(xmlWriter,"LightVolumetric",lightEntities[i]->GetAttr(Attr::LightVolumetric).ValueAsString());
		this->WriteString(xmlWriter,"LightVolumetricIntensity",lightEntities[i]->GetAttr(Attr::LightVolumetricIntensity).ValueAsString());
		this->WriteString(xmlWriter,"LightVolumetricScale",lightEntities[i]->GetAttr(Attr::LightVolumetricScale).ValueAsString());
		this->WriteString(xmlWriter,"LightProjectionMap",lightEntities[i]->GetAttr(Attr::LightProjectionMap).ValueAsString());

		xmlWriter->EndNode();
		xmlWriter->EndNode();	
	}


	Array<Ptr<Game::Entity>> groupEntities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Group));
	for(int i=0;i<groupEntities.Size();i++)
	{
		xmlWriter->BeginNode("Object"); 		
		xmlWriter->SetString("category","_Group");
		xmlWriter->BeginNode("Attributes");	
		this->WriteString(xmlWriter,"Id",groupEntities[i]->GetString(Attr::Id));
		this->WriteString(xmlWriter,"Guid",groupEntities[i]->GetGuid(Attr::EntityGuid).AsString());				
		this->WriteString(xmlWriter,"ParentGuid",groupEntities[i]->GetGuid(Attr::ParentGuid).AsString());		
		this->WriteString(xmlWriter,"Transform",groupEntities[i]->GetAttr(Attr::Transform).ValueAsString());		
		xmlWriter->EndNode();
		xmlWriter->EndNode();							
	}
    Array<Ptr<Game::Entity>> navMeshes = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,NavMesh));
	for(int i=0;i<navMeshes.Size();i++)
    {
        xmlWriter->BeginNode("Object"); 		
        xmlWriter->SetString("category","NavMeshData");
        xmlWriter->BeginNode("Attributes");	
		this->WriteString(xmlWriter, "_ID", this->AllocateID("NavMeshData", "", navMeshes[i]->GetString(Attr::Id)));
        this->WriteString(xmlWriter,"Id",navMeshes[i]->GetString(Attr::Id));
        this->WriteString(xmlWriter,"Guid",navMeshes[i]->GetGuid(Attr::EntityGuid).AsString());				
		this->WriteString(xmlWriter, "_Level", this->name.AsCharPtr());
        this->WriteString(xmlWriter,"ParentGuid",navMeshes[i]->GetGuid(Attr::ParentGuid).AsString());		
        this->WriteString(xmlWriter,"MaxEdgeLength",navMeshes[i]->GetAttr(Attr::MaxEdgeLength).ValueAsString());
        this->WriteString(xmlWriter,"RegionMinSize",navMeshes[i]->GetAttr(Attr::RegionMinSize).ValueAsString());
        this->WriteString(xmlWriter,"RegionMergeSize",navMeshes[i]->GetAttr(Attr::RegionMergeSize).ValueAsString());
        this->WriteString(xmlWriter,"AgentRadius",navMeshes[i]->GetAttr(Attr::AgentRadius).ValueAsString());
        this->WriteString(xmlWriter,"AgentHeight",navMeshes[i]->GetAttr(Attr::AgentHeight).ValueAsString());
        this->WriteString(xmlWriter,"AgentMaxClimb",navMeshes[i]->GetAttr(Attr::AgentMaxClimb).ValueAsString());
        this->WriteString(xmlWriter,"MaxEdgeError",navMeshes[i]->GetAttr(Attr::MaxEdgeError).ValueAsString());
        this->WriteString(xmlWriter,"DetailSampleDist",navMeshes[i]->GetAttr(Attr::DetailSampleDist).ValueAsString());
        this->WriteString(xmlWriter,"DetailSampleMaxError",navMeshes[i]->GetAttr(Attr::DetailSampleMaxError).ValueAsString());
        this->WriteString(xmlWriter,"MaxSlope",navMeshes[i]->GetAttr(Attr::MaxSlope).ValueAsString());
        this->WriteString(xmlWriter,"CellHeight",navMeshes[i]->GetAttr(Attr::CellHeight).ValueAsString());
        this->WriteString(xmlWriter,"CellSize",navMeshes[i]->GetAttr(Attr::CellSize).ValueAsString());
        this->WriteString(xmlWriter,"NavMeshData",navMeshes[i]->GetString(Attr::NavMeshData));		
		this->WriteString(xmlWriter,"EntityReferences",navMeshes[i]->GetAttr(Attr::EntityReferences).ValueAsString()); 
		this->WriteString(xmlWriter,"AreaEntityReferences", navMeshes[i]->GetAttr(Attr::AreaEntityReferences).ValueAsString());
		this->WriteString(xmlWriter,"NavMeshCenter",navMeshes[i]->GetAttr(Attr::NavMeshCenter).ValueAsString()); 
		this->WriteString(xmlWriter,"NavMeshExtends",navMeshes[i]->GetAttr(Attr::NavMeshExtends).ValueAsString()); 
		this->WriteString(xmlWriter,"NavMeshMeshString",navMeshes[i]->GetString(Attr::NavMeshMeshString));
        xmlWriter->EndNode();
        xmlWriter->EndNode();	
        Ptr<SaveNavMesh> msg = SaveNavMesh::Create();
        navMeshes[i]->SendSync(msg.cast<Messaging::Message>());
    }
    Array<Ptr<Game::Entity>> navAreas = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,NavMeshArea));
    for(int i=0;i<navAreas.Size();i++)
    {
        const Ptr<Game::Entity>& entity = navAreas[i];
        xmlWriter->BeginNode("Object"); 		
            xmlWriter->SetString("category","_NavigationArea");
            xmlWriter->BeginNode("Attributes");	
                this->WriteString(xmlWriter, "_ID", this->AllocateID("EditorNavAreaMarker", "", entity->GetString(Attr::Id)));
                this->WriteString(xmlWriter, "Id",entity->GetString(Attr::Id));
                this->WriteString(xmlWriter, "Guid",entity->GetGuid(Attr::EntityGuid).AsString());				
                this->WriteString(xmlWriter, "_Level", this->name.AsCharPtr());
				this->WriteString(xmlWriter, "_Layers", entity->GetString(Attr::_Layers));
                this->WriteString(xmlWriter, "Graphics",entity->GetString(Attr::Graphics));
                this->WriteString(xmlWriter, "ParentGuid",entity->GetGuid(Attr::ParentGuid).AsString());		
                this->WriteString(xmlWriter, "Transform", Util::String::FromMatrix44(entity->GetMatrix44(Attr::Transform)));
                this->WriteString(xmlWriter, "NavMeshAreaFlags", Util::String::FromInt(entity->GetInt(Attr::NavMeshAreaFlags)));
				this->WriteString(xmlWriter, "NavMeshAreaCost", Util::String::FromInt(entity->GetInt(Attr::NavMeshAreaCost)));
				this->WriteString(xmlWriter, "NavMeshMeshString", entity->GetString(Attr::NavMeshMeshString));
            xmlWriter->EndNode();
        xmlWriter->EndNode();	
    }
	Array<Ptr<Game::Entity>> lightProbes = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType, Probe));
	for (int i = 0; i < lightProbes.Size(); i++)
	{
		const Ptr<Game::Entity>& entity = lightProbes[i];
		xmlWriter->BeginNode("Object");
			xmlWriter->SetString("category", "LightProbe");
			xmlWriter->BeginNode("Attributes");
			this->WriteString(xmlWriter, "Id", entity->GetString(Attr::Id));
			this->WriteString(xmlWriter, "_ID", this->AllocateID("light", "lightprobe", entity->GetString(Attr::Id)));
			this->WriteString(xmlWriter, "Guid", entity->GetGuid(Attr::EntityGuid).AsString());
			this->WriteString(xmlWriter, "_Level", this->name.AsCharPtr());
			this->WriteString(xmlWriter, "_Layers", entity->GetString(Attr::_Layers));
			this->WriteString(xmlWriter, "ParentGuid", entity->GetGuid(Attr::ParentGuid).AsString());
			this->WriteString(xmlWriter, "Transform", Util::String::FromMatrix44(entity->GetMatrix44(Attr::Transform)));
			this->WriteString(xmlWriter, "ProbeReflectionMap", entity->GetString(Attr::ProbeReflectionMap));
			this->WriteString(xmlWriter, "ProbeIrradianceMap", entity->GetString(Attr::ProbeIrradianceMap));
			this->WriteString(xmlWriter, "ProbeInfluenceShapeType", entity->GetString(Attr::ProbeInfluenceShapeType));
			this->WriteString(xmlWriter, "ProbeInfluenceLayer", Util::String::FromInt(entity->GetInt(Attr::ProbeInfluenceLayer)));
			this->WriteString(xmlWriter, "ProbeInfluenceFalloff", Util::String::FromFloat(entity->GetFloat(Attr::ProbeInfluenceFalloff)));
			this->WriteString(xmlWriter, "ProbeInfluencePower", Util::String::FromFloat(entity->GetFloat(Attr::ProbeInfluencePower)));
			this->WriteString(xmlWriter, "ProbeBBExtents", Util::String::FromFloat4(entity->GetFloat4(Attr::ProbeBBExtents)));
			this->WriteString(xmlWriter, "ProbeParallaxCorrected", Util::String::FromBool(entity->GetBool(Attr::ProbeParallaxCorrected)));

			this->WriteString(xmlWriter, "ProbeResolutionWidth", Util::String::FromInt(entity->GetInt(Attr::ProbeResolutionWidth)));
			this->WriteString(xmlWriter, "ProbeResolutionHeight", Util::String::FromInt(entity->GetInt(Attr::ProbeResolutionHeight)));
			this->WriteString(xmlWriter, "ProbeName", entity->GetString(Attr::ProbeName));
			this->WriteString(xmlWriter, "ProbeBuildIrradiance", Util::String::FromBool(entity->GetBool(Attr::ProbeBuildIrradiance)));
			this->WriteString(xmlWriter, "ProbeBuildReflections", Util::String::FromBool(entity->GetBool(Attr::ProbeBuildReflections)));
			this->WriteString(xmlWriter, "ProbeGenerateMipmaps", Util::String::FromBool(entity->GetBool(Attr::ProbeGenerateMipmaps)));
			this->WriteString(xmlWriter, "ProbeAutoAssignMaps", Util::String::FromBool(entity->GetBool(Attr::ProbeAutoAssignMaps)));
			this->WriteString(xmlWriter, "ProbeOutputFolder", entity->GetString(Attr::ProbeOutputFolder));
			this->WriteString(xmlWriter, "ProbeOutputFilename", entity->GetString(Attr::ProbeOutputFilename));
			xmlWriter->EndNode();
		xmlWriter->EndNode();
	}

	xmlWriter->EndNode();	
	
	// global stuff
	xmlWriter->BeginNode("Global");
	xmlWriter->SetFloat4("WorldCenter", box.center());
	xmlWriter->SetFloat4("WorldExtents", box.size());
	xmlWriter->SetString("PostEffectPreset", LevelEditor2App::Instance()->GetWindow()->GetPostEffectController()->GetPreset());
	PostEffect::PostEffectEntity::ParamSet parms = PostEffect::PostEffectManager::Instance()->GetDefaultEntity()->Params();

	const Ptr<Layers::LayerHandler> layerHandler = LevelEditor2App::Instance()->GetWindow()->GetLayerHandler();
	Util::Array<Ptr<Layers::Layer>> layers = layerHandler->layers.ValuesAsArray();
	Util::String layerstring;
	for (int l = 0; l < layers.Size(); l++)
	{
		if (layers[l]->GetAutoLoad())
		{
			if (layerstring.Length())
			{
				layerstring += ";";
			}
			layerstring += layers[l]->GetName();
		}
	}
	xmlWriter->SetString("_Layers", layerstring);
	xmlWriter->SetString("GlobalLightTransform", Util::String::FromMatrix44(parms.light->GetLightTransform()));
	xmlWriter->EndNode();

	xmlWriter->EndNode();
	xmlWriter->EndNode();
	xmlWriter->Close();
	stream->Flush();
	stream->Close();

    // set startuplevel information
    if(this->startLevel)
    {
        LevelEditor2App::Instance()->GetGlobalAttrs()->SetString(Attr::_DefaultLevel,this->name);
        LevelEditor2App::Instance()->GetGlobalAttrs()->Save();
    }

	if(this->autoBatch)
	{
		this->ExportLevel(fileName);
	}
}

//------------------------------------------------------------------------------
/** Level::ExportLevel
*/
void
Level::ExportLevel(const Util::String& fileName)
{
    //FIXME
    LevelEditor2App::Instance()->GetWindow()->OnBatchGame();
#if 0
	Ptr<ToolkitUtil::LevelExporter> exporter = ToolkitUtil::LevelExporter::Create();
	exporter->SetLogger(LevelEditor2App::Instance()->GetLogger());
	exporter->Open();
	exporter->ExportFile(IO::URI(fileName));
	exporter->Close();
	Ptr<ToolkitUtil::PostEffectExporter> pfx = ToolkitUtil::PostEffectExporter::Create();
	pfx->SetLogger(LevelEditor2App::Instance()->GetLogger());
	pfx->Open();
	pfx->ExportAll();
	pfx->Close();
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::ReadPostEffectAttribute( const Util::String& attrName, const Util::String& content, const Ptr<PostEffect::PostEffectEntity>& postEffectEntity )
{
    if      (attrName == "GlobalLightTransform")        postEffectEntity->Params().light->SetLightTransform(content.AsMatrix44());
    else if (attrName == "GlobalLightAmbient")          postEffectEntity->Params().light->SetLightAmbientColor(content.AsFloat4());
    else if (attrName == "GlobalLightIntensity")        postEffectEntity->Params().light->SetLightIntensity(content.AsFloat());
    else if (attrName == "GlobalLightDiffuse")          postEffectEntity->Params().light->SetLightColor(content.AsFloat4());
    else if (attrName == "GlobalLightOpposite")         postEffectEntity->Params().light->SetLightOppositeColor(content.AsFloat4());
    else if (attrName == "GlobalLightBacklightFactor")  postEffectEntity->Params().light->SetBackLightFactor(content.AsFloat());
    else if (attrName == "GlobalLightCastShadows")      postEffectEntity->Params().light->SetLightCastShadows(content.AsBool());
    else if (attrName == "GlobalLightShadowIntensity")  postEffectEntity->Params().light->SetLightShadowIntensity(content.AsFloat());
    else if (attrName == "Saturation")                  postEffectEntity->Params().color->SetColorSaturation(content.AsFloat());
    else if (attrName == "Balance")                     postEffectEntity->Params().color->SetColorBalance(content.AsFloat4());
    else if (attrName == "MaxLuminance")                postEffectEntity->Params().color->SetColorMaxLuminance(content.AsFloat());
    else if (attrName == "FocusDistance")               postEffectEntity->Params().dof->SetFocusDistance(content.AsFloat());
    else if (attrName == "FocusLength")                 postEffectEntity->Params().dof->SetFocusLength(content.AsFloat());
    else if (attrName == "FocusRadius")                 postEffectEntity->Params().dof->SetFilterSize(content.AsFloat());
    else if (attrName == "FogColor")                    postEffectEntity->Params().fog->SetFogColorAndIntensity(content.AsFloat4());
    else if (attrName == "FogNearDist")                 postEffectEntity->Params().fog->SetFogNearDistance(content.AsFloat());
    else if (attrName == "FogFarDist")                  postEffectEntity->Params().fog->SetFogFarDistance(content.AsFloat());
    else if (attrName == "FogHeight")                   postEffectEntity->Params().fog->SetFogHeight(content.AsFloat());
    else if (attrName == "BloomColor")                  postEffectEntity->Params().hdr->SetHdrBloomColor(content.AsFloat4());
    else if (attrName == "BloomThreshold")              postEffectEntity->Params().hdr->SetHdrBloomThreshold(content.AsFloat());
    else if (attrName == "BloomScale")                  postEffectEntity->Params().hdr->SetHdrBloomIntensity(content.AsFloat());
    else if (attrName == "SkyTexture")                  postEffectEntity->Params().sky->SetSkyTexturePath(content);
    else if (attrName == "SkyContrast")                 postEffectEntity->Params().sky->SetSkyContrast(content.AsFloat());
    else if (attrName == "SkyBrightness")               postEffectEntity->Params().sky->SetSkyBrightness(content.AsFloat());
    else if (attrName == "SkyRotationFactor")           postEffectEntity->Params().sky->SetSkyRotationFactor(content.AsFloat());
    else if (attrName == "AOStrength")                  postEffectEntity->Params().ao->SetStrength(content.AsFloat());
    else if (attrName == "AORadius")                    postEffectEntity->Params().ao->SetRadius(content.AsFloat());
    else if (attrName == "AOPower")                     postEffectEntity->Params().ao->SetPower(content.AsFloat());
    else if (attrName == "AOAngleBias")                 postEffectEntity->Params().ao->SetAngleBias(content.AsFloat());
    else
    {
        n_printf("Unknown attribute name '%s'", attrName.AsCharPtr());
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::SetName(const Util::String & name)
{
    this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::AddLayer(const Util::String & name, bool visible, bool autoload, bool locked)
{
    const Ptr<Layers::LayerHandler>& handler = LevelEditor2App::Instance()->GetWindow()->GetLayerHandler();						

    if (!handler->HasLayer(name))
    {
        Ptr<Layers::Layer> layer = Layers::Layer::Create();
        layer->SetName(name);
        layer->SetAutoLoad(autoload);
        layer->SetVisible(visible);
        layer->SetLocked(locked);
        handler->AddLayer(layer);
    }
    else
    {
        Ptr<Layers::Layer> layer = handler->GetLayer(name);
        int row = handler->layerToRow[layer];
        ((QPushButton*)(handler->tableWidget->cellWidget(row, 0)))->setChecked(visible);
        ((QPushButton*)(handler->tableWidget->cellWidget(row, 1)))->setChecked(autoload);						
        layer->SetAutoLoad(autoload);
        layer->SetVisible(visible);
        layer->SetLocked(locked);

    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::AddEntity(const Util::String & category, const Attr::AttributeContainer & attrs)
{
    LevelEditor2EntityManager::Instance()->CreateEntityFromAttrContainer(category,attrs);
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::SetPosteffect(const Util::String & preset, const Math::matrix44 & globallightTransform)
{
 
    if (PostEffect::PostEffectRegistry::Instance()->HasPreset(preset))
    {
        LevelEditor2App::Instance()->GetWindow()->GetPostEffectController()->ActivatePrefix(preset);					
    }
    else
    {
        Util::String msg;
        msg.Format("Unkown posteffect preset %s in level, missing file from data/tables/posteffect ?",preset.AsCharPtr());
        n_warning(msg.AsCharPtr());
        LevelEditor2App::Instance()->GetWindow()->GetPostEffectController()->ActivatePrefix("Default");					
    }
   
    Ptr<Game::Entity> light = LevelEditor2EntityManager::Instance()->GetGlobalLight();
    Ptr<BaseGameFeature::UpdateTransform> update = BaseGameFeature::UpdateTransform::Create();
    update->SetMatrix(globallightTransform);
    __SendSync(light, update);
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::SetDimensions(const Math::bbox & box)
{
    // empty
}

} // namespace LevelEditor2
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
Level::Level(): startLevel(false), autoBatch(true), inImport(false)
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

//------------------------------------------------------------------------------
/**
*/
void
Level::UpdateGuids()
{
	Dictionary<Util::Guid, Util::Guid> newGuids;
	// generate new guids first
	for (int i = 0; i < this->importedEntities.Size(); i++)
	{
		Attr::AttributeContainer & cont = this->importedEntities[i].Value();
		Util::Guid newGuid;
		newGuid.Generate();
		newGuids.Add(cont.GetGuid(Attr::Guid), newGuid);
		cont.SetGuid(Attr::Guid, newGuid);
	}
	// now update any parent guids that were replaced
	for (int i = 0; i < this->importedEntities.Size(); i++)
	{
		Util::String category = this->importedEntities[i].Key();
		Attr::AttributeContainer & cont = this->importedEntities[i].Value();
		Util::Guid parent = cont.GetGuid(Attr::ParentGuid);
		if (newGuids.Contains(parent))
		{
			cont.SetGuid(Attr::ParentGuid, newGuids[parent]);
		}
	}
}


//------------------------------------------------------------------------------------
/**
*/
bool
Level::ImportLevel(const Util::String& level)
{
	this->inImport = true;
	return this->LoadLevel(level, false);
	this->inImport = false;
}

//------------------------------------------------------------------------------------
/**
*/
bool
Level::LoadLevel(const Util::String& level, bool clear)
{
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	Util::String fileName;
	fileName.Format("work:levels/%s.xml",level.AsCharPtr());
	if(!ioServer->FileExists(fileName))
	{
		return false;
	}
	Ptr<IO::Stream> stream = ioServer->CreateStream(fileName);
	stream->SetAccessMode(IO::Stream::ReadAccess);
	if (!stream->Open())
	{
		return false;
	}

	LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->OnBeginLoad();
	if (clear)
	{
		LevelEditor2EntityManager::Instance()->RemoveAllEntities();

		LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->Discard();
		LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->Setup();
	}

	this->importedEntities.Clear();
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
		if (!clear)
		{
			this->UpdateGuids();
		}
		for (int i = 0;i < this->importedEntities.Size();i++)
		{
			LevelEditor2EntityManager::Instance()->CreateEntityFromAttrContainer(this->importedEntities[i].Key(), this->importedEntities[i].Value());
		}


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
	LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->OnEndLoad();
	// add wrapper entity for global light
	if (clear)
	{
		LevelEditor2EntityManager::Instance()->CreateLightEntity("GlobalLight");
	}
	return result;	
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
Level::SaveSelection(const Util::String & name)
{
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	// make sure the levels directory exists
	ioServer->CreateDirectory("work:levels");
	// build a filename for the level file
	Util::String fileName;
	fileName.Format("work:levels/%s.xml", name.AsCharPtr());
	this->SaveLevelFile(name, IO::URI(fileName),true);
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
	fileName.Format("work:levels/%s.xml", this->name.AsCharPtr());
	this->SaveLevelFile(this->name, IO::URI(fileName), false);
}


//------------------------------------------------------------------------------
/**
*/
void
Level::SaveLevelFile(const Util::String& name, const IO::URI & fileName, bool selectedOnly)
{
	
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	

	Ptr<IO::XmlWriter> xmlWriter = IO::XmlWriter::Create();
	Ptr<IO::Stream> stream;

	stream = ioServer->CreateStream(fileName);
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
	xmlWriter->SetString("name", name);
	xmlWriter->SetString("id", name);

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
		
		if (this->SaveEntity(name, envEntities[i], xmlWriter, selectedOnly))
		{
			Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
			envEntities[i]->SendSync(msg.cast<Messaging::Message>());
			const Ptr<Graphics::ModelEntity> & model = msg->GetEntity();
			box.extend(model->GetGlobalBoundingBox());
		}
	}

	// real game entities
	Array<Ptr<Game::Entity>> gameEntities = SortEntityArray(BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Game)),Attr::EntityGuid);
	for(int i=0;i<gameEntities.Size();i++)
	{
		if (this->SaveEntity(name, gameEntities[i], xmlWriter, selectedOnly))
		{
			Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
			gameEntities[i]->SendSync(msg.cast<Messaging::Message>());
			const Ptr<Graphics::ModelEntity> & model = msg->GetEntity();
			box.extend(model->GetGlobalBoundingBox());
		}				
	}
	/// lights
	Array<Ptr<Game::Entity>> lightEntities = SortEntityArray(BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Light)),Attr::EntityGuid);
	for(int i=0;i<lightEntities.Size();i++)
	{	
		// ignore the global light, it gets handled by posteffect
		if (lightEntities[i]->GetInt(Attr::LightType) == Lighting::LightType::Global)
		{
			continue;
		}
		this->SaveEntity(name, lightEntities[i], xmlWriter, selectedOnly);
	}


	Array<Ptr<Game::Entity>> groupEntities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Group));
	for(int i=0;i<groupEntities.Size();i++)
	{
		this->SaveEntity(name, groupEntities[i], xmlWriter, selectedOnly);
	}
    Array<Ptr<Game::Entity>> navMeshes = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,NavMesh));
	for(int i=0;i<navMeshes.Size();i++)
    {
		this->SaveEntity(name, navMeshes[i], xmlWriter, selectedOnly);
        Ptr<SaveNavMesh> msg = SaveNavMesh::Create();
        navMeshes[i]->SendSync(msg.cast<Messaging::Message>());
    }
    Array<Ptr<Game::Entity>> navAreas = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,NavMeshArea));
    for(int i=0;i<navAreas.Size();i++)
    {
        const Ptr<Game::Entity>& entity = navAreas[i];
		this->SaveEntity(name, entity, xmlWriter, selectedOnly);
    }
	Array<Ptr<Game::Entity>> lightProbes = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType, Probe));
	for (int i = 0; i < lightProbes.Size(); i++)
	{
		const Ptr<Game::Entity>& entity = lightProbes[i];
		this->SaveEntity(name, entity, xmlWriter, selectedOnly);
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
		this->ExportLevel(fileName.LocalPath());
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
	if (!this->inImport)
	{
		this->name = name;
	}    
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
    this->importedEntities.Append(Util::KeyValuePair<Util::String, Attr::AttributeContainer>(category, attrs));        
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

//------------------------------------------------------------------------------
/** sorting function for attributes that sorts by name instead of pointer
*/
static bool
AttributeLesser(const Attr::Attribute& lhs, const Attr::Attribute& rhs)
{
	return lhs.GetName() < rhs.GetName();
}

//------------------------------------------------------------------------------
/**
*/
bool
Level::SaveEntity(const Util::String & levelName, const Ptr<Game::Entity>& entity, const Ptr<IO::XmlWriter>& stream, bool selectedOnly)
{
	if (selectedOnly && !entity->GetBool(Attr::IsSelected))
	{
		return false;
	}
	EntityType entityType = (EntityType)entity->GetInt(Attr::EntityType);	
	switch (entityType)
	{
	case Environment:
	{
		stream->BeginNode("Object");
			stream->SetString("category", "_Environment");
			stream->BeginNode("Attributes");
				// get graphics
				String gfx = entity->GetString(Attr::Graphics);
				this->WriteString(stream, "_ID", this->AllocateID("environment", "", gfx.ExtractFileName()));
				this->WriteString(stream, "Id", entity->GetString(Attr::Id));
				this->WriteString(stream, "_Level", levelName.AsCharPtr());
				this->WriteString(stream, "_Layers", entity->GetString(Attr::_Layers));
				this->WriteString(stream, "Guid", entity->GetGuid(Attr::EntityGuid).AsString());
				this->WriteString(stream, "ParentGuid", entity->GetGuid(Attr::ParentGuid).AsString());
				this->WriteString(stream, "Transform", Util::String::FromMatrix44(entity->GetMatrix44(Attr::Transform)));
				this->WriteString(stream, "CastShadows", Util::String::FromBool(entity->GetBool(Attr::CastShadows)));
				this->WriteString(stream, "PhysicMaterial", entity->GetString(Attr::PhysicMaterial));
				this->WriteString(stream, "Graphics", gfx);
				this->WriteString(stream, "StartAnimation", entity->GetString(Attr::StartAnimation));
				this->WriteString(stream, "LoadSynced", Util::String::FromBool(entity->GetBool(Attr::LoadSynced)));
				this->WriteString(stream, "CollisionEnabled", Util::String::FromBool(entity->GetBool(Attr::CollisionEnabled)));
				this->WriteString(stream, "DynamicObject", Util::String::FromBool(entity->GetBool(Attr::DynamicObject)));
				this->WriteString(stream, "VelocityVector", Util::String::FromFloat4(entity->GetFloat4(Attr::VelocityVector)));
				this->WriteString(stream, "Instanced", Util::String::FromBool(entity->GetBool(Attr::Instanced)));
			stream->EndNode();
		stream->EndNode();
	}
		break;	
	case Game:
	{
		stream->BeginNode("Object");
			stream->SetString("category", entity->GetString(Attr::EntityCategory));
			stream->BeginNode("Attributes");

				this->WriteString(stream, "_ID", this->AllocateID("object", entity->GetString(Attr::EntityCategory), entity->GetString(Attr::Id)));
				this->WriteString(stream, "Id", entity->GetString(Attr::Id));
				this->WriteString(stream, "_Level", levelName.AsCharPtr());
				this->WriteString(stream, "_Layers", entity->GetString(Attr::_Layers));
				this->WriteString(stream, "Guid", entity->GetGuid(Attr::EntityGuid).AsString());
				this->WriteString(stream, "ParentGuid", entity->GetGuid(Attr::ParentGuid).AsString());
				this->WriteString(stream, "Transform", Util::String::FromMatrix44(entity->GetMatrix44(Attr::Transform)));

				Ptr<GetEntityValues> gmsg = GetEntityValues::Create();
				entity->SendSync(gmsg.cast<Messaging::Message>());
				Array<Attribute> attrs = gmsg->GetAttrs().GetAttrs().ValuesAsArray();
				// sort attributes alphabetically
				attrs.SortWithFunc(AttributeLesser);
				for (IndexT idx = 0;idx < attrs.Size();idx++)
				{
					if (attrs[idx].GetAttrId() != Attr::Guid && attrs[idx].GetAttrId() != Attr::_Level && attrs[idx].GetAttrId() != Attr::_ID && attrs[idx].GetAttrId() != Attr::Id)
					{
						this->WriteString(stream, attrs[idx].GetName(), attrs[idx].ValueAsString());
					}
				}
			stream->EndNode();
		stream->EndNode();

	}
		break;
	case Light:
	{
		
		stream->BeginNode("Object");
		stream->SetString("category", "Light");
		stream->BeginNode("Attributes");

		this->WriteString(stream, "_ID", this->AllocateID("light", Util::String::FromInt(entity->GetInt(Attr::LightType)), entity->GetString(Attr::Id)));
		this->WriteString(stream, "Id", entity->GetString(Attr::Id));
		this->WriteString(stream, "_Level", levelName.AsCharPtr());
		this->WriteString(stream, "_Layers", entity->GetString(Attr::_Layers));
		this->WriteString(stream, "Guid", entity->GetGuid(Attr::EntityGuid).AsString());
		this->WriteString(stream, "ParentGuid", entity->GetGuid(Attr::ParentGuid).AsString());
		this->WriteString(stream, "Transform", Util::String::FromMatrix44(entity->GetMatrix44(Attr::Transform)));

		this->WriteString(stream, "LightType", entity->GetAttr(Attr::LightType).ValueAsString());
		this->WriteString(stream, "LightColor", entity->GetAttr(Attr::LightColor).ValueAsString());
		this->WriteString(stream, "LightOppositeColor", entity->GetAttr(Attr::LightOppositeColor).ValueAsString());
		this->WriteString(stream, "LightIntensity", entity->GetAttr(Attr::LightIntensity).ValueAsString());
		this->WriteString(stream, "LightRange", entity->GetAttr(Attr::LightRange).ValueAsString());
		this->WriteString(stream, "LightAmbient", entity->GetAttr(Attr::LightAmbient).ValueAsString());
		this->WriteString(stream, "LightCastShadows", entity->GetAttr(Attr::LightCastShadows).ValueAsString());
		this->WriteString(stream, "LightFlickerEnable", entity->GetAttr(Attr::LightFlickerEnable).ValueAsString());
		this->WriteString(stream, "LightFlickerFrequency", entity->GetAttr(Attr::LightFlickerFrequency).ValueAsString());
		this->WriteString(stream, "LightFlickerIntensity", entity->GetAttr(Attr::LightFlickerIntensity).ValueAsString());
		this->WriteString(stream, "LightFlickerPosition", entity->GetAttr(Attr::LightFlickerPosition).ValueAsString());
		this->WriteString(stream, "LightConeAngle", entity->GetAttr(Attr::LightConeAngle).ValueAsString());
		this->WriteString(stream, "LightVolumetric", entity->GetAttr(Attr::LightVolumetric).ValueAsString());
		this->WriteString(stream, "LightVolumetricIntensity", entity->GetAttr(Attr::LightVolumetricIntensity).ValueAsString());
		this->WriteString(stream, "LightVolumetricScale", entity->GetAttr(Attr::LightVolumetricScale).ValueAsString());
		this->WriteString(stream, "LightProjectionMap", entity->GetAttr(Attr::LightProjectionMap).ValueAsString());

		stream->EndNode();
		stream->EndNode();
	}
		break;
	case Group:
	{
		stream->BeginNode("Object");
		stream->SetString("category", "_Group");
		stream->BeginNode("Attributes");
		this->WriteString(stream, "Id", entity->GetString(Attr::Id));
		this->WriteString(stream, "Guid", entity->GetGuid(Attr::EntityGuid).AsString());
		this->WriteString(stream, "ParentGuid", entity->GetGuid(Attr::ParentGuid).AsString());
		this->WriteString(stream, "Transform", entity->GetAttr(Attr::Transform).ValueAsString());
		stream->EndNode();
		stream->EndNode();
	}
		break;
	case NavMesh:
	{
		stream->BeginNode("Object");
			stream->SetString("category", "NavMeshData");
			stream->BeginNode("Attributes");
				this->WriteString(stream, "_ID", this->AllocateID("NavMeshData", "", entity->GetString(Attr::Id)));
				this->WriteString(stream, "Id", entity->GetString(Attr::Id));
				this->WriteString(stream, "Guid", entity->GetGuid(Attr::EntityGuid).AsString());
				this->WriteString(stream, "_Level", levelName.AsCharPtr());
				this->WriteString(stream, "ParentGuid", entity->GetGuid(Attr::ParentGuid).AsString());
				this->WriteString(stream, "MaxEdgeLength", entity->GetAttr(Attr::MaxEdgeLength).ValueAsString());
				this->WriteString(stream, "RegionMinSize", entity->GetAttr(Attr::RegionMinSize).ValueAsString());
				this->WriteString(stream, "RegionMergeSize", entity->GetAttr(Attr::RegionMergeSize).ValueAsString());
				this->WriteString(stream, "AgentRadius", entity->GetAttr(Attr::AgentRadius).ValueAsString());
				this->WriteString(stream, "AgentHeight", entity->GetAttr(Attr::AgentHeight).ValueAsString());
				this->WriteString(stream, "AgentMaxClimb", entity->GetAttr(Attr::AgentMaxClimb).ValueAsString());
				this->WriteString(stream, "MaxEdgeError", entity->GetAttr(Attr::MaxEdgeError).ValueAsString());
				this->WriteString(stream, "DetailSampleDist", entity->GetAttr(Attr::DetailSampleDist).ValueAsString());
				this->WriteString(stream, "DetailSampleMaxError", entity->GetAttr(Attr::DetailSampleMaxError).ValueAsString());
				this->WriteString(stream, "MaxSlope", entity->GetAttr(Attr::MaxSlope).ValueAsString());
				this->WriteString(stream, "CellHeight", entity->GetAttr(Attr::CellHeight).ValueAsString());
				this->WriteString(stream, "CellSize", entity->GetAttr(Attr::CellSize).ValueAsString());
				this->WriteString(stream, "NavMeshData", entity->GetString(Attr::NavMeshData));
				this->WriteString(stream, "EntityReferences", entity->GetAttr(Attr::EntityReferences).ValueAsString());
				this->WriteString(stream, "AreaEntityReferences", entity->GetAttr(Attr::AreaEntityReferences).ValueAsString());
				this->WriteString(stream, "NavMeshCenter", entity->GetAttr(Attr::NavMeshCenter).ValueAsString());
				this->WriteString(stream, "NavMeshExtends", entity->GetAttr(Attr::NavMeshExtends).ValueAsString());
				this->WriteString(stream, "NavMeshMeshString", entity->GetString(Attr::NavMeshMeshString));
			stream->EndNode();
		stream->EndNode();
	}
		break;
	case Probe:
	{
		stream->BeginNode("Object");
			stream->SetString("category", "LightProbe");
			stream->BeginNode("Attributes");
				this->WriteString(stream, "Id", entity->GetString(Attr::Id));
				this->WriteString(stream, "_ID", this->AllocateID("light", "lightprobe", entity->GetString(Attr::Id)));
				this->WriteString(stream, "Guid", entity->GetGuid(Attr::EntityGuid).AsString());
				this->WriteString(stream, "_Level", levelName.AsCharPtr());
				this->WriteString(stream, "_Layers", entity->GetString(Attr::_Layers));
				this->WriteString(stream, "ParentGuid", entity->GetGuid(Attr::ParentGuid).AsString());
				this->WriteString(stream, "Transform", Util::String::FromMatrix44(entity->GetMatrix44(Attr::Transform)));
				this->WriteString(stream, "ProbeReflectionMap", entity->GetString(Attr::ProbeReflectionMap));
				this->WriteString(stream, "ProbeIrradianceMap", entity->GetString(Attr::ProbeIrradianceMap));
				this->WriteString(stream, "ProbeInfluenceShapeType", entity->GetString(Attr::ProbeInfluenceShapeType));
				this->WriteString(stream, "ProbeInfluenceLayer", Util::String::FromInt(entity->GetInt(Attr::ProbeInfluenceLayer)));
				this->WriteString(stream, "ProbeInfluenceFalloff", Util::String::FromFloat(entity->GetFloat(Attr::ProbeInfluenceFalloff)));
				this->WriteString(stream, "ProbeInfluencePower", Util::String::FromFloat(entity->GetFloat(Attr::ProbeInfluencePower)));
				this->WriteString(stream, "ProbeBBExtents", Util::String::FromFloat4(entity->GetFloat4(Attr::ProbeBBExtents)));
				this->WriteString(stream, "ProbeParallaxCorrected", Util::String::FromBool(entity->GetBool(Attr::ProbeParallaxCorrected)));

				this->WriteString(stream, "ProbeResolutionWidth", Util::String::FromInt(entity->GetInt(Attr::ProbeResolutionWidth)));
				this->WriteString(stream, "ProbeResolutionHeight", Util::String::FromInt(entity->GetInt(Attr::ProbeResolutionHeight)));
				this->WriteString(stream, "ProbeName", entity->GetString(Attr::ProbeName));
				this->WriteString(stream, "ProbeBuildIrradiance", Util::String::FromBool(entity->GetBool(Attr::ProbeBuildIrradiance)));
				this->WriteString(stream, "ProbeBuildReflections", Util::String::FromBool(entity->GetBool(Attr::ProbeBuildReflections)));
				this->WriteString(stream, "ProbeGenerateMipmaps", Util::String::FromBool(entity->GetBool(Attr::ProbeGenerateMipmaps)));
				this->WriteString(stream, "ProbeAutoAssignMaps", Util::String::FromBool(entity->GetBool(Attr::ProbeAutoAssignMaps)));
				this->WriteString(stream, "ProbeOutputFolder", entity->GetString(Attr::ProbeOutputFolder));
				this->WriteString(stream, "ProbeOutputFilename", entity->GetString(Attr::ProbeOutputFilename));
			stream->EndNode();
		stream->EndNode();
	}
		break;
	case NavMeshArea:
	{
		stream->BeginNode("Object");
			stream->SetString("category", "_NavigationArea");
			stream->BeginNode("Attributes");
				this->WriteString(stream, "_ID", this->AllocateID("EditorNavAreaMarker", "", entity->GetString(Attr::Id)));
				this->WriteString(stream, "Id", entity->GetString(Attr::Id));
				this->WriteString(stream, "Guid", entity->GetGuid(Attr::EntityGuid).AsString());
				this->WriteString(stream, "_Level", levelName.AsCharPtr());
				this->WriteString(stream, "_Layers", entity->GetString(Attr::_Layers));
				this->WriteString(stream, "Graphics", entity->GetString(Attr::Graphics));
				this->WriteString(stream, "ParentGuid", entity->GetGuid(Attr::ParentGuid).AsString());
				this->WriteString(stream, "Transform", Util::String::FromMatrix44(entity->GetMatrix44(Attr::Transform)));
				this->WriteString(stream, "NavMeshAreaFlags", Util::String::FromInt(entity->GetInt(Attr::NavMeshAreaFlags)));
				this->WriteString(stream, "NavMeshAreaCost", Util::String::FromInt(entity->GetInt(Attr::NavMeshAreaCost)));
				this->WriteString(stream, "NavMeshMeshString", entity->GetString(Attr::NavMeshMeshString));
			stream->EndNode();
		stream->EndNode();
	}
		break;	
	default:
		return false;
		break;

	}
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
Level::SaveEntityArray(const Util::Array<Ptr<Game::Entity>> & entities, const IO::URI & fileName)
{
	Ptr<IO::XmlWriter> xmlWriter = IO::XmlWriter::Create();
	Ptr<IO::Stream> stream;

	stream = IO::IoServer::Instance()->CreateStream(IO::URI(fileName));
	stream->SetAccessMode(IO::Stream::WriteAccess);
	stream->Open();

	if (!stream->IsOpen())
	{
		n_warning("cant open stream");
	}
	xmlWriter->SetStream(stream);
	xmlWriter->Open();
	xmlWriter->BeginNode("Entities");

	for (int i = 0; i < entities.Size();i++)
	{
		this->SaveEntity("_NOTUSED", entities[i], xmlWriter, false);
	}

	xmlWriter->EndNode();
	xmlWriter->Close();
	stream->Flush();
	stream->Close();
}

//------------------------------------------------------------------------------
/**
*/
bool
Level::LoadEntities(const IO::URI & fileName, bool cleanPerLevelData)
{
    
    Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();    
    if (!ioServer->FileExists(fileName))
    {
        return false;
    }
    

    Ptr<IO::Stream> stream = ioServer->CreateStream(fileName);
    stream->SetAccessMode(IO::Stream::ReadAccess);
    if (!stream->Open())
    {
        return false;
    }    
    this->inImport = true;
	this->importedEntities.Clear();
    Ptr<IO::XmlReader> reader = IO::XmlReader::Create();
    reader->SetStream(stream);
    if (reader->Open())
    {
        reader->SetToRoot();

        if (reader->HasNode("/Entities"))        
        {

            reader->SetToFirstChild();
            do
            {
                this->LoadEntity(reader);
            } while (reader->SetToNextChild("Object"));
        }

        reader->Close();
        stream->Close();
    }

    if (!this->importedEntities.IsEmpty())
    {
        LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->OnBeginLoad();

		this->UpdateGuids();
        for (int i = 0; i < this->importedEntities.Size(); i++)
        {            
			LevelEditor2EntityManager::Instance()->CreateEntityFromAttrContainer(this->importedEntities[i].Key(), this->importedEntities[i].Value());
        }
                
        // reparent all items
        LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->RebuildTree();       
        LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->OnEndLoad();
    }
    
    this->inImport = false;
    return true;
}

} // namespace LevelEditor2
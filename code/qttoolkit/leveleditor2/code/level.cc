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

//------------------------------------------------------------------------------
/**
*/
void
ParsedLevel::SetName(const Util::String & name)
{
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
void
ParsedLevel::AddLayer(const Util::String & name, bool visible, bool autoload, bool locked)
{
	_Layer layer = { name, visible, autoload, locked };
	this->layers.Append(layer);
}

//------------------------------------------------------------------------------
/**
*/
void
ParsedLevel::AddEntity(const Util::String & category, const Attr::AttributeContainer & attrs)
{
	this->entities.Append(Util::KeyValuePair<Util::String, Attr::AttributeContainer>(category, attrs));
}

//------------------------------------------------------------------------------
/**
*/
void
ParsedLevel::SetPosteffect(const Util::String & preset, const Math::matrix44 & globallightTransform)
{
	this->preset = preset;
	this->lightTrans = globallightTransform;
}

//------------------------------------------------------------------------------
/**
*/
void
ParsedLevel::UpdateGuids()
{
	Dictionary<Util::Guid, Util::Guid> newGuids;
	// generate new guids first
	for (int i = 0; i < this->entities.Size(); i++)
	{
		Attr::AttributeContainer & cont = this->entities[i].Value();
		Util::Guid newGuid;
		newGuid.Generate();
		newGuids.Add(cont.GetGuid(Attr::Guid), newGuid);
		cont.SetGuid(Attr::Guid, newGuid);
	}
	// now update any parent guids that were replaced
	for (int i = 0; i < this->entities.Size(); i++)
	{
		Util::String category = this->entities[i].Key();
		Attr::AttributeContainer & cont = this->entities[i].Value();
		Util::Guid parent = cont.GetGuid(Attr::ParentGuid);
		if (newGuids.Contains(parent))
		{
			cont.SetGuid(Attr::ParentGuid, newGuids[parent]);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
ParsedLevel::LoadEntities(const IO::URI & fileName)
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
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ParsedLevel::AddReference(const Util::String & name)
{
    this->references.Append(name);
}

__ImplementClass(LevelEditor2::ParsedLevel, 'PRVL', ToolkitUtil::LevelParser);



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
Level::LoadLevel(const Util::String& level, LoadMode mode)
{
    LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->OnBeginLoad();
    if (mode == Replace)
    {
        LevelEditor2EntityManager::Instance()->RemoveAllEntities(true);

        LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->Discard();
        LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->Setup();
        LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->ClearReferences();
        this->refLevels.Clear();
        LevelEditor2EntityManager::Instance()->CreateLightEntity("GlobalLight");
    }
   
    if (this->LoadLevelFile(level, mode))
    {
		if (mode == Replace)
		{
			if (this->refLevels.Contains(level))
			{
				Ptr<ParsedLevel> lvl = this->refLevels[level];

				for (int i = 0; i < lvl->references.Size(); i++)
				{
					this->LoadLevelFile(lvl->references[i], Reference);
				}
				this->refLevels.Erase(level);
			}
		}
        // reparent all items
        LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->RebuildTree();
        this->startLevel = false;
        if (LevelEditor2App::Instance()->GetGlobalAttrs()->HasAttr(Attr::_DefaultLevel))
        {
            if (LevelEditor2App::Instance()->GetGlobalAttrs()->GetString(Attr::_DefaultLevel) == this->name)
            {
                this->startLevel = true;
            }
        }       
    }
    LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->OnEndLoad();   
    return true;
}

//------------------------------------------------------------------------------------
/**
*/
bool
Level::LoadLevelFile(const Util::String& level, LoadMode mode)
{
    LevelEditor2Window * window = LevelEditor2App::Instance()->GetWindow();
    if (mode == Reference && this->refLevels.Contains(level))
    {        
        n_status("Trying to reference yourself\n");
        return false;
    }    
    n_status("Loading %s\n", level.AsCharPtr());
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	Util::String fileName;
	fileName.Format("work:levels/%s.xml",level.AsCharPtr());
	if(!ioServer->FileExists(fileName))
	{
        n_status("File %s does not exist\n", fileName.AsCharPtr());
		return false;
	}
	Ptr<IO::Stream> stream = ioServer->CreateStream(fileName);
	stream->SetAccessMode(IO::Stream::ReadAccess);
	if (!stream->Open())
	{
        n_status("Cannot open file: %s", fileName.AsCharPtr());
		return false;
	}
	   
	bool result = false;
    window->GetProgressBar()->reset();
	Ptr<ParsedLevel> lvl = ParsedLevel::Create();
	
	Ptr<IO::XmlReader> reader = IO::XmlReader::Create();
	reader->SetStream(stream);
	if(reader->Open())
	{
		result = lvl->LoadXmlLevel(reader);		
		reader->Close();
		stream->Close();
	}
    window->GetProgressBar()->setMaximum(lvl->entities.Size());
	
	if(result)
	{
        Util::String entityLevel;
        switch(mode)
        {
        case Replace:
			this->SetName(lvl->name);
            entityLevel = this->name;
			this->SetPosteffect(lvl->preset, lvl->lightTrans);
            this->refLevels.Add(this->name, lvl);
            break;
        case Merge:
            entityLevel = this->name;
            lvl->UpdateGuids();
            break;
        case Reference:            
            entityLevel = level;
            LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->AddReference(level);				
            this->refLevels.Add(level, lvl);            
            break;
        default:
            break;
        }        
		for (int i = 0;i < lvl->layers.Size();i++)
		{
			const ParsedLevel::_Layer & l = lvl->layers[i];
			this->AddLayer(l.name, l.visible, l.autoload, l.locked);
		}
        QProgressBar * bar = window->GetProgressBar();
		for (int i = 0;i < lvl->entities.Size();i++)
		{
			LevelEditor2EntityManager::Instance()->CreateEntityFromAttrContainer(entityLevel, lvl->entities[i].Key(), lvl->entities[i].Value());
            bar->setValue(bar->value() + 1);
		}		
        n_status("Success");        
	}	
	window->GetProgressBar()->reset();
	return result;	
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::SaveLevelAs(const Util::String& newName)
{
	Util::Array<Ptr<Game::Entity>> ents = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityLevel, this->name));
	for (int i = 0;i < ents.Size();i++)
	{
		ents[i]->SetString(Attr::EntityLevel, newName);
	}
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
		if(ents[i].isvalid() && ents[i]->HasAttr(Attr::EntityType))
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
	Math::bbox box;
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	// make sure the levels directory exists
	ioServer->CreateDirectory("work:levels");
	// build a filename for the level file
	Util::String fileName;
	fileName.Format("work:levels/%s.xml", name.AsCharPtr());
	this->SaveLevelFile(name, IO::URI(fileName),true, box);
}

//------------------------------------------------------------------------------
/**
*/
void 
Level::SaveLevel()
{
    n_status("Saving level...");
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	// make sure the levels directory exists
	ioServer->CreateDirectory("work:levels");
	// build a filename for the level file
	Util::String fileName;
	Math::bbox box;
	for (int i = 0;i < this->refLevels.Size();i++)
	{
		Math::bbox lvlbox;
		if (refLevels.KeyAtIndex(i) == this->name) continue;
		fileName.Format("work:levels/%s.xml", this->refLevels.KeyAtIndex(i).AsCharPtr());
		this->SaveLevelFile(this->refLevels.KeyAtIndex(i), fileName, false, lvlbox);
		box.extend(lvlbox);
	}

	fileName.Format("work:levels/%s.xml", this->name.AsCharPtr());
	this->SaveLevelFile(this->name, IO::URI(fileName), false, box);
    n_status("Done");
}


//------------------------------------------------------------------------------
/**
*/
void
Level::SaveLevelFile(const Util::String& name, const IO::URI & realfilename, bool selectedOnly, Math::bbox& box)
{
	
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	

	Ptr<IO::XmlWriter> xmlWriter = IO::XmlWriter::Create();
	Ptr<IO::Stream> stream;

	IO::URI filename;
	filename.SetScheme("safefile");
	filename.SetLocalPath(realfilename.LocalPath());
	stream = ioServer->CreateStream(filename);
	stream->SetAccessMode(IO::Stream::WriteAccess);
	stream->Open();

	if (!stream->IsOpen())
	{
        n_status("Failed to open file\n");
		return;
	}	
    
	this->objectCounters.Clear();
    n_status("Saving level: %s\n", realfilename.LocalPath().AsCharPtr());
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
	
	
	// build filter 
	Util::Array<Attr::Attribute> filter;
	if (selectedOnly)
	{
		filter.Append(Attr::Attribute(Attr::IsSelected, true));
	}
	else
	{
		filter.Append(Attr::Attribute(Attr::EntityLevel, name));
	}
	
    QProgressBar * bar = LevelEditor2App::Instance()->GetWindow()->GetProgressBar();
    bar->reset();

	Array<Ptr<Game::Entity>> ents = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttrs(filter);
    bar->setMaximum(ents.Size()-1);
	for (int i = 0;i < ents.Size();i++)
	{
		EntityType type = (EntityType)ents[i]->GetInt(Attr::EntityType);
		if (type == Environment || type == Game)
		{
			Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
			ents[i]->SendSync(msg.cast<Messaging::Message>());
			const Ptr<Graphics::ModelEntity> & model = msg->GetEntity();
			box.extend(model->GetGlobalBoundingBox());
		}
		this->SaveEntity(name, ents[i], xmlWriter);
        bar->setValue(bar->value() + 1);
	}
	xmlWriter->EndNode();	
	
	// global stuff
	xmlWriter->BeginNode("Global");
	xmlWriter->SetFloat4("WorldCenter", box.center());
	xmlWriter->SetFloat4("WorldExtents", box.size());
	if (name != this->name && this->refLevels.Contains(name))
	{
		xmlWriter->SetString("PostEffectPreset", refLevels[name]->preset);
		xmlWriter->SetString("GlobalLightTransform", Util::String::FromMatrix44(refLevels[name]->lightTrans));
	}
	else
	{
		xmlWriter->SetString("PostEffectPreset", LevelEditor2App::Instance()->GetWindow()->GetPostEffectController()->GetPreset());
		PostEffect::PostEffectEntity::ParamSet parms = PostEffect::PostEffectManager::Instance()->GetDefaultEntity()->Params();
		xmlWriter->SetString("GlobalLightTransform", Util::String::FromMatrix44(parms.light->GetLightTransform()));
	}
		
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
	if (name == this->name && !this->refLevels.IsEmpty())
	{		
		for (int i = 0;i < this->refLevels.Size();i++)
		{
            if (refLevels.KeyAtIndex(i) == this->name)
                continue;
			xmlWriter->BeginNode("Reference");
			xmlWriter->SetString("Level", refLevels.KeyAtIndex(i));
			xmlWriter->EndNode();
		}
	}
	
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
        n_status("Batching level\n");
		this->ExportLevel(filename.LocalPath());
	}
    bar->reset();
    n_status("Success");
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
	LevelEditor2EntityManager::Instance()->CreateEntityFromAttrContainer(this->name, category, attrs);
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
Level::SaveEntity(const Util::String & levelName, const Ptr<Game::Entity>& entity, const Ptr<IO::XmlWriter>& stream)
{
	
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
		if (entity->GetInt(Attr::LightType) == Lighting::LightType::Global)
		{
			return true;
		}
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
		Ptr<SaveNavMesh> msg = SaveNavMesh::Create();
		entity->SendSync(msg.cast<Messaging::Message>());
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
				this->WriteString(stream, "ProbeDepthMap", entity->GetString(Attr::ProbeDepthMap));
				this->WriteString(stream, "ProbeInfluenceShapeType", entity->GetString(Attr::ProbeInfluenceShapeType));
				this->WriteString(stream, "ProbeInfluenceLayer", Util::String::FromInt(entity->GetInt(Attr::ProbeInfluenceLayer)));
				this->WriteString(stream, "ProbeInfluenceFalloff", Util::String::FromFloat(entity->GetFloat(Attr::ProbeInfluenceFalloff)));
				this->WriteString(stream, "ProbeInfluencePower", Util::String::FromFloat(entity->GetFloat(Attr::ProbeInfluencePower)));
				this->WriteString(stream, "ProbeParallaxBox", Util::String::FromFloat4(entity->GetFloat4(Attr::ProbeParallaxBox)));
				this->WriteString(stream, "ProbeCorrectionMethod", entity->GetString(Attr::ProbeCorrectionMethod));

				this->WriteString(stream, "ProbeResolutionWidth", Util::String::FromInt(entity->GetInt(Attr::ProbeResolutionWidth)));
				this->WriteString(stream, "ProbeResolutionHeight", Util::String::FromInt(entity->GetInt(Attr::ProbeResolutionHeight)));
				this->WriteString(stream, "ProbeName", entity->GetString(Attr::ProbeName));
				this->WriteString(stream, "ProbeBuildIrradiance", Util::String::FromBool(entity->GetBool(Attr::ProbeBuildIrradiance)));
				this->WriteString(stream, "ProbeBuildReflections", Util::String::FromBool(entity->GetBool(Attr::ProbeBuildReflections)));
				this->WriteString(stream, "ProbeBuildReflections", Util::String::FromBool(entity->GetBool(Attr::ProbeBuildDepth)));
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
		this->SaveEntity("_NOTUSED", entities[i], xmlWriter);
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
Level::LoadEntities(const IO::URI & fileName)
{
	Ptr<ParsedLevel> lvl = ParsedLevel::Create();
	if (lvl->LoadEntities(fileName))
	{
		if (!lvl->entities.IsEmpty())
		{
			LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->OnBeginLoad();

			lvl->UpdateGuids();
			for (int i = 0; i < lvl->entities.Size(); i++)
			{
				LevelEditor2EntityManager::Instance()->CreateEntityFromAttrContainer(this->name, lvl->entities[i].Key(), lvl->entities[i].Value());
			}

			// reparent all items
			LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->RebuildTree();
			LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->OnEndLoad();
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
Level::Clear()
{
	this->refLevels.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
Level::RemoveReference(const Util::String & level)
{
    Util::Array<Ptr<Game::Entity>> ents = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityLevel, level));
    for (int i = 0; i < ents.Size(); i++)
    {
        LevelEditor2EntityManager::Instance()->RemoveEntity(ents[i]);
    }
    this->refLevels.Erase(level);
}

} // namespace LevelEditor2
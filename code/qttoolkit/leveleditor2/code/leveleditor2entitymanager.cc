//------------------------------------------------------------------------------
//  leveleditor2entitymanager.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "leveleditor2entitymanager.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "managers/entitymanager.h"
#include "util/guid.h"
#include "attr/attributecontainer.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "physicsfeature/physicsprotocol.h"
#include "lighting/lighttype.h"
#include "graphicsfeature/properties/lightproperty.h"
#include "managers/categorymanager.h"
#include "managers/factorymanager.h"
#include "properties/editorproperty.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "io/ioserver.h"
#include "io/xmlreader.h"
#include "leveleditor2app.h"
#include "leveleditorstate.h"
#include "idldocument/idldocument.h"
#include "idldocument/idlattributelib.h"
#include "editorfeatures/editorblueprintmanager.h"
#include "graphics/graphicsinterface.h"

using namespace Util;
using namespace Attr;
using namespace Lighting;
using namespace BaseGameFeature;
using namespace Toolkit;

namespace LevelEditor2
{
__ImplementClass(LevelEditor2EntityManager, 'LEEM', Game::Manager);
__ImplementSingleton(LevelEditor2EntityManager);

//------------------------------------------------------------------------------
/**
*/

LevelEditor2EntityManager::LevelEditor2EntityManager()
{
	__ConstructSingleton;
	
}

//------------------------------------------------------------------------------
/**
*/
LevelEditor2EntityManager::~LevelEditor2EntityManager()
{
	__DestructSingleton;

	this->entityTreeWidget = 0;
}

//------------------------------------------------------------------------------
/**
*/
Math::matrix44 
LevelEditor2EntityManager::GetPlacementTransform()
{
	BaseGameFeature::UserProfile* userProfile = BaseGameFeature::LoaderServer::Instance()->GetUserProfile();  
	if(!userProfile->GetBool("CreateCameraRelative"))
	{
		return Math::matrix44::identity();
	}
	else
	{
	
		// get camera
		const Ptr<Graphics::CameraEntity>& cam = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView()->GetCameraEntity();

		// calculate inversed view matrix
		Math::matrix44 invView = Math::matrix44::inverse(cam->GetViewTransform());
		Math::float4 pos = invView.get_position();
		Math::vector dir(0,0,1);
		dir = Math::matrix44::transform(dir, invView);

		// calculate translation matrix for entity
		return Math::matrix44::translation(pos + dir * -10.0f);	
	}
}

//------------------------------------------------------------------------------
/**
*/
EntityGuid 
LevelEditor2EntityManager::CreateGameEntity(const Util::String& category, const Util::String& _template)
{
	Util::Array<Attribute> attributes;

	AttributeContainer attrs;

	if(_template == " -empty entity-")
	{
		attrs = Toolkit::EditorBlueprintManager::Instance()->GetCategoryAttributes(category); 
	}
	else
	{
		attrs = Toolkit::EditorBlueprintManager::Instance()->GetTemplate(category, _template);
	}
	
	Math::matrix44 trans = this->GetPlacementTransform();
	// get all values for template from category manager entry's valuetable

	// get some special attrs first (the ones we use for displaying)
	if(attrs.HasAttr(Attr::Graphics))
	{
		attributes.Append(attrs.GetAttr(Attr::Graphics));
	}
	else
	{
		// ok, we got no graphics. put in placeholder
		attributes.Append(Attribute(Attr::Graphics,"system/placeholder"));
	}		
	attributes.Append(Attribute(Transform, trans));
	attributes.Append(Attribute(Attr::EntityType,Game));
	attributes.Append(Attribute(Attr::EntityCategory,category));
	attributes.Append(attrs.GetAttr(Attr::Id));
	
	if(attrs.HasAttr(Attr::Transform))
	{
		attrs.RemoveAttr(Attr::Transform);
	}
	
	Ptr<Game::Entity> newEnt = CreateEntityByAttrs(attributes);
	
	Ptr<SetAttributes> attrMsg = SetAttributes::Create();
	attrMsg->SetAttrs(attrs);
	newEnt->SendSync(attrMsg.cast<Messaging::Message>());	

	return newEnt->GetGuid(Attr::EntityGuid);
}


//------------------------------------------------------------------------------
/**
*/
LevelEditor2::EntityGuid  
LevelEditor2EntityManager::CreateTransformEntity(const Util::String & name)
{

	Util::Array<Attribute> attributes;
	
	attributes.Append(Attribute(Attr::Id, name));
	attributes.Append(Attribute(Transform, Math::matrix44::identity()));
	attributes.Append(Attribute(Attr::EntityType,Group));	
	attributes.Append(Attribute(Attr::EntityCategory,"Transform"));
	Ptr<Game::Entity> newEnt = CreateEntityByAttrs(attributes, "EditorTransform");	
    return newEnt->GetGuid(Attr::EntityGuid);	
}

//------------------------------------------------------------------------------
/**
*/
LevelEditor2::EntityGuid  
LevelEditor2EntityManager::CreateNavMeshEntity()
{

	Util::Array<Attribute> attributes;

	attributes.Append(Attribute(Attr::Id, "NavMesh"));
	attributes.Append(Attribute(Transform, Math::matrix44::identity()));
	attributes.Append(Attribute(Attr::EntityType,NavMesh));	
	attributes.Append(Attribute(Attr::EntityCategory,"NavMesh"));
	Ptr<Game::Entity> newEnt = CreateEntityByAttrs(attributes, "EditorNavMesh");
    return newEnt->GetGuid(Attr::EntityGuid);			
}

//------------------------------------------------------------------------------
/**
*/
LevelEditor2::EntityGuid 
LevelEditor2EntityManager::CreateEnvironmentEntity(const Util::String& _template)
{
	Util::Array<Attribute> attributes;

	Math::matrix44 trans = this->GetPlacementTransform();

	attributes.Append(Attribute(Attr::Graphics, _template));
	attributes.Append(Attribute(Attr::Id, _template.ExtractFileName()));	
	attributes.Append(Attribute(Transform, trans));
	attributes.Append(Attribute(Attr::EntityType,Environment));
	attributes.Append(Attribute(Attr::CollisionEnabled,true));
	attributes.Append(Attribute(Attr::CastShadows, true));
	attributes.Append(Attribute(Attr::DynamicObject,false));
	attributes.Append(Attribute(Attr::VelocityVector,Math::float4(0, 0, 0, 0)));
	attributes.Append(Attribute(Attr::EntityCategory,"_Environment"));
	attributes.Append(Attribute(Attr::PhysicMaterial, "EmptyWood"));

	Ptr<Game::Entity> newEnt =  CreateEntityByAttrs(attributes);	
	return newEnt->GetGuid(Attr::EntityGuid);	
}

//------------------------------------------------------------------------------
/**
*/
LevelEditor2::EntityGuid
LevelEditor2EntityManager::CreateLightProbeEntity()
{
	Util::Array<Attribute> attributes;

	attributes.Append(Attribute(Attr::Id, "lightprobe"));
	attributes.Append(Attribute(Transform, Math::matrix44::identity()));
	attributes.Append(Attribute(Attr::EntityType, Probe));
	attributes.Append(Attribute(Attr::EntityCategory, "LightProbe"));
	Ptr<Game::Entity> newEnt = CreateEntityByAttrs(attributes, "EditorLightProbe");
	return newEnt->GetGuid(Attr::EntityGuid);
}

//------------------------------------------------------------------------------
/**
*/
LevelEditor2::EntityGuid 
LevelEditor2EntityManager::CreateNavArea()
{
    Util::Array<Attribute> attributes;

    Math::matrix44 trans = this->GetPlacementTransform();

    attributes.Append(Attribute(Attr::Graphics, "system/placeholder"));
    attributes.Append(Attribute(Attr::Id, "NavigationArea"));	
    attributes.Append(Attribute(Transform, trans));
    attributes.Append(Attribute(Attr::EntityType,NavMeshArea));    
    attributes.Append(Attribute(Attr::EntityCategory,"_NavigationArea"));
    attributes.Append(Attribute(Attr::NavMeshAreaFlags,1));
	attributes.Append(Attribute(Attr::NavMeshAreaCost, 1));


    Ptr<Game::Entity> newEnt =  CreateEntityByAttrs(attributes,"EditorNavAreaMarker");	
    return newEnt->GetGuid(Attr::EntityGuid);	
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Game::Entity>
LevelEditor2EntityManager::CreateEntityByAttrs( const Util::Array<Attr::Attribute>& attributes, const Util::String & entityclass, const Util::String &guidStr)
{
	Ptr<Game::Entity> entity = BaseGameFeature::FactoryManager::Instance()->CreateEntityByAttrs(entityclass, attributes);
	    
    Util::Guid entguid;
    if(guidStr.Length())
    {
        entguid = Util::Guid::FromString(guidStr);
    }
    else
    {
        entguid.Generate();	
    }
    
    entity->SetGuid(Attr::EntityGuid,entguid);
    entity->SetString(Attr::EntityLevel, Level::Instance()->GetName());
    //entity->SetString(Attr::_ID, entguid.AsString());
	// attach game entity to world
	this->AddEntity(entity);
	
	return entity;
}

//------------------------------------------------------------------------------
/**
*/
EntityGuid 
LevelEditor2EntityManager::DuplicateEntity(const Ptr<Game::Entity>& entity)
{
	Ptr<Game::Entity> newentity = BaseGameFeature::FactoryManager::Instance()->CreateEntityByEntity(entity);

    Util::Guid newguid;
    newguid.Generate();	
    newentity->SetGuid(Attr::EntityGuid,newguid);	

	this->AddEntity(newentity);
		
	if(entity->GetInt(Attr::EntityType) ==  Game)
	{
		Ptr<GetEntityValues> gmsg = GetEntityValues::Create();
		entity->SendSync(gmsg.cast<Messaging::Message>());
		Ptr<SetAttributes> smsg = SetAttributes::Create();
		Ptr<SetProperties> pmsg = SetProperties::Create();		
		Attr::AttributeContainer attrs = gmsg->GetAttrs();		
		smsg->SetAttrs(attrs);
		pmsg->SetProps(gmsg->GetProps());
		newentity->SendSync(smsg.cast<Messaging::Message>());
		newentity->SendSync(pmsg.cast<Messaging::Message>());				
	}	

	// make sure it's not selected, since our currently selected entity will surely be!
	newentity->SetBool(Attr::IsSelected, false);
	Ptr<Layers::LayerHandler> handler = LevelEditor2App::Instance()->GetWindow()->GetLayerHandler();
	handler->HandleEntityCreated(newentity);
	return newguid;
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<EntityGuid>
LevelEditor2EntityManager::DuplicateEntities(const Util::Array<Ptr<Game::Entity>> & entities)
{
	Dictionary<EntityGuid,EntityGuid> parentMap;
	
	Array<EntityGuid> newGuids;
	for(IndexT i = 0 ; i < entities.Size() ; i++)
	{
		EntityGuid newGuid = this->DuplicateEntity(entities[i]);
		newGuids.Append(newGuid);
		parentMap.Add(entities[i]->GetGuid(Attr::EntityGuid), newGuid);
	}

	for(IndexT i = 0 ; i < newGuids.Size() ; i++)
	{
		Ptr<Game::Entity> newEnt = this->GetEntityById(newGuids[i]);
		EntityGuid oldParent = newEnt->GetGuid(Attr::ParentGuid);
		if(parentMap.Contains(oldParent))
		{
			EntityGuid newParentGuid = this->GetEntityById(parentMap[oldParent])->GetGuid(Attr::EntityGuid);
			newEnt->SetGuid(Attr::ParentGuid,newParentGuid);
		}
	}
	LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->RebuildTree();	
	return newGuids;
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2EntityManager::CreateEntityFromAttrContainer(const Util::String & levelName, const Util::String & category, Attr::AttributeContainer attrs)
{
	Util::Array<Attr::Attribute> at = attrs.GetAttrs().ValuesAsArray();				

	if(attrs.HasAttr(Attr::_ID))
	{
		Attr::Attribute idat = attrs.GetAttr(Attr::_ID);
		Util::Array<Attr::Attribute>::Iterator idx = at.Find(idat);
		at.Erase(idx);
	}
	// grab the layers attribute in case it exists and remove it from the container
	Util::String layers = "Default";
	if(attrs.HasAttr(Attr::_Layers))
	{ 
		layers = attrs.GetString(Attr::_Layers);
		attrs.RemoveAttr(Attr::_Layers);
	} 
	Ptr<Game::Entity> newEnt;
	if(category == "_Environment")
	{				
		at.Append(Attribute(Attr::EntityType, Environment));
		at.Append(Attribute(Attr::EntityCategory, "_Environment"));					
		if (!attrs.HasAttr(Attr::CollisionEnabled))
		{
			at.Append(Attribute(Attr::CollisionEnabled, true));
		}
		if (!attrs.HasAttr(Attr::DynamicObject))
		{
			at.Append(Attribute(Attr::DynamicObject, false));
		}
		if (!attrs.HasAttr(Attr::CastShadows))
		{
			at.Append(Attribute(Attr::CastShadows, true));
		}
		if (!attrs.HasAttr(Attr::PhysicMaterial))
		{
			at.Append(Attribute(Attr::PhysicMaterial, "EmptyWood"));
		}				
		if (!attrs.HasAttr(Attr::LoadSynced))
		{
			at.Append(Attribute(Attr::LoadSynced, false));
		}		
		if (!attrs.HasAttr(Attr::StartAnimation))
		{
			at.Append(Attribute(Attr::StartAnimation, ""));
		}
        if (!attrs.HasAttr(Attr::VelocityVector))
        {
            at.Append(Attribute(Attr::VelocityVector, Math::vector(0)));
        }
		newEnt = CreateEntityByAttrs(at,"EditorEntity", attrs.GetGuid(Attr::Guid).AsString());		
		newEnt->SetString(Attr::EntityLevel, levelName);
	}
	else if(category == "Light")
	{	
		at.Append(Attribute(Attr::EntityType,Light));
		at.Append(Attribute(Attr::EntityCategory,"Light"));
		at.Append(Attribute(Attr::Graphics,"system/placeholder"));
		newEnt = CreateEntityByAttrs(at,"EditorLight", attrs.GetGuid(Attr::Guid).AsString());
		const Util::Dictionary<AttrId, Attribute>& arr = attrs.GetAttrs();
		for (int i = 0; i < arr.Size();i++)
		{
			if(newEnt->HasAttr(arr.KeyAtIndex(i)))
			{
				newEnt->SetAttr(arr.ValueAtIndex(i));
			}
		}
		newEnt->SetString(Attr::EntityLevel, levelName);
	}
	else if(category == "_Group")
	{
		at.Append(Attribute(Attr::EntityType,Group));
		at.Append(Attribute(Attr::EntityCategory,"Transform"));
		newEnt = CreateEntityByAttrs(at,"EditorTransform", attrs.GetGuid(Attr::Guid).AsString());
        newEnt->SetString(Attr::EntityLevel, levelName);
	}
    else if(category == "NavMeshData")
    {
        at.Append(Attribute(Attr::EntityType,NavMesh));   
        at.Append(Attribute(Attr::EntityCategory,"EditorNavMesh"));
        at.Append(Attribute(Attr::EntityGuid,attrs.GetGuid(Attr::Guid)));
        at.Append(Attribute(Attr::EntityLevel,Level::Instance()->GetName()));
        newEnt = CreateEntityByAttrs(at,"EditorNavMesh",attrs.GetGuid(Attr::Guid).AsString());
		newEnt->SetString(Attr::EntityLevel, levelName);
    }
    else if(category == "_NavigationArea")
    {
        at.Append(Attribute(Attr::EntityType, NavMeshArea));   
        at.Append(Attribute(Attr::EntityCategory, "_NavigationArea"));
        at.Append(Attribute(Attr::EntityGuid, attrs.GetGuid(Attr::Guid)));
        at.Append(Attribute(Attr::EntityLevel, Level::Instance()->GetName()));    
        at.Append(Attribute(Attr::NavMeshAreaCost, attrs.GetInt(Attr::NavMeshAreaCost)));
		at.Append(Attribute(Attr::NavMeshAreaFlags, attrs.GetInt(Attr::NavMeshAreaFlags)));
        newEnt = CreateEntityByAttrs(at,"EditorNavAreaMarker",attrs.GetGuid(Attr::Guid).AsString());
        newEnt->SetString(Attr::EntityLevel, levelName);
    }
	else if (category == "LightProbe")
	{
		at.Append(Attribute(Attr::EntityType, Probe));
		at.Append(Attribute(Attr::EntityCategory, "LightProbe"));
		newEnt = CreateEntityByAttrs(at, "EditorLightProbe", attrs.GetGuid(Attr::Guid).AsString());
		newEnt->SetString(Attr::EntityLevel, levelName);
	}
	else 
	{
		if (Toolkit::EditorBlueprintManager::Instance()->HasCategory(category))
		{
			// add all available attributes to container (in case property was added to blueprint type after level creation)
			Util::Array<Attr::Attribute> allattrs = Toolkit::EditorBlueprintManager::Instance()->GetCategoryAttributes(category).GetAttrs().ValuesAsArray();

			IndexT i;
			for (i = 0; i < allattrs.Size(); i++)
			{
				if (!attrs.HasAttr(allattrs[i].GetAttrId()))
				{
					attrs.SetAttr(allattrs[i]);
				}
			}


			Util::Array<Attribute> attributes;

			if (attrs.HasAttr(Attr::Graphics))
			{
				attributes.Append(Attribute(Attr::Graphics, attrs.GetString(Attr::Graphics)));
			}
			else
			{
				// ok, we got no graphics. put in placeholder
				attributes.Append(Attribute(Attr::Graphics, "system/placeholder"));
			}

			attributes.Append(Attribute(VelocityVector, Math::float4()));
			attributes.Append(attrs.GetAttr(Attr::Transform));
			attributes.Append(Attribute(Attr::EntityType, Game));
			attributes.Append(Attribute(Attr::EntityCategory, category));
			attributes.Append(attrs.GetAttr(Attr::Id));

			newEnt = CreateEntityByAttrs(attributes, "EditorEntity", attrs.GetGuid(Attr::Guid).AsString());
			newEnt->SetString(Attr::EntityLevel, levelName);
			if (attrs.HasAttr(Attr::ParentGuid))
			{
				newEnt->SetGuid(Attr::ParentGuid, attrs.GetGuid(Attr::ParentGuid));
			}
			else
			{
				Util::Guid dummy;
				newEnt->SetGuid(Attr::ParentGuid, dummy);
			}

			attrs.RemoveAttr(Attr::Guid);
			attrs.RemoveAttr(Attr::Transform);
			if (attrs.HasAttr(Attr::ParentGuid))
			{
				attrs.RemoveAttr(Attr::ParentGuid);
			}

			Ptr<SetAttributes> attrMsg = SetAttributes::Create();
			attrMsg->SetAttrs(attrs);
			newEnt->SendSync(attrMsg.cast<Messaging::Message>());
		}
		else
		{
			Util::String format;
			format.Format("Object with unknown category \"%s\"found", category.AsCharPtr());
			n_message(format.AsCharPtr());
			return;
		}
	}
	// finally apply layer
	newEnt->SetString(Attr::_Layers, layers);

	Ptr<Layers::LayerHandler> handler = LevelEditor2App::Instance()->GetWindow()->GetLayerHandler();
	handler->HandleEntityCreated(newEnt);
}


//------------------------------------------------------------------------------
/**
*/
bool 
LevelEditor2EntityManager::EntityExists(EntityGuid id) const
{
	return BaseGameFeature::EntityManager::Instance()->ExistsEntityByAttr(Attribute(Attr::EntityGuid, id));
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2EntityManager::AddEntity(const Ptr<Game::Entity>& entity)
{
	// attach game entity to world
	BaseGameFeature::EntityManager::Instance()->AttachEntity(entity);

	// set kinematic for the entity, meaning object-to-object collision is disabled but picking is still enabled
	Ptr<PhysicsFeature::SetKinematic> setKinematicMessage = PhysicsFeature::SetKinematic::Create();
	setKinematicMessage->SetEnabled(true);
	entity->SendSync(setKinematicMessage.upcast<Messaging::Message>());	
	// add it to the treeview	
	this->AddTreeviewNode(entity);	
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2EntityManager::RemoveEntity(EntityGuid id)
{
	Ptr<Game::Entity> entity = this->GetEntityById(id);

	this->RemoveEntity(entity);
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2EntityManager::RemoveEntity(const Ptr<Game::Entity>& entity, bool immediate)
{
	// update the treeview	
	this->RemoveTreeviewNode(entity);	
	// remove the entity from the world
    if (immediate)
    {
        BaseGameFeature::EntityManager::Instance()->DeleteEntityImmediate(entity);
    }
    else
    {
        BaseGameFeature::EntityManager::Instance()->DeleteEntity(entity);
    }	
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2EntityManager::AddTreeviewNode(const Ptr<Game::Entity>& node)
{
	Util::String text = node->GetAttr(Attr::Id).GetString();

	EntityTreeItem* item = new EntityTreeItem();
	item->setText(0, text.AsCharPtr());
	item->SetEntityGuid(node->GetGuid(Attr::EntityGuid));

	int entityType = node->GetInt(Attr::EntityType);
	switch (entityType)
	{
		case Game:
		{
			Util::String cat = node->GetString(Attr::EntityCategory);
			item->SetCategory(cat);		
		}
		break;
		case Environment:
		{
			Util::String model = node->GetString(Attr::Graphics);
			item->SetCategory("Environment ("+model + ")");
		}
			break;
		case Light:
		{
			int lightType = node->GetInt(Attr::LightType);
			switch (lightType)
			{
			case 0:
				item->SetCategory("Global Light");
				break;
			case 1:
				item->SetCategory("Spotlight");
				break;
			case 2:
				item->SetCategory("Pointlight");
				break;
			default:
				break;
			}	
		}
		break;
		default:
		break;
	}

	this->entityTreeWidget->AddEntityTreeItem(item);
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2EntityManager::RemoveTreeviewNode(const Ptr<Game::Entity>& node)
{
	this->entityTreeWidget->RemoveEntityTreeItem(node->GetGuid(Attr::EntityGuid));
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Game::Entity> 
LevelEditor2EntityManager::GetEntityById(EntityGuid id) const
{
	n_assert(EntityExists(id));
	return BaseGameFeature::EntityManager::Instance()->GetEntityByAttr(Attribute(Attr::EntityGuid, id));
}

//------------------------------------------------------------------------------
/**
*/
LevelEditor2::EntityGuid 
LevelEditor2EntityManager::CreateLightEntity(const Util::String& lightType)
{
	Util::Array<Attr::Attribute> attributes;
	Ptr<Game::Entity> entity;

	LightType::Code lightTypeCode = LightType::InvalidLightType;
	Util::String id;

	Math::matrix44 trans = this->GetPlacementTransform();

	if(lightType == "GlobalLight")
	{
		Util::Array<Ptr<Game::Entity> > ents = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::LightType, (int)Lighting::LightType::Global));
		for(IndexT i = 0 ; i < ents.Size(); i++)
		{
			if(!BaseGameFeature::EntityManager::Instance()->IsEntityInDelayedJobs(ents[i]))
			{
				return ents[i]->GetGuid(Attr::Guid);
			}
		}		
		attributes.Append(Attr::Attribute(Attr::Graphics, "system/sphere"));
		attributes.Append(Attr::Attribute(Attr::Placeholder, "system/placeholder"));
		attributes.Append(Attr::Attribute(Attr::LightCastShadows,false));
		lightTypeCode = LightType::Global;
		id = "Global Light";		
		trans = Math::matrix44::identity();
	}
	else if(lightType == "PointLight")
	{
		attributes.Append(Attr::Attribute(Attr::Graphics, "system/pointlightshape"));
		attributes.Append(Attr::Attribute(Attr::Placeholder, "system/pointlight"));	
		attributes.Append(Attr::Attribute(Attr::LightCastShadows,false));
		lightTypeCode = LightType::Point;
		id = "Point Light";
	}
	else if(lightType == "SpotLight")
	{
		attributes.Append(Attr::Attribute(Attr::Graphics, "system/spotlightshape"));
		attributes.Append(Attr::Attribute(Attr::Placeholder, "system/spotlight"));
		attributes.Append(Attr::Attribute(Attr::LightCastShadows,false));
		lightTypeCode = LightType::Spot;
		id = "Spot Light";
	}
	else
	{
		n_error("LevelEditor2EntityManager::CreateLightEntity: Unsupported light type: '%s'", lightType.AsCharPtr());
	}

	
		

	entity = BaseGameFeature::FactoryManager::Instance()->CreateEntityByAttrs("EditorLight", attributes);

	entity->SetMatrix44(Attr::Transform, trans);
	entity->SetInt(Attr::LightType, lightTypeCode);
	entity->SetString(Attr::Id, id);

	entity->SetInt(Attr::EntityType,Light);
	entity->SetString(Attr::EntityCategory,"Light");

    Util::Guid guid;
    guid.Generate();
    entity->SetGuid(Attr::EntityGuid,guid);
	entity->SetString(Attr::EntityLevel, Level::Instance()->GetName());

	// attach entitiy to the world
	this->AddEntity(entity);

	// enable set kinematic for the entity, meaning object-to-object collision is disabled but picking is still enabled
	Ptr<PhysicsFeature::SetKinematic> setKinematicMessage = PhysicsFeature::SetKinematic::Create();
	setKinematicMessage->SetEnabled(true);
	entity->SendSync(setKinematicMessage.upcast<Messaging::Message>());	

	return entity->GetGuid(Attr::EntityGuid);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Game::Entity> 
LevelEditor2EntityManager::GetGlobalLight()
{
	return BaseGameFeature::EntityManager::Instance()->GetEntityByAttr(Attr::Attribute(Attr::LightType, (int)Lighting::LightType::Global), false);
}


//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2EntityManager::RemoveAllEntities(bool immediate)
{
	// FIXME ugly
	LevelEditor2App::Instance()->GetCurrentStateHandler().cast<LevelEditorState>()->ClearSelection();

	Util::Array<Ptr<Game::Entity>> entities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Environment));	
	for(IndexT i = 0; i< entities.Size();i++)
	{
		this->RemoveEntity(entities[i], immediate);
	}
	entities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Game));	
	for(IndexT i = 0; i< entities.Size();i++)
	{
		this->RemoveEntity(entities[i], immediate);
	}
	entities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Light));	
	for(IndexT i = 0; i< entities.Size();i++)
	{
		this->RemoveEntity(entities[i], immediate);
	}
	entities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,Group));	
	for(IndexT i = 0; i< entities.Size();i++)
	{
		this->RemoveEntity(entities[i], immediate);
	}
    entities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,NavMesh));	
    for(IndexT i = 0; i< entities.Size();i++)
    {
        this->RemoveEntity(entities[i], immediate);
    }
	entities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType, Probe));
	for (IndexT i = 0; i < entities.Size(); i++)
	{
		this->RemoveEntity(entities[i], immediate);
	}
    entities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityType,NavMeshArea));	
    for(IndexT i = 0; i< entities.Size();i++)
    {
        this->RemoveEntity(entities[i], immediate);
    }
}

//------------------------------------------------------------------------------
/** 
	LevelEditor2EntityManager::UpdateCategoryEntities
	update all existing entities and add/remove attributes that are no longer present or have been added through the blueprint change
*/
void
LevelEditor2EntityManager::UpdateCategoryEntities(const Util::String & category)
{
	Util::Array<Attr::Attribute> matchAttrs;
	matchAttrs.Append(Attribute(Attr::EntityType,Game));
	matchAttrs.Append(Attribute(Attr::EntityCategory,category));

	Util::Array<Ptr<Game::Entity> > entities = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttrs(matchAttrs);

	IndexT i;
	for(i=0;i<entities.Size();i++)
	{
		Ptr<GetEntityValues> msg = GetEntityValues::Create();
		entities[i]->SendSync(msg.cast<Messaging::Message>());		
		Util::Array<Attribute> oldAttrArray = msg->GetAttrs().GetAttrs().ValuesAsArray();

		Attr::AttributeContainer newAttrs = EditorBlueprintManager::Instance()->GetCategoryAttributes(category);
		if (newAttrs.HasAttr(Attr::Transform))
		{
			newAttrs.RemoveAttr(Attr::Transform);
		}

		IndexT j;
		for(j=0;j<oldAttrArray.Size();j++)
		{
			if(newAttrs.HasAttr(oldAttrArray[j].GetAttrId()))
			{
				newAttrs.SetAttr(oldAttrArray[j]);
			}
		}
		Ptr<SetAttributes> smsg = SetAttributes::Create();
		smsg->SetAttrs(newAttrs);
		entities[i]->SendSync(smsg.cast<Messaging::Message>());
	}		
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2EntityManager::MorphEntity(Ptr<Game::Entity> entity, const Util::String & target)
{
	Util::String oldcategory = entity->GetString(Attr::EntityCategory);
	if (oldcategory == target)
	{
		return;
	}
	
	if (target == "_Environment")
	{
		entity->SetInt(Attr::EntityType, Environment);
		Attr::AttributeContainer dummy;
		Ptr<SetAttributes> smsg = SetAttributes::Create();
		smsg->SetAttrs(dummy);
		__SendSync(entity, smsg);		
	}
	else if (entity->GetInt(Attr::EntityType) == Environment)
	{
		Attr::AttributeContainer newAttrs = EditorBlueprintManager::Instance()->GetCategoryAttributes(target);
		if (newAttrs.HasAttr(Attr::Graphics))
		{			
			newAttrs.SetAttr(entity->GetAttr(Attr::Graphics));
		}
		if (newAttrs.HasAttr(Attr::Transform))
		{
			newAttrs.RemoveAttr(Attr::Transform);
		}
		entity->SetInt(Attr::EntityType, Game);
		Ptr<SetAttributes> smsg = SetAttributes::Create();
		smsg->SetAttrs(newAttrs);
		__SendSync(entity, smsg);		
	}
	else
	{
		Ptr<GetEntityValues> msg = GetEntityValues::Create();
		__SendSync(entity, msg);
		Util::Array<Attribute> oldAttrArray = msg->GetAttrs().GetAttrs().ValuesAsArray();

		Attr::AttributeContainer newAttrs = EditorBlueprintManager::Instance()->GetCategoryAttributes(target);
		if (newAttrs.HasAttr(Attr::Transform))
		{
			newAttrs.RemoveAttr(Attr::Transform);
		}

		IndexT j;
		for (j = 0; j < oldAttrArray.Size(); j++)
		{
			if (newAttrs.HasAttr(oldAttrArray[j].GetAttrId()))
			{
				newAttrs.SetAttr(oldAttrArray[j]);
			}
		}
		Ptr<SetAttributes> smsg = SetAttributes::Create();
		smsg->SetAttrs(newAttrs);
		__SendSync(entity, smsg);		
	}
	entity->SetString(Attr::EntityCategory, target);
	LevelEditor2App::Instance()->GetCurrentStateHandler().cast<LevelEditor2::LevelEditorState>()->ClearSelection();
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2EntityManager::LoadLevel(const Util::String & name, Level::LoadMode mode)
{
    this->delayedLevel = name;
    this->delayedMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2EntityManager::OnEndFrame()
{
    Manager::OnEndFrame();
    if (!this->delayedLevel.IsEmpty())
    {
        Level::Instance()->LoadLevel(this->delayedLevel, this->delayedMode);
        this->delayedLevel.Clear();
    }
}

} // namespace LevelEditor2
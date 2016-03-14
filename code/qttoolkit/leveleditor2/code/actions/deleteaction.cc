//------------------------------------------------------------------------------
//  deleteaction.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "deleteaction.h"
#include "leveleditor2entitymanager.h"
#include "leveleditor2app.h"
#include "managers/factorymanager.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "properties/editorproperty.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "physicsfeature/physicsprotocol.h"

using namespace BaseGameFeature;
using namespace Util;

//-----------------------------------------------
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::DeleteAction, 'LEDA', LevelEditor2::Action);

//------------------------------------------------------------------------------
/**
*/
DeleteAction::DeleteAction():
performed(false)
{

}

//------------------------------------------------------------------------------
/**
*/
void 
DeleteAction::Perform()
{
	n_assert(!this->entities.IsEmpty());
	n_assert(!this->performed);

	Ptr<LevelEditor2EntityManager> manager = LevelEditor2EntityManager::Instance();

	EntityTreeWidget * tree = LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget();

	IndexT i;
	for (i = 0; i < this->entities.Size(); i++)
	{		
		// ignore global light
		if(this->entities[i]->GetInt(Attr::EntityType) == Light && this->entities[i]->GetInt(Attr::LightType) == Lighting::LightType::Global)
		{
			continue;
		}		
		Util::Guid parentGuid = tree->GetParent(this->entities[i]->GetGuid(Attr::EntityGuid));
		if(parentGuid.IsValid())
		{
			Ptr<Game::Entity> parent = LevelEditor2EntityManager::Instance()->GetEntityById(parentGuid);
			parentGuid = parent->GetGuid(Attr::EntityGuid);
		}
		this->entities[i]->SetGuid(Attr::ParentGuid, parentGuid);
		// create copies of the old instances but don't attach them
		this->savedEntities.Append(BaseGameFeature::FactoryManager::Instance()->CreateEntityByEntity(this->entities[i]));
		
		// save all attributes from attributecontainer as well
		// FIXME put stuff in some global table instead, adressed by guid, this is ugly
		if(this->entities[i]->GetInt(Attr::EntityType) ==  Game)
		{
			Ptr<GetEntityValues> gmsg = GetEntityValues::Create();
			this->entities[i]->SendSync(gmsg.cast<Messaging::Message>());
			Ptr<SetAttributes> smsg = SetAttributes::Create();
			Ptr<SetProperties> pmsg = SetProperties::Create();
			smsg->SetAttrs(gmsg->GetAttrs());
			pmsg->SetProps(gmsg->GetProps());
			this->attrMessages.Append(smsg);
			this->propMessages.Append(pmsg);			
		}
		
		manager->RemoveEntity(this->entities[i]);
		LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->HandleEntityDeleted(this->entities[i]);
	}
	this->entities.Clear();

	LevelEditor2App::Instance()->GetCurrentStateHandler().cast<LevelEditor2::LevelEditorState>()->ClearSelection();

	this->performed = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
DeleteAction::Undo()
{
	n_assert(!this->savedEntities.IsEmpty());
	n_assert(this->performed);

	Ptr<LevelEditor2EntityManager> manager = LevelEditor2EntityManager::Instance();

	// attach the copies
	IndexT i,j;
	for (i = 0 , j = 0; i < this->savedEntities.Size(); i++)
	{		
		LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->HandleEntityCreated(this->savedEntities[i]);
		manager->AddEntity(this->savedEntities[i]);	
		if(savedEntities[i]->GetInt(Attr::EntityType) ==  Game)
		{
			savedEntities[i]->SendSync(attrMessages[j].cast<Messaging::Message>());
			savedEntities[i]->SendSync(propMessages[j++].cast<Messaging::Message>());
		}		
	}


	// recreate parent relations

	EntityTreeWidget * tree = LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget();
	for (i = 0 ; i < this->savedEntities.Size(); i++)
	{
		Guid parentGuid = this->savedEntities[i]->GetGuid(Attr::ParentGuid);
		if(parentGuid.IsValid())
		{
			Ptr<Game::Entity> parent = EntityManager::Instance()->GetEntityByAttr(Attr::Attribute(Attr::EntityGuid,parentGuid));
			if(parent.isvalid())
			{
				EntityTreeItem* treeItem = dynamic_cast<EntityTreeItem*>(tree->GetEntityTreeItem(savedEntities[i]->GetGuid(Attr::EntityGuid)));				
				EntityTreeItem* parentTreeItem = dynamic_cast<EntityTreeItem*>(tree->GetEntityTreeItem(parent->GetGuid(Attr::EntityGuid)));			
				int idx = tree->indexOfTopLevelItem (treeItem);
				tree->takeTopLevelItem(idx);
				parentTreeItem->addChild(treeItem);
			}
		}
	}
	this->savedEntities.Clear();
	this->attrMessages.Clear();
	this->propMessages.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
DeleteAction::SetEntities(const Util::Array<Ptr<Game::Entity> >& _entities)
{	
	this->entities = _entities;	
}

} // namespace LevelEditor2
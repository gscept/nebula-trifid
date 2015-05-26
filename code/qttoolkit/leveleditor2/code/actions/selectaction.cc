//------------------------------------------------------------------------------
//  selectaction.cc
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "selectaction.h"
#include "basegamefeature/basegameprotocol.h"
#include "leveleditor2app.h"
#include "leveleditor2entitymanager.h"
#include "entityutils/selectionutil.h"
#include "leveleditor2protocol.h"
#include "managers/attributewidgetmanager.h"
#include "properties/editorproperty.h"
#include "entityutils/placementutil.h"

Ptr<Game::Entity> LevelEditor2::SelectAction::multiEntity;
//-----------------------------------------------
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::SelectAction, 'LESC', LevelEditor2::Action);

//------------------------------------------------------------------------------
/**
*/
void 
SelectAction::Perform()
{	
	if (!this->multiEntity.isvalid())
	{

		Util::Array<Attr::Attribute> attributes;
		attributes.Append(Attr::Attribute(Attr::EntityType, Game));
		attributes.Append(Attr::Attribute(Attr::EntityCategory, "Multi"));
		this->multiEntity = BaseGameFeature::FactoryManager::Instance()->CreateEntityByAttrs("EditorMultiselect", attributes);
		BaseGameFeature::EntityManager::Instance()->AttachEntity(this->multiEntity);
	}

	const Ptr<Layers::LayerHandler> layerHandler = LevelEditor2App::Instance()->GetWindow()->GetLayerHandler();
	EntityTreeWidget * tree = LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget();

	switch(this->mode)
	{
		case ClearSelection:
			{
				this->lastEntityIds = SelectionUtil::Instance()->GetSelectedEntityIds();

				for(IndexT i = 0 ; i < lastEntityIds.Size() ; i++)
				{				
					tree->RemoveFromSelection(this->lastEntityIds[i]);				
					LevelEditor2EntityManager::Instance()->GetEntityById(this->lastEntityIds[i])->SetBool(Attr::IsSelected,false);
				}

				SelectionUtil::Instance()->ClearSelection();
				AttributeWidgetManager::Instance()->ClearAttributeControllers();	
				layerHandler->HandleEntitySelected(0);
			}			
			break;
		case AppendSelection:
			{
				SelectionUtil::Instance()->AppendToSelection(this->entityIds);
				
				for(IndexT i = 0 ; i < entityIds.Size() ; i++)
				{				
					tree->AppendToSelection(this->entityIds[i]);
					LevelEditor2EntityManager::Instance()->GetEntityById(this->entityIds[i])->SetBool(Attr::IsSelected,true);					
				}

				AttributeWidgetManager::Instance()->ClearAttributeControllers();		
				Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();
				if(ents.Size() == 1)
				{
					AttributeWidgetManager::Instance()->ViewEntityAttributes(ents[0]);
					layerHandler->HandleEntitySelected(ents[0]);
				}
				else
				{					
					Ptr<SetMultiSelection> smsg = SetMultiSelection::Create();
					smsg->SetEntities(SelectionUtil::Instance()->GetSelectedEntities());
					__SendSync(this->multiEntity, smsg);
					AttributeWidgetManager::Instance()->ViewEntityAttributes(this->multiEntity);
				}
			}
			break;
		case RemoveSelection:
			{
				SelectionUtil::Instance()->RemoveFromSelection(this->entityIds);
				for(IndexT i = 0 ; i < entityIds.Size() ; i++)
				{				
					tree->RemoveFromSelection(this->entityIds[i]);
					LevelEditor2EntityManager::Instance()->GetEntityById(this->entityIds[i])->SetBool(Attr::IsSelected,false);
				}

				AttributeWidgetManager::Instance()->ClearAttributeControllers();		
				Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();
				if(ents.Size() == 1)
				{
					AttributeWidgetManager::Instance()->ViewEntityAttributes(ents[0]);
					layerHandler->HandleEntitySelected(ents[0]);
				}
			}
			break;
		case SetSelection:
			{
				this->lastEntityIds = SelectionUtil::Instance()->GetSelectedEntityIds();
				for(IndexT i = 0 ; i < lastEntityIds.Size() ; i++)
				{				
					tree->RemoveFromSelection(this->lastEntityIds[i]);
					LevelEditor2EntityManager::Instance()->GetEntityById(this->lastEntityIds[i])->SetBool(Attr::IsSelected,false);
				}

				SelectionUtil::Instance()->SetSelection(this->entityIds);						
				for(IndexT i = 0 ; i < entityIds.Size() ; i++)
				{
					tree->AppendToSelection(this->entityIds[i]);
					LevelEditor2EntityManager::Instance()->GetEntityById(this->entityIds[i])->SetBool(Attr::IsSelected,true);										
				}

				AttributeWidgetManager::Instance()->ClearAttributeControllers();		
				Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();
				if(ents.Size() == 1)
				{
					AttributeWidgetManager::Instance()->ViewEntityAttributes(ents[0]);
					layerHandler->HandleEntitySelected(ents[0]);
				}
			}
	}
	this->SyncUtils();
}

//------------------------------------------------------------------------------
/**
*/
void 
SelectAction::Undo()
{
	EntityTreeWidget * tree = LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget();
	const Ptr<Layers::LayerHandler> layerHandler = LevelEditor2App::Instance()->GetWindow()->GetLayerHandler();
	switch(this->mode)
	{
	case ClearSelection:
		{
			SelectionUtil::Instance()->SetSelection(this->lastEntityIds);						
			for(IndexT i = 0 ; i < lastEntityIds.Size() ; i++)
			{
				tree->AppendToSelection(this->lastEntityIds[i]);
				LevelEditor2EntityManager::Instance()->GetEntityById(this->lastEntityIds[i])->SetBool(Attr::IsSelected,true);										
			}

			AttributeWidgetManager::Instance()->ClearAttributeControllers();		
			Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();
			if(ents.Size() == 1)
			{
				AttributeWidgetManager::Instance()->ViewEntityAttributes(ents[0]);
				layerHandler->HandleEntitySelected(ents[0]);
			}
		}			
		break;
	case AppendSelection:
		{
			SelectionUtil::Instance()->RemoveFromSelection(this->entityIds);
			for(IndexT i = 0 ; i < entityIds.Size() ; i++)
			{				
				tree->RemoveFromSelection(this->entityIds[i]);
				LevelEditor2EntityManager::Instance()->GetEntityById(this->entityIds[i])->SetBool(Attr::IsSelected,false);					
			}

			AttributeWidgetManager::Instance()->ClearAttributeControllers();		
			Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();
			if(ents.Size() == 1)
			{
				AttributeWidgetManager::Instance()->ViewEntityAttributes(ents[0]);
				layerHandler->HandleEntitySelected(ents[0]);
			}
		}
		break;
	case RemoveSelection:
		{
			SelectionUtil::Instance()->AppendToSelection(this->entityIds);
			for(IndexT i = 0 ; i < entityIds.Size() ; i++)
			{				
				tree->AppendToSelection(this->entityIds[i]);
				LevelEditor2EntityManager::Instance()->GetEntityById(this->entityIds[i])->SetBool(Attr::IsSelected,true);
			}
			AttributeWidgetManager::Instance()->ClearAttributeControllers();		
			Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();
			if(ents.Size() == 1)
			{
				AttributeWidgetManager::Instance()->ViewEntityAttributes(ents[0]);
				layerHandler->HandleEntitySelected(ents[0]);
			}
		}
		break;
	case SetSelection:
		{
			for(IndexT i = 0 ; i < entityIds.Size() ; i++)
			{				
				tree->RemoveFromSelection(this->entityIds[i]);
				LevelEditor2EntityManager::Instance()->GetEntityById(this->entityIds[i])->SetBool(Attr::IsSelected,false);
			}

			SelectionUtil::Instance()->SetSelection(this->lastEntityIds);						

			for(IndexT i = 0 ; i < lastEntityIds.Size() ; i++)
			{
				tree->AppendToSelection(this->lastEntityIds[i]);
				LevelEditor2EntityManager::Instance()->GetEntityById(this->lastEntityIds[i])->SetBool(Attr::IsSelected,true);										
			}

			AttributeWidgetManager::Instance()->ClearAttributeControllers();		
			Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();
			if(ents.Size() == 1)
			{
				AttributeWidgetManager::Instance()->ViewEntityAttributes(ents[0]);
				layerHandler->HandleEntitySelected(ents[0]);
			}
		}
	}
	this->SyncUtils();
}

//------------------------------------------------------------------------------
/**
*/
void 
SelectAction::SetEntities(const Util::Array<EntityGuid>& ids)
{
	this->entityIds = ids;
}

//------------------------------------------------------------------------------
/**
*/
void 
SelectAction::SetEntity(const EntityGuid & id)
{
	this->entityIds.Append(id);
}

//------------------------------------------------------------------------------
/**
*/
void 
SelectAction::SetSelectionMode( SelectAction::SelectMode inmode)
{	
	this->mode = inmode;
}

//------------------------------------------------------------------------------
/**
*/
void 
SelectAction::SyncUtils()
{

	Util::Array<Math::matrix44> entityMatrices;
	Util::Array<Ptr<Game::Entity> > selectedEntities = SelectionUtil::Instance()->GetSelectedEntities();

	IndexT i;
	for (i = 0; i < selectedEntities.Size(); i++)
	{
		entityMatrices.Append(selectedEntities[i]->GetMatrix44(Attr::Transform));
	}

	PlacementUtil::Instance()->SetSelection(entityMatrices);

}
} // namespace LevelEditor2
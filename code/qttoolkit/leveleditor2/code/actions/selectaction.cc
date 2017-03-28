//------------------------------------------------------------------------------
//  selectaction.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "selectaction.h"
#include "basegamefeature/basegameprotocol.h"
#include "leveleditor2app.h"
#include "leveleditor2entitymanager.h"
#include "entityutils/selectionutil.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "managers/attributewidgetmanager.h"
#include "properties/editorproperty.h"
#include "entityutils/placementutil.h"

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
				for(IndexT i = 0 ; i < entityIds.Size() ; i++)
				{				
					tree->AppendToSelection(this->entityIds[i]);
					LevelEditor2EntityManager::Instance()->GetEntityById(this->entityIds[i])->SetBool(Attr::IsSelected,true);					
				}
				SelectionUtil::Instance()->AppendToSelection(this->entityIds);

				AttributeWidgetManager::Instance()->ClearAttributeControllers();		
				Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();

				AttributeWidgetManager::Instance()->ViewEntityAttributes(ents);
				if (ents.Size() == 1)
				{					
					layerHandler->HandleEntitySelected(ents[0]);
				}				
			}
			break;
		case RemoveSelection:
			{				
				for(IndexT i = 0 ; i < entityIds.Size() ; i++)
				{				
					tree->RemoveFromSelection(this->entityIds[i]);
					LevelEditor2EntityManager::Instance()->GetEntityById(this->entityIds[i])->SetBool(Attr::IsSelected,false);
				}
				SelectionUtil::Instance()->RemoveFromSelection(this->entityIds);

				AttributeWidgetManager::Instance()->ClearAttributeControllers();		
				Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();
				AttributeWidgetManager::Instance()->ViewEntityAttributes(ents);
				if(ents.Size() == 1)
				{					
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
						
				for(IndexT i = 0 ; i < entityIds.Size() ; i++)
				{
					tree->AppendToSelection(this->entityIds[i]);
					LevelEditor2EntityManager::Instance()->GetEntityById(this->entityIds[i])->SetBool(Attr::IsSelected,true);										
				}
				SelectionUtil::Instance()->SetSelection(this->entityIds);

				AttributeWidgetManager::Instance()->ClearAttributeControllers();		
				Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();
				AttributeWidgetManager::Instance()->ViewEntityAttributes(ents);
				if(ents.Size() == 1)
				{				
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
			AttributeWidgetManager::Instance()->ViewEntityAttributes(ents);
			if(ents.Size() == 1)
			{				
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
			AttributeWidgetManager::Instance()->ViewEntityAttributes(ents);
			if(ents.Size() == 1)
			{
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
			AttributeWidgetManager::Instance()->ViewEntityAttributes(ents);
			if(ents.Size() == 1)
			{				
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
			AttributeWidgetManager::Instance()->ViewEntityAttributes(ents);
			if (ents.Size() == 1)
			{			
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
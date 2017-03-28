//------------------------------------------------------------------------------
//  createaction.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "createaction.h"
#include "leveleditor2entitymanager.h"
#include "leveleditor2app.h"

using namespace Game;

//------------------------------------------------------------------------------
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::CreateAction, 'LECA', LevelEditor2::Action);

//------------------------------------------------------------------------------
/**
*/
void 
CreateAction::Perform()
{
	n_assert(InvalidType != this->entityType);
	n_assert(this->category.IsValid());

	if (Environment == this->entityType)
	{
		n_assert(this->templ.IsValid());

		this->entityId = LevelEditor2EntityManager::Instance()->CreateEnvironmentEntity(this->category + "/" + this->templ);		
	}
	else if (Light == this->entityType)
	{
		this->entityId = LevelEditor2EntityManager::Instance()->CreateLightEntity(this->category);
	}
	else if (Game == this->entityType)
	{
		this->entityId = LevelEditor2EntityManager::Instance()->CreateGameEntity(this->category, this->templ);
	}
	else if (NavMesh == this->entityType)
	{
		this->entityId = LevelEditor2EntityManager::Instance()->CreateNavMeshEntity();
	}
	else if (Probe == this->entityType)
	{
		this->entityId = LevelEditor2EntityManager::Instance()->CreateLightProbeEntity();
	}
    else if (NavMeshArea == this->entityType)
    {
        this->entityId = LevelEditor2EntityManager::Instance()->CreateNavArea();
    }
	else
	{
		n_error("CreateAction::Perform: Cannot handle entity type of enum %d", (int)this->entityType);
	}
	Ptr<Layers::LayerHandler> handler = LevelEditor2App::Instance()->GetWindow()->GetLayerHandler();	
	handler->HandleEntityCreated(LevelEditor2EntityManager::Instance()->GetEntityById(this->entityId));
}

//------------------------------------------------------------------------------
/**
*/
void 
CreateAction::Undo()
{
	n_assert(this->entityId.IsValid());
	n_assert(LevelEditor2EntityManager::Instance()->EntityExists(this->entityId));

	LevelEditor2EntityManager::Instance()->RemoveEntity(this->entityId);

	//FIXME: Only deselect the created entity
	LevelEditor2App::Instance()->GetCurrentStateHandler().cast<LevelEditor2::LevelEditorState>()->ClearSelection();
}

//------------------------------------------------------------------------------
/**
*/
bool 
CreateAction::CheckIfAllowed(Util::String& outErrorMessage)
{
	outErrorMessage = "";

	if (Light == this->entityType && "GlobalLight" == this->category)
	{
		if (LevelEditor2EntityManager::Instance()->GetGlobalLight().isvalid())
		{
			outErrorMessage = "A global light already exists!";
			return false;
		}
	}

	return true;
}

} // namespace LevelEditor2
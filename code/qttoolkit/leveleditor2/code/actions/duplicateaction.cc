//------------------------------------------------------------------------------
//  duplicateaction.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "duplicateaction.h"
#include "leveleditor2entitymanager.h"
#include "leveleditor2app.h"
#include "properties/editorproperty.h"
#include "lighting/lighttype.h"

using namespace Game;

//------------------------------------------------------------------------------
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::DuplicateAction, 'LECD', LevelEditor2::Action);

//------------------------------------------------------------------------------
/**
*/
void 
DuplicateAction::Perform()
{	
	n_assert(this->entities.Size()>0);
	
	this->entityIds = LevelEditor2EntityManager::Instance()->DuplicateEntities(this->entities);
	this->entities.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
DuplicateAction::Undo()
{
	
	IndexT i;
	for(i = 0 ; i < this->entityIds.Size() ; i++)
	{
		LevelEditor2EntityManager::Instance()->RemoveEntity(this->entityIds[i]);
	}

	this->entityIds.Clear();

	//FIXME: Only deselect the created entity
	LevelEditor2App::Instance()->GetCurrentStateHandler().cast<LevelEditor2::LevelEditorState>()->ClearSelection();
}

//------------------------------------------------------------------------------
/**
*/
bool 
DuplicateAction::CheckIfAllowed(Util::String& outErrorMessage)
{
	outErrorMessage = "";

	IndexT i;
	for(i = 0 ; i < this->entities.Size() ; i++)
	{
		if((this->entities[i]->GetInt(Attr::EntityType) == Light) && (this->entities[i]->GetInt(Attr::LightType) == Lighting::LightType::Global))
		{
			outErrorMessage = "A global light already exists!";
			return false;
		}
	}	

	return true;
}

} // namespace LevelEditor2
#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::DuplicateAction
    
    An undoable action which duplicates an array of entities
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "action.h"
#include "game/entity.h"
#include "entityguid.h"

//-----------------------------------------------
namespace LevelEditor2
{
class DuplicateAction : public Action
{
__DeclareClass(DuplicateAction);
public:
	/// create the entities
	void Perform();
	/// delete the created entities
	void Undo();

	/// check if creating an entity of this type is allowed
	bool CheckIfAllowed(Util::String& outErrorMessage);

	void SetEntities(Util::Array<Ptr<Game::Entity>> sourceEntities) ;
private:
	Util::Array<Ptr<Game::Entity>> entities;
	Util::Array<EntityGuid> entityIds;	
};

//------------------------------------------------------------------------------
/**
*/
inline void 
DuplicateAction::SetEntities(Util::Array<Ptr<Game::Entity>> sourceEntities)
{
	this->entities = sourceEntities;
}

} // namespace LevelEditor2
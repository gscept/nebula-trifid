#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::DeleteAction
    
    An undoable action which deletes one or more objects.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "action.h"
#include "game/entity.h"
#include "leveleditor2/leveleditor2protocol.h"

//-----------------------------------------------
namespace LevelEditor2
{
class DeleteAction : public Action
{
__DeclareClass(DeleteAction);
public:
	/// constructor
	DeleteAction();

	/// set the entities to be removed
	void SetEntities(const Util::Array<Ptr<Game::Entity> >& _entities);

	/// delete the entity
	void Perform();
	/// re-create the deleted entity
	void Undo();

private:
	bool performed;
	Util::Array<Ptr<Game::Entity> > entities;	
	Util::Array<Ptr<Game::Entity> > savedEntities;
	Util::Array<Ptr<LevelEditor2::SetAttributes>> attrMessages;
	Util::Array<Ptr<LevelEditor2::SetProperties>> propMessages;
};

} // namespace LevelEditor2
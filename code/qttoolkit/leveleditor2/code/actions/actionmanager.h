#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::ActionManager
    
    Handles undoable actions, such as create entity, transform entity etc.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/

#include "core/singleton.h"
#include "game/manager.h"
#include "game/entity.h"
#include "util/stack.h"
#include "action.h"
#include "entityguid.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class ActionManager: public Game::Manager
{
	__DeclareClass(ActionManager);
	__DeclareSingleton(ActionManager);
public:

	/// constructor
	ActionManager();
	/// destructor
	~ActionManager();
	
	/// called when attached to game server
	void OnActivate();

	/// create an entity, returns false and sets the error message if creating was not possible, else sets the entityId of the new entity
	bool CreateEntity(EntityType type, const Util::String& category, const Util::String& _template, EntityGuid& outId, Util::String& outErrorMessage);

	//FIXME: Is there a more proper way to do this? For example, get the selection beforehand and only call RemoveEntities
	/// get the entities that are currently selected (if any), and remove them
	void RemoveCurrentSelection();
	/// remove a number of entities
	void RemoveEntities(const Util::Array<Ptr<Game::Entity> > entities);

	/// performs and action and adds it to the undo stack
	void PerformAction(Ptr<Action> action);
	/// undoes last performed action
	void UndoLatestAction();
	/// returns true if there are an action to revert
	bool CanUndo();

	/// clear the undo stack
	void ClearStack();

private:

	Util::Stack<Ptr<Action> > undoStack;
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
ActionManager::CanUndo()
{
	return !this->undoStack.IsEmpty();
}

} // namespace LevelEditor2
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  actionmanager.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "actionmanager.h"
#include "transformaction.h"
#include "createaction.h"
#include "deleteaction.h"
#include "application/game/entity.h"
#include "basegamefeature/managers/factorymanager.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "attr/attrid.h"
#include "addons/attr/attribute.h"
#include "managers/categorymanager.h"
#include "basegamefeature/managers/entitymanager.h"

#include "leveleditorstate.h"
#include "leveleditor2app.h"
#include "entityutils/selectionutil.h"

using namespace Attr;
using namespace BaseGameFeature;

namespace LevelEditor2
{
__ImplementClass(ActionManager, 'ACMG', Game::Manager);
__ImplementSingleton(ActionManager);
//------------------------------------------------------------------------------
/**
*/
ActionManager::ActionManager()
{
	__ConstructSingleton;

	this->undoStack = Util::Stack<Ptr<Action> >();
}

//------------------------------------------------------------------------------
/**
*/
ActionManager::~ActionManager()
{
	this->undoStack.Clear();

	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void 
ActionManager::OnActivate()
{
	Manager::OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void 
ActionManager::PerformAction( Ptr<Action> action )
{
	action->Perform();
	this->undoStack.Push(action);
}

//------------------------------------------------------------------------------
/**
*/
void 
ActionManager::UndoLatestAction()
{
	n_assert(this->CanUndo());
	this->undoStack.Pop()->Undo();
}

//------------------------------------------------------------------------------
/**
	//FIXME: Is there a more proper way to do this? For example, get the selection beforehand and only call RemoveEntities.
	When fixing this, remove unecessary includes
*/
void 
ActionManager::RemoveCurrentSelection()
{
	Ptr<LevelEditorState> editorState = LevelEditor2App::Instance()->GetCurrentStateHandler().downcast<LevelEditorState>();

	Util::Array<Ptr<Game::Entity> > selectedEntities = editorState->selectionUtil->GetSelectedEntities();

	if (!selectedEntities.IsEmpty())
	{
		this->RemoveEntities(selectedEntities);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ActionManager::RemoveEntities(const Util::Array<Ptr<Game::Entity> > entities)
{
	Ptr<DeleteAction> deleteAction = DeleteAction::Create();

	deleteAction->SetEntities(entities);
	this->PerformAction(deleteAction.upcast<Action>());
}

//------------------------------------------------------------------------------
/**
*/
bool 
ActionManager::CreateEntity(EntityType type, const Util::String& category, const Util::String& _template, EntityGuid& outId, Util::String& outErrorMessage)
{
	n_assert(InvalidType != type);
	n_assert(category.IsValid());
	
	outErrorMessage = "";
	outId = InvalidEntityId;

	Ptr<CreateAction> createAction = CreateAction::Create();
	createAction->SetEntityType(type);
	createAction->SetCategory(category);

	Util::String errorMessage;
	if (!createAction-> CheckIfAllowed(errorMessage))
	{
		outErrorMessage = errorMessage;
		return false;
	}

	if (Environment == type ||
		Game == type)
	{
		// add the template as well
		n_assert(_template.IsValid());
		createAction->SetTemplate(_template);
	}

	this->PerformAction(createAction.upcast<Action>());
	outId = createAction->GetEntityId();
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
ActionManager::ClearStack()
{
	this->undoStack.Clear();
}

} // namespace LevelEditor2
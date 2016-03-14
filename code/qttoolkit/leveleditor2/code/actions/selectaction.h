#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::SelectAction
    
    An undoable action which selects an entity
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "action.h"
#include "util/array.h"
#include "game/entity.h"
#include "entityguid.h"

//-----------------------------------------------
namespace LevelEditor2
{

class SelectAction : public Action
{
__DeclareClass(SelectAction);

public:	
	enum SelectMode
	{
		ClearSelection,
		AppendSelection,
		RemoveSelection,
		SetSelection
	};

	/// sets the selection mode
	void SetSelectionMode(SelectMode mode);
	/// set the Ids of the entities that will be selected/unselected
	void SetEntities(const Util::Array<EntityGuid>& ids);
	/// convenience function with only one entity
	void SetEntity(const EntityGuid & id);
	

protected:
	/// select objects
	void Perform();
	/// unselects objects
	void Undo();

private:
	/// sync placement matrices
	void SyncUtils();

	Util::Array<EntityGuid> entityIds;
	Util::Array<EntityGuid> lastEntityIds;
	SelectMode mode;		
};
} // namespace LevelEditor2
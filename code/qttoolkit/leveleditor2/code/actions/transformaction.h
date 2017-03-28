#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::TransformAction
    
    An undoable action which changes the transformation of some objects.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "action.h"
#include "util/array.h"
#include "math/matrix44.h"
#include "game/entity.h"
#include "entityguid.h"

//-----------------------------------------------
namespace LevelEditor2
{

class TransformAction : public Action
{
__DeclareClass(TransformAction);

public:
	/// set the entities that will be transformed, and their current transforms
	void SetEntitiesStart(Util::Array<Ptr<Game::Entity> > _entities, Util::Array<Math::matrix44> _startTransforms);
	/// set the Ids of the entities that will be transformed, and their current transforms
	void SetEntitiesStart(Util::Array<EntityGuid> ids, Util::Array<Math::matrix44> _startTransforms);
	/// set the end transforms for the current entities
	void SetEntitiesEnd(Util::Array<Math::matrix44> _endTransforms);

protected:
	/// update the transformation of the object(s)
	void Perform();
	/// revert the transformation of the object(s)
	void Undo();

	Util::Array<EntityGuid> entityIds;
	Util::Array<Math::matrix44> startTransforms;
	Util::Array<Math::matrix44> endTransforms;
};
} // namespace LevelEditor2
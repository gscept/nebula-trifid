//------------------------------------------------------------------------------
//  transformaction.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "transformaction.h"
#include "basegamefeature/basegameprotocol.h"
#include "leveleditor2app.h"
#include "leveleditor2entitymanager.h"
#include "properties/editorproperty.h"

//-----------------------------------------------
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::TransformAction, 'LEMC', LevelEditor2::Action);

//------------------------------------------------------------------------------
/**
*/
void 
TransformAction::Perform()
{
	n_assert(!this->entityIds.IsEmpty());
	n_assert(!this->endTransforms.IsEmpty());
	n_assert(this->entityIds.Size() == this->endTransforms.Size());

	Ptr<LevelEditor2EntityManager> manager = LevelEditor2EntityManager::Instance();

	IndexT i;
	for (i = 0; i < this->entityIds.Size() ; i++)
	{
		Ptr<BaseGameFeature::SetTransform> setTransformMsg = BaseGameFeature::SetTransform::Create();
		setTransformMsg->SetMatrix(this->endTransforms[i]);

		Ptr<Game::Entity> entity = manager->GetEntityById(entityIds[i]);
		entity->SendSync(setTransformMsg.cast<Messaging::Message>());
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
TransformAction::Undo()
{
	n_assert(!this->entityIds.IsEmpty());
	n_assert(!this->startTransforms.IsEmpty());
	n_assert(this->entityIds.Size() == this->startTransforms.Size());


	Ptr<LevelEditor2EntityManager> manager = LevelEditor2EntityManager::Instance();
	// save an array to update the selection
	Util::Array<Ptr<Game::Entity> > entities;

	IndexT i;
	for (i = 0; i < this->entityIds.Size() ; i++)
	{
		Ptr<BaseGameFeature::SetTransform> setTransformMsg = BaseGameFeature::SetTransform::Create();
		setTransformMsg->SetMatrix(this->startTransforms[i]);

		Ptr<Game::Entity> entity = manager->GetEntityById(entityIds[i]);
		__SendSync(entity, setTransformMsg);
		entities.Append(entity);
	}

	// update the selected entities, this ensures the placement utility knows we've changed the transforms of said entities
	LevelEditor2App::Instance()->GetCurrentStateHandler().cast<LevelEditor2::LevelEditorState>()->UpdateSelection(entityIds);
}

//------------------------------------------------------------------------------
/**
*/
void 
TransformAction::SetEntitiesStart( Util::Array<Ptr<Game::Entity> > _entities, Util::Array<Math::matrix44> _startTransforms )
{
	Util::Array<EntityGuid> ids;

	IndexT i;
	for (i = 0; i < _entities.Size(); i++)
	{
		Ptr<Game::Entity> entity = _entities[i];
		n_assert(entity.isvalid());

		ids.Append(entity->GetGuid(Attr::EntityGuid));
	}

	this->SetEntitiesStart(ids, _startTransforms);
}

//------------------------------------------------------------------------------
/**
*/
void 
TransformAction::SetEntitiesStart(Util::Array<EntityGuid> ids, Util::Array<Math::matrix44> _startTransforms)
{
	n_assert(ids.Size() == _startTransforms.Size());

	this->entityIds = ids;
	this->startTransforms = _startTransforms;
}

//------------------------------------------------------------------------------
/**
*/
void 
TransformAction::SetEntitiesEnd( Util::Array<Math::matrix44> _endTransforms )
{
	n_assert(!this->entityIds.IsEmpty());
	n_assert(this->entityIds.Size() == _endTransforms.Size());
	this->endTransforms = _endTransforms;
}

} // namespace LevelEditor2
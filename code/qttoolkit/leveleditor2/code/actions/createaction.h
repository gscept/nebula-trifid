#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::CreateAction
    
    An undoable action which creates an object.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "action.h"
#include "game/entity.h"
#include "entityguid.h"

//-----------------------------------------------
namespace LevelEditor2
{
class CreateAction : public Action
{
__DeclareClass(CreateAction);
public:
	/// create the entity
	void Perform();
	/// delete the created entity
	void Undo();

	/// check if creating an entity of this type is allowed
	bool CheckIfAllowed(Util::String& outErrorMessage);

	/// set the graphics category
	void SetCategory(const Util::String& cat);
	/// get the graphics category
	Util::String GetCategory() const;
	/// set the graphics template
	void SetTemplate(const Util::String& _templ);
	/// get the graphics template
	Util::String GetTemplate() const;
	/// get the entity type
	EntityType GetEntityType() const;
	/// set the entity type
	void SetEntityType(EntityType type);
	/// get the id of the created entity
	EntityGuid GetEntityId() const;

private:
	Util::String category;
	Util::String templ;	//< template
	EntityGuid entityId;
	EntityType entityType;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
CreateAction::SetCategory(const Util::String& cat)
{
	this->category = cat;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::String 
CreateAction::GetCategory() const
{
	return this->category;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
CreateAction::SetTemplate(const Util::String& _templ)
{
	this->templ = _templ;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::String 
CreateAction::GetTemplate() const
{
	return this->templ;
}


//------------------------------------------------------------------------------
/**
*/
inline EntityType 
CreateAction::GetEntityType() const
{
	return this->entityType;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
CreateAction::SetEntityType(EntityType type)
{
	this->entityType = type;
}

//------------------------------------------------------------------------------
/**
*/
inline LevelEditor2::EntityGuid 
CreateAction::GetEntityId() const
{
	n_assert(InvalidEntityId != this->entityId);
	return this->entityId;
}

} // namespace LevelEditor2
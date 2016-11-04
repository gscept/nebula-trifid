#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::GameFilterSet

    A filter set object allows to define checking for game entity id exclusion as well
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "util/array.h"
#include "physics/filterset.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
class GameFilterSet : public Physics::FilterSet
{
public:
    /// constructor
    GameFilterSet();
    /// destructor
    virtual ~GameFilterSet();
    /// clear the filter set
    virtual void Clear();
    /// add an entity unique id to the set
    void AddEntityId(uint id);
    /// access to entity id's
    const Util::Array<uint>& GetEntityIds() const;    
    /// check a object for exlusion
	virtual bool CheckObject(const Physics::PhysicsObject *object) const;	
    

protected:
	/// return true if entity id is in set
	bool CheckEntityId(uint id) const;

    Util::Array<uint> entityIds;    
};

//------------------------------------------------------------------------------
/**
*/
inline
GameFilterSet::GameFilterSet()    
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
GameFilterSet::~GameFilterSet()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline void
GameFilterSet::Clear()
{
    this->entityIds.Clear();
	FilterSet::Clear();    
}

//------------------------------------------------------------------------------
/**
*/
inline void
GameFilterSet::AddEntityId(uint id)
{
    this->entityIds.Append(id);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<uint>&
GameFilterSet::GetEntityIds() const
{
    return this->entityIds;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
GameFilterSet::CheckEntityId(uint id) const
{
    return (this->entityIds.Find(id) != 0);
}

} // namespace PhysicsFeature
//------------------------------------------------------------------------------


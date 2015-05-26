#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::EnvironmentCollideProperty

    This property adds pieces of static collide geometry to the game world.
    It is very similar to the class EnvironmentGraphicsProperty, but instead
    of graphics it handles collision. All static collide geometry in a level
    will usually be added to one EnvironmentCollideProperty, which in
    turn lives in a single game entity which represent the environment
    graphics and collission. That way the game entity pool isn't flooded
    with hundreds of game entities which would end up doing nothing because
    they just represent static geometry. Instead, everything static about
    the level is put into a single entity.
    
    NOTE: usually you don't need to care about this class, it's used
    by the level loader which automatically collects all environment objects
    into a single game entity.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "game/property.h"
#include "physics/physicsobject.h"
#include "util/dictionary.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{

class EnvironmentCollideProperty : public Game::Property
{
	__DeclareClass(EnvironmentCollideProperty);
public:
    /// constructor
    EnvironmentCollideProperty();
    /// destructor
    virtual ~EnvironmentCollideProperty();	
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
	/// add from resource file
	void AddShapes(const Util::String& id, const Math::matrix44& worldMatrix, const Util::String & res, Util::String physicsMaterial);
    /// delete shapes associated with given id
    void DeleteShapes(const Util::String& id);
    /// return true if a shape group of the given id exists
    bool HasShapes(const Util::String& id) const;
    /// get shape group by id
    const Util::Array<Ptr<Physics::PhysicsObject> >& GetShapes(const Util::String& id) const;
	
private:
	
	Util::Dictionary<Util::String, Util::Array<Ptr<Physics::PhysicsObject>>> entries;
};
__RegisterClass(EnvironmentCollideProperty);

}; // namespace Properties
//------------------------------------------------------------------------------
    
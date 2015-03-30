#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::EnvEntityManager
    
    Manages creation and updating of environment entities. All simple 
    environment entities are kept in a single game entity to prevent pollution 
    of the game entity pool with entities that don't actually do anything.
    Non-simple environment entities (entities with animations or physics)
    will still be created as normal game entities. 

    The EnvEntityManager basically hides all differences between those
    types of environment entities.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file	
*/
#include "core/singleton.h"
#include "game/manager.h"
#include "attr/attributecontainer.h"
#include "game/entity.h"
#include "graphics/graphicsentity.h"
#include "graphicsfeature/properties/environmentgraphicsproperty.h"
#include "appgame/appconfig.h"
namespace PhysicsFeature
{
    class EnvironmentCollideProperty;
}

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class EnvEntityManager : public Game::Manager
{
    __DeclareClass(EnvEntityManager);
    __DeclareSingleton(EnvEntityManager);

public:
    /// constructor
    EnvEntityManager();
    /// destructor
    virtual ~EnvEntityManager();

    /// called when removed from game server
    virtual void OnDeactivate();

    /// create environment entity from scratch from provided attributes
    void CreateEnvEntity(const Util::Array<Attr::Attribute>& attrs);
    /// create an environment entity and attach to world
    void CreateEnvEntity(const Ptr<Db::ValueTable>& instTable, IndexT instTableRowIndex);
    /// check if an environment entity exists by _ID
    bool EnvEntityExists(const Util::String& id) const;
    /// set the world transform of an environment entity
    void SetEnvEntityTransform(const Util::String& id, const Math::matrix44& m);
    /// delete environment with given id
    void DeleteEnvEntity(const Util::String& id);
    /// get graphics entities by id (may return empty array)
    Util::Array<Ptr<Graphics::ModelEntity> > GetGraphicsEntities(const Util::String& id) const;

    /// clear enviornment entity
    void ClearEnvEntity();

private:
    /// create an animated environment entity
    void CreateAnimatedEntity(const Ptr<Db::ValueTable>& instTable, IndexT instTableRowIndex);
    /// create an environment entity with physics
    void CreatePhysicsEntity(const Ptr<Db::ValueTable>& instTable, IndexT instTableRowIndex);
    /// create internal pool env entity if not exists yet
    void ValidateEnvEntity();

    Ptr<Game::Entity> envEntity;    // the one big environment entity
    Ptr<GraphicsFeature::EnvironmentGraphicsProperty> envGraphicsProperty;
    Ptr<PhysicsFeature::EnvironmentCollideProperty> envCollideProperty;

};

} // namespace BaseGameFeature
//------------------------------------------------------------------------------

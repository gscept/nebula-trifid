#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::EntityManager

    The entity manager object keeps track of all active game entities
    and calls their per-frame-update methods to keep them alive. It
    also contains methods to iterate through existing entities. 
    Derive from this class if your application needs different or 
    more advanced game entity management, but make sure that all
    methods which are defined in entity manager still do the expected thing
    in your derived class.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file	
*/
#include "core/ptr.h"
#include "core/singleton.h"
#include "game/manager.h"
#include "util/array.h"
#include "util/dictionary.h"
#include "game/entity.h"
#include "math/point.h"
#include "debug/debugtimer.h"
#include "appgame/appconfig.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class EntityManager : public Game::Manager
{
	__DeclareClass(EntityManager);
    __DeclareSingleton(EntityManager);
public:
    /// constructor
    EntityManager();
    /// destructor
    virtual ~EntityManager();

    /// TEMP HACK: set the maximum-trigger-distance (default is 100 meters)
    void SetMaxTriggerDistance(float d);
    /// TEMP HACK: get the maximum-trigger-distance
    float GetMaxTriggerDistance() const;

    /// called when removed from game server
    virtual void OnDeactivate();
    /// called on start by game server
    void OnStart();    
    /// called per-frame by game server
    virtual void OnBeginFrame();
    /// called per-frame by game server
    virtual void OnEndFrame();    
    /// called after level loaded
    virtual void OnLoad();
    /// called before saving game state
    virtual void OnSave();
    /// called if a render debug visulalization is requested
    void OnRenderDebug();
    /// cleanup the entity manager, removes/deactivates all entities
    void Cleanup();
    /// return true if at least one active entity exists in the world
    bool HasActiveEntities() const;
    /// get currently active entities
    const Util::Array<Ptr<Game::Entity> >& GetEntities() const;

    /// immediately attach an entity to the world
    void AttachEntity(const Ptr<Game::Entity>& entity);
    /// remove an entity from the world, delayed until end of frame
    void RemoveEntity(const Ptr<Game::Entity>& entity);
    /// delete an entity from the world (also deletes the entity from DB!), delayed until end of frame
    void DeleteEntity(const Ptr<Game::Entity>& entity);

    /// remove an entity from the world, calling restrictions apply
    void RemoveEntityImmediate(const Ptr<Game::Entity>& entity);
    /// delete an entity from the world (also deletes the entity from DB!), calling restrictions apply
    void DeleteEntityImmediate(const Ptr<Game::Entity>& entity);
    
    /// return true if a entity with the given unique id exists
    bool ExistsEntityByUniqueId(Game::Entity::EntityId id) const;
    /// get the entity for the given unique id
    Ptr<Game::Entity> GetEntityByUniqueId(Game::Entity::EntityId id) const;

    /// return true if at least one entity exists with the given attribute
    bool ExistsEntityByAttr(const Attr::Attribute& attr) const;
    /// get the entities for the given attribute 
    Util::Array<Ptr<Game::Entity> > GetEntitiesByAttr(const Attr::Attribute& attr, bool onlyFirstEntity = false);
    /// get the entities by multiple matching attributes 
    Util::Array<Ptr<Game::Entity> > GetEntitiesByAttrs(const Util::Array<Attr::Attribute>& attr, bool onlyFirstEntity = false);
	/// get entities by category
	Util::Array<Ptr<Game::Entity>> GetEntitiesByCategory(const Util::String & category);
    /// get a single entity by a single attribute 
    Ptr<Game::Entity> GetEntityByAttr(const Attr::Attribute& attr, bool failOnError = true);
    /// get a single entity by multiple matching attributes 
    Ptr<Game::Entity> GetEntityByAttrs(const Util::Array<Attr::Attribute>& attr, bool failOnError = true);
	
    /// fill provided array with all entities inside the activity bubble
    void GetEntitiesInActivityBubble(Util::Array<Ptr<Game::Entity> >& outEntities);
	/// returns true if entity is in delayed jobs for delete or remove
	bool IsEntityInDelayedJobs(const Ptr<Game::Entity>& entity);

protected:
    friend class Game::Entity;

    /// delayed job types
    enum DelayedJobType
    {
        InvalidJobType,
        Remove,
        Delete,
    };

    /// delayed jobs
    class DelayedJob
    {
    public:
        /// default constructor
        DelayedJob() : type(InvalidJobType) {};
        /// constructor
        DelayedJob(DelayedJobType t, const Ptr<Game::Entity>& e) : type(t), entity(e) {};
        DelayedJobType type;
        Ptr<Game::Entity> entity;
    };
    /// call OnStart() on all entities
    void StartEntities();

    /// activate an entity
    void ActivateEntity(const Ptr<Game::Entity>& entity);
    /// deactivate an entity
    void DeactivateEntity(const Ptr<Game::Entity>& entity);
    /// add a delayed job (Remove or Delete entity)
    void AddDelayedJob(DelayedJobType type, const Ptr<Game::Entity>& entity);
    /// handle all queued up delayed jobs, called at beginning and end of frame
    void HandleDelayedJobs();
    /// remove all null entries from the internal arrays
    void RemoveNullEntriesFromArrays();

    /// update both arrays
    void UpdateTriggeredEntities();                 
    /// check if entity is near focus entity
    bool IsInFocus(const Ptr<Game::Entity>& entity, Math::point& focusEntityPos);  
    /// remove entity from triggerd/untriggered arrays
    void RemoveEntityFromTriggered(const Ptr<Game::Entity>& entity);  

    float maxTriggerDistance;
    bool activeEntitiesLocked;
    Util::Dictionary<Game::Entity::EntityId, Ptr<Game::Entity> > entityRegistry;  // map unique ids to entities
    Util::Array<Ptr<Game::Entity> > activeEntities;                               // currently active entities
    Util::Array<DelayedJob> delayedJobs;

    // triggered vs untriggered entities (untriggered = not in focus/ outside activity bubble)
    // all newly created entities go to "untriggered entites"
    Util::Array<Ptr<Game::Entity> > triggeredEntities;    // will be triggered                               
    Util::Array<Ptr<Game::Entity> > untriggeredEntities;  // wont be triggered
    
#if NEBULA3_ENABLE_PROFILING
    // profiling stuff
    _declare_timer(EntityManagerOnBeginFrame);
    _declare_timer(EntityManagerOnMoveBefore);
    _declare_timer(EntityManagerOnMoveAfter);
    _declare_timer(EntityManagerOnRender);
    _declare_timer(EntityManagerUpdateRegistry);
    _declare_timer(EntityLoad);
    _declare_timer(EntityStart);
    _declare_timer(EntityActivate);
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline void
EntityManager::SetMaxTriggerDistance(float f)
{
    this->maxTriggerDistance = f;
}

//------------------------------------------------------------------------------
/**
*/
inline float
EntityManager::GetMaxTriggerDistance() const
{
    return this->maxTriggerDistance;
}

} // namespace Managers
//------------------------------------------------------------------------------

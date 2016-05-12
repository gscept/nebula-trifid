//------------------------------------------------------------------------------
//  managers/entitymanager.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "game/entity.h"
#include "game/gameserver.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "managers/factorymanager.h"
#include "managers/focusmanager.h"
#include "managers/categorymanager.h"
#include "managers/entitymanager.h"

namespace BaseGameFeature
{
__ImplementClass(EntityManager, 'MENT', Game::Manager);
__ImplementSingleton(EntityManager);

using namespace Game;
using namespace Attr;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
EntityManager::EntityManager() :
    maxTriggerDistance(1000.0f),
    activeEntitiesLocked(false)  
#if __NEBULA_STATS__
        ,statsNumEntities("mangaNumEntities", nArg::Int),
        statsNumTriggeredEntities("mangaNumTriggeredEntities", nArg::Int),
        propertyProfilersSum(Util::String("profManga.Frame.Game.OnFrame." + this->GetRtti()->GetName() + "." + Game::Entity::RTTI.GetName() + ".Properties:: ").Get(), nArg::Float),
#endif
{
    __ConstructSingleton;

#if NEBULA3_ENABLE_PROFILING
    _setup_timer(EntityManagerOnBeginFrame);
    _setup_timer(EntityManagerOnMoveBefore);
    _setup_timer(EntityManagerOnMoveAfter);
    _setup_timer(EntityManagerOnRender);
    _setup_timer(EntityManagerUpdateRegistry);
    _setup_timer(EntityLoad);
    _setup_timer(EntityStart);
    _setup_timer(EntityActivate);
#endif
}

//------------------------------------------------------------------------------
/**
*/
EntityManager::~EntityManager()
{
    n_assert(0 == this->activeEntities.Size());
    n_assert(0 == this->entityRegistry.Size());
    n_assert(0 == this->delayedJobs.Size());

#if NEBULA3_ENABLE_PROFILING
    _discard_timer(EntityManagerOnBeginFrame);
    _discard_timer(EntityManagerOnMoveBefore);
    _discard_timer(EntityManagerOnMoveAfter);
    _discard_timer(EntityManagerOnRender);
    _discard_timer(EntityManagerUpdateRegistry);
    _discard_timer(EntityLoad);
    _discard_timer(EntityStart);
    _discard_timer(EntityActivate);
#endif

    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityManager::OnDeactivate()
{
    this->Cleanup();

    Game::Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityManager::ExistsEntityByUniqueId(Game::Entity::EntityId uniqueId) const
{
    if (this->entityRegistry.Contains(uniqueId))
    {
        return (this->entityRegistry[uniqueId].isvalid());        
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Return entity by unique id or an invalid Ptr<> if the entity doesn't exist.
*/
Ptr<Game::Entity>
EntityManager::GetEntityByUniqueId(Game::Entity::EntityId uniqueId) const
{
    Ptr<Game::Entity> entity;
    IndexT index = this->entityRegistry.FindIndex(uniqueId);
    if (InvalidIndex != index)
    {
        entity = this->entityRegistry.ValueAtIndex(index);        
    }
    return entity;
}

//------------------------------------------------------------------------------
/**
    Deactivate an entity. This will remove the entity from the world and
    call its OnDeactivate() method. This is a private helper method
    and will be called from RemoveEntity() or RemoveDismissedEntities().
*/
void
EntityManager::DeactivateEntity(const Ptr<Entity>& entity)
{
    n_assert(0 != entity);
    n_assert(entity->IsActive());

    entity->OnDeactivate();
    IndexT entityIndex = this->activeEntities.FindIndex(entity);
    n_assert(InvalidIndex != entityIndex);

    // don't change array layouts!
    Entity::EntityId entityId = entity->GetUniqueId();
    this->activeEntities[entityIndex] = 0;
    this->entityRegistry[entityId] = 0;

    // remove entity from triggered/ untriggered arrays
    this->RemoveEntityFromTriggered(entity);
}

//------------------------------------------------------------------------------
/**
    Remove a game entity from the entity manager. This will just mark the entity
    as dismissed, deactivation will happen at the beginning of the next frame
    to prevent any data incosistencies. The entity will just function as 
    usual for the rest of the frame.
*/
void
EntityManager::RemoveEntity(const Ptr<Entity>& entity)
{
    n_assert(entity.isvalid());
    n_assert(entity->IsActive());
    this->AddDelayedJob(Remove, entity);
}

//------------------------------------------------------------------------------
/**
    Delete an entity (remove it from the world and delete it from the 
    database) at the beginning of the next frame.
*/
void
EntityManager::DeleteEntity(const Ptr<Entity>& entity)
{
    n_assert(entity.isvalid());
    n_assert(entity->IsActive());
    this->AddDelayedJob(Delete, entity);
}

//------------------------------------------------------------------------------
/**
    Immediately attach (and activate) a game entity. 
*/
void
EntityManager::AttachEntity(const Ptr<Entity>& entity)
{
    n_assert(entity.isvalid());
    n_assert(!entity->IsActive());

    this->activeEntities.Append(entity);
    this->untriggeredEntities.Append(entity);
    this->entityRegistry.Add(entity->GetUniqueId(), entity);

#if NEBULA3_ENABLE_PROFILING
    _start_timer(EntityActivate)
#endif
     // activate entity
     entity->OnActivate();

#if NEBULA3_ENABLE_PROFILING
    _stop_timer(EntityActivate)
#endif
    
    // make sure OnLoad() and OnStart() get called even when the
    // game is already running
    if (Game::GameServer::Instance()->HasStarted())
    {
    #if NEBULA3_ENABLE_PROFILING
        _start_timer(EntityLoad)
    #endif
        // Load entity
        entity->OnLoad();

    #if NEBULA3_ENABLE_PROFILING
        _stop_timer(EntityLoad)
        _start_timer(EntityStart)
    #endif
        // start entity
        entity->OnStart();

    #if NEBULA3_ENABLE_PROFILING
        _stop_timer(EntityStart)
    #endif
    }

    #if __ENTITY_STATS__
    // create a profiler if needed
    if (!this->entityProfilersByClass.Contains(entity->GetRtti()))
    {
        Util::String name = Util::String("profManga.Frame.Game.OnFrame.") + this->GetRtti()->GetName() +
            "." + entity->GetRtti()->GetName() + " ";
        nProfiler profiler;
        profiler.Initialize(name.Get());        
        this->entityProfilersByClass.Add(entity->GetRtti(), profiler);
    }
    #endif
}

//------------------------------------------------------------------------------
/**
    Immediately remove an entity from the entity manager. This method is
    more restrictive then RemoveEntity() because it must not be called from
    inside the game loop.
*/
void
EntityManager::RemoveEntityFromTriggered(const Ptr<Entity>& entity)
{
    // remove entity from triggered/untriggered arrays
    IndexT entityIndex;
    entityIndex = this->triggeredEntities.FindIndex(entity);
    if (InvalidIndex != entityIndex)
    {
        this->triggeredEntities.EraseIndex(entityIndex);
    }
    entityIndex = this->untriggeredEntities.FindIndex(entity);
    if (InvalidIndex != entityIndex)
    {
        this->untriggeredEntities.EraseIndex(entityIndex);
    }
}
    
//------------------------------------------------------------------------------
/**
    Immediately remove an entity from the entity manager. This method is
    more restrictive then RemoveEntity() because it must not be called from
    inside the game loop.
*/
void
EntityManager::RemoveEntityImmediate(const Ptr<Entity>& entity)
{
    n_assert(entity.isvalid());
    n_assert(entity->IsActive());
    n_assert(!this->activeEntitiesLocked);

    entity->OnDeactivate();
    IndexT entityIndex = this->activeEntities.FindIndex(entity);
    n_assert(InvalidIndex != entityIndex);

    // don't change array layouts!
    this->activeEntities[entityIndex] = 0;
    this->entityRegistry[entity->GetUniqueId()] = 0;

    // debug check: make sure the entity didn't exist twice
    n_assert(InvalidIndex == this->activeEntities.FindIndex(entity));

    // remove entity from triggered/ untriggered arrays
    this->RemoveEntityFromTriggered(entity);
}

//------------------------------------------------------------------------------
/**
    Immediately delete an entity (remove from world and delete from
    database). This method is more restrictive then DeleteEntity() because
    it must not be called from inside the game loop.
*/
void
EntityManager::DeleteEntityImmediate(const Ptr<Entity>& entity)
{
    n_assert(entity.isvalid());
    n_assert(entity->IsActive());
    n_assert(!this->activeEntitiesLocked);

    // create a category entry object for the game entity as long as its internal pointers
    // are valid (before Entity::OnDeactivate() in RemoveEntityImmediate()
    CategoryManager::Entry catEntry(entity->GetCategory(), entity->GetAttrTable(), entity->GetAttrTableRowIndex());

    // first do a usual immediate remove
    this->RemoveEntityImmediate(entity);

    // then delete database entry
    CategoryManager::Instance()->DeleteInstance(catEntry);
}

//------------------------------------------------------------------------------
/**
    This immediately removes and deactivates all entities from the entity 
    manager. This method is usually only called at the end of a level.
*/
void
EntityManager::Cleanup()
{
    // discard all delayed jobs
    this->delayedJobs.Clear();

    // cleanup triggered lists
    this->triggeredEntities.Clear();
    this->untriggeredEntities.Clear();

    // remove all active entities
    while (!this->activeEntities.IsEmpty())
    {
        // need to make a smart pointer copy, because
        // the entry in the activeEntities array will be cleared
        Ptr<Game::Entity> entityPtr = this->activeEntities[0];
        if (entityPtr.isvalid())
        {
            this->RemoveEntityImmediate(entityPtr);
        }
        else
        {
            this->activeEntities.EraseIndex(0);
        }
    }

    // clear entity registries
    this->entityRegistry.Clear();
}

//------------------------------------------------------------------------------
/**
    Return true if there is at least one active entity in the world.
*/
bool
EntityManager::HasActiveEntities() const
{
    return this->activeEntities.Size() > 0;
}

//------------------------------------------------------------------------------
/**
    Get the array of currently active entities.
*/
const Util::Array<Ptr<Game::Entity> >&
EntityManager::GetEntities() const
{
    return this->activeEntities;
}

//------------------------------------------------------------------------------
/**
    This method should be called once at the end of the frame. It will
    check the internal arrays for null entries, and remove those entries.
    Null entries are written by the RemoveEntityImmediate() method because
    it may be dangerous to change array layouts while an iteration is
    running over the array.
*/
void
EntityManager::RemoveNullEntriesFromArrays()
{
    n_assert(!this->activeEntitiesLocked);

    // compact the active entities array
    IndexT i;
    for (i = 0; i < this->activeEntities.Size();)
    {
        if (!this->activeEntities[i].isvalid())
        {
            this->activeEntities.EraseIndex(i);
        }
        else
        {
            i++;
        }
    }

    // compact the entity registry
    for (i = 0; i < this->entityRegistry.Size(); i++)
    {
        if (!this->entityRegistry.ValueAtIndex(i).isvalid())
        {
            this->entityRegistry.EraseAtIndex(i);
        }
        else
        {
            i++;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Add a new delayed job to the internal job queue. Delayed jobs are executed
    at the beginning of the next frame.
*/
void
EntityManager::AddDelayedJob(DelayedJobType type, const Ptr<Game::Entity>& entity)
{
    this->delayedJobs.Append(DelayedJob(type, entity));
}

//------------------------------------------------------------------------------
/**
    This handles all delayed jobs. This method is executed once at the 
    end of each frame.
*/
void
EntityManager::HandleDelayedJobs()
{
    n_assert(!this->activeEntitiesLocked);

    // note: order of operation is important!
    while (!this->delayedJobs.IsEmpty())
    {
        // need to make a copy of the smart pointer because 
        // it may be reset to 0 in the following procedures
        Ptr<Game::Entity> entityPtr = this->delayedJobs[0].entity;
        switch (this->delayedJobs[0].type)
        {
            case Remove:
                this->RemoveEntityImmediate(entityPtr);
                break;
            case Delete:
                this->DeleteEntityImmediate(entityPtr);
                break;
            case InvalidJobType:
                n_error("EntityManager::HandleDelayedJobs():InvalidJobType");
                break;
        }
        this->delayedJobs.EraseIndex(0);
    }
}


//------------------------------------------------------------------------------
/**
    Invoke the OnStart() method on all active entities.
*/
void
EntityManager::StartEntities()
{
    // first handle all delayed jobs that may have queued up so far...
    this->HandleDelayedJobs();

    // call OnStart() on all active entities
    IndexT i;
    for (i = 0; i < this->activeEntities.Size(); i++)
    {
        this->activeEntities[i]->OnStart();
    }

    // handle all delayed jobs that have been added during OnStart()
    this->HandleDelayedJobs();
}

//------------------------------------------------------------------------------
/**
    This updates updates the activityBubbleEntities array which contains
    all entities around the current viewer which should be triggered.

    25-Jan-07   floh    added optional per-entity trigger radius
*/
void
EntityManager::GetEntitiesInActivityBubble(Util::Array<Ptr<Entity> >& outEntities)
{
    // get current transform of focus entity
    const Ptr<Entity> focusEntity = FocusManager::Instance()->GetCameraFocusEntity();
    Math::point focusEntityPos;
    if (focusEntity.isvalid())
    {
        focusEntityPos = focusEntity->GetMatrix44(Attr::Transform).getrow3();
    }

    // update the activity bubble array
    // FIXME: make use of frame coherency!
    outEntities.Clear();
    IndexT entityIndex;
    if (focusEntity.isvalid())
    {
        for (entityIndex = 0; entityIndex < this->activeEntities.Size(); entityIndex++)
        {
            const Ptr<Entity>& curEntity = this->activeEntities[entityIndex];
            if (curEntity.isvalid())
            {
                if (curEntity == focusEntity)
                {
                    outEntities.Append(curEntity);
                }
                else if (curEntity->HasAttr(Attr::Transform))
                {
                    float entityTriggerRadius = 0.0f;
                    if (curEntity->HasAttr(Attr::EntityTriggerRadius))
                    {
                        entityTriggerRadius = curEntity->GetFloat(Attr::EntityTriggerRadius);
                    }
                    vector dist = curEntity->GetMatrix44(Attr::Transform).getrow3() - focusEntityPos;
                    if (dist.length() < (this->maxTriggerDistance + entityTriggerRadius))
                    {
                        outEntities.Append(curEntity);
                    }
                }
                else
                {
                    // no Transform attribute, trigger anyway
                    outEntities.Append(curEntity);
                }
            }
        }
    }
    else
    {
        for (entityIndex = 0; entityIndex < this->activeEntities.Size(); entityIndex++)
        {
            const Ptr<Entity>& curEntity = this->activeEntities[entityIndex];
            if (curEntity.isvalid())
            {
                outEntities.Append(curEntity);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Checks if an entity is "in reach" of the focus entity, i.e. 
    within Attr::EntityTriggerRadius + this->maxTriggerDistance.

    Note: will also return true on entities without transform attribute
*/
bool
EntityManager::IsInFocus(const Ptr<Game::Entity>& curEntity, point& focusEntityPos)
{
    if (curEntity->HasAttr(Attr::Transform))
    {
        float entityTriggerRadius = 0.0f;
        if (curEntity->HasAttr(Attr::EntityTriggerRadius))
        {
            entityTriggerRadius = curEntity->GetFloat(Attr::EntityTriggerRadius);
        }
        vector dist = curEntity->GetMatrix44(Attr::Transform).getrow3() - focusEntityPos;
        return dist.length() < (this->maxTriggerDistance + entityTriggerRadius);
    }

    // cant decide? then yes.
    return true;
}

//------------------------------------------------------------------------------
/**
    This updates updates the triggeredEntities/untriggeredEntities arrays that divide
    all entities around the current viewer in those who should be triggered and those
    who shouldnt be triggered.

*/
void
EntityManager::UpdateTriggeredEntities()
{
    // get current transform of focus entity
    const Ptr<Entity> focusEntity = FocusManager::Instance()->GetCameraFocusEntity();
    const Ptr<Entity> inputFocusEntity = FocusManager::Instance()->GetInputFocusEntity();
    point focusEntityPos;
    if (focusEntity.isvalid())
    {
        focusEntityPos = focusEntity->GetMatrix44(Attr::Transform).getrow3();
    }

    // update the activity bubble array
    // FIXME: make use of frame coherency!

    IndexT entityIndex;
    if (focusEntity.isvalid())
    {
        // make inactive
        for (entityIndex = 0; entityIndex < this->triggeredEntities.Size();)
        {
            const Ptr<Entity>& curEntity = this->triggeredEntities[entityIndex];
            if (curEntity == focusEntity || curEntity == inputFocusEntity)
            {
                // never make fokusentity inaktiv
                entityIndex++;
                continue;
            }
            else if (!this->IsInFocus(curEntity, focusEntityPos))
            {
                if (curEntity->IsActive()) curEntity->OnLoseActivity();
                this->untriggeredEntities.Append(curEntity);
                this->triggeredEntities.EraseIndex(entityIndex);
                continue; // skip index increase
            }            
            entityIndex++;
        }

        // make active
        for (entityIndex = 0; entityIndex < this->untriggeredEntities.Size(); )
        {
            const Ptr<Entity>& curEntity = this->untriggeredEntities[entityIndex];            
            if (curEntity == focusEntity || curEntity == inputFocusEntity)
            {
                // always make focus entity active
                if (curEntity->IsActive()) curEntity->OnGainActivity();
                this->triggeredEntities.Append(curEntity);
                this->untriggeredEntities.EraseIndex(entityIndex);
                continue; // skip index increase
            }
            else if (this->IsInFocus(curEntity, focusEntityPos))
            {
                if (curEntity->IsActive()) curEntity->OnGainActivity();
                this->triggeredEntities.Append(curEntity);
                this->untriggeredEntities.EraseIndex(entityIndex);
                continue; // skip index increase
            }            
            entityIndex++;
        }
    }
    else
    {
        // make all entities active
        for (entityIndex = 0; entityIndex < this->untriggeredEntities.Size(); entityIndex++)
        {
            const Ptr<Entity>& curEntity = this->untriggeredEntities[entityIndex];            
            if (curEntity->IsActive()) curEntity->OnGainActivity();
            this->triggeredEntities.Append(curEntity);
            this->untriggeredEntities.EraseIndex(entityIndex); 
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EntityManager::OnStart()
{
    this->StartEntities();
}

//------------------------------------------------------------------------------
/**
    Handles delayed jobs, like removing entities. 
    Calls OnBeginFrame and OnMoveBefore on all Entities.
    OnMoveAfter and OnRender is called in OnEndFrame of the EntityManager to allow
    any physics system to step its world.
*/
void
EntityManager::OnBeginFrame()
{
    n_assert(!this->activeEntitiesLocked);
    
    #if __ENTITY_STATS__
    // reset entity profilers
    int i;
    for (i = 0; i < this->entityProfilersByClass.Size(); i++)
    {
        this->entityProfilersByClass.ValueAtIndex(i).ResetAccum();
    }
    // reset property profilers
    for (i = 0; i < Game::Entity::propertyProfilersByClass.Size(); i++)
    {
        Game::Entity::propertyProfilersByClass.ValueAtIndex(i).ResetAccum();
    }
    #endif

    // handle any delayed jobs queued up outside of OnFrame()
    this->HandleDelayedJobs();

    // lock the active entities array, no changes of the array layout allowed
    // from here on
    this->activeEntitiesLocked = true;

    // check which entities are in activity bubble
    this->UpdateTriggeredEntities();

    // Hotfix for above bugfix (crashes):
    // get all entities in activity bubble, and of those, all properties 
    // that should be triggered
    //this->GetEntitiesInActivityBubble(this->triggeredEntities);

    IndexT entityIndex;
    #if __NEBULA_STATS__
    // count statistics
    this->statsNumEntities->SetI(this->activeEntities.Size());
    this->statsNumTriggeredEntities->SetI(this->triggeredEntities.Size());
    #endif

    // invoke OnBeginFrame() on registered properties
    _start_timer(EntityManagerOnBeginFrame);
    for (entityIndex = 0; entityIndex < this->triggeredEntities.Size(); entityIndex++)
    {
        if (this->triggeredEntities[entityIndex]->IsActive())
        {
            #if __ENTITY_STATS__
            Core::Rtti* entityRtti = this->triggeredEntities[entityIndex]->GetRtti();
            this->entityProfilersByClass[entityRtti].StartAccum();
            #endif

            this->triggeredEntities[entityIndex]->OnBeginFrame();
            
            #if __ENTITY_STATS__
            this->entityProfilersByClass[entityRtti].StopAccum();
            #endif
        }
    }
    _stop_timer(EntityManagerOnBeginFrame);

    // invoke OnMoveBefore() on all entities
    _start_timer(EntityManagerOnMoveBefore);
    for (entityIndex = 0; entityIndex < this->triggeredEntities.Size(); entityIndex++)
    {
        if (this->triggeredEntities[entityIndex]->IsActive())
        {
            #if __ENTITY_STATS__
            Core::Rtti* entityRtti =  this->triggeredEntities[entityIndex]->GetRtti();                
            this->entityProfilersByClass[entityRtti].StartAccum();
            #endif

            this->triggeredEntities[entityIndex]->OnMoveBefore();
            
            #if __ENTITY_STATS__
            this->entityProfilersByClass[entityRtti].StopAccum();
            #endif
        }
    }
    _stop_timer(EntityManagerOnMoveBefore);
}

//------------------------------------------------------------------------------
/** 
    Calls OnMoveAfter and OnRender on all Entities.
*/
void
EntityManager::OnEndFrame()
{
    n_assert(this->activeEntitiesLocked);
    
    IndexT entityIndex;
    
    // invoke OnMoveAfter() on all entities
    _start_timer(EntityManagerOnMoveAfter);
    for (entityIndex = 0; entityIndex < this->triggeredEntities.Size(); entityIndex++)
    {
        if (this->triggeredEntities[entityIndex]->IsActive())
        {
            #if __ENTITY_STATS__
            Core::Rtti* entityRtti =  this->triggeredEntities[entityIndex]->GetRtti();                
            this->entityProfilersByClass[entityRtti].StartAccum();
            #endif
            
            this->triggeredEntities[entityIndex]->OnMoveAfter();
            
            #if __ENTITY_STATS__
            this->entityProfilersByClass[entityRtti].StopAccum();
            #endif
        }
    }
    _stop_timer(EntityManagerOnMoveAfter);

    // invoke OnRender() on all entities
    _start_timer(EntityManagerOnRender);
    for (entityIndex = 0; entityIndex < this->triggeredEntities.Size(); entityIndex++)
    {
        if (this->triggeredEntities[entityIndex]->IsActive())
        {
            #if __ENTITY_STATS__
            Core::Rtti* entityRtti =  this->triggeredEntities[entityIndex]->GetRtti();
            this->entityProfilersByClass[entityRtti].StartAccum();
            #endif

            this->triggeredEntities[entityIndex]->OnRender();
            
            #if __ENTITY_STATS__
            this->entityProfilersByClass[entityRtti].StopAccum();
            #endif
        }
    }
    _stop_timer(EntityManagerOnRender);

	// invoke OnHandleDeferred() on all entities
	_start_timer(EntityManagerOnRender);
	for (entityIndex = 0; entityIndex < this->triggeredEntities.Size(); entityIndex++)
	{
		if (this->triggeredEntities[entityIndex]->IsActive())
		{
			#if __ENTITY_STATS__
			Core::Rtti* entityRtti =  this->triggeredEntities[entityIndex]->GetRtti();
			this->entityProfilersByClass[entityRtti].StartAccum();
			#endif

			this->triggeredEntities[entityIndex]->OnHandleDeferred();

			#if __ENTITY_STATS__
			this->entityProfilersByClass[entityRtti].StopAccum();
			#endif
		}
	}
	_stop_timer(EntityManagerOnRender);

    // unlock the active entities array
    n_assert(this->activeEntitiesLocked);
    this->activeEntitiesLocked = false;

    // handle all delayed jobs that have queued up during the frame
    // and cleanup internal arrays
    _start_timer(EntityManagerUpdateRegistry);
    this->HandleDelayedJobs();
    this->RemoveNullEntriesFromArrays();
    _stop_timer(EntityManagerUpdateRegistry);

    #if __ENTITY_STATS__
    // sum property profilers
    float sum = 0.f;
    for (i = 0; i < Game::Entity::propertyProfilersByClass.Size(); i++)
    {
        sum += Game::Entity::propertyProfilersByClass.ValueAtIndex(i).GetTime();
    }
    this->propertyProfilersSum->SetF(sum);
    #endif
}

//------------------------------------------------------------------------------
/**
    Returns true if at least one entity exists with a matching
    attribute.
*/
bool
EntityManager::ExistsEntityByAttr(const Attribute& attr) const
{
    CategoryManager* catManager = CategoryManager::Instance();
    Util::Array<CategoryManager::Entry> catEntries;
    catEntries = catManager->GetInstancesByAttr(attr, true, false);
    IndexT i;
    for (i = 0; i < catEntries.Size(); i++)
    {
        Game::Entity* entity = (Game::Entity*) catEntries[i].Values()->GetRowUserData(catEntries[i].RowIndex());
        if (0 != entity)
        {
            n_assert(entity->IsA(Game::Entity::RTTI));
            return true;            
        }
    }
    // fallthrough: no entity exists with this attribute
    return false;
}

//------------------------------------------------------------------------------
/**
    Returns all entities which match a given attribute. If only the
    first entity is interesting (if you know that there will only one result)
    the onlyFirstEntity flag can be used to stop searching after the first match.
*/
Util::Array<Ptr<Game::Entity> >
EntityManager::GetEntitiesByAttr(const Attribute& attr, bool onlyFirstEntity)
{
    Util::Array<Ptr<Game::Entity> > result;
    CategoryManager* catManager = CategoryManager::Instance();

    // get all category manager instances according to the parameters
    Util::Array<CategoryManager::Entry> catEntries;
    catEntries = catManager->GetInstancesByAttr(attr, false, onlyFirstEntity);

    // update result, and create any missing entities
    IndexT i;
    for (i = 0; i < catEntries.Size(); i++)
    {
        Ptr<Game::Entity> entity = (Game::Entity*) catEntries[i].Values()->GetRowUserData(catEntries[i].RowIndex());
        if (entity.isvalid() && entity->IsA(Game::Entity::RTTI))
        {
            result.Append(entity);
        }        
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Returns all entities which match multiple attributes. If only the
    first entity is interesting (if you know that there will only one result)
    the onlyFirstEntity flag can be used to stop searching after the first match.
*/
Util::Array<Ptr<Game::Entity> >
EntityManager::GetEntitiesByAttrs(const Util::Array<Attribute>& attrs, bool onlyFirstEntity)
{
    Util::Array<Ptr<Game::Entity> > result;
    CategoryManager* catManager = CategoryManager::Instance();

    // get all category manager instances according to the parameters
    Util::Array<CategoryManager::Entry> catEntries;
    catEntries = catManager->GetInstancesByAttrs(attrs, false, onlyFirstEntity);

    // update result, and create any missing entities
    IndexT i;
    for (i = 0; i < catEntries.Size(); i++)
    {
        Ptr<Game::Entity> entity = (Game::Entity*) catEntries[i].Values()->GetRowUserData(catEntries[i].RowIndex());
		// FIXME why do we assert here, and why does instance table contain invalid entities to begin with (they seem to be deleted)
		//   n_assert(entity.isvalid());
		//   n_assert(entity->IsA(Game::Entity::RTTI));
		if (entity.isvalid() && entity->IsA(Game::Entity::RTTI))
		{
			result.Append(entity);
		}        
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Game::Entity>>
EntityManager::GetEntitiesByCategory(const Util::String & category)
{
	Util::Array<Ptr<Game::Entity>> result;
	CategoryManager* catManager = CategoryManager::Instance();
	Ptr<Db::ValueTable> instances = catManager->GetInstanceTable(category);
	for (int i = 0; i < instances->GetNumRows(); i++)
	{
		Ptr<Game::Entity> ent = (Game::Entity*)instances->GetRowUserData(i);
		result.Append(ent);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void
EntityManager::OnRenderDebug() 
{
    int entityIndex;
    int numEntities = this->activeEntities.Size();
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        if (this->activeEntities[entityIndex] != 0)
        {
            this->activeEntities[entityIndex]->OnRenderDebug();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Game::Entity>
EntityManager::GetEntityByAttr(const Attr::Attribute& attr, bool failOnError)
{
    Util::Array<Ptr<Game::Entity> > entities = this->GetEntitiesByAttr(attr, true);
    if (failOnError && (entities.Size() == 0))
    {        
        n_error("GetEntityByAttr failed, no entity found (attr: %s=%s)!",
            attr.GetName().AsCharPtr(),
            attr.ValueAsString().AsCharPtr());
    }
    if (entities.Size() == 0)
    {
        return 0;
    }
    return entities[0];
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Game::Entity>
EntityManager::GetEntityByAttrs(const Util::Array<Attr::Attribute>& attrs, bool failOnError)
{
    n_assert(attrs.Size() > 0);
    Util::Array<Ptr<Game::Entity> > entities = this->GetEntitiesByAttrs(attrs, true);
    if (failOnError && (entities.Size() == 0))
    {
        Util::String msg;
        msg.Format("GetEntityByAttrs failed! \n");
        IndexT i;
        for (i = 0; i < attrs.Size(); i++)
        {
            Util::String attrMsg;
            attrMsg.Format("attr: %s=%s\n", attrs[i].GetName().AsCharPtr(), attrs[i].ValueAsString().AsCharPtr());
            msg.Append(attrMsg);
        }
        n_error(msg.AsCharPtr());
    }
    if (entities.Size() == 0)
    {
        return 0;
    }
    return entities[0];
}

//------------------------------------------------------------------------------
/**
*/
void
EntityManager::OnLoad()
{
    // notify entities that a load is pending
    const Util::Array<Ptr<Game::Entity> >& entities = this->GetEntities();
    int entityIndex;
    int numEntities = entities.Size();
    if (numEntities > 0)
    {
        for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
        {
            if (entities[entityIndex].isvalid())
            {
                entities[entityIndex]->OnLoad();
            }
        }
    }

    Game::Manager::OnLoad();
}

//------------------------------------------------------------------------------
/**
*/
void
EntityManager::OnSave()
{
    // notify entities that a save is pending
    const Util::Array<Ptr<Game::Entity> >& entities = this->GetEntities();
    int entityIndex;
    int numEntities = entities.Size();
    if (numEntities > 0)
    {
        for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
        {
            if (entities[entityIndex].isvalid())
            {
                entities[entityIndex]->OnSave();
            }
        }
    }

    Game::Manager::OnSave();
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityManager::IsEntityInDelayedJobs(const Ptr<Game::Entity>& _entity)
{
	int index;

	for(index = 0 ; index < this->delayedJobs.Size() ; index++ )
	{
		if(this->delayedJobs[index].entity.isvalid() && this->delayedJobs[index].entity == _entity)	
		{
			return true;
		}
	}

	return false;
}

} // namespace Managers

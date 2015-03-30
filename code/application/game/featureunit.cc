//------------------------------------------------------------------------------
//  game/featureunit.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "game/featureunit.h"
#include "game/gameserver.h"

namespace Game
{
__ImplementClass(FeatureUnit, 'GAFE' , Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
FeatureUnit::FeatureUnit() : 
    active(false),
	renderDebug(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FeatureUnit::~FeatureUnit()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called by Game::GameServer::ActivateProperties(). 
    Use this method for one-time initializations of the FeatureUnit.
*/
void
FeatureUnit::OnActivate()
{
	n_assert(!this->IsActive());
	this->active = true;
}

//------------------------------------------------------------------------------
/**
    This method is called by Game::GameServer::DeactivateProperties(). Use this 
    method to cleanup stuff which has been initialized in OnActivate().
*/
void
FeatureUnit::OnDeactivate()
{
    n_assert(this->IsActive());

    // remove all managers
    while (this->managers.Size() > 0)
    {
        this->managers[0]->OnDeactivate();
        this->managers.EraseIndex(0);
    }

    this->active = false;
}

//------------------------------------------------------------------------------
/**
    This method is called from within Game::GameServer::Load()
    on load of a savegame.
*/
void
FeatureUnit::OnLoad()
{
    // now call the OnLoad() method on all managers
    int managerIndex;
    int numManagers = this->managers.Size();
    for (managerIndex = 0; managerIndex < numManagers; managerIndex++)
    {
        // invoke OnLoad() on manager
        this->managers[managerIndex]->OnLoad();
    }
}

//------------------------------------------------------------------------------
/**
    This method is called from within Game::GameServer::OnStart(). 
    Its called after all game features are activated and have initialized their subsystems.
    Use this
*/
void
FeatureUnit::OnStart()
{
    // call the OnStart method on all managers
    int i;
    int num = this->managers.Size();
    for (i = 0; i < num; i++)
    {
        this->managers[i]->OnStart();
    }
}

//------------------------------------------------------------------------------
/**
    This method is called from within Game::GameServer::Save().
    It's called on save of a game.
*/
void
FeatureUnit::OnSave()
{
    int managerIndex;
    int numManagers = this->managers.Size();
    for (managerIndex = 0; managerIndex < numManagers; managerIndex++)
    {
        this->managers[managerIndex]->OnSave();
    }  
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::GameServer::OnBeginFrame() on all 
    game features attached to an GameServer in the order of attachment. Override this 
    method if your FeatureUnit has to do any work at the beginning of the frame.
*/
void
FeatureUnit::OnBeginFrame()
{
    // call OnBeginFrame() on managers
    IndexT i;
    SizeT num = this->managers.Size();
    for (i = 0; i < num; i++)
    {
        #if __MANAGERS_STATS__
        // reset profiler.
        n_assert(this->managerProfilers.Size() > i);
        this->managerProfilers[i].ResetAccum();        
        this->managerProfilers[i].StartAccum();
        #endif

        this->managers[i]->OnBeginFrame();
        
        #if __MANAGERS_STATS__
        this->managerProfilers[i].StopAccum();
        #endif
    }    
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::GameServer::OnMoveBefore() on all 
    game features attached to an GameServer in the order of attachment. Override this
    method if your FeatureUnit has any work to do before the physics subsystem
    is triggered.
*/
void
FeatureUnit::OnFrame()
{
    // call OnFrame() on managers   
    IndexT i;
    SizeT num = this->managers.Size();
    for (i = 0; i < num; i++)
    {
        #if __MANAGERS_STATS__
        this->managerProfilers[i].StartAccum();
        #endif

        this->managers[i]->OnFrame();
        
        #if __MANAGERS_STATS__
        this->managerProfilers[i].StopAccum();
        #endif
    }
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::GameServer::OnRender() on all
    game features attached to an GameServer in the order of attachment. Override
    this method if your FeatureUnit has any work to do before rendering happens.
*/
void
FeatureUnit::OnEndFrame()
{
    // call OnEndFrame() on managers
    IndexT i;
    SizeT num = this->managers.Size();
    for (i = 0; i < num; i++)
    {
        #if __MANAGERS_STATS__
        this->managerProfilers[i].StartAccum();
        #endif

        this->managers[i]->OnEndFrame();
        
        #if __MANAGERS_STATS__
        this->managerProfilers[i].StopAccum();
        #endif
    }
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::GameServer::OnRenderDebug() on all
    game features attached to an GameServer in the order of attachment. It's meant for debug
    issues. It will be called when debug mode is enabled.
*/
void
FeatureUnit::OnRenderDebug()
{
    // call OnEndFrame() on managers
    IndexT i;
    SizeT num = this->managers.Size();
    for (i = 0; i < num; i++)
    {
        this->managers[i]->OnRenderDebug();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
FeatureUnit::StartRenderDebug()
{
    // overwrite in subclass if needed
}

//------------------------------------------------------------------------------
/**
*/
void
FeatureUnit::StopRenderDebug()
{
    // overwrite in subclass if needed
}

//------------------------------------------------------------------------------
/**
    Attach a manager object to the game world. The manager's OnActivate()
    method will be called once right away, and then its OnFrame() method
    once per frame.
*/
void
FeatureUnit::AttachManager(const Ptr<Manager>& manager)
{
    n_assert(manager);
    IndexT index = this->managers.FindIndex(manager);
    n_assert(InvalidIndex == index);

    manager->OnActivate();
    this->managers.Append(manager);

    #if __MANAGERS_STATS__
    // create profiler
    Util::String name = Util::String("profManga.Frame.Game.OnFrame.") + manager->GetRtti()->GetName() + " ";
    nProfiler profiler;
    profiler.Initialize(name.Get());
    this->managerProfilers.Append(profiler);
    #endif
}

//------------------------------------------------------------------------------
/**
    Remove a manager object from the game world. The manager's OnDeactivate()
    method will be called.
*/
void
FeatureUnit::RemoveManager(const Ptr<Manager>& manager)
{
    n_assert(manager);
    IndexT index = this->managers.FindIndex(manager);
    if (InvalidIndex != index)
    {
        this->managers[index]->OnDeactivate();
        this->managers.EraseIndex(index);

        #if __MANAGERS_STATS__
        // delete profiler
        n_assert(this->managerProfilers.Size() > index);
        this->managerProfilers.EraseIndex(index);
        #endif
    }
}
}; // namespace Game

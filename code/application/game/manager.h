#pragma once
//------------------------------------------------------------------------------
/**
    @class Game::Manager
    
    Managers are Singleton objects which care about some "specific global 
    stuff". They should be subclassed by Mangalore applications to implement
    globals aspects of the application (mainly global game play related stuff).
    
    Managers are created and triggered by game features. The frame trigger
    functions are invoked when the gameserver triggers the game feature.
    
    Standard-Mangalore uses several Managers to offer timing information
    (TimeManager), create entities and properties (FactoryManager), 
    manage game entities (EntityManager) and so forth.

    Managers are derived from Messaging::Port, so you *can* optionally use them to 
    receive and process messages.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "messaging/dispatcher.h"

//------------------------------------------------------------------------------
namespace Game
{
class Manager : public Messaging::Dispatcher
{
    __DeclareClass(Manager);
public:
    /// constructor
    Manager();
    /// destructor
    virtual ~Manager();

    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();
    /// return true if currently active
    bool IsActive() const;
    /// called before frame by the game server
    virtual void OnBeginFrame();
    /// called per-frame by the game server
    virtual void OnFrame();
    /// called after frame by the game server
    virtual void OnEndFrame();
    /// called after loading game state
    virtual void OnLoad();
    /// called before saving game state
    virtual void OnSave();
    /// called by Game::Server::Start() when the world is started
    virtual void OnStart();
    /// render a debug visualization 
    virtual void OnRenderDebug();

private:
    bool isActive;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
Manager::IsActive() const
{
    return this->isActive;
}

}; // namespace Game
//------------------------------------------------------------------------------
 
    
    
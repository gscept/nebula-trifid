#pragma once
//------------------------------------------------------------------------------
/**
    @class Application::StateHandler

    State handlers implement actual application state behaviour in subclasses
    of Application::StateHandler. The Application class calls state handler
    objects when a new state is entered, when the current state is left, and
    for each frame.
    
    State handlers must implement the OnStateEnter(), OnStateLeave() and
    OnStateFrame() methods accordingly.
    
    (C) 2007 RadonLabs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "app/application.h"

//------------------------------------------------------------------------------
namespace App
{
class StateHandler : public Core::RefCounted
{
    __DeclareClass(StateHandler);
public:
    /// constructor
    StateHandler();
    /// destructor
    virtual ~StateHandler();

    /// set state name
    void SetName(const Util::String& n);
    /// get state name
    const Util::String& GetName() const;
    /// called when attached to application object
    virtual void OnAttachToApplication();
    /// called when removed from the application object
    virtual void OnRemoveFromApplication();
    /// called when the state represented by this state handler is entered
    virtual void OnStateEnter(const Util::String& prevState);
    /// called when the state represented by this state handler is left
    virtual void OnStateLeave(const Util::String& nextState);
    /// called each frame as long as state is current, return new state
    virtual Util::String OnFrame();
    /// called after Db is opened, and before entities are loaded
    virtual void OnLoadBefore();
    /// called after entities are loaded
    virtual void OnLoadAfter();

protected:
    Util::String stateName;
};

//------------------------------------------------------------------------------
/**
*/
inline void
StateHandler::SetName(const Util::String& n)
{
    this->stateName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
StateHandler::GetName() const
{
    return this->stateName;
}

};
//------------------------------------------------------------------------------

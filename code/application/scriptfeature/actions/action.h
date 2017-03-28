#pragma once
//------------------------------------------------------------------------------
/**
    @class Actions::Action
    
    Actions are blocks that request a state change to the entitys, the world.
    By default the actions are just msgs that have the receiver set and on execute
    they will just be send to the target entity. The real action execution must be
    implemented in the property that handles this action msg.
    
    Actions are used by several other DSA subsystems, like the quest and dialog
    subsystem. Actions could be created from scripting or console.

    Actions can be created from a type string by DsaFactoryManager.
    
    (C) 2005 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "messaging/message.h"
#include "util/commandlineargs.h"
#include "game/entity.h"
#include "timing/time.h"

namespace Script
{
    class ActionReader;
	class InfoLog;
}

//------------------------------------------------------------------------------
namespace Actions
{

class Action : public Messaging::Message
{
    __DeclareClass(Action);
    __DeclareMsgId;
public:
    /// constructor
    Action();
    /// destructor
    virtual ~Action();
    /// create complete action from command string
    static Ptr<Action> CreateActionFromString(const Util::String& cmd);
    /// create several actions from semicolon-separated commands
    static Util::Array<Ptr<Action> > CreateActionsFromString(const Util::String& cmd);

    /// called when state is activated
    virtual void OnActivate();
    /// called when state is deactivated
    virtual void OnDeactivate();
    /// notify about incoming message
    virtual void Notify(const Ptr<Messaging::Message>& msg);

    /// start the action
    virtual bool Start();
    /// stop the action
    virtual void Stop();    

    /// execute the action, by default this sends self as a message to the target entity
    virtual void Execute();
    /// trigger the action, return if the action is still running, by default 
    /// this calls execute and returns false
    virtual bool Trigger();
    /// assert that all required data is present in the world database
    virtual void Assert();
    /// like Assert() but adds errors to the info log object instead of closing the application
    virtual bool Assert(const Ptr<Script::InfoLog>& infoLog);
    /// parse arguments from command line args object
	virtual void ParseArgs(const Util::CommandLineArgs& args);
    /// make the action show information what would happen if executed; default: do nothing
    virtual void ShowActionInfo();
    /// get time left (default: 0)
    virtual Timing::Time GetTimeLeft();  
    /// get current debug txt
    virtual Util::String GetDebugTxt();

	/// Set target entity to `v'.
	virtual void SetEntity(const Ptr<Game::Entity>& v);
	/// Target entity if exists.
	const Ptr<Game::Entity>& GetEntity() const;
	/// Does this contain a target entity?
	bool HasEntity() const;
    /// init after creation, parse args and set entity
    virtual void Init();

    /// write to action reader
    virtual void Write(const Ptr<Script::ActionReader>& actionReader);
    /// read from action reader
    virtual void Read(const Ptr<Script::ActionReader>& actionReader);

protected:
	Ptr<Game::Entity> entity;
    Timing::Time timeLeft;
};
__RegisterClass(Action);

//------------------------------------------------------------------------------
/**
*/
inline
Timing::Time
Action::GetTimeLeft()
{
    return this->timeLeft;
}

}; // namespace Actions
//------------------------------------------------------------------------------
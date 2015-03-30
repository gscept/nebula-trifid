#pragma once
//------------------------------------------------------------------------------
/**
    @class Conditions::Condition
    
    The base class for conditions. Conditions and Actions are the basic
    building blocks of world interaction. Conditions are small C++ objects
    which check whether some condition in the world is true. They are
    used by several other subsystems, like the quest and dialog system.
   
    (C) 2005 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "messaging/message.h"
#include "util/commandlineargs.h"
#include "game/entity.h"

namespace Script
{
    class ActionReader;
	class InfoLog;
}

//------------------------------------------------------------------------------
namespace Conditions
{
class Condition : public Messaging::Message
{
    __DeclareClass(Condition);
    __DeclareMsgId;
public:
    /// constructor
    Condition();
    /// create a complete condition object from a string
    static Ptr<Condition> CreateConditionFromString(const Util::String& cmd);
    /// create several conditions from semicolon-separated string
    static Util::Array<Ptr<Condition> > CreateConditionsFromString(const Util::String& cmd);

    /// assert correct initialisation 
    virtual void Assert();
    /// like Assert() but adds errors to the info log object instead of closing the application
    virtual bool Assert(const Ptr<Script::InfoLog>& infoLog);
    /// parse arguments from command line args object
	virtual void ParseArgs(const Util::CommandLineArgs& args);
	/// called when parent states becomes active
	virtual void OnActivate();
	/// called when parent states becomes inactive
	virtual void OnDeactivate();
	/// notify FSMConditions about message
	virtual void Notify(const Ptr<Messaging::Message>& msg);
    /// internal evaluation
    virtual bool Evaluate();
	/// Set optional entity to `v'.
	virtual void SetEntity(const Ptr<Game::Entity>& v);
	/// Get optional target entity if exists.
	const Ptr<Game::Entity>& GetEntity() const;
	/// Does this contain a target entity?
	bool HasEntity() const;
    /// set the condition's result, set by msg handler which may handle this
    void SetResult(bool b);
    /// get the set result, may be called by subclasses only
    bool GetResult() const;

    /// write to action reader
    virtual void Write(const Ptr<Script::ActionReader>& actionReader);
    /// read from action reader
    virtual void Read(const Ptr<Script::ActionReader>& actionReader);

protected:
	Ptr<Game::Entity> entity;

    bool result;
};
__RegisterClass(Condition);

//------------------------------------------------------------------------------
/**
*/
inline
void
Condition::SetResult(bool b)
{
    this->result = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Condition::GetResult() const
{
    return this->result;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
Condition::SetEntity(const Ptr<Game::Entity>& gameEntity)
{
    n_assert(gameEntity);
    this->entity = gameEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Ptr<Game::Entity>&
Condition::GetEntity() const
{
    return this->entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Condition::HasEntity() const
{
    return this->entity.isvalid();
}

}; // namespace Conditions
//------------------------------------------------------------------------------

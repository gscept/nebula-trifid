#pragma once
//------------------------------------------------------------------------------
/**
    @class Actions::IfThenElseAction

    contains a list of masterEvents

    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "scriptfeature/actions/action.h"

namespace Conditions
{
    class Condition;
}

//------------------------------------------------------------------------------
namespace Actions
{
class SequenceAction;
class ActionList;

class IfThenElseAction : public Actions::Action
{
    __DeclareClass(IfThenElseAction);
    __DeclareMsgId;

public:
    /// constructor
    IfThenElseAction();

    /// execute the actions in action list
    virtual void Execute();
    /// execute the actions in action list
    virtual bool Trigger();
    /// assert all actions in action list are valid 
    virtual void Assert();
    /// like Assert() but adds errors to the info log object instead of closing the application
    virtual bool Assert(const Ptr<Script::InfoLog>& infoLog);

    /// set condition block
    void SetCondition(const Ptr<Conditions::Condition>& c);
    /// get condition block
    const Ptr<Conditions::Condition>& GetCondition() const;

    /// set ActionList for then 
    void SetThenBlock(const Ptr<Actions::ActionList>& then);
    /// get then ActionList
    const Ptr<Actions::SequenceAction>& GetThenBlock() const;

    /// set else ActionList
    void SetElseBlock(const Ptr<Actions::ActionList>& elseBlock);
    /// get else ActionList
    const Ptr<Actions::SequenceAction>& GetElseBlock() const;
    /// has else block
    bool HasElseBlock() const;

	/// Set target entity to `v', also sets entity for subsequent actions
	virtual void SetEntity(const Ptr<Game::Entity>& v);

    /// write to action reader
    virtual void Write(const Ptr<Script::ActionReader>& actionReader);
    /// read from action reader
    virtual void Read(const Ptr<Script::ActionReader>& actionReader);

protected:
    Ptr<Conditions::Condition> condition;
    Ptr<Actions::SequenceAction> thenActionList;
    Ptr<Actions::SequenceAction> elseActionList;

    // the state of the action, if executed within a behaviour
    // if == eval condition, then == execute then action, else == execute else action
    enum IfThenState
    {
        ITS_IF,
        ITS_THEN,
        ITS_ELSE
    };

    IfThenState currState;
};

__RegisterClass(IfThenElseAction);

}; // namespace Actions
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
/**
    @class Actions::SequenceAction

    Executes a sequence of masterEvents.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "scriptfeature/actions/action.h"

//------------------------------------------------------------------------------
namespace Actions
{
class SequenceAction : public Actions::Action
{
    __DeclareClass(SequenceAction);
    __DeclareMsgId;

public:
    /// constructor
    SequenceAction();
    /// destructor
    virtual ~SequenceAction();

    /// notify about incoming message
    virtual void Notify(const Ptr<Messaging::Message>& msg);
    /// called when state is activated
    virtual void OnActivate();
    /// called when state is deactivated
    virtual void OnDeactivate();

    /// like Assert() but adds errors to the info log object instead of closing the application
    virtual bool Assert(const Ptr<Script::InfoLog>& infoLog);

    /// start the action
    virtual bool Start();
    /// stop the action
    virtual void Stop();    
    /// trigger the actions. use this for sequencially executing the actions within
    /// e.g. a FSM
    virtual bool Trigger();
    /// trigger the actions. will call execute on all actions. use this for
    /// action lists that shall instantly be executed
    virtual void Execute();

    /// append an action to the sequence (NOT allowed if action is running)
    /// by default the added action will be noted as the effective action (flag)
    /// the effective action will be used to evaluate "ShowActionInfow" (s.b.)
    void AppendAction(const Ptr<Action>& action, bool isEffectiveAction = true);

    /// gets the whole action list
    Util::Array<Ptr<Actions::Action> > GetActionList() const;

    /// write to action reader
    virtual void Write(const Ptr<Script::ActionReader>& actionReader);
    /// read from action reader
    virtual void Read(const Ptr<Script::ActionReader>& actionReader);

    /// any setup stuff that needs to be done on load
    virtual void OnLoad();

    /// insert an action, it will flatten an inserted sequence action 
    void FromList(const Ptr<Actions::Action>& actionOrList);

    /// append an array of actions, it will flatten sequence actions
    void FromList(const Util::Array<Ptr<Actions::Action> >& actions);

    /// get time left of all actions
    virtual Timing::Time GetTimeLeft();

    /// overridden set entity method to set entities to sequenced actions
    virtual void SetEntity(const Ptr<Game::Entity>& v);

    /// set save actions flag (wether or not action list should be saved)
    void SetSaveActionsFlag(bool s);
    /// get save actions flag (wether or not action list should be saved)
    bool GetSaveActionsFlag() const;

protected:
    /// start the current action, return false if out of actions or action returned false
    bool StartCurrAction();

    Util::Array<Ptr<Actions::Action> > actionList;
    int currAction;
    int effectiveAction;
    bool saveActions;   // flag, save action list on Write
    
    // from continuous action
    bool    isRunning;
    Timing::Time   lastFrameTime;
};
__RegisterClass(SequenceAction);

//------------------------------------------------------------------------------
/**
*/
inline
Util::Array<Ptr<Actions::Action> > 
SequenceAction::GetActionList() const
{
    return this->actionList;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
SequenceAction::AppendAction(const Ptr<Action>& action, bool effectiveAction)
{
    if (effectiveAction)
    {
        this->effectiveAction = this->actionList.Size();
    }
    this->actionList.Append(action);
}

//------------------------------------------------------------------------------
/**
*/
inline
Timing::Time
SequenceAction::GetTimeLeft()
{
    Timing::Time time = 0;
    int i;
    for (i=this->currAction; i < this->actionList.Size(); i++)
    {
        if (i < 0) continue;
        time += this->actionList[i]->GetTimeLeft();
    }
    return time;
}

}; // namespace FSM
//------------------------------------------------------------------------------

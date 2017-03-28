//------------------------------------------------------------------------------
//  scriptfeature/masterEvents/sequenceaction.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/actions/sequenceaction.h"
#include "scriptfeature/actionreader.h"
#include "scriptfeature/log/infolog.h"
#include "basegamefeature/basegametiming/gametimesource.h"

namespace Actions
{
__ImplementClass(Actions::SequenceAction, 'SEQA', Actions::Action);
__ImplementMsgId(SequenceAction);

//------------------------------------------------------------------------------
/**
*/
SequenceAction::SequenceAction() :
    currAction(-1),
    effectiveAction(-1),
    saveActions(true),
    isRunning(false),
    lastFrameTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SequenceAction::~SequenceAction()
{
    // empty
	
}

//------------------------------------------------------------------------------
/**
*/
bool
SequenceAction::Assert(const Ptr<Script::InfoLog>& infoLog)
{
    bool noErrors = true;
    int i;
    for (i = 0; i < this->actionList.Size(); i++)
    {
        noErrors &= this->actionList[i]->Assert(infoLog);
    }
    return noErrors;
}

//------------------------------------------------------------------------------
/**
*/
void
SequenceAction::SetEntity(const Ptr<Game::Entity>& v)
{
    Actions::Action::SetEntity(v);

    // can't soft assert the actions without a valid entity
    // no soft assert the actions will crash if the entity is not set
    IndexT idxAction;
    for (idxAction = 0; idxAction < this->actionList.Size(); idxAction++)
    {
        this->actionList[idxAction]->SetEntity(v);
    }
}

//------------------------------------------------------------------------------
/**
    The state machine will forward any incoming messages to the action.
*/
void
SequenceAction::Notify(const Ptr<Messaging::Message>& msg)
{
    // FIXME: combat stuff... 
    int i;
    for (i=0; i<this->actionList.Size(); i++)
    {
        this->actionList[i]->Notify(msg);       
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SequenceAction::OnActivate()
{
    this->isRunning = false;
    this->lastFrameTime = 0;

    // FIXME: combat stuff...
    int i;
    for (i=0; i<this->actionList.Size(); i++)
    {
        this->actionList[i]->OnActivate();      
    }
    this->currAction = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
SequenceAction::OnDeactivate()
{
    // FIXME: combat stuff... hairy and sticky
    int i;
    for (i=0; i<this->actionList.Size(); i++)
    {
        this->actionList[i]->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
*/    
void 
SequenceAction::SetSaveActionsFlag(bool s)
{
    this->saveActions = s;
}

//------------------------------------------------------------------------------
/**
*/    
bool 
SequenceAction::GetSaveActionsFlag() const
{
    return this->saveActions;
}

//------------------------------------------------------------------------------
/**
*/
bool
SequenceAction::Trigger()
{
    // itialize on first start
    if (!this->isRunning)
    {
        bool goOn = this->Start();
        if (!goOn)
        {
            this->Stop();
        }
        return goOn;
    }

    // trigger actions until hitting one that isnt finished (return true)
    // or all are finished (return false)
    if (this->StartCurrAction())
    {
        return true;
    }
    else
    {
        this->Stop();
        return false;
    }
}

//------------------------------------------------------------------------------
/**

*/
void
SequenceAction::Read(const Ptr<Script::ActionReader>& actionReader)
{
    Actions::Action::Read(actionReader);
    int numActions = actionReader->GetInt();
    this->currAction = actionReader->GetInt();
    this->effectiveAction = actionReader->GetInt();
    this->saveActions = actionReader->GetBool();
    if (this->GetSaveActionsFlag())
    {
        int i;
        for (i=0; i<numActions; i++)
        {
            Ptr<Action> newAction;
            newAction = actionReader->GetAction();
            this->actionList.Append(newAction);
        }
    }
}

//------------------------------------------------------------------------------
/**

*/
void
SequenceAction::Write(const Ptr<Script::ActionReader>& actionReader)
{
    Actions::Action::Write(actionReader);
    int numActions = this->actionList.Size();
    actionReader->PutInt(numActions);
    actionReader->PutInt(this->currAction);
    actionReader->PutInt(this->effectiveAction);
    actionReader->PutBool(this->saveActions);
    if (this->GetSaveActionsFlag())
    {
        int i;
        for (i=0; i<numActions; i++)
        {
            this->actionList[i]->Write(actionReader);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Called from action player property if action will be started
*/
bool
SequenceAction::StartCurrAction()
{
    while (this->currAction < this->actionList.Size())
    {
        actionList[this->currAction]->SetEntity(this->GetEntity());
        if (actionList[this->currAction]->Trigger())
        {
            return true;
        }
        else
        {
            // bail out if we got stopped
            if (!this->isRunning) return false;
            // action finished immediately, move on to next
            this->currAction++;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Called from action player property if action will be started
*/
bool
SequenceAction::Start()
{
    this->isRunning = true;
    this->lastFrameTime = BaseGameFeature::GameTimeSource::Instance()->GetTime();

    bool goOn = true;
    this->currAction = 0;
    goOn &= this->StartCurrAction();
    return goOn;
}

//------------------------------------------------------------------------------
/**
    Called from action player property if action will be stoped
*/
void
SequenceAction::Stop()
{
    // execute Stop on every action currently playing
    if (this->currAction >= 0 && this->currAction < this->actionList.Size())
    {
        if (this->actionList[this->currAction] != 0)
        {
            this->actionList[this->currAction]->Stop();
        }
    }
    this->currAction = -1;
    this->isRunning = false;
}

//------------------------------------------------------------------------------
/**

*/
void
SequenceAction::OnLoad()
{
    if (this->currAction >= 0)
    {
        n_assert(this->currAction < this->actionList.Size());
        this->actionList[this->currAction]->SetEntity(this->GetEntity());
    }
}

//------------------------------------------------------------------------------
/**
    insert an action, or flaten and insert a sequence
*/
void 
SequenceAction::FromList(const Ptr<Actions::Action>& actionOrList)
{
    if (actionOrList->IsA(Actions::SequenceAction::RTTI))
    {
        Ptr<Actions::SequenceAction> list = (const Ptr<Actions::SequenceAction>&) actionOrList;
        Util::Array<Ptr<Actions::Action> > actions = list->GetActionList();
        int i;
        for (i = 0; i < actions.Size(); i++)
        {
            this->FromList(actions[i]);
        }
    }
    else
    {
        this->actionList.Append(actionOrList);
    }
    if (actionOrList->HasEntity() && !this->HasEntity())
    {
        this->SetEntity(actionOrList->GetEntity());
    }
}

//------------------------------------------------------------------------------
/**

*/
void 
SequenceAction::FromList(const Util::Array<Ptr<Actions::Action> >& actions)
{
    int i;
    for (i = 0; i < actions.Size(); i++)
    {
        this->FromList(actions[i]);
    }
}

//------------------------------------------------------------------------------
/**
    execute action in action list
*/
void
SequenceAction::Execute()
{
    int i;
    // set trigger entities and entities attached to if it has not already happend
    // only do this if we got an entity (if an open action in quests etc. we dont have any)
    if (this->HasEntity())
    {
        for (i = 0; i < this->actionList.Size(); i++)
        {
            this->actionList[i]->SetEntity(this->GetEntity());
        }
    }
    
	int firstActionToExecute = Math::n_max(0, this->currAction);
    // execute actions beginning from current action
    for (i = firstActionToExecute; i < this->actionList.Size(); i++)
    {
        this->actionList[i]->Execute();
    }
}

} // namespace Actions


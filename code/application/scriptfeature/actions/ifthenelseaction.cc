//------------------------------------------------------------------------------
//  scriptfeature/masterEvents/ifthenelseaction.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/actions/ifthenelseaction.h"
#include "scriptfeature/log/infolog.h"
#include "scriptfeature/actionreader.h"
#include "scriptfeature/conditions/condition.h"
#include "scriptfeature/actions/sequenceaction.h"
#include "scriptfeature/actions/actionlist.h"

namespace Actions
{
__ImplementClass(Actions::IfThenElseAction, 'ITEA', Actions::Action);
__ImplementMsgId(IfThenElseAction);

//------------------------------------------------------------------------------
/**
*/
IfThenElseAction::IfThenElseAction() :
    currState(ITS_IF)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    execute action
*/
void
IfThenElseAction::Execute()
{
    n_assert(this->condition != 0);
    n_assert(this->thenActionList != 0);

    if (this->condition->Evaluate())
    {
        if (this->thenActionList->HasEntity())
        {
            this->thenActionList->SetEntity(this->GetEntity());
        }
        this->thenActionList->Execute();
    }
    else if (this->HasElseBlock())
    {
        if (this->elseActionList->HasEntity())
        {
            this->elseActionList->SetEntity(this->GetEntity());
        }

        this->elseActionList->Execute();
    }
}

//------------------------------------------------------------------------------
/**
    trigger action
*/
bool
IfThenElseAction::Trigger()
{
    n_assert(this->condition != 0);
    n_assert(this->thenActionList != 0);



    // initially eval condition
    if (this->condition->Evaluate())
    {
        this->currState = ITS_THEN;
    }
    else 
    {
        this->currState = ITS_ELSE;
    }

    // now either trigger if or else action
    if (ITS_THEN == this->currState)
    {
        this->thenActionList->SetEntity(this->GetEntity());
        return this->thenActionList->Trigger();
    }
    else if (ITS_ELSE == this->currState)
    {
        if (this->HasElseBlock())
        {
            this->elseActionList->SetEntity(this->GetEntity());
            return this->elseActionList->Trigger();
        }
    }
    else
    {
        n_error("some wired error in if-then-else action ::Trigger");
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
IfThenElseAction::Assert()
{
    n_assert(this->condition != 0);
    n_assert(this->thenActionList != 0);
    this->condition->Assert();
}

//------------------------------------------------------------------------------
/**
*/
bool
IfThenElseAction::Assert(const Ptr<Script::InfoLog>& infoLog)
{
    bool noErrors = true;
    infoLog->BeginSection("<" + this->GetClassName() + ">");

    if (this->condition == 0)
    {
        infoLog->AddInfo("No condition assigned!");
        noErrors = false;
    }

    if (this->thenActionList == 0)
    {
        infoLog->AddInfo("No then action list assigned!");
        noErrors = false;
    }

    if (noErrors)
    {
        noErrors &= this->condition->Assert(infoLog);

        infoLog->BeginSection("<ThenActionList>");
        noErrors &= this->thenActionList->Assert(infoLog);
        infoLog->EndSection("</ThenActionList>");

        if (this->elseActionList != 0)
        {
            infoLog->BeginSection("<ElseActionList>");
            noErrors &= this->elseActionList->Assert(infoLog);
            infoLog->EndSection("</ElseActionList>");
        }
    }

    infoLog->EndSection("</" + this->GetClassName() + ">");
    return noErrors;
}

//------------------------------------------------------------------------------
/**
*/    
void 
IfThenElseAction::SetCondition(const Ptr<Conditions::Condition>& c)
{
    n_assert(c != 0);
    this->condition = c; 
}

//------------------------------------------------------------------------------
/**
*/    
const Ptr<Conditions::Condition>& 
IfThenElseAction::GetCondition() const
{
    n_assert(this->condition != 0);
    return this->condition;
}

//------------------------------------------------------------------------------
/**
*/    
void 
IfThenElseAction::SetThenBlock(const Ptr<Actions::ActionList>& then)
{
    n_assert(then != 0);
    //this->thenActionList = then;
    this->thenActionList = Actions::SequenceAction::Create();
    this->thenActionList->FromList(then.cast<Actions::Action>());
}

//------------------------------------------------------------------------------
/**
*/    
const Ptr<Actions::SequenceAction>& 
IfThenElseAction::GetThenBlock() const
{
    n_assert(this->thenActionList != 0);
    return this->thenActionList;
}

//------------------------------------------------------------------------------
/**
*/    
void 
IfThenElseAction::SetElseBlock(const Ptr<Actions::ActionList>& elseBlock)
{
    n_assert(elseBlock != 0);
    //this->elseActionList = elseBlock;
    this->elseActionList = Actions::SequenceAction::Create();
    this->elseActionList->FromList(elseBlock.cast<Actions::Action>());
}

//------------------------------------------------------------------------------
/**
*/    
const Ptr<Actions::SequenceAction>& 
IfThenElseAction::GetElseBlock() const
{
    n_assert(this->HasElseBlock());
    return this->elseActionList;
}

//------------------------------------------------------------------------------
/**
*/   
bool 
IfThenElseAction::HasElseBlock() const
{
    return (this->elseActionList != 0); 
}

//------------------------------------------------------------------------------
/**
*/    
void 
IfThenElseAction::SetEntity(const Ptr<Game::Entity>& entity)
{
    Action::SetEntity(entity);

    if (this->thenActionList != 0)
    {
        this->thenActionList->SetEntity(this->GetEntity());
    }
    if (this->elseActionList != 0)
    {
        this->elseActionList->SetEntity(this->GetEntity());
    }
    if (this->condition != 0)
    {
        this->condition->SetEntity(this->GetEntity());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
IfThenElseAction::Read(const Ptr<Script::ActionReader>& actionReader)
{
    Action::Read(actionReader);

    // read trigger (execution) state
    this->currState = (IfThenState)(actionReader->GetInt());
    n_assert(this->currState >= ITS_IF && this->currState <= ITS_ELSE);

    // read condition 
    this->SetCondition(actionReader->GetCondition());

    // read then and optional else action
    Ptr<Actions::Action> seqAction = actionReader->GetAction();
    n_assert(seqAction->IsA(Actions::SequenceAction::RTTI));
	this->thenActionList = seqAction.cast<Actions::SequenceAction>();
    bool hasElseBlock = actionReader->GetBool();
    if (hasElseBlock)
    {
        Ptr<Actions::Action> seqAction = actionReader->GetAction();
        n_assert(seqAction->IsA(Actions::SequenceAction::RTTI));
        this->elseActionList = seqAction.cast<Actions::SequenceAction>();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
IfThenElseAction::Write(const Ptr<Script::ActionReader>& actionReader)
{
    Action::Write(actionReader);

    // write trigger (execution) state
    actionReader->PutInt((int)this->currState);

    this->condition->Write(actionReader);

    // write then and optional else action
    this->thenActionList->Write(actionReader);
    actionReader->PutBool(this->HasElseBlock());
    if (this->HasElseBlock())
    {
        this->elseActionList->Write(actionReader);
    }
}

}; // namespace Actions

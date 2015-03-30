//------------------------------------------------------------------------------
//  scriptfeature/conditions/and.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/conditions/and.h"
#include "scriptfeature/log/infolog.h"
#include "scriptfeature/actionreader.h"

namespace Conditions
{
__ImplementClass(Conditions::And, 'COAN', Conditions::Condition);
__ImplementMsgId(And);

//------------------------------------------------------------------------------
/**
*/
void 
And::AddCondition(const Ptr<Conditions::Condition>& condition)
{
    n_assert(condition);
    this->conditions.Append(condition);
}

//------------------------------------------------------------------------------
/**
*/
void
And::Assert()
{
    n_assert(this->conditions.Size() > 0);
    
	// check if the conditions are valid
    int i;
	for(i = 0; i < this->conditions.Size(); i++)
	{
		this->conditions[i]->Assert();
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
And::Assert(const Ptr<Script::InfoLog>& infoLog)
{
    bool noErrors = true;
    n_assert(this->conditions.Size() > 0);

	// check if the conditions are valid
    int i;
	for (i = 0; i < this->conditions.Size(); i++)
	{
        if (!this->conditions[i]->Assert(infoLog))
        {
            noErrors = false;
        }
	}

    return noErrors;
}

//------------------------------------------------------------------------------
/**
*/
void
And::OnActivate()
{
	for (int i = 0; i < this->conditions.Size(); i++)
	{
		this->conditions[i]->OnActivate();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
And::OnDeactivate()
{
	for (int i = 0; i < this->conditions.Size(); i++)
	{
		this->conditions[i]->OnDeactivate();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
And::Notify(const Ptr<Messaging::Message>& msg)
{
	for (int i = 0; i < this->conditions.Size(); i++)
	{
		this->conditions[i]->Notify(msg);
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
And::Evaluate()
{
	// check if for all conditions
	for(int i = 0; i < this->conditions.Size(); i++)
	{
        if (this->HasEntity())
        {
            this->conditions[i]->SetEntity(this->GetEntity());
        }
		if(!this->conditions[i]->Evaluate())
		{
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
And::Read(const Ptr<Script::ActionReader>& actionReader)
{
    Condition::Read(actionReader);
    int numConditions = actionReader->GetInt();
    int i;
    for (i=0; i<numConditions; i++)
    {
        Ptr<Condition> newCondition;
        newCondition = actionReader->GetCondition();
        this->conditions.Append(newCondition);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
And::Write(const Ptr<Script::ActionReader>& actionReader)
{
    Condition::Write(actionReader);
    int numConditions = this->conditions.Size();
    actionReader->PutInt(numConditions);
    int i;
    for (i=0; i<numConditions; i++)
    {
        this->conditions[i]->Write(actionReader);
    }
}

}; // namespace Conditions
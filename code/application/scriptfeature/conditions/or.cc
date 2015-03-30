//------------------------------------------------------------------------------
//  conditions/or.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/conditions/or.h"
#include "scriptfeature/log/infolog.h"
#include "scriptfeature/actionreader.h"

namespace Conditions
{
__ImplementClass(Conditions::Or, 'COOR', Conditions::Condition);
__ImplementMsgId(Or);

//------------------------------------------------------------------------------
/**
*/
void
Or::Assert()
{
	// check if there is a condition
	n_assert(this->conditions.Size() > 0);
    
	// check if the conditions are valid
    int i;
	for (i = 0; i < this->conditions.Size(); i++)
	{
		this->conditions[i]->Assert();
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
Or::Assert(const Ptr<Script::InfoLog>& infoLog)
{
    bool noErrors = true;

	// check if there is a condition
	if (this->conditions.Size() == 0)
    {
        noErrors = false;
    }
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
Or::OnActivate()
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
Or::OnDeactivate()
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
Or::Notify(const Ptr<Messaging::Message>& msg)
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
Or::Evaluate()
{
    n_assert(this->conditions.Size() > 0);
	// check if for all conditions
    int i;
	for (i = 0; i < this->conditions.Size(); i++)
	{
        if (this->HasEntity())
        {
            this->conditions[i]->SetEntity(this->GetEntity());
        }
		// check if one of the conditions evaluate with true
		if (this->conditions[i]->Evaluate())
		{
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
Or::AddCondition(const Ptr<Conditions::Condition>& condition)
{
    n_assert(condition);
    this->conditions.Append(condition);
}

//------------------------------------------------------------------------------
/**
*/
void
Or::Read(const Ptr<Script::ActionReader>& actionReader)
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
Or::Write(const Ptr<Script::ActionReader>& actionReader)
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
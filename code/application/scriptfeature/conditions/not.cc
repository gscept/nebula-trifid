//------------------------------------------------------------------------------
//  scriptfeature/conditions/not.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/conditions/not.h"
#include "scriptfeature/log/infolog.h"
#include "scriptfeature/actionreader.h"

namespace Conditions
{
__ImplementClass(Conditions::Not, 'CNOT', Conditions::Condition);
__ImplementMsgId(Not);

//------------------------------------------------------------------------------
/**
*/
void
Not::Assert()
{
	this->condition->Assert();
}

//------------------------------------------------------------------------------
/**
*/
bool
Not::Assert(const Ptr<Script::InfoLog>& infoLog)
{
    bool noErrors = true;

	// check if the condition is valid
	noErrors = this->condition->Assert(infoLog);

    return noErrors;
}

//------------------------------------------------------------------------------
/**
*/
void
Not::OnActivate()
{
	this->condition->OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void
Not::OnDeactivate()
{
	this->condition->OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
Not::Notify(const Ptr<Messaging::Message>& msg)
{
	this->condition->Notify(msg);
}

//------------------------------------------------------------------------------
/**
*/
bool
Not::Evaluate()
{
	return !this->condition->Evaluate();
}

//------------------------------------------------------------------------------
/**
*/
void
Not::Read(const Ptr<Script::ActionReader>& actionReader)
{
    Condition::Read(actionReader);
    this->condition = actionReader->GetCondition();
}

//------------------------------------------------------------------------------
/**
*/
void
Not::Write(const Ptr<Script::ActionReader>& actionReader)
{
    Condition::Write(actionReader);
    this->condition->Write(actionReader);
}

}; // namespace Conditions
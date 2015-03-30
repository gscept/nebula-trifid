//------------------------------------------------------------------------------
//  scriptfeature/conditions/condition.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/conditions/condition.h"
#include "scriptfeature/log/infolog.h"
#include "scriptfeature/actionreader.h"

namespace Conditions
{
__ImplementClass(Conditions::Condition, 'COND', Core::RefCounted);
__ImplementMsgId(Condition);

//------------------------------------------------------------------------------
/**
*/
Condition::Condition() :
    result(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Static method which creates any condition object from a command string
    of the form:

    cmd key0=value0 key1=value1 key2=value2
*/
Ptr<Condition>
Condition::CreateConditionFromString(const Util::String& cmd)
{
    n_assert(cmd.IsValid());

    // parse into a cmd line args object
    Util::CommandLineArgs args(cmd);

    // create condition object, and let it initialize from the command line args
    Util::String className("Conditions::");
    className.Append(args.GetCmdName());
	n_assert2(Core::Factory::Instance()->ClassExists(className), ("Condition::CreateConditionFromString: Unknown class \"" + className + "\"").AsCharPtr());
	Ptr<Core::RefCounted> ptr = Core::Factory::Instance()->Create(className);
	Ptr<Condition> condition = ptr.cast<Condition>();
    n_assert(condition && condition->IsA(Condition::RTTI));
    condition->ParseArgs(args);   

    return condition;
}

//------------------------------------------------------------------------------
/**
    Static method which creates many action from a string of the
    form accepted by CreateConditionFromString() where several conditions
    are separated by a semicolon.
*/
Util::Array<Ptr<Condition> >
Condition::CreateConditionsFromString(const Util::String& cmd)
{
    Util::Array<Util::String> tokens = cmd.Tokenize(";");
    Util::Array<Ptr<Condition> > conditions;
    int i;
    int num = tokens.Size();
    for (i = 0; i < num; i++)
    {
        // make sure we don't try to create from empty string
        if (!tokens[i].CheckValidCharSet(" \t\r="))
        {
            conditions.Append(CreateConditionFromString(tokens[i]));
        }
    }
    return conditions;
}

//------------------------------------------------------------------------------
/**
    This method evaluates whether the condition is true, Negates result if not flag is set.
*/
bool
Condition::Evaluate()
{
    // the default handling sends the condition to a target entity for evaluation
    this->entity->SendSync(this);
    return this->result;
}

//------------------------------------------------------------------------------
/**
    This method asserts that all data required by the conditions actually
    exists. Override in subclass!
*/
void
Condition::Assert()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method asserts that all data actually required by the conditions exist.
    In subclasses errors can be added to the info log object and in case of
    errors false can be returned instead of closing the application.

    Override in subclass! (infoLog in this class is ignored; Assert() will be
    called; returns always true)
*/
bool
Condition::Assert(const Ptr<Script::InfoLog>& infoLog)
{
    Assert();
    return true;
}

//------------------------------------------------------------------------------
/**
    This method should initialize the Condition object from a CmdLineArgs
    object. Override in subclass.
*/
void
Condition::ParseArgs(const Util::CommandLineArgs& args)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Condition::Read(const Ptr<Script::ActionReader>& actionReader)
{
    Util::String className = actionReader->GetClass();
    n_assert(className == this->GetRtti()->GetName());
    this->entity = actionReader->GetEntity();
    this->result = actionReader->GetBool();
}

//------------------------------------------------------------------------------
/**
*/
void
Condition::Write(const Ptr<Script::ActionReader>& actionReader)
{
	Ptr<Condition> thisPtr(this);
	Ptr<Core::RefCounted> thisRef = thisPtr.cast<Core::RefCounted>();
	actionReader->PutClass(thisRef);
    actionReader->PutEntity(this->entity);
    actionReader->PutBool(this->result);
}

//------------------------------------------------------------------------------
/**
*/
void
Condition::OnActivate()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
Condition::OnDeactivate()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
Condition::Notify(const Ptr<Messaging::Message>& msg)
{
	// empty
}

}; // namespace Conditions
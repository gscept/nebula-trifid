//------------------------------------------------------------------------------
//  scriptfeature/masterEvents/action.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/actions/action.h"
#include "scriptfeature/log/infolog.h"
#include "scriptfeature/actionreader.h"

using namespace Game;

namespace Actions
{
__ImplementClass(Actions::Action, 'ACTI', Messaging::Message);
__ImplementMsgId(Action);

//------------------------------------------------------------------------------
/**
*/
Action::Action():
    timeLeft(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Action::~Action()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Static method which creates any action object from a command string
    of the form:

    cmd key0=value0 key1=value1 key2=value2

    So you would create an action for instance like this:

    Action::CreateFromCmdString("UnlockQuest quest=Testquest");
*/
Ptr<Action>
Action::CreateActionFromString(const Util::String& cmd)
{
    n_assert(cmd.IsValid());

    // parse into a cmd line args object
	Util::CommandLineArgs args(cmd);

    // create action object, and let it initialize from the command line args
    Util::String className("Actions::");
    className.Append(args.GetCmdName());
	Ptr<Core::RefCounted> ptr = Core::Factory::Instance()->Create(className);
	Ptr<Action> action = ptr.cast<Action>();
    n_assert(action && action->IsA(Action::RTTI));
    action->ParseArgs(args);
    return action;
}

//------------------------------------------------------------------------------
/**
    Static method which creates many action from a string of the
    form accepted by CreateActionFromString() where several actions
    are separated by a semicolon.
*/
Util::Array<Ptr<Action> >
Action::CreateActionsFromString(const Util::String& cmd)
{
    Util::Array<Util::String> tokens = cmd.Tokenize(";");
    Util::Array<Ptr<Action> > actions;
    int i;
    int num = tokens.Size();
    for (i = 0; i < num; i++)
    {
        if (!tokens[i].CheckValidCharSet(" \t\r="))
        {
            actions.Append(CreateActionFromString(tokens[i]));
        }
    }
    return actions;
}

//------------------------------------------------------------------------------
/**
*/
void
Action::SetEntity(const Ptr<Game::Entity>& v)
{
	n_assert(v != 0);
	this->entity = v;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Game::Entity>&
Action::GetEntity() const
{
	n_assert2(HasEntity(), ("No Entity Set in Action: " + this->GetRtti()->GetName()).AsCharPtr());
	return this->entity;
}

//------------------------------------------------------------------------------
/**
*/
bool
Action::HasEntity() const
{
	return this->entity.isvalid();
}

//------------------------------------------------------------------------------
/**
    As default just send self to target.
*/
void
Action::Execute()
{
    n_assert(HasEntity());
    GetEntity()->SendSync(this);
}

//------------------------------------------------------------------------------
/**
    override in special action
*/
void
Action::ShowActionInfo()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Make sure the data required by the action is valid. This may not change the
    state of the world.
*/
void
Action::Assert()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method makes sure the data required by the action is valid. This may
    not change the state of the world. In sub classes errors can be added to the
    info log object and in case of errors false can be returned instead of
    closing the application.

    Override in subclass! (infoLog in this class is ignored; Assert() will be
    called; returns always true)
*/
bool
Action::Assert(const Ptr<Script::InfoLog>& infoLog)
{
    Assert();
    return true;
}

//------------------------------------------------------------------------------
/**
    Initialize the action from a CmdLineArgs object. This is necessary
    for the automatic scripting support.
*/
void
Action::ParseArgs(const Util::CommandLineArgs& args)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Action::Read(const Ptr<Script::ActionReader>& actionReader)
{
    Util::String className = actionReader->GetClass();
    n_assert(className == this->GetRtti()->GetName());
    this->entity = actionReader->GetEntity();
	this->timeLeft = actionReader->GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
void
Action::Write(const Ptr<Script::ActionReader>& actionReader)
{
	Ptr<Action> thisPtr(this);
	Ptr<Core::RefCounted> thisRef = thisPtr.cast<Core::RefCounted>();
	actionReader->PutClass(thisRef);
    actionReader->PutEntity(this->entity);
	actionReader->PutFloat((float)this->timeLeft);
}

//------------------------------------------------------------------------------
/**
*/
bool
Action::Trigger()
{
    this->Execute();
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
Action::Notify(const Ptr<Messaging::Message>& msg)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Action::OnActivate()
{
    // empty, derive in subclass
#if NEBULA3_DEBUG
    // print result if true
    n_printf("Action:%s::OnActivate!\n", this->GetClassName().AsCharPtr());    
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
Action::OnDeactivate()
{
    // empty, derive in subclass
}

//------------------------------------------------------------------------------
/**
*/
bool
Action::Start()
{
    // empty, derive in subclass
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
Action::Stop()
{
    // empty, derive in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
Action::Init()
{
    // implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
Action::GetDebugTxt()
{
    // override in subclass
    return this->GetClassName();
}
}; // namespace Actions

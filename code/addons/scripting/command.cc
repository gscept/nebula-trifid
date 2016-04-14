//------------------------------------------------------------------------------
//  scripting/command.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scripting/command.h"

namespace Scripting
{
__ImplementClass(Scripting::Command, 'SCMD', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
Command::Command() :
    isRegistered(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Command::~Command()
{
    n_assert(!this->IsRegistered());
    n_assert(this->args.IsEmpty());
    n_assert(this->results.IsEmpty());
}

//------------------------------------------------------------------------------
/**
    Called when the command is registered with the script server.
    Derive this method and setup the arguments the command requires and
    the block of results the command returns.
*/
void
Command::OnRegister()
{
    n_assert(!this->isRegistered);
    this->isRegistered = true;
}

//------------------------------------------------------------------------------
/**
    Called when the command is unregistered from the script server.
    This method must undo everything done in the OnRegister() method.
*/
void
Command::OnUnregister()
{
    n_assert(this->isRegistered);
    this->args.Clear();
    this->results.Clear();
    this->isRegistered = false;
}

//------------------------------------------------------------------------------
/**
    Called when the script command should be executed. The command arguments
    will be filled with the args from the script function call, after
    the command functionality has been executed the command should
    provide any results in its results block. If there was an error
    executing the command the command should return false and have
    an error string set.
*/
bool
Command::OnExecute()
{
    n_assert(this->isRegistered);
    this->error.Clear();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
String
Command::GetHelp() const
{
    return "No help provided for this command.";
}

//------------------------------------------------------------------------------
/**
    Returns a string describing the command syntax in the
    form "type name = cmd(type name, type name, ...)"
*/
String
Command::GetSyntax() const
{
    String str;
    str.Reserve(64);

    // output args
    IndexT resIndex;
    SizeT numResults = this->results.GetNumArgs();
    for (resIndex = 0; resIndex < numResults; resIndex++)
    {
        str.Append(Arg::TypeToString(this->results.GetArgValue(resIndex).GetType()));
        str.Append(" ");
        str.Append(this->results.GetArgName(resIndex));
        if (resIndex < (numResults - 1))
        {
            str.Append(", ");
        }
    }
    if (numResults > 0)
    {
        str.Append(" = ");
    }

    // cmd name
    str.Append(this->GetName());

    // format input args
    str.Append("(");
    IndexT argIndex;
    SizeT numArgs = this->args.GetNumArgs();
    for (argIndex = 0; argIndex < numArgs; argIndex++)
    {
        str.Append(Arg::TypeToString(this->args.GetArgValue(argIndex).GetType()));
        str.Append(" ");
        str.Append(this->args.GetArgName(argIndex));
        if (argIndex < (numArgs - 1))
        {
            str.Append(", ");
        }
    }
    str.Append(")");
    return str;
}

//------------------------------------------------------------------------------
/**
*/
void
Command::SetError(const char* fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);
    this->error.FormatArgList(fmt, argList);
    va_end(argList);
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Command::GetError() const
{
    return this->error;
}

} // namespace Scripting

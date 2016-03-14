#pragma once
#ifndef SCRIPTING_COMMAND_H
#define SCRIPTING_COMMAND_H
//------------------------------------------------------------------------------
/**
    @class Scripting::Command
  
    Base class for script commands. A script command object implements a
    new scripting command in a language independent fashion. New script
    commands are added by deriving a new class from Scripting::Command
    and registering it with the script server.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "scripting/argsblock.h"

//------------------------------------------------------------------------------
namespace Scripting
{
class Command : public Core::RefCounted
{
    __DeclareClass(Command);
public:
    /// constructor
    Command();
    /// destructor
    virtual ~Command();
    /// called when the scripting command is registered
    virtual void OnRegister();
    /// called when the scripting command is unregistered
    virtual void OnUnregister();
    /// return true if currently registered
    bool IsRegistered() const;
    /// called when the script command is executed
    virtual bool OnExecute();
    /// return a short help string, describing the command
    virtual Util::String GetHelp() const;
    /// get the name under which the command has been registered
    const Util::String& GetName() const;
    /// get syntax as string
    Util::String GetSyntax() const;
    /// read-only access to command args
    const ArgsBlock& GetArguments() const;
    /// read/write access to command arguments
    ArgsBlock& Arguments();
    /// access to result arguments
    const ArgsBlock& GetResults() const;
    /// get error string
    const Util::String& GetError() const;

private:
    friend class ScriptServer;

    /// set name used when registering the command
    void SetName(const Util::String& n);

protected:
    /// set error
    void __cdecl SetError(const char* fmt, ...);

    bool isRegistered;
    Util::String name;
    ArgsBlock args;
    ArgsBlock results;
    Util::String error;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
Command::IsRegistered() const
{
    return this->isRegistered;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Command::SetName(const Util::String& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
Command::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const ArgsBlock&
Command::GetArguments() const
{
    return this->args;
}

//------------------------------------------------------------------------------
/**
*/
inline
ArgsBlock&
Command::Arguments()
{
    return this->args;
}

//------------------------------------------------------------------------------
/**
*/
inline
const ArgsBlock&
Command::GetResults() const
{
    return this->results;
}

} // namespace Scripting
//------------------------------------------------------------------------------
#endif

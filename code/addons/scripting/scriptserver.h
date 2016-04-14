#pragma once
#ifndef SCRIPTING_SCRIPTSERVER_H
#define SCRIPTING_SCRIPTSERVER_H
//------------------------------------------------------------------------------
/**
    @class Scripting::ScriptServer

    Server class of the scripting subsystem. The scripting server keeps
    track of all registered class script interfaces and registered
    global script commands. Subclasses of script server know how
    to execute scripts of a specific language.
    
    (C) 2006 Radon Labs
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "io/uri.h"
#include "debug/scriptingpagehandler.h"
#include "util/variant.h"

//------------------------------------------------------------------------------
namespace Scripting
{
class Command;

class ScriptServer : public Core::RefCounted
{
    __DeclareClass(ScriptServer);
    __DeclareSingleton(ScriptServer);
public:
    /// constructor
    ScriptServer();
    /// destructor
    virtual ~ScriptServer();
    /// open the script server
    virtual bool Open();
    /// close the script server
    virtual void Close();
    /// return true if open
    bool IsOpen() const;
    /// set debugging with the HTTP interface
    void SetDebug(const bool b);
    /// register a command with the script server
    virtual void RegisterCommand(const Util::String& name, const Ptr<Command>& cmd);
    /// unregister a command from the script server
    virtual void UnregisterCommand(const Util::String& name);
    /// evaluate a script statement in a string
    virtual bool Eval(const Util::String& str);
    /// evaluate a script file
    virtual bool EvalScript(const IO::URI& uri);
	/// evaluate a script statement in a string with a single integer argument
	virtual bool EvalWithParameter(const Util::String& str, const Util::String& entry,uint parm);
	/// check if script contains a function, will assign nil to the name before (might remove global function)
	virtual bool ScriptHasFunction(const Util::String& script, const Util::String& func);
	/// creates a table for storing functions for faster calling
	virtual void CreateFunctionTable(const Util::String& name);
	/// store a function in table for faster calling
	virtual bool RegisterFunction(const Util::String& script, const Util::String& func, const Util::String& table, unsigned int entry, const Util::String & itemName = "object");
	/// remove a function from the table
	virtual bool UnregisterFunction(const Util::String& table, unsigned int entry);
	/// call previously registered function
	virtual bool CallFunction(const Util::String& table, unsigned int entry);
	/// call previously registered function with additional argument
	virtual bool CallFunction(const Util::String& table, unsigned int entry, uint parm);
	/// call previously registered function with additional argument
	virtual bool CallFunction(const Util::String& table, unsigned int entry, const Util::Variant& param);
    /// call previously registered function with an array of arguments
    virtual bool CallFunction(const Util::String& table, unsigned int entry, const Util::Array<Util::Variant>& params);

    /// return true if a command has been registered by name
    bool HasCommand(const Util::String& cmdName) const;
    /// return number of registered commands
    SizeT GetNumCommands() const;
    /// return pointer to command at index
    const Ptr<Command>& GetCommandByIndex(IndexT i) const;
    /// get pointer to command by command name
    const Ptr<Command>& GetCommandByName(const Util::String& cmdName) const;
    /// get error string if evaluation fails
    const Util::String& GetError() const;
	/// last call to eval or callfunction resulted in an error
	const bool HasError() const;
    /// print all registered commands to stdout
    void PrintCommandList() const;
    /// print help on a command on stdout
    void PrintCommandHelp(const Util::String& cmdName) const;

	/// add folder to script search path
	virtual void AddPath(const IO::URI& path);

protected:
    /// clear current error
    void ClearError();
    /// set error string
    void SetError(const Util::String& err);

private:
    bool isOpen;
    bool debug;
    Util::String error;
    Util::Dictionary<Util::String, Ptr<Command> > commandRegistry;
	Ptr<Debug::ScriptingPageHandler> pageHandler;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
ScriptServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ScriptServer::SetDebug(const bool b)
{
    this->debug = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ScriptServer::ClearError()
{
    this->error.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
ScriptServer::GetError() const
{
    return this->error;
}

//------------------------------------------------------------------------------
/**
*/
inline
const bool
ScriptServer::HasError() const
{
	return this->error.Length()>0;
}
} // namespace Scripting
//------------------------------------------------------------------------------
#endif


#pragma once
#ifndef SCRIPTING_LUASERVER_H
#define SCRIPTING_LUASERVER_H
//------------------------------------------------------------------------------
/**
    @class Scripting::LuaServer
  
    LUA backend for the Nebula3 scripting subsystem.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "scripting/scriptserver.h"
#include "lua/lua.h"

//------------------------------------------------------------------------------
namespace Scripting
{
class LuaServer : public ScriptServer
{
    __DeclareClass(LuaServer);
    __DeclareSingleton(LuaServer);
public:
    /// constructor
    LuaServer();
    /// destructor
    virtual ~LuaServer();
    /// open the script server
    virtual bool Open();
    /// close the script server
    virtual void Close();
    /// register a command with the script server
    virtual void RegisterCommand(const Util::String& name, const Ptr<Command>& cmd);
    /// unregister a command from the script server
    virtual void UnregisterCommand(const Util::String& name);
    /// evaluate a script statement in a string
    virtual bool Eval(const Util::String& str);
	/// evaluate a script statement in a string with a single integer argument
	virtual bool EvalWithParameter(const Util::String& str, const Util::String& entry, uint parm);
    /// evaluate a script file
    virtual bool EvalScript(const IO::URI& uri);
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
	virtual bool CallFunction(const Util::String& table, unsigned int entry, const Util::Variant& parm);
    /// call previously registered function with an array of arguments
    virtual bool CallFunction(const Util::String& table, unsigned int entry, const Util::Array<Util::Variant>& params);

	/// set lua package search path
	virtual void AddPath(const IO::URI & path);

private:
    struct LuaStringReaderData
    {
        const Util::String* str;
    };

	

    /// LUA function callback
    static int LuaFunctionCallback(lua_State* s);
    /// LUA string chunk reader
    static const char* LuaStringReader(lua_State* s, LuaStringReaderData* data, size_t* size);
    /// LUA memory allocator
    static void* LuaAllocator(void* ud, void* ptr, size_t osize, size_t nsize);

    lua_State* luaState;
};

} // namespace Scripting
//------------------------------------------------------------------------------
#endif


//------------------------------------------------------------------------------
//  luaserver.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scripting/lua/luaserver.h"
#include "scripting/command.h"
#include "scripting/argsblock.h"
#include "io/console.h"
#include "lua/lualib.h"
#include "io/ioserver.h"
#include "io/filestream.h"
#include "io/textreader.h"

using namespace IO;

namespace Scripting
{
__ImplementClass(Scripting::LuaServer, 'LUAS', Scripting::ScriptServer);
__ImplementSingleton(Scripting::LuaServer);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
LuaServer::LuaServer() :
    luaState(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
LuaServer::~LuaServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    The custom memory allocator function for LUA.
*/
void*
LuaServer::LuaAllocator(void* /*ud*/, void* ptr, size_t osize, size_t nsize)
{
    if (0 == nsize)
    {
        // free a block of memory
        if (0 != ptr)
        {
            Memory::Free(Memory::DefaultHeap, ptr);
        }
        return 0;
    }
    else
    {
        if (0 == osize)
        {
            // allocate a block of memory
            return Memory::Alloc(Memory::DefaultHeap, nsize);
        }
        else
        {
            // reallocate a block of memory
            return Memory::Realloc(Memory::DefaultHeap, ptr, nsize);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Callback function for lua_load() to load Lua code from a String object.
*/
const char*
LuaServer::LuaStringReader(lua_State* /*s*/, LuaStringReaderData* data, size_t* size)
{
    n_assert(0 != data);
    if (0 != data->str)
    {
        *size = data->str->Length();
        const char* code = data->str->AsCharPtr();
        data->str = 0;
        return code;
    }
    else
    {
        // we have already been called, return end-of-data
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
LuaServer::AddLuaPath(const IO::URI & path)
{	
	lua_getglobal(this->luaState, "package");
	lua_getfield(this->luaState, -1, "path"); // get field "path" from table at top of stack (-1)
	Util::String cur_path = lua_tostring(this->luaState, -1); // grab path string from top of stack
	cur_path.Append(";"); // do your path magic here
	Util::String extrapath = path.LocalPath();
#ifdef WIN32
	extrapath.SubstituteChar('/', '\\');
#endif
	cur_path.Append(extrapath);
	lua_pop(this->luaState, 1); // get rid of the string on the stack we just pushed on line 5
	lua_pushstring(this->luaState, cur_path.AsCharPtr()); // push the new one
	lua_setfield(this->luaState, -2, "path"); // set the field "path" in table at -2 with value at top of stack
	lua_pop(this->luaState, 1); // get rid of package table from top of stack			
}

//------------------------------------------------------------------------------
/**
    This is the global callback function for all custom LUA commands which
    have been created with RegisterCommand().
*/
int
LuaServer::LuaFunctionCallback(lua_State* s)
{
    n_assert(0 != s);
    LuaServer* luaServer = LuaServer::Instance();
    IO::Console* con = IO::Console::Instance();

    // get the associated scripting command object
    Command* cmd = (Command*) lua_touserdata(luaServer->luaState, lua_upvalueindex(1));

    // pop arguments from stack
    ArgsBlock& cmdArgs = cmd->Arguments();
    SizeT numLuaArgs = lua_gettop(luaServer->luaState);
    if (numLuaArgs == cmdArgs.GetNumArgs())
    {
        IndexT argIndex;
        for (argIndex = 0; argIndex < cmdArgs.GetNumArgs(); argIndex++)
        {
            // Lua arg indices are 1-based
            Arg& curArg = cmdArgs.ArgValue(argIndex);
            switch (curArg.GetType())
            {
                case Arg::Int:
                    curArg.SetInt((int)lua_tonumber(luaServer->luaState, argIndex + 1));
                    break;

                case Arg::Float:
                    curArg.SetFloat((float)lua_tonumber(luaServer->luaState, argIndex + 1));
                    break;

                case Arg::Bool:
                    curArg.SetBool((lua_toboolean(luaServer->luaState, argIndex + 1) == 0) ? false : true);
                    break;

                case Arg::String:
                    curArg.SetString(lua_tostring(luaServer->luaState, argIndex + 1));
                    break;

				case Arg::UInt:
					curArg.SetUInt((int)lua_tonumber(luaServer->luaState, argIndex + 1));
					break;
				case Arg::Float4:
					{
						Math::float4 f;
						for(int i=0;i<4;i++)
						{
							lua_pushinteger(luaServer->luaState, i+1);
							lua_gettable(luaServer->luaState,argIndex +1);
							f[i] = (float)lua_tonumber(luaServer->luaState,-1);
						}
						curArg.SetFloat4(f);
					}
					break;
				case Arg::Matrix44:
					{
						Util::String matString = lua_tostring(luaServer->luaState, argIndex + 1);
						curArg.SetMatrix44(matString.AsMatrix44());
					}
					break;
                default:
                    con->Print("Invalid input type '%s' in cmd '%s'!\n",
                        Arg::TypeToString(curArg.GetType()).AsCharPtr(),
                        cmd->GetName().AsCharPtr());
                    return 0;
            }
        }

        // now execute the command
        cmd->OnExecute();
        
        // transfer results back to LUA
        const ArgsBlock& results = cmd->GetResults();
        IndexT resIndex;
        SizeT numResults = results.GetNumArgs();
        for (resIndex = 0; resIndex < numResults; resIndex++)
        {
            const Arg& curArg = results.GetArgValue(resIndex);
            switch (curArg.GetType())
            {
                case Arg::Int:
                    lua_pushnumber(luaServer->luaState, curArg.GetInt());
                    break;
				case Arg::UInt:
					lua_pushnumber(luaServer->luaState, curArg.GetUInt());
					break;
                case Arg::Float:
                    lua_pushnumber(luaServer->luaState, curArg.GetFloat());
                    break;
                case Arg::Bool:
                    lua_pushboolean(luaServer->luaState, curArg.GetBool());
                    break;
                case Arg::String:
                    lua_pushstring(luaServer->luaState, curArg.GetString().AsCharPtr());
                    break;
				case Arg::Float4:
					{
						lua_newtable(luaServer->luaState);
						int tableIndex = lua_gettop(luaServer->luaState);
						IndexT i;
						for (i = 0; i < 4; i++)
						{
							lua_pushnumber(luaServer->luaState, i+1);
							lua_pushnumber(luaServer->luaState, curArg.GetFloat4()[i]);
							lua_settable(luaServer->luaState, tableIndex);
						}
					}					
					break;
                case Arg::IntArray:
                    {
                        const Array<int>& intArray = curArg.GetIntArray();
                        lua_newtable(luaServer->luaState);
                        int tableIndex = lua_gettop(luaServer->luaState);
                        IndexT i;
                        for (i = 0; i < intArray.Size(); i++)
                        {
                            lua_pushnumber(luaServer->luaState, i);
                            lua_pushnumber(luaServer->luaState, intArray[i]);
                            lua_settable(luaServer->luaState, tableIndex);
                        }
                    }
                    break;
                case Arg::FloatArray:
                    {
                        const Array<float>& floatArray = curArg.GetFloatArray();
                        lua_newtable(luaServer->luaState);
                        int tableIndex = lua_gettop(luaServer->luaState);
                        IndexT i;
                        for (i = 0; i < floatArray.Size(); i++)
                        {
                            lua_pushnumber(luaServer->luaState, i);
                            lua_pushnumber(luaServer->luaState, floatArray[i]);
                            lua_settable(luaServer->luaState, tableIndex);
                        }
                    }
                    break;

                case Arg::BoolArray:
                    {
                        const Array<bool>& boolArray = curArg.GetBoolArray();
                        lua_newtable(luaServer->luaState);
                        int tableIndex = lua_gettop(luaServer->luaState);
                        IndexT i;
                        for (i = 0; i < boolArray.Size(); i++)
                        {
                            lua_pushnumber(luaServer->luaState, i);
                            lua_pushboolean(luaServer->luaState, boolArray[i]);
                            lua_settable(luaServer->luaState, tableIndex);
                        }
                    }
                    break;

                case Arg::StringArray:
                    {
                        const Array<String>& stringArray = curArg.GetStringArray();
                        lua_newtable(luaServer->luaState);
                        int tableIndex = lua_gettop(luaServer->luaState);
                        IndexT i;
                        for (i = 0; i < stringArray.Size(); i++)
                        {
                            lua_pushnumber(luaServer->luaState, i);
                            lua_pushstring(luaServer->luaState, stringArray[i].AsCharPtr());
                            lua_settable(luaServer->luaState, tableIndex);
                        }
                    }
                    break;

				case Arg::Matrix44:
					{
						/// treat as opaque string
						Util::String mat = Util::String::FromMatrix44(curArg.GetMatrix44());
						lua_pushstring(luaServer->luaState, mat.AsCharPtr());
					}
					break;

                default:
                    con->Error("Invalid result type '%s' in cmd '%s'!\n",
                        Arg::TypeToString(curArg.GetType()).AsCharPtr(),
                        cmd->GetName().AsCharPtr());
                    return 0;
            }
        }
        return numResults;
    }
    else
    {
		con->Error("Wrong number of args in '%s'\n", cmd->GetName().AsCharPtr());
		con->Error("Syntax is: %s\n", cmd->GetSyntax().AsCharPtr());
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
LuaServer::Open()
{
    n_assert(!this->IsOpen());
    n_assert(0 == this->luaState);
    if (ScriptServer::Open())
    {
        // initialize Lua
        this->luaState = lua_newstate(LuaAllocator, 0);
        n_assert(0 != this->luaState);

        // provide access to some standard libraries
		luaL_openlibs(this->luaState);        		
		this->AddLuaPath("scr:/?/init.lua");
		this->AddLuaPath("scr:/?.lua");
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
LuaServer::Close()
{
    n_assert(this->IsOpen());
    n_assert(0 != this->luaState);
    
    // this will unregister all commands
    ScriptServer::Close();

    // close Lua
    lua_close(this->luaState);
    this->luaState = 0;    
}

//------------------------------------------------------------------------------
/**
    Registers a new script command with the LUA server.
*/
void
LuaServer::RegisterCommand(const String& name, const Ptr<Command>& cmd)
{
    n_assert(0 != this->luaState);

    // call parent class to add command to the registry
    ScriptServer::RegisterCommand(name, cmd);
    
    // Register the command with Lua, all commands are assigned
    // to the same C-callback function. In order to identify which
    // command has called the C-callback we push the pointer
    // to the Command object as a C closure (see Lua ref manual for details)
    lua_pushstring(this->luaState, name.AsCharPtr());
    lua_pushlightuserdata(this->luaState, cmd);
    lua_pushcclosure(this->luaState, LuaFunctionCallback, 1);
    lua_settable(this->luaState, LUA_GLOBALSINDEX);
}

//------------------------------------------------------------------------------
/**
    Unregister a script command.
*/
void
LuaServer::UnregisterCommand(const String& name)
{
    // FIXME: hmm, how do we unregister a command from LUA?

    ScriptServer::UnregisterCommand(name);
}

//------------------------------------------------------------------------------
/**
    Evaluates a piece of LUA code in a string.
*/
bool
LuaServer::Eval(const String& str)
{
    n_assert(this->IsOpen());
    n_assert(0 != this->luaState);
    n_assert(str.IsValid());

    // clear current error string
    this->ClearError();

    // let Lua compile the chunk
    LuaStringReaderData data;
    data.str = &str;
    int res = lua_load(this->luaState, (lua_Reader) LuaStringReader, &data, "LuaServer::Eval()");
    if (0 == res)
    {
        // execute the piece of code
        // NOTE: the error handler is 0, but if we want a
        // stack trace this would be the place to create it
        int res = lua_pcall(this->luaState, 0, 0, 0);
        if (res == 0)
        {
            // all ok
            return true;
        }
        else
        {
            // an error occured during execution, get the error message from the stack
            this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));			
            return false;
        }
    }
    else
    {
        // an error occured during compilation, get the error message from the stack
        this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));		
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
LuaServer::EvalWithParameter(const Util::String& str, const Util::String& entry, uint parm)
{
	// FIXME ugly copy/paste from Eval, need something more generic later
	n_assert(this->IsOpen());
	n_assert(0 != this->luaState);
	n_assert(str.IsValid());

	// clear current error string
	this->ClearError();

	// let Lua compile the chunk
	LuaStringReaderData data;
	data.str = &str;
	int res = lua_load(this->luaState, (lua_Reader) LuaStringReader, &data, "LuaServer::EvalWithParameter()");
	if (0 == res)
	{
		// prime the script (dunno if necessary, internet says so
		if (0 != lua_pcall(this->luaState, 0, 0, 0))                  
		{
			// an error occured during execution, get the error message from the stack
			this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));			
			return false;
		}
			
		lua_getglobal(this->luaState,entry.AsCharPtr());
		lua_pushinteger(this->luaState,parm);
		// execute the piece of code
		// NOTE: the error handler is 0, but if we want a
		// stack trace this would be the place to create it
		int res = lua_pcall(this->luaState, 1, 0, 0);
		if (res == 0)
		{
			// all ok
			return true;
		}
		else
		{
			// an error occured during execution, get the error message from the stack
			this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));			
			return false;
		}
	}
	else
	{
		// an error occured during compilation, get the error message from the stack
		this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));		
		return false;
	}

}

//------------------------------------------------------------------------------
/**
*/
bool
LuaServer::EvalScript(const IO::URI& uri)
{
	Ptr<Stream> stream = IoServer::Instance()->CreateStream(uri);
	Ptr<TextReader> reader = TextReader::Create();
	reader->SetStream(stream);
	if(reader->Open())
	{
		Util::String script = reader->ReadAll();
		reader->Close();
		return Eval(script);		
	}
	else
		return false;
	
}

//------------------------------------------------------------------------------
/**
*/
bool 
LuaServer::ScriptHasFunction(const Util::String& str, const Util::String& func)
{
	n_assert(this->IsOpen());
	n_assert(0 != this->luaState);
	n_assert(str.IsValid());

	// clear current error string
	this->ClearError();

	LuaStringReaderData data;

	// clear out an existing function in case it exists
	Util::String clearScript;
	{
		clearScript.Format("%s = nil\n",func.AsCharPtr());
		data.str = &clearScript;
		int res = lua_load(this->luaState,(lua_Reader)LuaStringReader,&data, "LuaServer::ScriptHasFunction()");
		if(res == 0)
		{
			res = lua_pcall(this->luaState, 0, 0, 0);
		}
	}
	
	// let Lua compile the chunk
	
	data.str = &str;
	int res = lua_load(this->luaState, (lua_Reader) LuaStringReader, &data, "LuaServer::ScriptHasFunction()");
	if (0 == res)
	{
		// execute the piece of code
		// NOTE: the error handler is 0, but if we want a
		// stack trace this would be the place to create it
		int res = lua_pcall(this->luaState, 0, 0, 0);
		if (res == 0)
		{
			lua_getglobal(this->luaState,func.AsCharPtr());
			if(lua_isfunction(this->luaState,lua_gettop(this->luaState)))
			{
				return true;
			}
			return false;
		}
		else
		{
			// an error occured during execution, get the error message from the stack
			this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));			
			return false;
		}
	}
	else
	{
		// an error occured during compilation, get the error message from the stack
		this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));		
		return false;
	}
}

//------------------------------------------------------------------------------
/**
*/
void LuaServer::CreateFunctionTable(const Util::String& name)
{
	// create a table for registering functions
	lua_newtable(this->luaState);
	lua_setglobal(this->luaState,name.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
bool LuaServer::RegisterFunction( const Util::String& script, const Util::String& func, const Util::String& table, unsigned int entry, const Util::String & itemName )
{
	n_assert(this->IsOpen());
	n_assert(0 != this->luaState);
	n_assert(script.IsValid());

	// clear current error string
	this->ClearError();

	LuaStringReaderData data;

	Util::String debugName;
#if !PUBLIC_BUILD	
	debugName.Format("%s_%d",itemName.AsCharPtr(),entry);
#else
	debugName=func;
#endif

	data.str = &script;
	int res = lua_load(this->luaState, (lua_Reader) LuaStringReader, &data, debugName.AsCharPtr());
	if (0 == res)
	{
		// execute the piece of code
		// NOTE: the error handler is 0, but if we want a
		// stack trace this would be the place to create it
		int res = lua_pcall(this->luaState, 0, 0, 0);
		if (res == 0)
		{
			lua_getglobal(this->luaState,func.AsCharPtr());
			if(lua_isfunction(this->luaState,lua_gettop(this->luaState)))
			{
				lua_getglobal(this->luaState,table.AsCharPtr());
				lua_pushnumber(this->luaState,entry);
				lua_getglobal(this->luaState,func.AsCharPtr());
				n_assert2(lua_isfunction(this->luaState,lua_gettop(this->luaState)),"tried to assign a non-function to table");
				lua_settable(this->luaState,-3);
				lua_pop(this->luaState,1);
				return true;
			}
			return false;
		}
		else
		{
			// an error occured during execution, get the error message from the stack
			this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));			
			return false;
		}
	}
	else
	{
		// an error occured during compilation, get the error message from the stack
		this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));		
		return false;
	}			
		
	return false;
}


//------------------------------------------------------------------------------
/**
*/
bool
LuaServer::CallFunction(const Util::String& table, unsigned int entry)
{
	lua_getglobal(this->luaState,table.AsCharPtr());
	lua_pushnumber(this->luaState,entry);
	lua_gettable(this->luaState,-2);
	n_assert2(lua_isfunction(this->luaState,lua_gettop(this->luaState)),"tried to call non existant function");
	lua_pushinteger(this->luaState,entry);
	// execute the piece of code
	// NOTE: the error handler is 0, but if we want a
	// stack trace this would be the place to create it
	int res = lua_pcall(this->luaState, 1, 0, 0);
	if (res == 0)
	{
		// all ok
		return true;
	}
	else
	{
		// an error occured during execution, get the error message from the stack
		this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));		
		return false;
	}
	
}

//------------------------------------------------------------------------------
/**
*/
bool
LuaServer::CallFunction(const Util::String& table, unsigned int entry, uint parm)
{
	lua_getglobal(this->luaState,table.AsCharPtr());
	lua_pushnumber(this->luaState,entry);
	lua_gettable(this->luaState,-2);
	n_assert2(lua_isfunction(this->luaState,lua_gettop(this->luaState)),"tried to call non existant function");
	lua_pushinteger(this->luaState,entry);
	lua_pushinteger(this->luaState,parm);
	// execute the piece of code
	// NOTE: the error handler is 0, but if we want a
	// stack trace this would be the place to create it
	int res = lua_pcall(this->luaState, 2, 0, 0);
	if (res == 0)
	{
		// all ok
		return true;
	}
	else
	{
		// an error occured during execution, get the error message from the stack
		this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));				
		return false;
	}

}

//------------------------------------------------------------------------------
/**
*/
bool
LuaServer::CallFunction(const Util::String& table, unsigned int entry, const Util::Variant& param)
{
	lua_getglobal(this->luaState, table.AsCharPtr());
	lua_pushnumber(this->luaState, entry);
	lua_gettable(this->luaState, -2);
	n_assert2(lua_isfunction(this->luaState, lua_gettop(this->luaState)), "Tried to call non existent function");
	lua_pushinteger(this->luaState, entry);
	switch(param.GetType())
	{		
		case Variant::Int:
			lua_pushnumber(this->luaState, param.GetInt());
			break;
		case Variant::UInt:
			lua_pushnumber(this->luaState, param.GetUInt());
			break;
		case Variant::Float:
			lua_pushnumber(this->luaState, param.GetFloat());
			break;
		case Variant::Bool:
			lua_pushboolean(this->luaState, param.GetBool());
			break;
		case Variant::String:
			lua_pushstring(this->luaState, param.GetString().AsCharPtr());
			break;
		case Variant::Float4:
			{
				lua_newtable(this->luaState);
				int tableIndex = lua_gettop(this->luaState);
                Math::float4 val = param.GetFloat4();
				IndexT i;
				for (i = 0; i < 4; i++)
				{
					lua_pushnumber(this->luaState, i+1);
					lua_pushnumber(this->luaState, val[i]);
					lua_settable(this->luaState, tableIndex);
				}
			}					
			break;
		default:
			n_error("Parameter type not implemented");
			break;	
	}	
	// execute the piece of code
	// NOTE: the error handler is 0, but if we want a
	// stack trace this would be the place to create it
	int res = lua_pcall(this->luaState, 2, 0, 0);
	if (res == 0)
	{
		// all ok
		return true;
	}
	else
	{
		// an error occured during execution, get the error message from the stack
		this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));				
		return false;
	}

}

//------------------------------------------------------------------------------
/**
*/
bool 
LuaServer::CallFunction( const Util::String& table, unsigned int entry, const Util::Array<Util::Variant>& params )
{
    lua_getglobal(this->luaState, table.AsCharPtr());
    lua_pushnumber(this->luaState, entry);
    lua_gettable(this->luaState, -2);
    n_assert2(lua_isfunction(this->luaState, lua_gettop(this->luaState)), "Tried to call non existent function");
    lua_pushinteger(this->luaState, entry);

    IndexT paramIndex;
    for (paramIndex = 0; paramIndex < params.Size(); paramIndex++)
    {
        const Util::Variant& param = params[paramIndex];
        switch(param.GetType())
        {		
        case Variant::Int:
            lua_pushnumber(this->luaState, param.GetInt());
            break;
        case Variant::UInt:
            lua_pushnumber(this->luaState, param.GetUInt());
            break;
        case Variant::Float:
            lua_pushnumber(this->luaState, param.GetFloat());
            break;
        case Variant::Bool:
            lua_pushboolean(this->luaState, param.GetBool());
            break;
        case Variant::String:
            lua_pushstring(this->luaState, param.GetString().AsCharPtr());
            break;
        case Variant::Float4:
            {
                lua_newtable(this->luaState);
                int tableIndex = lua_gettop(this->luaState);
                Math::float4 val = param.GetFloat4();
                IndexT i;
                for (i = 0; i < 4; i++)
                {
                    lua_pushnumber(this->luaState, i+1);
                    lua_pushnumber(this->luaState, val[i]);
                    lua_settable(this->luaState, tableIndex);
                }
            }					
            break;
        default:
            n_error("Parameter type not implemented");
            break;	
        }	
    }
 
    // execute the piece of code
    // NOTE: the error handler is 0, but if we want a
    // stack trace this would be the place to create it
    int res = lua_pcall(this->luaState, params.Size() + 1, 0, 0);
    if (res == 0)
    {
        // all ok
        return true;
    }
    else
    {
        // an error occured during execution, get the error message from the stack
        this->SetError(lua_tostring(this->luaState, lua_gettop(this->luaState)));		        
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
LuaServer::UnregisterFunction(const Util::String& table, unsigned int entry)
{
	lua_getglobal(this->luaState,table.AsCharPtr());
	lua_pushnumber(this->luaState,entry);
	lua_pushnil(this->luaState);	
	lua_settable(this->luaState,-3);
	lua_pop(this->luaState,1);
	return true;
}

 
} // namespace Scripting
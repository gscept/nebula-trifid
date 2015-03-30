//------------------------------------------------------------------------------
//  shellapplication.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shellapplication.h"
#include "io/console.h"
#include "scripting/scriptserver.h"
#include "commands/stdlibrary.h"
#include "scripting/lua/luaserver.h"

using namespace Scripting;
using namespace IO;
using namespace Util;
namespace Tools
{

//------------------------------------------------------------------------------
/**
*/
ShellApplication::ShellApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShellApplication::~ShellApplication()
{
    n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
*/
void
ShellApplication::Run()
{
    // first check for command line args
    if (this->args.HasArg("-help"))
    {
        n_printf("Nebula3 command shell.\n"
                 "Arguments:\n\n"
                 "-help: display this help\n"
                 "-file: evaluate script file and return\n");
    }
    else if (this->args.HasArg("-file"))
    {
        // FIXME!
    }
    else
    {        
        
        Console* con = Console::Instance();
		Ptr<ScriptServer> scriptServer = LuaServer::Create();
		scriptServer->Open();
		Commands::StdLibrary::Register();
        con->Print("Welcome! Enter 'listcmds()' to get started.\n");
        bool running = true;
        while (running)
        {
            con->Print("> ");
            while (!con->HasInput())
            {
                n_sleep(0.1);
            }
            String cmd = con->GetInput();
            if (cmd.IsValid())
            {
                if (cmd == "exit")
                {
                    running = false;
                }
                else
                {
                    if (!scriptServer->Eval(cmd))
                    {
                        con->Print("%s\n", scriptServer->GetError().AsCharPtr());
                    }
                }
            }
        }
        
    }
}

} // namespace Tools
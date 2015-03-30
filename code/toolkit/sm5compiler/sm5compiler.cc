//------------------------------------------------------------------------------
//  nsh3.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "toolkit/sm5compiler/shadercompiler.h"

using namespace Tools;
using namespace Util;

//------------------------------------------------------------------------------
/**
    NOTE: we cannot use ImplementNebulaApplication here, since nsh3 is 
    a command line tool.
*/
void __cdecl
main(int argc, const char** argv)
{
    CommandLineArgs args(argc, argv);
    ShaderCompiler app;
    app.SetCompanyName("LTU Skellefteå");
    app.SetAppTitle("Nebula3 Shader Model 5.0 compiler");
    app.SetCmdLineArgs(args);
	
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}

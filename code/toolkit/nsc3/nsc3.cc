//------------------------------------------------------------------------------
//  nsc3.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadercompilerapplication.h"

//------------------------------------------------------------------------------
/**
*/
void __cdecl
main(int argc, const char** argv)
{
    Util::CommandLineArgs args(argc, argv);
    Tools::ShaderCompilerApplication app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("Nebula3 Shader Compiler");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}


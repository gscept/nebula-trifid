//------------------------------------------------------------------------------
//  genrandtable.cc
//  A little command line tool to generate a random number table.
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "genrandtableapplication.h"

ImplementNebulaApplication();

using namespace Tools;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const CommandLineArgs& args)
{
    GenRandTableApplication app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("GenRandTable");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}

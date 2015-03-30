//------------------------------------------------------------------------------
//  stresstestvieweropenclose.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "apprender/viewerapplication.h"

ImplementNebulaApplication();

using namespace App;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const CommandLineArgs& args)
{
    ViewerApplication app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppID("ViewerStressTest");
    app.SetCmdLineArgs(args);
    
    IndexT i;
    for (i = 0; i < 100; i++)
    {
        app.Open();
        app.Close();
    }
    app.Exit();
}
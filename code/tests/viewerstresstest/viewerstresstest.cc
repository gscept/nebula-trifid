//------------------------------------------------------------------------------
//  viewerstresstest.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "viewerstresstestapplication.h"

ImplementNebulaApplication();

using namespace Test;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const CommandLineArgs& args)
{
    ViewerStressTestApplication app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppID("ViewerStressTest");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}
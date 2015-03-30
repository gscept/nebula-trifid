//------------------------------------------------------------------------------
//  n3viewer.cc
//  (C) 2010 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "n3viewerapp.h"

ImplementNebulaApplication();

using namespace Util;
using namespace Toolkit;

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const CommandLineArgs& args)
{
    N3ViewerApp app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("N3Viewer");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}
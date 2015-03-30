//------------------------------------------------------------------------------
//  forestviewer.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tests/forestviewer/forestviewerapplication.h"

ImplementNebulaApplication()

using namespace Tools;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const CommandLineArgs& args)
{
    ForestViewerApplication app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("Forest Viewer");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}
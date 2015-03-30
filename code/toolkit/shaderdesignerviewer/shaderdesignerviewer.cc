//------------------------------------------------------------------------------
//  shaderdesignerviewer.cc
//  (C) 2011 gsCEPT
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shaderdesignerviewerapplication.h"

ImplementNebulaApplication()

using namespace Tools;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const CommandLineArgs& args)
{
    ShaderDesignerViewerApplication app;
    app.SetCompanyName("LTU - Lulea University of Technology");
    app.SetAppTitle("Shader Designer Viewer");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}
//------------------------------------------------------------------------------
//  faudiotestsimple.cc
//      (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudioapplication.h"
#include "system/appentry.h"

ImplementNebulaApplication();

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const Util::CommandLineArgs& args)
{
    Test::FAudioApplication app;
    if (app.Open())
    {
        app.Run();
        app.Close();
    }    
}

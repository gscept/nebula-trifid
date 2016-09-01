//------------------------------------------------------------------------------
//  testscript.cc
//  (C) 2010 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame2testapplication.h"
#include "system/appentry.h"

ImplementNebulaApplication();

using namespace Test;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const CommandLineArgs& args)
{
    Frame2TestApplication app;
    
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
 
}
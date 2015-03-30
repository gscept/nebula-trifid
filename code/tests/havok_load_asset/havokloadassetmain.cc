//------------------------------------------------------------------------------
//  havokloadassetmain.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokloadassetapplication.h"
#include "system/appentry.h"

ImplementNebulaApplication();

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const Util::CommandLineArgs& args)
{
    Tests::HavokLoadAssetApplication app;
    app.SetAppTitle("Havok Hello-world application");
    app.SetAppID("havok_helloworld");
    if (app.Open())
    {
        app.Run();
        app.Close();
	}    
	app.Exit();
}

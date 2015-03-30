//------------------------------------------------------------------------------
//  jobstest.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "spumathtestapplication.h"

//------------------------------------------------------------------------------
/**
*/
int
__cdecl main()
{
    Test::JobsTestApplication app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppName("Jobs Test");
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
    return 0;
}


//------------------------------------------------------------------------------
//  animconverter3.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "animconverterapp.h"

//------------------------------------------------------------------------------
/**
*/
void __cdecl
main(int argc, const char** argv)
{
    Util::CommandLineArgs args(argc, argv);
    Toolkit::AnimConverterApp app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("Nebula3 Animation Processing Tool");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}


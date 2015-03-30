//------------------------------------------------------------------------------
//  countlines3.cc
//  (C) 2009 Radon  Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "countlinesapp.h"

//------------------------------------------------------------------------------
/**
*/
void __cdecl
main(int argc, const char** argv)
{
    Util::CommandLineArgs args(argc, argv);
    Toolkit::CountLinesApp app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("Nebula3 Line Counter Tool");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}
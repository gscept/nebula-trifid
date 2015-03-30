//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "convertfbxtonax3app.h"

//------------------------------------------------------------------------------
/**
*/
void __cdecl
main(int argc, const char** argv)
{
    Util::CommandLineArgs args(argc, argv);
    Toolkit::ConvertFbxToNax3App app;
    app.SetCompanyName("LTU Luleå University of Technology");
    app.SetAppTitle("Nebula3 FBX to Nax3 Converter");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}

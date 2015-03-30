//------------------------------------------------------------------------------
//  buildresdict3.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resourcedictionaryapp.h"

//------------------------------------------------------------------------------
/**
*/
void __cdecl
main(int argc, const char** argv)
{
    Util::CommandLineArgs args(argc, argv);
    Toolkit::ResourceDictionaryApp app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("Nebula3 Resource Dictionary Builder");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}

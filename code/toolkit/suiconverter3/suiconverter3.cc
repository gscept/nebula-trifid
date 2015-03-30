//------------------------------------------------------------------------------
//  suiconverter3.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "suiconverterapp.h"

//------------------------------------------------------------------------------
/**
*/
void __cdecl
main(int argc, const char** argv)
{
    Util::CommandLineArgs args(argc, argv);
    Toolkit::SUIConverterApp app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("Nebula3 SimpleUI Converter Tool");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}

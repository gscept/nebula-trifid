//------------------------------------------------------------------------------
//  n2tableconverter.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "n2tableconverterapp.h"

//------------------------------------------------------------------------------
/**
*/
void __cdecl
main(int argc, const char** argv)
{
    Util::CommandLineArgs args(argc, argv);
    Toolkit::N2TableConverterApp app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("Mangalore Table Converter");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}

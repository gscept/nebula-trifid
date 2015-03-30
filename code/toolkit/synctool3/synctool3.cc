//------------------------------------------------------------------------------
//  synctool3.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "synctoolapp.h"
#include "util/globalstringatomtable.h"

//------------------------------------------------------------------------------
/**
*/
void __cdecl
main(int argc, const char** argv)
{
    Util::CommandLineArgs args(argc, argv);
    Toolkit::SyncToolApp app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("Nebula3 Build Sync Tool");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
}

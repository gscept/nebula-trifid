//------------------------------------------------------------------------------
//  charxmltodb.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "charxmltodbapplication.h"

using namespace Util;
using namespace Toolkit;

//------------------------------------------------------------------------------
/**
*/
int
main(int argc, const char** argv)
{    
    CommandLineArgs args(argc, argv);
    CharXmlToDbApplication app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("Character Animation XML to DB Tool");
    app.SetCmdLineArgs(args);
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit(); // <--- creates memory leaks

}


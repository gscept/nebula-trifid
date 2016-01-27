//------------------------------------------------------------------------------
#include "stdneb.h"
#include "contentbrowserwindow.h"
#include "contentbrowserapp.h"
#include "extlibs/libqimg/qdevilplugin.h"


Q_IMPORT_PLUGIN(qdevil);

//------------------------------------------------------------------------------
/**
*/
int __cdecl
main(int argc, const char** argv)
{
	Util::CommandLineArgs args(argc, argv);
	ContentBrowser::ContentBrowserApp app;
	app.SetCompanyName("gscept");
	app.SetAppTitle("NebulaT Content Browser");	
	app.SetCmdLineArgs(args);
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
}
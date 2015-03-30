//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadywindow.h"
#include "shadyapp.h"
#include "libqimg/qdevilplugin.h"


Q_IMPORT_PLUGIN(qdevil);


//------------------------------------------------------------------------------
/**
*/
int __cdecl
main(int argc, const char** argv)
{
	Util::CommandLineArgs args(argc, argv);
	Shady::ShadyApp app;
	app.SetCompanyName("gscept");
	app.SetAppTitle("Shady");	
	app.SetCmdLineArgs(args);
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
}
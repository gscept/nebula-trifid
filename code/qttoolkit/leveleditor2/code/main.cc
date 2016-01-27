//------------------------------------------------------------------------------
#include "stdneb.h"
#include "leveleditor2app.h"
#include "extlibs/libqimg/qdevilplugin.h"
Q_IMPORT_PLUGIN(qdevil);

//------------------------------------------------------------------------------
/**
*/

int __cdecl
main(int argc, const char** argv)
{
	Util::CommandLineArgs args(argc, argv);
	LevelEditor2::LevelEditor2App app;
	app.SetCompanyName("gscept");
	app.SetAppTitle("NebulaT Level Editor");	
	app.SetCmdLineArgs(args);
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
}
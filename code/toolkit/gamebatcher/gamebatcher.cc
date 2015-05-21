//------------------------------------------------------------------------------
#include "stdneb.h"
#include "gamebatcherapp.h"

//------------------------------------------------------------------------------
/**
*/
int
main(int argc, const char** argv)
{
	Util::CommandLineArgs args(argc, argv);
	Toolkit::GameBatcherApp app;
	app.SetCompanyName("gscept");
	app.SetAppTitle("Nebula game batcher");
	app.SetCmdLineArgs(args);
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
}

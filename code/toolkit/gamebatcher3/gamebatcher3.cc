//------------------------------------------------------------------------------
#include "stdneb.h"
#include "gamebatcher3app.h"

//------------------------------------------------------------------------------
/**
*/
int
main(int argc, const char** argv)
{
	Util::CommandLineArgs args(argc, argv);
	Toolkit::GameBatcher3App app;
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

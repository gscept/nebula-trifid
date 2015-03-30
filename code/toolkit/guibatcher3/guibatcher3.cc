//------------------------------------------------------------------------------
#include "stdneb.h"
#include "guibatcher3app.h"

//------------------------------------------------------------------------------
/**
*/
int __cdecl
main(int argc, const char** argv)
{
	Util::CommandLineArgs args(argc, argv);
	Toolkit::GuiBatcher3App app;
	app.SetCompanyName("gscept");
	app.SetAppTitle("Nebula GUI batcher");
	app.SetCmdLineArgs(args);
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
}

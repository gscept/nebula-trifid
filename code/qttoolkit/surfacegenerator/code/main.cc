//------------------------------------------------------------------------------
#include "stdneb.h"
#include "surfacegeneratorapp.h"
#include <QApplication>
#ifdef __USE_QT4
#include <QPlastiqueStyle>
#endif

#ifdef __USE_QT4
#include "extlibs/libqimg/qdevilplugin.h"
//
Q_IMPORT_PLUGIN(qdevil);
#endif
//------------------------------------------------------------------------------
/**
*/

int __cdecl
main(int argc, const char** argv)
{
	QApplication a(argc, (char**)argv);
#ifdef __USE_QT4
	a.setStyle(new QPlastiqueStyle);
#endif
	Util::CommandLineArgs args(argc, argv);
	ToolkitUtil::SurfaceGeneratorApp app;
	a.setQuitOnLastWindowClosed(true);
	app.SetCompanyName("gscept");
	app.SetAppTitle("Nebula3 Level Editor 2.0");	
	app.SetCmdLineArgs(args);
	app.Open();
	a.exec();
	app.Exit();
}
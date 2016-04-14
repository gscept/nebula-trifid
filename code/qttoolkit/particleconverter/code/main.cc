//------------------------------------------------------------------------------
#include "stdneb.h"
#include "particleconverterapp.h"
#include <QApplication>
#include <QPlastiqueStyle>
//#include <QApplication>
//#include <QTimer>

//#include "extlibs/libqimg/qdevilplugin.h"
//
//Q_IMPORT_PLUGIN(qdevil);

//------------------------------------------------------------------------------
/**
*/

int __cdecl
main(int argc, const char** argv)
{
	QApplication a(argc, (char**)argv);
	a.setStyle(new QPlastiqueStyle);
	Util::CommandLineArgs args(argc, argv);
	ToolkitUtil::ParticleConverterApp app;
	a.setQuitOnLastWindowClosed(true);
	app.SetCompanyName("gscept");
	app.SetAppTitle("Nebula3 Level Editor 2.0");	
	app.SetCmdLineArgs(args);
	app.Open();
	a.exec();
	app.Exit();
}
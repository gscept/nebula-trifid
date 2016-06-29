//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QApplication>
#include <QPlastiqueStyle>
#include "batchexporterapp.h"
#include "style/nebulastyletool.h"

using namespace BatchExporter;
using namespace QtTools;

//------------------------------------------------------------------------------
/**
*/
int __cdecl
main(int argc, char** argv)
{
	QApplication a(argc, argv);
	a.setStyle(new QPlastiqueStyle);
	CommandLineArgs args(argc, argv);
	BatchExporterApp exporterApp;// = BatchExporterApp::Create();
	exporterApp.Open(args);
	exporterApp.show();
	a.setQuitOnLastWindowClosed(true);
	return a.exec();
}

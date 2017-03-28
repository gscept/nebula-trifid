//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QApplication>
#ifdef __USE_QT4
#include <QPlastiqueStyle>
#endif
#include "batchexporterapp.h"
#include "style/nebulastyletool.h"

#ifdef __USE_QT5 
#ifdef __N_STATIC_BUILD
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif
#endif
using namespace BatchExporter;
using namespace QtTools;

//------------------------------------------------------------------------------
/**
*/
int __cdecl
main(int argc, char** argv)
{
	QApplication a(argc, argv);
#ifdef __USE_QT4
	a.setStyle(new QPlastiqueStyle);
#endif
	CommandLineArgs args(argc, argv);
	BatchExporterApp exporterApp(args);
	exporterApp.show();
	a.setQuitOnLastWindowClosed(true);
	return a.exec();
}

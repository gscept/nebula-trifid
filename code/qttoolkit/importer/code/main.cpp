#include "importerqt.h"
#include "tools/commandlineargs.h"
#include "tools/pathresolver.h"
#include "tools/style.h"

using namespace Importer;
using namespace QtTools;

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	CommandLineArgs args(argc, argv);
	Style defaultStyle;
	ImporterQt w;
	a.setStyleSheet(defaultStyle.GetStyle());
	w.setStyleSheet(defaultStyle.GetStyle());
	w.SetCommandLineArgs(args);
	if (args.HasArgument("-workingdir"))
	{
		PathResolver::Instance()->SetWorkingDirectory(args.GetArgument("-workingdir"));
	}
	w.show();
	w.Open();
	a.setQuitOnLastWindowClosed(true);
	return a.exec();
}
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QApplication>
#include <QPlastiqueStyle>

#if __WIN32__
#include <windows.h>
#endif
#include "system/nebulasettings.h"

#include "style/nebulastyletool.h"
#include "qfiledialog.h"
#include "qinputdialog.h"
#include "qmessagebox.h"
#include "io/assignregistry.h"
#include "io/ioserver.h"
#include "util/string.h"
#include "core/coreserver.h"

#include "../toolkit/toolkitutil/applauncher.h"



//------------------------------------------------------------------------------
/**
*/
int __cdecl
main(int argc, char** argv)
{
	QApplication a(argc, argv);
	a.setStyle(new QPlastiqueStyle);	
	QString folder = QFileDialog::getExistingDirectory(0,"Location for demo project");
	QString subfolder = QInputDialog::getText(0,"Project name","Enter new project folder name");
	if(subfolder.isEmpty())
	{
		QMessageBox   msg;
		msg.setText("Empty folder name!");
		msg.setStandardButtons(QMessageBox::Close);
		msg.setIcon(QMessageBox::Critical);
		msg.exec();
		exit(0);
	}
	QDir newDir(folder);
	if(newDir.exists(subfolder))
	{
		QMessageBox   msg;
		msg.setText("Folder exists!");
		msg.setStandardButtons(QMessageBox::Close);
		msg.setIcon(QMessageBox::Critical);
		msg.exec();
		exit(0);
	}	
	Ptr<Core::CoreServer> core = Core::CoreServer::Create();
	Util::String toolkitdir;
	if (System::NebulaSettings::Exists("gscept","ToolkitShared", "path"))
	{
		toolkitdir = System::NebulaSettings::ReadString("gscept","ToolkitShared", "path");				
	}
	Util::String proj = toolkitdir + "/emptyproject.zip";

	// FIXME, use projinfo
	Util::String unzip = toolkitdir+ "/bin/win32/unzip.exe";
	
	newDir.mkdir(subfolder);
	Util::String outfolder = (folder + "/"+ subfolder).toAscii();
	ToolkitUtil::AppLauncher appLauncher;
	appLauncher.SetExecutable(unzip);
	appLauncher.SetWorkingDirectory(outfolder.AsCharPtr());
	Util::String zipargs;
	zipargs.Format("\"%s\"",proj.AsCharPtr());
	appLauncher.SetArguments(zipargs.AsCharPtr());
	appLauncher.SetNoConsoleWindow(false);
	appLauncher.LaunchWait();

	System::NebulaSettings::WriteString("gscept","ToolkitShared", "workdir",outfolder.AsCharPtr());
		
}

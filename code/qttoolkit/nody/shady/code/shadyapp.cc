//------------------------------------------------------------------------------
//  shadyapp.cc
//  (C) 2012 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadyapp.h"
#include "core/coreserver.h"
#include "shadywindow.h"
#include "graypalette.h"
#include "io/fswrapper.h"
#include "system/nebulasettings.h"
#include <QtGui/QStyleFactory>
#include "code/simulation/simulationcommands.h"

using namespace Core;
using namespace IO;
using namespace Nody;
using namespace Util;
namespace Shady
{

//------------------------------------------------------------------------------
/**
*/
ShadyApp::ShadyApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ShadyApp::~ShadyApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
ShadyApp::Open()
{
	if (App::Application::Open())
	{
		// setup basic ${N3ROOT} runtime system
		this->coreServer = CoreServer::Create();
		this->coreServer->SetCompanyName(Application::Instance()->GetCompanyName());
		this->coreServer->SetAppName(Application::Instance()->GetAppTitle());

        Util::String root = IO::FSWrapper::GetHomeDirectory();
#if PUBLIC_BUILD
        if (System::NebulaSettings::Exists(Application::Instance()->GetCompanyName(),Application::Instance()->GetAppTitle(),"path"))
        {
            root = System::NebulaSettings::ReadString(Application::Instance()->GetCompanyName(),Application::Instance()->GetAppTitle(),"path");
        }
#else 
        if (System::NebulaSettings::Exists("gscept", "ToolkitShared", "workdir"))
        {
            root = System::NebulaSettings::ReadString("gscept", "ToolkitShared", "workdir");
        }
#endif
        this->coreServer->SetRootDirectory(root);
		this->coreServer->Open();

		// setup io subsystem
		this->ioServer = IoServer::Create();

        // setup frame sync
        this->frameSync = FrameSync::FrameSyncTimer::Create();
        this->frameSync->StartTime();

        // setup project info
#ifdef __WIN32__
        this->projInfo.SetCurrentPlatform(ToolkitUtil::Platform::Win32);
#elif __LINUX__
        this->projInfo.SetCurrentPlatform(ToolkitUtil::Platform::Linux);
#endif
        this->projInfo.Setup();

		// create application
		int dummy = 0;
		this->qtApp = new QApplication(dummy, NULL);
		QApplication::setGraphicsSystem("native");

		// set style to Plastique
		QStyle* style = QStyleFactory::create("plastique");
		this->qtApp->setStyle(style);
		this->qtApp->setPalette(GrayPalette());

        // setup script server
        this->scriptServer = Scripting::ScriptServer::Create();
        this->scriptServer->SetDebug(false);
        this->scriptServer->Open();
        Commands::SimulationCommands::Register();

		// initialize window
		this->window = ShadyWindow::Create();
		this->window->show();
		this->window->Setup();

		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyApp::Close()
{
	n_assert(this->IsOpen());
	this->window->Close();
	this->window = 0;

    this->scriptServer->Close();
    this->scriptServer = 0;

    this->project = 0;

    this->frameSync->StopTime();
    this->frameSync = 0;

	this->ioServer = 0;       

	this->coreServer->Close();
	this->coreServer = 0;
	App::Application::Close();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyApp::Run()
{
	this->qtApp->exec();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyApp::Exit()
{
	App::Application::Exit();
}


} // namespace Shady
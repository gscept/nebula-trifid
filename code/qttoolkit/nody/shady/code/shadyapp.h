#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadyApp
    
    Application entry point for Shady
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

#include <QApplication>
#include "core/refcounted.h"
#include "app/application.h"
#include "shadywindow.h"
#include "core/coreserver.h"
#include "io/ioserver.h"
#include "project/shadyproject.h"
#include "toolkitutil/projectinfo.h"
#include "scripting/scriptserver.h"
#include "framesync/framesynctimer.h"

namespace Shady
{

class ShadyApp : public App::Application
{
public:

	/// constructor
	ShadyApp();
	/// destructor
	virtual ~ShadyApp();

	/// opens app
	bool Open();
	/// closes app
	void Close();
	/// runs app
	void Run();
	/// exits app
	void Exit();

	/// sets up palette
	void SetupPalette();


private:
	Ptr<ShadyWindow> window;
	
    Ptr<Shady::ShadyProject> project;
    Ptr<Core::CoreServer> coreServer;
    Ptr<Scripting::ScriptServer> scriptServer;
	Ptr<IO::IoServer> ioServer;
    Ptr<FrameSync::FrameSyncTimer> frameSync;
	QApplication* qtApp;
    ToolkitUtil::ProjectInfo projInfo;
}; 
} // namespace Shady
//------------------------------------------------------------------------------
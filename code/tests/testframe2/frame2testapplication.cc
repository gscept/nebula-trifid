//------------------------------------------------------------------------------
//  ScriptTestApplication.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame2testapplication.h"
#include "math/matrix44.h"
#include "input/gamepad.h"
#include "io/ioserver.h"
#include "util/stringatom.h"
#include "io/fswrapper.h"
#include "math/vector.h"
#include "io/logfileconsolehandler.h"
#include "jobs/jobsystem.h"
#include "commands/stdlibrary.h"
#include "scripting/lua/luaserver.h"
#include "scripting/debug/scriptingpagehandler.h"
#include "frame2/framescriptloader.h"

using namespace Frame2;
using namespace Math;
using namespace Base;
using namespace Test;
#if USE_HTTP
using namespace Http;
#endif


//------------------------------------------------------------------------------
/*
*/
Frame2TestApplication::Frame2TestApplication()
{
}

//------------------------------------------------------------------------------
/*
*/
Frame2TestApplication::~Frame2TestApplication()
{
}

//------------------------------------------------------------------------------
/*
*/
bool 
Frame2TestApplication::Open()
{
    this->coreServer = Core::CoreServer::Create();
    this->coreServer->SetCompanyName(Util::StringAtom("Radon Labs GmbH"));
    this->coreServer->SetAppName(Util::StringAtom("Frame 2 test application"));
    this->coreServer->Open();

	Ptr<IO::AssignRegistry> assignRegistry = IO::AssignRegistry::Create();
	assignRegistry->Setup();

	Ptr<IO::IoServer> ioServer = IO::IoServer::Create();
   
	Ptr<FrameScript> script = FrameScriptLoader::LoadFrameScript("vkdebug", "home:work/frame/win32/vkdebug.json");
#if USE_HTTP
    // setup HTTP server
    this->httpInterface = Http::HttpInterface::Create();
    this->httpInterface->Open();
    this->httpServerProxy = HttpServerProxy::Create();
    this->httpServerProxy->Open();

#endif
	this->debugInterface = Debug::DebugInterface::Create();
	this->debugInterface->Open();

    this->masterTime.Start();

    

    return true;
}

//------------------------------------------------------------------------------
/*
*/
void 
Frame2TestApplication::Close()
{    

    this->masterTime.Stop();

#if USE_HTTP
    this->httpServerProxy->Close();
    this->httpServerProxy = 0;

    this->httpInterface->Close();
    this->httpInterface = 0;
#endif


    this->coreServer->Close();
    this->coreServer = 0;
}

//------------------------------------------------------------------------------
/*
*/
void 
Frame2TestApplication::Run()
{
    // waiting for game pad
    

    while(true)
    {
    
        Core::SysFunc::Sleep(0.01);
    }
}

//------------------------------------------------------------------------------
/*
*/

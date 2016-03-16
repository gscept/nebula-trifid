//------------------------------------------------------------------------------
//  gameapplication.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "appgame/gameapplication.h"
#include "core/debug/corepagehandler.h"
#include "threading/debug/threadpagehandler.h"
#include "memory/debug/memorypagehandler.h"
#include "io/debug/iopagehandler.h"
#include "io/logfileconsolehandler.h"
#include "io/debug/consolepagehandler.h"
#include "render_classregistry.h"
#include "commandhandlers/createentitycommand.h"
#include "messaging/messagecallbackhandler.h"
#include "system/nebulasettings.h"
#include "io/fswrapper.h"

namespace App
{
using namespace Util;

__ImplementSingleton(App::GameApplication);

using namespace Util;
using namespace Core;
using namespace IO;
using namespace Http;
using namespace Debug;

//------------------------------------------------------------------------------
/**
*/
GameApplication::GameApplication()
#if __NEBULA3_HTTP__
	:defaultTcpPort(2100)
#endif
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
GameApplication::~GameApplication()
{
    n_assert(!this->IsOpen());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
GameApplication::Open()
{
    n_assert(!this->IsOpen());
    if (Application::Open())
    {
        // setup from cmd line args
        this->SetupAppFromCmdLineArgs();
               
        // setup basic Nebula3 runtime system
        this->coreServer = CoreServer::Create();
        this->coreServer->SetCompanyName(Application::Instance()->GetCompanyName());
        this->coreServer->SetAppName(Application::Instance()->GetAppTitle());
                
		Util::String root = IO::FSWrapper::GetHomeDirectory();

#if PUBLIC_BUILD
		if(System::NebulaSettings::Exists(Application::Instance()->GetCompanyName(),Application::Instance()->GetAppTitle(),"path"))
		{
			root = System::NebulaSettings::ReadString(Application::Instance()->GetCompanyName(),Application::Instance()->GetAppTitle(),"path");
		}
#else 
		if(System::NebulaSettings::Exists("gscept", "ToolkitShared", "workdir"))
		{
			root = System::NebulaSettings::ReadString("gscept", "ToolkitShared", "workdir");
		}
#endif
				
		//n_assert2(System::NebulaSettings::ReadString("gscept", "ToolkitShared", "workdir"), "No working directory defined!");

        this->coreServer->SetRootDirectory(root);
        this->coreServer->Open();

        // setup the job system
        this->jobSystem = Jobs::JobSystem::Create();
        this->jobSystem->Setup();

        // setup game content server
        this->gameContentServer = GameContentServer::Create();
        this->gameContentServer->SetTitle(this->GetAppTitle());
        this->gameContentServer->SetTitleId(this->GetAppID());
        this->gameContentServer->SetVersion(this->GetAppVersion());
#if __PS3__
        this->gameContentServer->SetGameContentOnBdvdFlag(this->gameContentOnBdvdFlag);
#endif
        this->gameContentServer->Setup();

        // setup io subsystem
        this->ioServer = IoServer::Create();
        this->ioServer->MountStandardArchives();
        this->ioInterface = IoInterface::Create();
        this->ioInterface->Open();

        // attach a log file console handler
#if __WIN32__
        Ptr<LogFileConsoleHandler> logFileHandler = LogFileConsoleHandler::Create();
        Console::Instance()->AttachHandler(logFileHandler.upcast<ConsoleHandler>());
#endif

#if __NEBULA3_HTTP_FILESYSTEM__
		// setup http subsystem
		this->httpClientRegistry = Http::HttpClientRegistry::Create();
		this->httpClientRegistry->Setup();
#endif

#if __NEBULA3_HTTP__
        // setup http subsystem
        this->httpInterface = Http::HttpInterface::Create();
		this->httpInterface->SetTcpPort(this->defaultTcpPort);
        this->httpInterface->Open();
        this->httpServerProxy = Http::HttpServerProxy::Create();
        this->httpServerProxy->Open();
        this->httpServerProxy->AttachRequestHandler(Debug::CorePageHandler::Create());
        this->httpServerProxy->AttachRequestHandler(Debug::ThreadPageHandler::Create());
        this->httpServerProxy->AttachRequestHandler(Debug::MemoryPageHandler::Create());
        this->httpServerProxy->AttachRequestHandler(Debug::ConsolePageHandler::Create());
        this->httpServerProxy->AttachRequestHandler(Debug::IoPageHandler::Create());

        // setup debug subsystem
        this->debugInterface = DebugInterface::Create();
        this->debugInterface->Open();

        this->remoteInterface = Remote::RemoteInterface::Create();
        this->remoteInterface->SetPortNumber(13031);
        this->remoteInterface->Open();
        this->remoteControlProxy = Remote::RemoteControlProxy::Create();
        this->remoteControlProxy->Open();

        // register default remote command handlers
        Ptr<Commands::CreateEntityCommand> createEntityCommand = Commands::CreateEntityCommand::Create();
        this->remoteControlProxy->AttachCommandHandler(createEntityCommand.cast<Remote::RemoteCommandHandler>());
#endif

        // create our game server and open it
        this->gameServer = Game::GameServer::Create();
        this->gameServer->Open();

        // create resource manager
        //this->resManager = Resources::ResourceManager::Create();
        //this->resManager->Open();

        // create and add new game features
        this->SetupGameFeatures();

        // setup application state handlers
        this->curState.Clear();
        this->nextState.Clear();
        this->SetupStateHandlers();

        // setup profiling stuff
        _setup_timer(GameApplicationFrameTimeAll);

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
GameApplication::Close()
{
    n_assert(this->IsOpen());

    _discard_timer(GameApplicationFrameTimeAll);

    // shutdown basic Nebula3 runtime
    this->CleanupGameFeatures();
    this->CleanupStateHandlers();
    this->gameServer->Close();
    this->gameServer = 0;

    this->gameContentServer->Discard();
    this->gameContentServer = 0;

    //this->resManager->Close();
    //this->resManager = 0;

    this->ioInterface->Close();
    this->ioInterface = 0;
    this->ioServer = 0;

    this->jobSystem->Discard();
    this->jobSystem = 0;

#if __NEBULA3_HTTP__
    this->debugInterface->Close();
    this->debugInterface = 0;

    this->httpServerProxy->Close();
    this->httpServerProxy = 0;
    this->httpInterface->Close();
    this->httpInterface = 0;

    this->remoteControlProxy->Close();
    this->remoteControlProxy = 0;
    this->remoteInterface->Close();
    this->remoteInterface = 0;
#endif

#if __NEBULA3_HTTP_FILESYSTEM__
	this->httpClientRegistry->Discard();
	this->httpClientRegistry = 0;
#endif

    this->coreServer->Close();
    this->coreServer = 0;

    Application::Close();
}

//------------------------------------------------------------------------------
/**
    Setup the application state handlers. This method is called by App::Open()
    after the Mangalore subsystems have been initialized. Override this method
    to create and attach your application state handlers with the
    application object.
*/
void
GameApplication::SetupStateHandlers()
{
    // FIXME
}

//------------------------------------------------------------------------------
/**
    Cleanup the application state handlers. This will call the
    OnRemoveFromApplication() method on all attached state handlers
    and release them. Usually you don't need to override this method in
    your app.
*/
void
GameApplication::CleanupStateHandlers()
{
    // release game state handlers
    this->requestedState.Clear();
    this->curState.Clear();
    this->nextState.Clear();

    // cleanup state handlers
    int i;
    int num = this->stateHandlers.Size();
    for (i = 0; i < num; i++)
    {
        this->stateHandlers.ValueAtIndex(i)->OnRemoveFromApplication();
    }
    this->stateHandlers.Clear();
}

//------------------------------------------------------------------------------
/**
    Run the application. This method will return when the application wishes
    to exist.
*/
void
GameApplication::Run()
{
    while (this->GetCurrentState() != "Exit")
    {
        _start_timer(GameApplicationFrameTimeAll);

#if __NEBULA3_HTTP__
        this->httpServerProxy->HandlePendingRequests();
        this->remoteControlProxy->HandlePendingCommands();
#endif
        // trigger core server
        this->coreServer->Trigger();

        // trigger beginning of frame for feature units
        this->gameServer->OnBeginFrame();

        String curState = this->GetCurrentState();
        Ptr<StateHandler> curStateHandler;
        String newState;
        if (curState.IsValid())
        {
            // call the current state handler
            curStateHandler = this->FindStateHandlerByName(curState);
            n_assert(curStateHandler);
            newState = curStateHandler->OnFrame();
        }

		// trigger frame for feature units
		this->gameServer->OnFrame();

        // call the app's Run() method
        Application::Run();

		// trigger end of frame for feature units
		this->gameServer->OnEndFrame();

        // update message callbacks
        Messaging::MessageCallbackHandler::Update();

        // a requested state always overrides the returned state
        if (this->requestedState.IsValid())
        {
            this->SetState(this->requestedState);
        }
        else if (newState.IsValid() && newState != curStateHandler->GetName())
        {
            // a normal state transition
            this->SetState(newState);
        }

        _stop_timer(GameApplicationFrameTimeAll);
    }
}

//------------------------------------------------------------------------------
/**
    Do a state transition. This method is called by SetState() when the
    new state is different from the previous state.
*/
void
GameApplication::DoStateTransition()
{
    // cleanup previous state
    if (this->curState.IsValid())
    {
        const Ptr<StateHandler>& curStateHandler = this->FindStateHandlerByName(this->curState);
        n_assert(curStateHandler);
        curStateHandler->OnStateLeave(this->nextState);
    }

    // initialize new state
    String prevState = this->curState;
    this->curState = this->nextState;
    if (this->nextState.IsValid() && this->nextState != "Exit")
    {
        const Ptr<StateHandler>& nextStateHandler =  this->FindStateHandlerByName(this->nextState);
        if (nextStateHandler)
        {
            nextStateHandler->OnStateEnter(prevState);
        }
    }

    this->requestedState.Clear();
}

//------------------------------------------------------------------------------
/**
    Request a new state. This is a public method to switch states
    (SetState() is private because it invokes some internal voodoo). The
    requested state will be activated at the end of the frame.
*/
void
GameApplication::RequestState(const String& s)
{
    this->requestedState = s;
}

//------------------------------------------------------------------------------
/**
    Set a new application state. This method will call DoStateTransition().
*/
void
GameApplication::SetState(const String& s)
{
    this->nextState = s;
    this->DoStateTransition();
}

//------------------------------------------------------------------------------
/**
    Register a state handler object with the application.

    @param  state   pointer to a state handler object
*/
void
GameApplication::AddStateHandler(const Ptr<StateHandler>& handler)
{
    this->stateHandlers.Add(StringAtom(handler->GetName()), handler);
    handler->OnAttachToApplication();
}

//------------------------------------------------------------------------------
/**
    Find a state handler by name.

    @param          name of state to return the state handler for
    @return         pointer to state handler object associated with the state (can be 0)
*/
const Ptr<StateHandler>&
GameApplication::FindStateHandlerByName(const String& name) const
{
    n_assert(this->stateHandlers.Contains(StringAtom(name)));
    return this->stateHandlers[StringAtom(name)];
}

//------------------------------------------------------------------------------
/**
    Get the current state handler.
*/
const Ptr<StateHandler>&
GameApplication::GetCurrentStateHandler() const
{
    n_assert(this->curState.IsValid());
    const Ptr<StateHandler>& curStateHandler = this->FindStateHandlerByName(this->curState);
    n_assert(0 != curStateHandler);
    return curStateHandler;
}

//------------------------------------------------------------------------------
/**
    Setup new game features which should be used by this application.
    Overwrite for all features which have to be used.
*/
void
GameApplication::SetupGameFeatures()
{
    // create any features in derived class
}

//------------------------------------------------------------------------------
/**
    Cleanup all added game features
*/
void
GameApplication::CleanupGameFeatures()
{
    // cleanup your features in derived class
}

//------------------------------------------------------------------------------
/**
*/
void
GameApplication::SetupAppFromCmdLineArgs()
{
    // allow rename of application
    const Util::CommandLineArgs& args = this->GetCmdLineArgs();
    if (args.HasArg("-appname"))
    {
        this->SetAppTitle(args.GetString("-appname"));
    }
}
} // namespace App

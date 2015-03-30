//------------------------------------------------------------------------------
//  graphicsinterface.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicsinterface.h"
#include "graphics/graphicshandler.h"
#include "resources/resourcemanager.h"
#include "debugrender/debuggraphicshandler.h"

namespace Graphics
{
__ImplementClass(Graphics::GraphicsInterface, 'GRIF', Core::RefCounted);
__ImplementInterfaceSingleton(Graphics::GraphicsInterface);

using namespace Threading;
using namespace Interface;
using namespace Messaging;
using namespace FrameSync;

//------------------------------------------------------------------------------
/**
*/
GraphicsInterface::GraphicsInterface()
{
    __ConstructSingleton;
    this->mainThreadId = Thread::GetMyThreadId();
}

//------------------------------------------------------------------------------
/**
*/
GraphicsInterface::~GraphicsInterface()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsInterface::Open()
{
    // setup the message handler
    this->graphicsHandler = GraphicsHandler::Create();
    this->debugGraphicsHandler = Debug::DebugGraphicsHandler::Create();

    this->AttachHandler(this->graphicsHandler.cast<Handler>());
    this->AttachHandler(this->debugGraphicsHandler.cast<Handler>());
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsInterface::Close()
{
    this->RemoveHandler(this->debugGraphicsHandler.cast<Handler>());
    this->debugGraphicsHandler->Close();
    this->debugGraphicsHandler = 0;

    this->RemoveHandler(this->graphicsHandler.cast<Handler>());
    this->graphicsHandler->Close();
    this->graphicsHandler = 0;	
}


//------------------------------------------------------------------------------
/**
*/
void 
GraphicsInterface::WaitForPendingResources()
{
    n_assert(this->graphicsHandler.isvalid());
    this->graphicsHandler->WaitForPendingResources();
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsInterface::OnFrame()
{
    // update frame timings
    FrameSyncTimer::Instance()->UpdateTimePolling();

    // perform graphics handler work
    this->graphicsHandler->DoWork();
}

} // namespace Graphics

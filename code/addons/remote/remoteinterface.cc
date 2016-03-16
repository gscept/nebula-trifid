//------------------------------------------------------------------------------
//  remoteinterface.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "remote/remoteinterface.h"
#include "remote/remotemessagehandler.h"       
#include "messaging/runthroughhandlerthread.h"

namespace Remote
{
__ImplementClass(Remote::RemoteInterface, 'REIF', Interface::InterfaceBase);
__ImplementInterfaceSingleton(Remote::RemoteInterface);

using namespace Interface;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
RemoteInterface::RemoteInterface() :
    portNum(2102)
{
    __ConstructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
RemoteInterface::~RemoteInterface()
{
    __DestructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
RemoteInterface::Open()
{
    // setup runthrough handler thread
    Ptr<RunThroughHandlerThread> handlerThread = RunThroughHandlerThread::Create();
    handlerThread->SetName("RemoteInterface Thread");
    handlerThread->SetCoreId(System::Cpu::MiscThreadCore);
#if __WII_
    handlerThread->SetPriority(Thread::NormalBoost);
#endif    
    Ptr<RemoteMessageHandler> handler = RemoteMessageHandler::Create();
    handler->SetPortNumber(this->portNum);
    handlerThread->AttachHandler(handler.cast<Handler>());
    this->SetHandlerThread(handlerThread.cast<HandlerThreadBase>());
            
    InterfaceBase::Open();
}

} // namespace Remote

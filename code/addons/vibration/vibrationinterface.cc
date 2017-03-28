//------------------------------------------------------------------------------
//  vibrationinterface.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vibration/vibrationinterface.h"
#include "vibration/vibrationhandler.h"
#include "messaging/blockinghandlerthread.h"

namespace Vibration
{
__ImplementClass(Vibration::VibrationInterface, 'VIBI', Interface::InterfaceBase);
__ImplementInterfaceSingleton(Vibration::VibrationInterface);

using namespace Interface;
using namespace Messaging;
using namespace Threading;

//------------------------------------------------------------------------------
/**
*/
VibrationInterface::VibrationInterface()
{
    __ConstructSingleton;
    this->mainThreadId = Thread::GetMyThreadId();
}    

//------------------------------------------------------------------------------
/**
*/
VibrationInterface::~VibrationInterface()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
VibrationInterface::Open()
{
    // setup the message handler thread object
    Ptr<BlockingHandlerThread> handlerThread = BlockingHandlerThread::Create();
    handlerThread->SetName("VibrationInterface Thread");
    handlerThread->SetCoreId(System::Cpu::MiscThreadCore);
    handlerThread->AttachHandler(VibrationHandler::Create());
    handlerThread->SetWaitTimeout(500); // 20 times per second
    this->SetHandlerThread(handlerThread.cast<HandlerThreadBase>());

    this->batchMessage = BatchMessage::Create();
    InterfaceBase::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
VibrationInterface::Close()
{
    InterfaceBase::Close();
    this->batchMessage = 0;    
}

//------------------------------------------------------------------------------
/**
    Send a batched message. The message will not be sent to the audio thread
    immediately, but instead will be collected in a batch message for later
    sending. Use this method to reduce communication overhead
    between the main and audio thread by sending only one batch message
    instead of many messages. This method may only be called from the 
    main thread!
*/
void
VibrationInterface::SendBatched(const Ptr<Message>& msg)
{
    #if NEBULA3_DEBUG
    n_assert(Thread::GetMyThreadId() == this->mainThreadId);
    #endif
    this->batchMessage->AddMessage(msg);
}

//------------------------------------------------------------------------------
/**
    Send off the batched messages to the audio thread. This reduces 
    thread synchronization overhead dramatically if many messages must
    be sent per-frame. Only the main thread may call this method.
*/
void
VibrationInterface::FlushBatchedMessages()
{
    #if NEBULA3_DEBUG
    n_assert(Thread::GetMyThreadId() == this->mainThreadId);
    #endif

    // send current batch message
    this->Send(this->batchMessage.cast<Message>());

    // create a new, empty batch message
    this->batchMessage = BatchMessage::Create();
}

} // namespace Vibration



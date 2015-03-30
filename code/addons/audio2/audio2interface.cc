//------------------------------------------------------------------------------
//  audio2interface.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audio2/audio2interface.h"
#include "audio2/fmodcoreaudio/fmodaudiohandler.h"
#include "messaging/blockinghandlerthread.h"

__ImplementClass(Audio2::Audio2Interface, 'AUDI', Interface::InterfaceBase);
__ImplementInterfaceSingleton(Audio2::Audio2Interface);

using namespace Interface;
using namespace Messaging;
using namespace Threading;
using namespace Audio2;
using namespace FmodCoreAudio;

//------------------------------------------------------------------------------
/**
*/
Audio2Interface::Audio2Interface()
{
    __ConstructSingleton;
    this->mainThreadId = Thread::GetMyThreadId();
}    

//------------------------------------------------------------------------------
/**
*/
Audio2Interface::~Audio2Interface()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Interface::Open()
{
    // the audio device needs to be triggered from time to time, but we
    // also need to react immediately to messages
    //
    // PS3 stack size: we used 16384 in the first place, then we had a DATA_HTAB_MISS cpu-exception,
    // program was in this line:
    //    stdu       r1,-0x4520(r1)
    // trying to create some room on the stack in an fmod-function
    // so if there is a similar exception in this thread, increase the thread-size here
    Ptr<BlockingHandlerThread> handlerThread = BlockingHandlerThread::Create();
    handlerThread->SetName("Audio2Interface Thread");
    handlerThread->SetCoreId(System::Cpu::AudioThreadCore);
    handlerThread->SetWaitTimeout(int(0.1f * 1000.0f));
    handlerThread->SetStackSize(81920);
    handlerThread->AttachHandler(FmodAudioHandler::Create());
    this->SetHandlerThread(handlerThread.cast<HandlerThreadBase>());

    this->batchMessage = BatchMessage::Create();
    InterfaceBase::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Interface::Close()
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
Audio2Interface::SendBatchedInternal(const Ptr<Message>& msg)
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
Audio2Interface::FlushBatchedMessages()
{
    #if NEBULA3_DEBUG
    n_assert(Thread::GetMyThreadId() == this->mainThreadId);
    #endif

    // send current batch message
    this->Send(this->batchMessage.cast<Message>());

    // create a new, empty batch message
    this->batchMessage = BatchMessage::Create();
}



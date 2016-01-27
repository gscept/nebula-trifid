//------------------------------------------------------------------------------
//  audiohandler.cc
//  (C) 2014-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudio/audiohandler.h"
#include "faudio/fmodstudiopagehandler.h"
#include "faudio/audioprotocol.h"
#include "faudio/audiodevice.h"
#include "faudio/fmoderror.h"
#include "faudio/eventinstance.h"
#include "faudio/eventstate.h"
#include "messaging/staticmessagehandler.h"

#if __NEBULA3_HTTP__
#include "http/httpinterface.h"
#endif

#include <fmod.hpp>


__ImplementClass(FAudio::AudioHandler, 'AUDH', Messaging::Handler);

using namespace FAudio;
using namespace Util;
using namespace Timing;
using namespace Messaging;



__Handler(FAudio::AudioHandler, LoadBankFile)
{
	AudioDevice::Instance()->LoadBank(msg->GetBank(), msg->GetBlocking());
}

__Handler(FAudio::AudioHandler, UnloadBankFile)
{
	AudioDevice::Instance()->UnloadBank(msg->GetBank());
}

__Handler(FAudio::AudioHandler, UpdateListener)
{
	AudioDevice::Instance()->SetListener(msg->GetTransform(), msg->GetVelocity());
}

__Handler(FAudio::AudioHandler, DiscardAllEvents)
{
	AudioDevice::Instance()->DiscardAllEvents();
}

__Handler(FAudio::AudioHandler, EventPlayFireAndForget)
{
	AudioDevice::Instance()->EventPlayFireAndForget(msg->GetEventId(), msg->GetVolume());
}

__Handler(FAudio::AudioHandler, EventPlayFireAndForget3DMatrix)
{
	AudioDevice::Instance()->EventPlayFireAndForget3D(msg->GetEventId(), msg->GetTransform(), msg->GetVelocity(), msg->GetVolume());
}
__Handler(FAudio::AudioHandler, EventPlayFireAndForget3D)
{
	AudioDevice::Instance()->EventPlayFireAndForget3D(msg->GetEventId(), msg->GetPosition(), msg->GetForward(), msg->GetUp(), msg->GetVelocity(), msg->GetVolume());
}

__Handler(FAudio::AudioHandler, BusSetVolume)
{
	AudioDevice::Instance()->GetBus(msg->GetBusId().AsCharPtr())->setFaderLevel(msg->GetVolume());
}

__Handler(FAudio::AudioHandler, BusSetPaused)
{
	AudioDevice::Instance()->GetBus(msg->GetBusId().AsCharPtr())->setPaused(msg->GetPaused());
}

__Handler(FAudio::AudioHandler, VCASetVolume)
{
	AudioDevice::Instance()->SetVCAVolume(msg->GetVolume(), msg->GetVCAId());
}

__Dispatcher(FAudio::AudioHandler)
{
	__Handle(FAudio::AudioHandler, LoadBankFile);
	__Handle(FAudio::AudioHandler, UnloadBankFile);
	__Handle(FAudio::AudioHandler, UpdateListener);
	__Handle(FAudio::AudioHandler, DiscardAllEvents);
	__Handle(FAudio::AudioHandler, EventPlayFireAndForget);
	__Handle(FAudio::AudioHandler, EventPlayFireAndForget3D);
	__Handle(FAudio::AudioHandler, EventPlayFireAndForget3DMatrix);
	__Handle(FAudio::AudioHandler, BusSetVolume);
	__Handle(FAudio::AudioHandler, BusSetPaused);
	__Handle(FAudio::AudioHandler, VCASetVolume);
}


//------------------------------------------------------------------------------
/**
*/
AudioHandler::AudioHandler() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AudioHandler::~AudioHandler()
{
    // empty
}            

//------------------------------------------------------------------------------
/**
*/
void
AudioHandler::Open()
{
    n_assert(!this->IsOpen());   

	Handler::Open();

#if __NEBULA3_HTTP__
    // setup http page handler
	this->debugPageHandler = Debug::FmodStudioPageHandler::Create();
	Http::HttpServerProxy::Instance()->AttachRequestHandler(this->debugPageHandler.cast<Http::HttpRequestHandler>());    
#endif

}

//------------------------------------------------------------------------------
/**
*/
void
AudioHandler::Close()
{
    n_assert(this->IsOpen());
   
#if __NEBULA3_HTTP__
	if (this->debugPageHandler.isvalid())
    {
		Http::HttpServerProxy::Instance()->RemoveRequestHandler(this->debugPageHandler.cast<Http::HttpRequestHandler>());
		this->debugPageHandler = 0;
    }
#endif
	Handler::Close();
}


//------------------------------------------------------------------------------
/**
*/
bool
AudioHandler::HandleMessage(const Ptr<Message>& msg)
{
    n_assert(msg.isvalid());
	__Dispatch(AudioHandler, this, msg);        
    return true;
}


//------------------------------------------------------------------------------
//  FAudiointerface.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudio/audiointerface.h"
#include "faudio/audiohandler.h"

__ImplementClass(FAudio::AudioInterface, 'AUD3', Messaging::Port);
__ImplementInterfaceSingleton(FAudio::AudioInterface);

using namespace Messaging;
using namespace Threading;
using namespace FAudio;


//------------------------------------------------------------------------------
/**
*/
AudioInterface::AudioInterface()
{
    __ConstructSingleton;
}    

//------------------------------------------------------------------------------
/**
*/
AudioInterface::~AudioInterface()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioInterface::Open()
{
    this->handler = AudioHandler::Create();
    this->AttachHandler(this->handler.cast<Messaging::Handler>());
}

//------------------------------------------------------------------------------
/**
*/
void
AudioInterface::Close()
{
    this->RemoveAllHandlers();
    this->handler = 0;
}


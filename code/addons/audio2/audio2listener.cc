//------------------------------------------------------------------------------
//  audio2listener.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audio2/audio2listener.h"
#include "audio2protocol.h"
#include "audio2/audio2interface.h"

__ImplementClass(Audio2::Audio2Listener, 'AUDL', Core::RefCounted);
__ImplementSingleton(Audio2::Audio2Listener);

using namespace Math;
using namespace Messaging;
using namespace Audio2;

//------------------------------------------------------------------------------
/**
*/
Audio2Listener::Audio2Listener() :
    transform(matrix44::identity()),
    velocity(vector::nullvec()),
    mustSentListenerData(true)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
Audio2Listener::~Audio2Listener()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Listener::OnFrame()
{
    // send an SetListener message to the audio thread
    if(this->mustSentListenerData)
    {
        Ptr<Audio2::UpdateListener> msg = Audio2::UpdateListener::Create();
        msg->SetTransform(this->transform);
        msg->SetVelocity(this->velocity);
        Audio2Interface::Instance()->SendBatched(msg);
        this->mustSentListenerData = false;
    }
}

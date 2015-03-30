//------------------------------------------------------------------------------
//  audio2player.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audio2/audio2player.h"
#include "audio2protocol.h"
#include "audio2/audio2interface.h"

__ImplementClass(Audio2::Audio2Player, 'APLR', Core::RefCounted);
__ImplementSingleton(Audio2::Audio2Player);

using namespace Audio2;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
Audio2Player::Audio2Player()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
Audio2Player::~Audio2Player()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Player::PlayEvent(const Audio2::FmodEventId &eventId, float volume)
{
    Ptr<Audio2::EventPlayFireAndForget> msg = Audio2::EventPlayFireAndForget::Create();
    msg->SetEventId(eventId);
    msg->SetVolume(volume);
    Audio2Interface::Instance()->SendBatched(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Player::PlayEvent(const Audio2::FmodEventId& eventId, const Math::matrix44 &transform, const Math::vector &velocity, float volume)
{
    Ptr<Audio2::EventPlayFireAndForget3D> msg = Audio2::EventPlayFireAndForget3D::Create();
    msg->SetEventId(eventId);
    msg->SetTransform(transform);
    msg->SetVelocity(velocity);
    msg->SetVolume(volume);
    Audio2Interface::Instance()->SendBatched(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Player::PlayCue(const int cueId)
{
    Ptr<Audio2::PlayCue> msg = Audio2::PlayCue::Create();
    msg->SetCueId(cueId);
    Audio2Interface::Instance()->Send(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Player::StopCue(const int cueId)
{
    Ptr<Audio2::StopCue> msg = Audio2::StopCue::Create();
    msg->SetCueId(cueId);
    Audio2Interface::Instance()->Send(msg);
}

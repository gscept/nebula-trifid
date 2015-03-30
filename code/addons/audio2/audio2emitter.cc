//------------------------------------------------------------------------------
//  audio2emitter.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audio2/audio2emitter.h"
#include "audio2/audio2server.h"
#include "audio2protocol.h"
#include "audio2/audio2interface.h"
#include "audio2/fmodeventparameterid.h"

using namespace Audio2;

__ImplementClass(Audio2::Audio2Emitter, 'AEMB', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Audio2Emitter::Audio2Emitter() :
    emitterRegistered(false),
    state(EventStateUnknown),
    eventId(),
    eventHandle()
{
}

//------------------------------------------------------------------------------
/**
*/
Audio2Emitter::~Audio2Emitter()
{
    n_assert(!this->emitterRegistered);
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::Setup(const Audio2::FmodEventId& eventId)
{
    n_assert(!this->emitterRegistered);
    n_assert(!this->eventId.IsValid());
    n_assert(!this->eventHandle.isvalid());
    n_assert(!this->pendingCreateEventMessage.isvalid());
    n_assert(EventStateUnknown == this->state);
    n_assert(!this->pendingUpdateStateMsg.isvalid());

    Audio2Server::Instance()->RegisterAudioEmitter(this);
    this->emitterRegistered = true;

    this->SetEventId(eventId);
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::Discard()
{
    /// Setup must be called, before it can be discarded
    n_assert(this->emitterRegistered);

    if(this->pendingUpdateStateMsg.isvalid())
    {
        if(!this->pendingUpdateStateMsg->Handled())
        {
            Audio2Interface::Instance()->Cancel(this->pendingUpdateStateMsg);
        }
        this->pendingUpdateStateMsg = NULL;
    }
    if(this->pendingCreateEventMessage.isvalid())
    {
        this->ValidateEventHandle(true);
        n_assert(this->eventHandle.isvalid());
    }

    // create and send the EventDiscard message
    Ptr<EventDiscard> msg = EventDiscard::Create();
    msg->SetEventHandle(this->eventHandle);
    Audio2Interface::Instance()->SendBatched(msg);

    this->eventId.Clear();
    this->eventHandle = 0;
    /// clear all pending messages
    this->pendingMessages.Clear();
    this->state = EventStateUnknown;

    Audio2Server::Instance()->UnregisterAudioEmitter(this);
    this->emitterRegistered = false;
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::SetEventId(const Audio2::FmodEventId& eventId)
{
    n_assert(!this->eventId.IsValid());
    n_assert(!this->eventHandle.isvalid());
    n_assert(!this->pendingCreateEventMessage.isvalid());
    n_assert(!this->pendingMessages.Size());

    this->eventId = eventId;

    // send off a GetEvent message, store the message because
    // we need to read back the event handle later
    this->pendingCreateEventMessage = Audio2::EventCreate::Create();
    this->pendingCreateEventMessage->SetEventId(this->eventId);
    Audio2Interface::Instance()->Send(this->pendingCreateEventMessage);
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::OnFrame()
{
    n_assert(this->emitterRegistered);
    n_assert(this->eventId.IsValid());

    this->ValidateEventHandle(false);

    // only send update messages, after eventHandle came back from
    // Create Message
    if(this->eventHandle.isvalid())
    {
        if(this->pendingUpdateStateMsg.isvalid())
        {
            if(!this->pendingUpdateStateMsg->Handled()) return;
            this->state = this->pendingUpdateStateMsg->GetState();
            this->pendingUpdateStateMsg = 0;
        }
        this->pendingUpdateStateMsg = Audio2::EventUpdateState::Create();
        this->SendEventMessage(this->pendingUpdateStateMsg.upcast<Audio2::EventControlBase>());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::ValidateEventHandle(bool wait)
{
    // Setup must be called, before any other Setters are being called
    n_assert(this->eventId.IsValid());

    if (!this->pendingCreateEventMessage.isvalid()) 
    {
        n_assert(this->eventHandle.isvalid());
        return;
    }

    n_assert(!this->eventHandle.isvalid());
    
    // need to wait for the server-side creation? this should
    // only be the case for the final discard message
    if(wait)
    {
        if(!this->pendingCreateEventMessage->Handled())
        {
            Audio2Interface::Instance()->Wait(this->pendingCreateEventMessage);
            n_assert(this->pendingCreateEventMessage->Handled());
        }
    }
    else
    {
        if (!this->pendingCreateEventMessage->Handled()) return;
    }

    // fmodcoreaudio thread handled our GetEvent-Message already, 
    // read handle from message, delete message and send off any pending messages
    this->eventHandle = this->pendingCreateEventMessage->GetEventHandle();
    n_assert(this->eventHandle.isvalid());
    this->pendingCreateEventMessage = 0;

    // send off any pending messages which have been created before
    // the server-side entity was created
    IndexT i;
    for (i = 0; i < this->pendingMessages.Size(); i++)
    {
        this->pendingMessages[i]->SetEventHandle(this->eventHandle);
        Audio2Interface::Instance()->SendBatched(this->pendingMessages[i]);
    }
    this->pendingMessages.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::SendEventMessage(const Ptr<Audio2::EventControlBase> &msg)
{
    // Setup must be called, before any other Setters are being called
    n_assert(this->eventId.IsValid());
    n_assert(msg.isvalid());

    // try to validate the entity handle, do not wait for completion!
    this->ValidateEventHandle(false);

    // check if we have to wait for the result of the GetEvent-message
    if(this->pendingCreateEventMessage.isvalid())
    {
        // as long as we wait for the event-handle, put the new message
        // in the pending message array
        this->pendingMessages.Append(msg);
        return;
    }

    msg->SetEventHandle(this->eventHandle);
    Audio2Interface::Instance()->SendBatched(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
Audio2Emitter::CancelPendingUpdateMessage()
{
    if(!this->pendingUpdateStateMsg.isvalid()) return;
    if(!this->pendingUpdateStateMsg->Handled())
    {
        Audio2Interface::Instance()->Cancel(this->pendingUpdateStateMsg);
    }
    this->pendingUpdateStateMsg = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void 
Audio2Emitter::SetStateBit(FmodEventState bits)
{
    this->state = (FmodEventState)(this->state | bits);
}

//------------------------------------------------------------------------------
/**
*/
void 
Audio2Emitter::UnsetStateBit(FmodEventState bits)
{
    this->state = (FmodEventState)(this->state & ~bits);
}

//------------------------------------------------------------------------------
/**
*/
bool 
Audio2Emitter::IsPlaying() const
{
    return (0 != (this->state & EventPlaying));
}

//------------------------------------------------------------------------------
/**
*/
bool 
Audio2Emitter::IsPaused() const
{
    return (0 != (this->state & EventPaused));
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::Play()
{
    n_assert(this->emitterRegistered);
    n_assert(this->eventId.IsValid());

    Ptr<Audio2::EventPlay> msg = Audio2::EventPlay::Create();
    this->SendEventMessage(msg.upcast<Audio2::EventControlBase>());
    this->SetStateBit(EventPlaying);

    // we need to cancel old update messages, otherwise they give us the
    // previous state
    this->CancelPendingUpdateMessage();
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::PlayProgrammerSound(const Util::String &name)
{
    n_assert(this->emitterRegistered);
    n_assert(this->eventId.IsValid());

    Ptr<Audio2::EventPlayProgrammerSound> msg = Audio2::EventPlayProgrammerSound::Create();
	msg->SetName(name);
    this->SendEventMessage(msg.upcast<Audio2::EventControlBase>());
    this->SetStateBit(EventPlaying);

	// we need to cancel old update messages, otherwise they give us the
    // previous state
    this->CancelPendingUpdateMessage();
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::Stop()
{
    n_assert(this->emitterRegistered);
    n_assert(this->eventId.IsValid());

    Ptr<Audio2::EventStop> msg = Audio2::EventStop::Create();
    this->SendEventMessage(msg.upcast<Audio2::EventControlBase>());
    this->UnsetStateBit(EventPlaying);

    // we need to cancel old update messages, otherwise they give us the
    // previous state
    this->CancelPendingUpdateMessage();
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::Pause()
{
    n_assert(this->emitterRegistered);
    n_assert(this->eventId.IsValid());

    Ptr<Audio2::EventPause> msg = Audio2::EventPause::Create();
    this->SendEventMessage(msg.upcast<Audio2::EventControlBase>());
    this->SetStateBit(EventPaused);
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::Resume()
{
    n_assert(this->emitterRegistered);
    n_assert(this->eventId.IsValid());

    Ptr<Audio2::EventResume> msg = Audio2::EventResume::Create();
    this->SendEventMessage(msg.upcast<Audio2::EventControlBase>());
    this->UnsetStateBit(EventPaused);
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::Set3DAttributes(const Math::vector &pos, const Math::vector &velocity)
{
    n_assert(this->emitterRegistered);
    n_assert(this->eventId.IsValid());

    Ptr<Audio2::EventUpdate3D> msg = Audio2::EventUpdate3D::Create();
    msg->SetPosition(pos);
    msg->SetVelocity(velocity);
    msg->SetHasOrientation(false);
    this->SendEventMessage(msg.upcast<Audio2::EventControlBase>());
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::SetParameter(const FmodEventParameterId &parameterId, float value)
{
    n_assert(this->emitterRegistered);
    n_assert(this->eventId.IsValid());

    Ptr<Audio2::EventSetParameter> msg = Audio2::EventSetParameter::Create();
    msg->SetParameterId(parameterId);
    msg->SetValue(value);
    this->SendEventMessage(msg.upcast<Audio2::EventControlBase>());
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Emitter::SetVolume(float volume)
{
    n_assert(this->emitterRegistered);
    n_assert(this->eventId.IsValid());

    Ptr<Audio2::EventSetVolume> msg = Audio2::EventSetVolume::Create();
    msg->SetVolume(volume);
    this->SendEventMessage(msg.upcast<Audio2::EventControlBase>());
}

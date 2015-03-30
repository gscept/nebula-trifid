#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::Audio2Emitter
  
    Emitter for Audio-Events, takes care of message-handling with
    FmodCoreAudio-subsystem
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/rtti.h"
#include "core/ptr.h"
#include "audio2/fmodeventid.h"
#include "audio2/fmodeventhandle.h"
#include "audio2/fmodeventstate.h"

namespace Math { class vector; }

//------------------------------------------------------------------------------
namespace Audio2
{
class EventCreate;
class EventControlBase;
class EventUpdateState;
class FmodEventParameterId;
class Audio2Emitter : public Core::RefCounted
{
    friend class Audio2Server;

    __DeclareClass(Audio2Emitter);

public:
    /// constructor
    Audio2Emitter();
    /// destructor
    virtual ~Audio2Emitter();
    
    /// setup the audio emitter object and create an event
    void Setup(const Audio2::FmodEventId& eventId);
    /// discard the audio emitter object
    void Discard();

    /// get the event id of the event this emitter is in charge of
    const Audio2::FmodEventId& GetEventId() const;
    /// get the event handle of the event this emitter is in charge of
    const Audio2::FmodEventHandle& GetEventHandle() const;

    /// play the event
    void Play();
	/// play a programmer sound
	void PlayProgrammerSound(const Util::String &name);
    /// stop the event
    void Stop();
    /// pause the event
    void Pause();
    /// Resume the event
    void Resume();
    /// sets position & velocity of the event
    void Set3DAttributes(const Math::vector &pos, const Math::vector &velocity);
    /// set a custom parameter
    void SetParameter(const Audio2::FmodEventParameterId &parameterId, float value);
    /// set volume (0.0f >= volume <= 1.0f)
    void SetVolume(float volume);

    /// event playing? returns true for paused events too
    bool IsPlaying() const;
    /// event paused?
    bool IsPaused() const;
    /// get the state of the event
    Audio2::FmodEventState GetState() const;

private:
    /// set the id of the event to emit
    void SetEventId(const Audio2::FmodEventId& eventId);
    /// send an event-control-message to audio-thread (play,pause etc)
    void SendEventMessage(const Ptr<Audio2::EventControlBase> &msg);
    
    /// validate event-handle
    void ValidateEventHandle(bool wait); 
    /// cancel a pending update message
    void CancelPendingUpdateMessage();

private:
    /// called in regular intervals by the audio server
    /// !!! IMPORTANT, try not to do anything here, only if we really really need to !!!
    /// if it works, we dont need to trigger those sounds each frame, cause it might be
    /// a lot of sounds in big scenes
    void OnFrame();
    /// set state bits
    void SetStateBit(FmodEventState bits);
    /// unset state bits
    void UnsetStateBit(FmodEventState bits);

    bool emitterRegistered;
    Audio2::FmodEventState state;
    Audio2::FmodEventId eventId;   
    Audio2::FmodEventHandle eventHandle;
    // before any message can be sent, we need to get the EventHandle for this event
    // if this message is not null, we are waiting for that message. meantime, any 
    // other messages are cached in pendingMessages, and are being sent, when we 
    // have the eventHandle, respectively when pendingCreateEventMessage is being handled
    Ptr<Audio2::EventCreate> pendingCreateEventMessage;    
    Util::Array<Ptr<EventControlBase> > pendingMessages; 
    Ptr<Audio2::EventUpdateState> pendingUpdateStateMsg;
};

//------------------------------------------------------------------------------
/**
*/
inline 
const Audio2::FmodEventId& 
Audio2Emitter::GetEventId() const
{
    return this->eventId;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Audio2::FmodEventHandle& 
Audio2Emitter::GetEventHandle() const
{
    return this->eventHandle;
}

//------------------------------------------------------------------------------
/**
*/
inline 
Audio2::FmodEventState 
Audio2Emitter::GetState() const
{
    return this->state;
}


} // namespace Audio2
//------------------------------------------------------------------------------

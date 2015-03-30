#pragma once
//------------------------------------------------------------------------------
/**
    @class FmodCoreAudio::FmodEvent
    
    Wrapper class for FMOD::Event

    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/

#include "core/refcounted.h"
#include "audio2/fmodeventid.h"
#include "audio2/fmodeventstate.h"

namespace FMOD { class Event; class EventProject; }
namespace Math { class vector; }
namespace Audio2 { class FmodEventParameterId; }

//------------------------------------------------------------------------------
namespace FmodCoreAudio
{

class FmodEventWrapper : public Core::RefCounted
{
    __DeclareClass(FmodEventWrapper);
public:
    /// constructor
    FmodEventWrapper();
    /// destructor
    virtual ~FmodEventWrapper();

    /// setup the wrapper
    void Setup(const Audio2::FmodEventId &eventId);
    /// discard the wrapper
    void Discard();    
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
    void Set3DAttributes(const Math::vector &pos, const Math::vector &velocity, const Math::vector *orientation);
    /// set a custom parameter
    void SetParameter(const Audio2::FmodEventParameterId &parameterId, float value);
    /// set volume (0.0f >= volume <= 1.0f)
    void SetVolume(float volume);
    /// get the event state
    const Audio2::FmodEventState& GetState() const;
    /// voices of this event are being stolen
    void OnVoicesStolen();
    /// this event starts playing now
    void OnPlay();
    /// this event stops playing now
    void OnStop();
    /// get the project, this event belongs to
    FMOD::EventProject* GetEventProject();
	/// get the name of the programmersound
	const Util::String& GetProgrammerSoundName() const; 

private:
    /// gets a new eventinstance for this event from fmod
    FMOD::Event* GetNewEventInstance() const;
    /// checks for stolen voices and gives a warning if NEBULA3_FMOD_STOLEN_VOICES_WARNINGS == 1
    /// returns false, if eventinstance is not valid anymore, because voices were stolen
    bool CheckForStolenVoices();
    /// set event state bits
    void SetStateBit(Audio2::FmodEventState bits);
    /// unset event state bits
    void UnsetStateBit(Audio2::FmodEventState bits);

private:
    Audio2::FmodEventId eventId;
    FMOD::Event *eventInstance;
    Audio2::FmodEventState state;
	Util::String programmerSoundName;
};

//------------------------------------------------------------------------------
/**
*/
inline
const Audio2::FmodEventState& 
FmodEventWrapper::GetState() const
{
    return this->state;
}

} // namespace FmodCoreAudio
//------------------------------------------------------------------------------

#pragma once
//------------------------------------------------------------------------------
/**
    @class FAudio::EventInstance
    
    Wrapper class for FMOD::Studio::EventInstance
    
    (C) 2014-2016 Individual contributors, see AUTHORS file
*/

#include "core/refcounted.h"
#include "faudio/eventid.h"
#include "faudio/eventstate.h"

namespace FMOD { namespace Studio { class EventInstance; } }
namespace Math { class vector; }
namespace FAudio { class EventParameterId; }

//------------------------------------------------------------------------------
namespace FAudio
{

class EventInstance : public Core::RefCounted
{
	__DeclareClass(EventInstance);
public:
    /// constructor
	EventInstance();
    /// destructor
	virtual ~EventInstance();

    /// setup the wrapper
    void Setup(const FAudio::EventId &eventId);
    /// discard the wrapper
    void Discard();    
    /// play the event
    void Play();	
    /// stop the event
    void Stop();
    /// pause the event
    void Pause();
    /// Resume the event
    void Resume();
    /// sets position & velocity of the event
    void Set3DAttributes(const Math::vector &pos, const Math::vector &velocity, const Math::vector *orientation);
    /// set a custom parameter
    void SetParameter(const FAudio::EventParameterId &parameterId, float value);
    /// set volume (0.0f >= volume <= 1.0f)
    void SetVolume(float volume);
    /// get the event state
    FAudio::EventState GetState() const;
    /// voices of this event are being stolen
    void OnVoicesStolen();    
	
private:
    /// gets a new eventinstance for this event from fmod
    FMOD::Studio::EventInstance* GetNewEventInstance() const;
    
private:
    FAudio::EventId eventId;
    FMOD::Studio::EventInstance *eventInstance;    	
};

typedef FAudio::EventInstance AudioEmitter;


} // namespace FAudio
//------------------------------------------------------------------------------

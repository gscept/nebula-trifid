//------------------------------------------------------------------------------
//  eventinstance.cc
//  (C) 2014-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudio/eventinstance.h"
#include "faudio/audiodevice.h"
#include "faudio/fmoderror.h"
#include "faudio/eventparameterid.h"

#include <fmod.hpp>
#include <fmod_studio.hpp>

using namespace FAudio;

__ImplementClass(FAudio::EventInstance, 'FCFE', Core::RefCounted);


//------------------------------------------------------------------------------
/**
*/
EventInstance::EventInstance() :
    eventInstance(NULL)    
{
    
}

//------------------------------------------------------------------------------
/**
*/
EventInstance::~EventInstance()
{
    // event must have been discarded
    n_assert(!this->eventInstance);
}

//------------------------------------------------------------------------------
/**
*/
void
EventInstance::Setup(const FAudio::EventId &eventId)
{
    n_assert(!this->eventInstance);
    this->eventId = eventId;
    this->eventInstance = this->GetNewEventInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
EventInstance::Discard()
{
    FMOD_COREAUDIO_VERBOSE("[event: \"%s\"  instance: 0x%p] Discard()",                            
                            eventId.GetEventPath().AsCharPtr(),
                            this->eventInstance);
    
    if(!this->eventInstance)
    {		
        return;
    }
    this->Stop();

    // reset callback
    FMOD_RESULT result = this->eventInstance->setCallback(NULL);
    FMOD_CHECK_ERROR(result);

    this->eventInstance = 0;
}

//------------------------------------------------------------------------------
/**
*/
FMOD::Studio::EventInstance*
EventInstance::GetNewEventInstance() const
{    
	n_assert(!this->eventInstance);
    n_assert(this->eventId.IsValid());
	FMOD::Studio::EventInstance * eventInst = NULL;
	
	if (AudioDevice::Instance()->HasSoundDevice())
	{
		FMOD::Studio::EventDescription* eventDesc = NULL;
		FMOD::Studio::System * system = AudioDevice::Instance()->GetSystem();
		FMOD_RESULT result = system->getEvent(eventId.GetEventPath().AsCharPtr(), &eventDesc);
		FMOD_CHECK_ERROR_EXT(result, "[event: \"%s\"] failed to get event description", eventId.GetEventPath().AsCharPtr());

		FMOD_STUDIO_LOADING_STATE state;
		result = eventDesc->getSampleLoadingState(&state);
		FMOD_CHECK_ERROR(result);
		if (state == FMOD_STUDIO_LOADING_STATE_UNLOADED || state == FMOD_STUDIO_LOADING_STATE_UNLOADING)
		{
			result = eventDesc->loadSampleData();
			FMOD_CHECK_ERROR(result);
		}


		
		result = eventDesc->createInstance(&eventInst);
		FMOD_CHECK_ERROR_EXT(result, "[event: \"%s\"] failed to get event instance", eventId.GetEventPath().AsCharPtr());

		FMOD_COREAUDIO_VERBOSE("[event: \"%s\"  instance: 0x%p] GetNewEventInstance()",
			eventId.GetEventPath().AsCharPtr(),
			eventInst);
	}
	return eventInst;
 
}

//------------------------------------------------------------------------------
/**
*/
void EventInstance::Play()
{        
	if (this->eventInstance)
	{
		FMOD_RESULT result = this->eventInstance->start();
		FMOD_CHECK_ERROR(result);
		FMOD_COREAUDIO_VERBOSE("[event: \"%s\" instance: 0x%p] Play()",
			eventId.GetEventPath().AsCharPtr(),
			this->eventInstance);
	}
}

//------------------------------------------------------------------------------
/**
*/
void EventInstance::Stop()
{
	if (this->eventInstance)
	{
		FMOD_STUDIO_PLAYBACK_STATE state;
		FMOD_RESULT result = this->eventInstance->getPlaybackState(&state);
		FMOD_CHECK_ERROR(result);

		if (state != FMOD_STUDIO_PLAYBACK_PLAYING) return;

		result = this->eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		FMOD_CHECK_ERROR(result);
		FMOD_COREAUDIO_VERBOSE("[project: event: \"%s\"  instance: 0x%p] Stop()",
			eventId.GetEventPath().AsCharPtr(),
			this->eventInstance);
	}
}

//------------------------------------------------------------------------------
/**
*/
void EventInstance::Pause()
{
	if (this->eventInstance)
	{
		FMOD_STUDIO_PLAYBACK_STATE state;
		FMOD_RESULT result = this->eventInstance->getPlaybackState(&state);
		FMOD_CHECK_ERROR(result);

		if (state != FMOD_STUDIO_PLAYBACK_PLAYING) return;

		this->eventInstance->setPaused(true);

		FMOD_COREAUDIO_VERBOSE("[event: \"%s\"  instance: 0x%p] Pause()",
			eventId.GetEventPath().AsCharPtr(),
			this->eventInstance);
	}
}

//------------------------------------------------------------------------------
/**
*/
void EventInstance::Resume()
{
	if (this->eventInstance)
	{
		FMOD_STUDIO_PLAYBACK_STATE state;
		FMOD_RESULT result = this->eventInstance->getPlaybackState(&state);
		FMOD_CHECK_ERROR(result);

		if (state != FMOD_STUDIO_PLAYBACK_PLAYING) return;
		bool paused;
		result = this->eventInstance->getPaused(&paused);
		FMOD_CHECK_ERROR(result);

		if (paused)
		{
			result = this->eventInstance->setPaused(false);
		}


		FMOD_COREAUDIO_VERBOSE("[event: \"%s\"  instance: 0x%p] Resume()",
			eventId.GetEventPath().AsCharPtr(),
			this->eventInstance);
	}
}

//------------------------------------------------------------------------------
/**
*/
void EventInstance::SetVolume(float volume)
{
	if (this->eventInstance)
	{
		n_assert(volume >= 0.0f);
		n_assert(volume <= 1.0f);

		FMOD_RESULT result = this->eventInstance->setVolume(volume);
		FMOD_CHECK_ERROR(result);
		FMOD_COREAUDIO_VERBOSE("[event: \"%s\"  instance: 0x%p] SetVolume(%f)",
			eventId.GetEventPath().AsCharPtr(),
			this->eventInstance,
			volume);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EventInstance::SetParameter(const FAudio::EventParameterId &parameterId, float value)
{
	if (this->eventInstance)
	{
		FMOD::Studio::ParameterInstance*param;
		FMOD_RESULT result = this->eventInstance->getParameter(parameterId.GetParameterName().AsCharPtr(), &param);
		FMOD_CHECK_ERROR_EXT(result, "[event: \"%s\"  instance: 0x%p  parameter: \"%s\"] failed to get event parameter",
			eventId.GetEventPath().AsCharPtr(),
			this->eventInstance,
			parameterId.GetParameterName().AsCharPtr());
		result = param->setValue(value);
		FMOD_CHECK_ERROR(result);
		FMOD_COREAUDIO_VERBOSE("[event: \"%s\"  instance: 0x%p  parameter: \"%s\"] SetParameter(%f)",
			eventId.GetEventPath().AsCharPtr(),
			this->eventInstance,
			parameterId.GetParameterName().AsCharPtr(),
			value);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EventInstance::Set3DAttributes(const Math::vector &pos, const Math::vector &velocity, const Math::vector *pOrientation)
{
	if (this->eventInstance)
	{
		FMOD_3D_ATTRIBUTES newattrs;
		this->eventInstance->get3DAttributes(&newattrs);

		newattrs.position.x = pos.x();
		newattrs.position.y = pos.y();
		newattrs.position.z = pos.z();


		newattrs.velocity.x = velocity.x();
		newattrs.velocity.y = velocity.y();
		newattrs.velocity.z = velocity.z();

		if (pOrientation)
		{
			newattrs.forward.x = pOrientation->x();
			newattrs.forward.y = pOrientation->y();
			newattrs.forward.z = pOrientation->z();
		}
		FMOD_RESULT result = this->eventInstance->set3DAttributes(&newattrs);
		FMOD_CHECK_ERROR(result);
	}
}

//------------------------------------------------------------------------------
/**
*/
FAudio::EventState
EventInstance::GetState() const
{
	if (!this->eventInstance) return EventStateUnknown;

	FMOD_STUDIO_PLAYBACK_STATE state;
	FMOD_RESULT result = this->eventInstance->getPlaybackState(&state);
	FMOD_CHECK_ERROR(result);
	switch (state)
	{
		case FMOD_STUDIO_PLAYBACK_PLAYING:
		{
			bool paused;
			result =  this->eventInstance->getPaused(&paused);
			FMOD_CHECK_ERROR(result);
			if (paused)
			{
				return EventPaused;
			}
			else
			{
				return EventPlaying;
			}
		}			
			break;
		case FMOD_STUDIO_PLAYBACK_STOPPED:
			return EventStopped;
			break;
		default:
			return EventStateUnknown;
	}
}
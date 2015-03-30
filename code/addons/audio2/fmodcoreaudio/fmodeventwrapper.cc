//------------------------------------------------------------------------------
//  fmodeventwrapper.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audio2/fmodcoreaudio/fmodeventwrapper.h"
#include "audio2/fmodcoreaudio/fmodaudiodevice.h"
#include "audio2/fmodcoreaudio/fmoderror.h"
#include "audio2/fmodeventparameterid.h"

#include <fmod_event.hpp>

using namespace FmodCoreAudio;

__ImplementClass(FmodCoreAudio::FmodEventWrapper, 'FCFE', Core::RefCounted);

//------------------------------------------------------------------------------
/**
    FMOD event callback
*/
FMOD_RESULT F_CALLBACK EventCallback(FMOD_EVENT *event, FMOD_EVENT_CALLBACKTYPE type, void *param1, void *param2, void *userdata)
{
    ASSERT_COREAUDIO_THREAD;

    switch(type)
    {
        case FMOD_EVENT_CALLBACKTYPE_EVENTSTARTED:
            {
                FmodEventWrapper *wrapper = reinterpret_cast<FmodEventWrapper*>(userdata);
                wrapper->OnPlay();
            }
            break;
        case FMOD_EVENT_CALLBACKTYPE_EVENTFINISHED:
            {
                FmodEventWrapper *wrapper = reinterpret_cast<FmodEventWrapper*>(userdata);
                wrapper->OnStop();
            }
            break;
        case FMOD_EVENT_CALLBACKTYPE_STOLEN:
            {
                FmodEventWrapper *wrapper = reinterpret_cast<FmodEventWrapper*>(userdata);
                wrapper->OnVoicesStolen();
            }
            break;
		case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_CREATE:
            {
				FmodEventWrapper *wrapper = reinterpret_cast<FmodEventWrapper*>(userdata);
                wrapper->OnPlay();
                FMOD::Sound **sound = reinterpret_cast<FMOD::Sound**>(param2);
				FMOD_RESULT result = FmodAudioDevice::Instance()->GetSystem()->createSound(wrapper->GetProgrammerSoundName().AsCharPtr(), FMOD_SOFTWARE | FMOD_LOOP_OFF | FMOD_2D, 0, sound);
                FMOD_CHECK_ERROR(result);
			}
			break;
		case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_RELEASE:
			{
                FmodEventWrapper *wrapper = reinterpret_cast<FmodEventWrapper*>(userdata);
                wrapper->OnStop();
                FMOD::Sound *sound = reinterpret_cast<FMOD::Sound*>(param2);
                char soundName[128];
                FMOD_RESULT result = sound->getName(soundName, 128);
				FMOD_CHECK_ERROR(result);
			}
			break;
        default:
            break;
    }

    return FMOD_OK;
}

//------------------------------------------------------------------------------
/**
*/
FmodEventWrapper::FmodEventWrapper() :
    eventInstance(NULL),
    state(Audio2::EventStateUnknown)
{
    ASSERT_COREAUDIO_THREAD;
}

//------------------------------------------------------------------------------
/**
*/
FmodEventWrapper::~FmodEventWrapper()
{
    // event must have been discarded
    n_assert(!this->eventInstance);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodEventWrapper::Setup(const Audio2::FmodEventId &eventId)
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(!this->eventInstance);
    this->eventId = eventId;
    this->eventInstance = this->GetNewEventInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
FmodEventWrapper::Discard()
{
    ASSERT_COREAUDIO_THREAD;
    FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p] Discard()",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            this->eventInstance);

    ASSERT_COREAUDIO_THREAD;
    if(!this->eventInstance)
    {
        n_assert(Audio2::EventStateUnknown == this->state || Audio2::EventVoicesStolen == this->state);
        return;
    }
    if(this->state & Audio2::EventPlaying) this->Stop();

    // reset callback
    FMOD_RESULT result = this->eventInstance->setCallback(NULL, NULL);
    FMOD_CHECK_ERROR(result);

    this->eventInstance = 0;
}

//------------------------------------------------------------------------------
/**
*/
FMOD::EventProject*
FmodEventWrapper::GetEventProject()
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(this->eventInstance);

    FMOD::EventGroup *group;
    FMOD_RESULT result = this->eventInstance->getParentGroup(&group);
    FMOD_CHECK_ERROR(result);
    FMOD::EventProject *project;
    result = group->getParentProject(&project);
    FMOD_CHECK_ERROR(result);
    return project;
}

//------------------------------------------------------------------------------
/**
*/
FMOD::Event*
FmodEventWrapper::GetNewEventInstance() const
{
    ASSERT_COREAUDIO_THREAD;
    // open must be called before loading an event project
    n_assert(!this->eventInstance);
    n_assert(this->eventId.IsValid());

    FMOD::EventProject *eventProject = FmodAudioDevice::Instance()->GetEventProject(eventId.GetEventProjectId());
    FMOD::Event *event;
    FMOD_RESULT result = eventProject->getEvent(eventId.GetEventPath().AsCharPtr(), FMOD_EVENT_DEFAULT, &event);
    FMOD_CHECK_ERROR_EXT(result, "[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p] failed to get event",
                         eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                         eventId.GetEventPath().AsCharPtr(),
                         this->eventInstance);
    result = event->setCallback(EventCallback, (void*)this);
    FMOD_CHECK_ERROR(result);
    return event;
}

//------------------------------------------------------------------------------
/**
*/
bool FmodEventWrapper::CheckForStolenVoices()
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(this->eventId.IsValid());
    if(this->eventInstance) return true;
    FMOD_COREAUDIO_WARNING("[project: \"audio:%s\"  event: \"%s\" instance: 0x%p] event-voices has been stolen, properties of such an event cant be changed, unless it gets its voices back, via Play.\n",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            this->eventInstance);
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void FmodEventWrapper::Play()
{
    ASSERT_COREAUDIO_THREAD;
    // it might happen, that the voices of this event has being stolen
    // in such case, we try to get a new one
    if(!this->eventInstance)
    {
        n_assert(this->state & Audio2::EventVoicesStolen);
        this->eventInstance = this->GetNewEventInstance();
        if(!this->eventInstance) return;
        this->UnsetStateBit(Audio2::EventVoicesStolen);
    }
    FMOD_RESULT result = this->eventInstance->start();
    FMOD_CHECK_ERROR(result);
    this->SetStateBit(Audio2::EventPlaying);
    FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\" instance: 0x%p] Play()",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            this->eventInstance);
}

//------------------------------------------------------------------------------
/**
*/
void FmodEventWrapper::PlayProgrammerSound(const Util::String &name)
{
	this->programmerSoundName = name;
	this->Play();
}

//------------------------------------------------------------------------------
/**
*/
void FmodEventWrapper::Stop()
{
    ASSERT_COREAUDIO_THREAD;
    if(!(this->state & Audio2::EventPlaying)) return;
    if(!this->CheckForStolenVoices()) return;
    FMOD_RESULT result = this->eventInstance->stop();
    FMOD_CHECK_ERROR(result);
    this->UnsetStateBit(Audio2::EventPlaying);
    FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p] Stop()",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            this->eventInstance);
}

//------------------------------------------------------------------------------
/**
*/
void FmodEventWrapper::Pause()
{
    ASSERT_COREAUDIO_THREAD;
    if(!this->CheckForStolenVoices()) return;
    FMOD_RESULT result = this->eventInstance->setPaused(true);
    FMOD_CHECK_ERROR(result);
    this->SetStateBit(Audio2::EventPaused);
    FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p] Pause()",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            this->eventInstance);
}

//------------------------------------------------------------------------------
/**
*/
void FmodEventWrapper::Resume()
{
    ASSERT_COREAUDIO_THREAD;
    if(!this->CheckForStolenVoices()) return;
    FMOD_RESULT result = this->eventInstance->setPaused(false);
    FMOD_CHECK_ERROR(result);
    this->UnsetStateBit(Audio2::EventPaused);
    FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p] Resume()",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            this->eventInstance);
}

//------------------------------------------------------------------------------
/**
*/
void FmodEventWrapper::SetVolume(float volume)
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(volume >= 0.0f);
    n_assert(volume <= 1.0f);
    if(!this->CheckForStolenVoices()) return;
    FMOD_RESULT result = this->eventInstance->setVolume(volume);
    FMOD_CHECK_ERROR(result);
    FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p] SetVolume(%f)",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            this->eventInstance,
                            volume);
}

//------------------------------------------------------------------------------
/**
*/
void FmodEventWrapper::SetParameter(const Audio2::FmodEventParameterId &parameterId, float value)
{
    ASSERT_COREAUDIO_THREAD;
    if(!this->CheckForStolenVoices()) return;
    FMOD::EventParameter *param;
    FMOD_RESULT result = this->eventInstance->getParameter(parameterId.GetParameterName().AsCharPtr(), &param);
    FMOD_CHECK_ERROR_EXT(result, "[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p  parameter: \"%s\"] failed to get event parameter",
                         eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                         eventId.GetEventPath().AsCharPtr(),
                         this->eventInstance,
                         parameterId.GetParameterName().AsCharPtr());
    result = param->setValue(value);
    FMOD_CHECK_ERROR(result);
    FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p  parameter: \"%s\"] SetParameter(%f)",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            this->eventInstance,
                            parameterId.GetParameterName().AsCharPtr(),
                            value);
}

//------------------------------------------------------------------------------
/**
*/
void FmodEventWrapper::Set3DAttributes(const Math::vector &pos, const Math::vector &vel, const Math::vector *pOrientation)
{
    ASSERT_COREAUDIO_THREAD;
    if(!this->CheckForStolenVoices()) return;
    FMOD_VECTOR position, velocity, orientation, *pOrientationFmod = NULL;
    position.x = pos.x();
    position.y = pos.y();
    position.z = pos.z();
    velocity.x = vel.x();
    velocity.y = vel.y();
    velocity.z = vel.z();
    if(pOrientation)
    {
        orientation.x = pOrientation->x();
        orientation.y = pOrientation->y();
        orientation.z = pOrientation->z();
        pOrientationFmod = &orientation;
    }
    FMOD_RESULT result = this->eventInstance->set3DAttributes(&position, &velocity, pOrientationFmod);
    FMOD_CHECK_ERROR(result);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodEventWrapper::SetStateBit(Audio2::FmodEventState bits)
{
    ASSERT_COREAUDIO_THREAD;
    this->state = (Audio2::FmodEventState)(this->state | bits);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodEventWrapper::UnsetStateBit(Audio2::FmodEventState bits)
{
    ASSERT_COREAUDIO_THREAD;
    this->state = (Audio2::FmodEventState)(this->state & ~bits);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodEventWrapper::OnVoicesStolen()
{
    ASSERT_COREAUDIO_THREAD;
    // stealing voices occured, so this instance is not valid anymore
    n_assert(this->eventInstance);
    const int unsetMask = Audio2::EventPlaying | Audio2::EventPaused;
    this->UnsetStateBit((Audio2::FmodEventState)unsetMask);
    this->SetStateBit(Audio2::EventVoicesStolen);
    // the instance is not valid anymore
    this->eventInstance = NULL;
    FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p] OnVoicesStolen()",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            this->eventInstance);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodEventWrapper::OnPlay()
{
    this->SetStateBit(Audio2::EventPlaying);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodEventWrapper::OnStop()
{
    this->UnsetStateBit(Audio2::EventPlaying);
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
FmodEventWrapper::GetProgrammerSoundName() const
{
    return this->programmerSoundName;
}


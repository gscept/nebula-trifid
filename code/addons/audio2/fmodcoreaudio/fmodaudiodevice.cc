//------------------------------------------------------------------------------
//  fmodaudiodevice.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audio2/fmodcoreaudio/fmodaudiodevice.h"
#include "audio2/fmodcoreaudio/fmoderror.h"
#include "audio2/fmodcoreaudio/fmodeventwrapper.h"
#include "audio2/fmodeventprojectid.h"
#include "audio2/fmodeventid.h"
#include "audio2/fmodeventcategoryid.h"
#include "io/assignregistry.h"
#include "threading/thread.h"
#include "jobs/jobsystem.h"
#include <fmod_event_net.hpp>

#if __PS3__
#include <fmodps3.h>
#include "io/ps3/ps3fswrapper.h"
#include "io/ps3/ps3fioswrapper.h"
#endif

using namespace FmodCoreAudio;
using namespace Math;

__ImplementClass(FmodCoreAudio::FmodAudioDevice, 'AUDF', Core::RefCounted);
__ImplementSingleton(FmodCoreAudio::FmodAudioDevice);

const int FmodAudioDevice::MAX_CHANNELS = 64;
Threading::ThreadId FmodAudioDevice::FmodCoreAudioThreadId = Threading::InvalidThreadId;

//------------------------------------------------------------------------------
/**
*/
FMOD_RESULT F_CALLBACK systemcallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACKTYPE type, void *commanddata1, void *commanddata2)
{
    switch (type)
    {
        case FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED:
            n_error("FMOD ERROR : FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED occured. %d bytes.\n", (int)commanddata2);
            break;
        case FMOD_SYSTEM_CALLBACKTYPE_BADDSPCONNECTION:
            n_error("FMOD ERROR : FMOD_SYSTEM_CALLBACKTYPE_BADDSPCONNECTION occured\n");
            break;
        case FMOD_SYSTEM_CALLBACKTYPE_BADDSPLEVEL:
            n_error("FMOD ERROR : FMOD_SYSTEM_CALLBACKTYPE_BADDSPLEVEL occured\n");
            break;
        default:
            break;
    }

    return FMOD_OK;
}

//------------------------------------------------------------------------------
/**
*/
FMOD_RESULT F_CALLBACK EventCallbackFireAndForget(FMOD_EVENT *event, FMOD_EVENT_CALLBACKTYPE type, void *param1, void *param2, void *userdata)
{
    ASSERT_COREAUDIO_THREAD;

    switch(type)
    {
        case FMOD_EVENT_CALLBACKTYPE_STOLEN:
            FMOD_COREAUDIO_VERBOSE("[instance: 0x%p] fire-and-forget-event voices stolen", event);
            break;

        case FMOD_EVENT_CALLBACKTYPE_EVENTFINISHED:
            FMOD_COREAUDIO_VERBOSE("[instance: 0x%p] fire-and-forget-event finished", event);
            break;

        default:
            break;
    }

    return FMOD_OK;
}

//------------------------------------------------------------------------------
/**
*/
FmodAudioDevice::FmodAudioDevice() :
    eventSystem(NULL),
    system(NULL),
	musicSystem(NULL),
    categoryMaster(NULL),
    listener(),
	auditioningEnabled(false),
	hasSoundDevice(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FmodAudioDevice::~FmodAudioDevice()
{
    n_assert(!this->IsOpen());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
FmodAudioDevice::Open()
{
    n_assert(!this->IsOpen());
    n_assert(!this->eventSystem);
    n_assert(!this->system);
    n_assert(!this->musicSystem);
    n_assert(FmodAudioDevice::FmodCoreAudioThreadId == Threading::InvalidThreadId);

    FmodAudioDevice::FmodCoreAudioThreadId = Threading::Thread::GetMyThreadId();
    n_assert(FmodAudioDevice::FmodCoreAudioThreadId != Threading::InvalidThreadId);

    FMOD_RESULT result = FMOD::EventSystem_Create(&this->eventSystem);
    FMOD_CHECK_ERROR(result);

    // get the fmod ex lib from fmoddesigner
    result = this->eventSystem->getSystemObject(&this->system);
    FMOD_CHECK_ERROR(result);

    // check fmod version
    unsigned int version;
    result = this->system->getVersion(&version);
    FMOD_CHECK_ERROR(result);
    if (version < FMOD_VERSION)
    {
        n_error("You are using an old version of FMOD %08x.  This program requires %08x\nFILE: %s\nLine:%d\n", version, FMOD_VERSION, __FILE__, __LINE__);
    }

    int numdrivers;
    result = this->system->getNumDrivers(&numdrivers);
    FMOD_CHECK_ERROR(result);
    if (!numdrivers)
    {
        n_warning("No Sound-Device found\nFILE: %s\nLine:%d\n", __FILE__, __LINE__);
		this->hasSoundDevice = false;
		return false;
    }
	else
	{
		this->hasSoundDevice = true;
	}
	

    // this prevents from buggy audio-driver-crashs
    result = this->system->setHardwareChannels(0);
    FMOD_CHECK_ERROR(result); 

    char driverName[256] = {'\0'};
    result = system->getDriverInfo(0, driverName, 256, 0); 
    FMOD_CHECK_ERROR(result); 
    driverName[255] ='\0';
    FMOD_COREAUDIO_VERBOSE("audio device %s\n", driverName);

    // set fmod to the same speaker-mode as configured in the system
    FMOD_CAPS caps;
    FMOD_SPEAKERMODE speakermode;
    result = this->system->getDriverCaps(0, &caps, 0, &speakermode);
    FMOD_CHECK_ERROR(result);
#if __PS3__
    speakermode = FMOD_SPEAKERMODE_7POINT1;
#endif
    result = this->system->setSpeakerMode(speakermode);
    FMOD_CHECK_ERROR(result);

#if NEBULA3_FMOD_COREAUDIO_VERBOSE_ENABLED
    if(caps & FMOD_CAPS_HARDWARE) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_HARDWARE");
    if(caps & FMOD_CAPS_HARDWARE_EMULATED) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_HARDWARE_EMULATED");
    if(caps & FMOD_CAPS_OUTPUT_MULTICHANNEL) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_OUTPUT_MULTICHANNEL");
    if(caps & FMOD_CAPS_OUTPUT_FORMAT_PCM8) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_OUTPUT_FORMAT_PCM8");
    if(caps & FMOD_CAPS_OUTPUT_FORMAT_PCM16) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_OUTPUT_FORMAT_PCM16");
    if(caps & FMOD_CAPS_OUTPUT_FORMAT_PCM24) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_OUTPUT_FORMAT_PCM24");
    if(caps & FMOD_CAPS_OUTPUT_FORMAT_PCM32) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_OUTPUT_FORMAT_PCM32");
    if(caps & FMOD_CAPS_REVERB_EAX2) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_REVERB_EAX2");
    if(caps & FMOD_CAPS_REVERB_EAX3) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_REVERB_EAX3");
    if(caps & FMOD_CAPS_REVERB_EAX4) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_REVERB_EAX4");
    if(caps & FMOD_CAPS_REVERB_EAX5) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_REVERB_EAX5");
    if(caps & FMOD_CAPS_REVERB_I3DL2) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_REVERB_I3DL2");
    if(caps & FMOD_CAPS_REVERB_LIMITED) FMOD_COREAUDIO_VERBOSE("audio device caps: FMOD_CAPS_REVERB_LIMITED");
#endif

    if (caps & FMOD_CAPS_HARDWARE_EMULATED)
    {
        // The user has the 'Acceleration' slider set to off!  This is really bad for latency!. 
        n_printf("FMOD warning: Audio 'Acceleration' slider set to off!  This is really bad for latency!.\n");
        result = this->system->setDSPBufferSize(1024, 10); 
        FMOD_CHECK_ERROR(result); 
    }

    // Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it
    if (strstr(driverName, "SigmaTel"))   
    { 
        result = this->system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
		FMOD_CHECK_ERROR(result); 
    }

    void *pExtraDriverData = NULL;
#if __PS3__
    FMOD_PS3_EXTRADRIVERDATA extradriverdata;
    memset(&extradriverdata, 0, sizeof(FMOD_PS3_EXTRADRIVERDATA));
    extradriverdata.spurs = Jobs::JobSystem::Instance()->PS3GetSpursInstance();
    // if it fails here, then the IO::GameContentServer was not setup properly,
    // since it sets the default-scheduler
    n_assert(PS3::PS3FIOSWrapper::Instance()->GetDefaultScheduler());
    extradriverdata.fios_scheduler = PS3::PS3FIOSWrapper::Instance()->GetDefaultScheduler();
    // https://ps3.scedev.net/forums/thread/68099
    // otherwise we have time outs in spurs-tuner
    extradriverdata.spursmode = FMOD_PS3_SPURSMODE_CREATECONTEXT;
    pExtraDriverData = &extradriverdata;
#endif

#if NEBULA3_FMOD_ENABLE_PROFILING
    result = this->eventSystem->init(FmodAudioDevice::MAX_CHANNELS, FMOD_INIT_ENABLE_PROFILE | FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, pExtraDriverData, FMOD_EVENT_INIT_NORMAL);
#else
    result = this->eventSystem->init(FmodAudioDevice::MAX_CHANNELS, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, pExtraDriverData, FMOD_EVENT_INIT_NORMAL);
#endif
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)        
    { 
        // Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... 
        result = this->system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO); 
        FMOD_CHECK_ERROR(result); 
        
        // reinit
        result = this->eventSystem->init(FmodAudioDevice::MAX_CHANNELS, FMOD_INIT_NORMAL|FMOD_INIT_3D_RIGHTHANDED, pExtraDriverData, FMOD_EVENT_INIT_NORMAL);
        FMOD_CHECK_ERROR(result); 
    }
    else
    {
        FMOD_CHECK_ERROR(result);
    }

    // setting audio media root ("audio:")
    const IO::URI path = IO::AssignRegistry::Instance()->ResolveAssignsInString("audio:");
    const char *rawPath;
#if __PS3__
    Util::String strRawPath = Util::String(PS3::PS3FSWrapper::ConvertPath(path.LocalPath())) + "/";
    rawPath = strRawPath.AsCharPtr();
#else
    Util::String strRawPath = path.LocalPath() + "/";
    rawPath = strRawPath.AsCharPtr();
#endif
    result = this->eventSystem->setMediaPath(rawPath);
    FMOD_CHECK_ERROR(result);

    result = this->eventSystem->getCategory("master", &this->categoryMaster);
    FMOD_CHECK_ERROR(result);

    result = this->system->setCallback(systemcallback);
    FMOD_CHECK_ERROR(result);

    result = this->eventSystem->getMusicSystem(&this->musicSystem);
    FMOD_CHECK_ERROR(result);

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::Close()
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(this->IsOpen());
    n_assert(this->eventSystem);

    this->DiscardAllEvents();
    n_assert(!this->events.Size());

    this->DiscardAllCues();
    n_assert(!this->musicPrompts.Size());

    // reset callback
    FMOD_RESULT result = this->system->setCallback(NULL);
    FMOD_CHECK_ERROR(result);

    // close and release fmod
    result = this->eventSystem->release();
    FMOD_CHECK_ERROR(result);
    this->eventSystem = NULL;
    this->system = NULL;
	this->musicSystem = NULL;
    this->categoryMaster = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::OnFrame()
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(this->eventSystem);

	if (!this->hasSoundDevice) return;

    this->listener.OnFrame();

    FMOD_RESULT result = this->eventSystem->update();
    FMOD_CHECK_ERROR(result);

	if (this->auditioningEnabled)
	{
        result = FMOD::NetEventSystem_Update();
        FMOD_CHECK_ERROR(result);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::LoadEventProject(const Audio2::FmodEventProjectId &projectId)
{
    ASSERT_COREAUDIO_THREAD;
    // open must be called before loading an event project
    n_assert(this->eventSystem);
    n_assert(projectId.IsValid());

    FMOD::EventProject *eventProject;
	Util::String foo = projectId.GetFullFilePathAndName().AsCharPtr();
    FMOD_RESULT result = this->eventSystem->load(projectId.GetFullFilePathAndName().AsCharPtr(), 0, &eventProject);
    FMOD_CHECK_ERROR_EXT(result, "faild to load \"audio:%s\"", projectId.GetFullFilePathAndName().AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::UnloadEventProject(const Audio2::FmodEventProjectId &projectId)
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(projectId.IsValid());

    FMOD::EventProject *eventProject = this->GetEventProject(projectId);
    this->DiscardEventsOfProject(eventProject);
    FMOD_RESULT result = eventProject->stopAllEvents(true);
    FMOD_CHECK_ERROR(result);
    result = eventProject->release();
    FMOD_CHECK_ERROR(result);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<FmodEventWrapper>
FmodAudioDevice::CreateEvent(const Audio2::FmodEventId &eventId)
{
    ASSERT_COREAUDIO_THREAD;
    // open must be called before loading an event project
    n_assert(this->eventSystem);
    n_assert(eventId.IsValid());

    Ptr<FmodEventWrapper> wrapper = FmodEventWrapper::Create();
    wrapper->Setup(eventId);
    this->events.Add(wrapper, wrapper);
    return wrapper;
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::DiscardEvent(const Ptr<FmodEventWrapper> &wrapper)
{
    ASSERT_COREAUDIO_THREAD;
    // open must be called before loading an event project
    n_assert(this->eventSystem);
    n_assert(wrapper.isvalid());

    IndexT index = this->events.FindIndex(wrapper);
    // nothing to do, if it was already removed, might happen if the client
    // calls UnloadProject and after that he discards the audioemitter
    if(index == InvalidIndex) return;

    wrapper->Discard();
    this->events.EraseAtIndex(index);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::DiscardAllEvents()
{
    ASSERT_COREAUDIO_THREAD;
    int index;
    for(index = 0; index < this->events.Size(); ++index)
    {
        this->events.ValueAtIndex(index)->Discard();
    }
    this->events.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::DiscardEventsOfProject(const FMOD::EventProject *project)
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(project);

    Util::Array<Ptr<FmodEventWrapper> > removeEvents;
    int index;
    for(index = 0; index < this->events.Size(); ++index)
    {
        Ptr<FmodEventWrapper> wrapper = this->events.ValueAtIndex(index);
        if(wrapper->GetEventProject() == project)
        {
            removeEvents.Append(wrapper);
        }
    }

    for(index = 0; index < removeEvents.Size(); ++index)
    {
        IndexT i = this->events.FindIndex(removeEvents[index]);
        n_assert(i != InvalidIndex);
        removeEvents[index]->Discard();
        removeEvents[index] = 0;
        this->events.EraseAtIndex(i);
    }
    removeEvents.Clear();
}

//------------------------------------------------------------------------------
/**
*/
FMOD::EventCategory*
FmodAudioDevice::GetCategory(const Audio2::FmodEventCategoryId &categoryId)
{
    ASSERT_COREAUDIO_THREAD;
    // open must be called before loading an event project
    n_assert(this->eventSystem);
    n_assert(categoryId.IsValid());

    if(categoryId.GetCategory() == "master")
    {
        return this->categoryMaster;
    }
    FMOD::EventCategory *category;
    FMOD_RESULT result = this->categoryMaster->getCategory(categoryId.GetCategory().AsCharPtr(), &category);
    FMOD_CHECK_ERROR_EXT(result, "failed to get event category [category: \"master/%s\"]", categoryId.GetCategory().AsCharPtr());
    return category;
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::SetListener(const Math::matrix44& transform, const Math::vector& velocity)
{
    ASSERT_COREAUDIO_THREAD;
    this->listener.Set(transform, velocity);
}

//------------------------------------------------------------------------------
/**
*/
FMOD::EventProject*
FmodAudioDevice::GetEventProject(const Audio2::FmodEventProjectId &projectId)
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(projectId.IsValid());
    FMOD::EventProject *eventProject;
    FMOD_RESULT result = this->eventSystem->getProject(projectId.GetName().AsCharPtr(), &eventProject);
    FMOD_CHECK_ERROR_EXT(result, "faild to access event project \"%s\"", projectId.GetName().AsCharPtr());
    return eventProject;
}

#define Type2String(type) case type: return ((#type)+13); break;


//------------------------------------------------------------------------------
/**
*/
void FmodAudioDevice::DumpMemoryInfo(const char *where) const
{

	// FIXME
#if 0
	ASSERT_COREAUDIO_THREAD;
    unsigned int memoryused;
    unsigned int memoryused_array[FMOD_MEMTYPE_MAX];
    this->eventSystem->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &memoryused, memoryused_array);
    n_printf("\n\n===================================================\n");
    n_printf("FMOD Memory Usage at %s:\n", where);
    n_printf("  used overall: %9d bytes  %7.3f MB\n", memoryused, (float)memoryused / (1024.0f*1024.0f));
    int i;
    for(i = 0; i < FMOD_MEMTYPE_MAX; ++i)
    {
        n_printf("    [%-25s]     %9d bytes  %7.3f MB\n", FmodMemtypeToString((FMOD_MEMTYPE)i), memoryused_array[i], (float)memoryused_array[i] / (1024.0f*1024.0f));
    }
    n_printf("\n\n===================================================\n");
#endif
}

//------------------------------------------------------------------------------
/**
*/
void FmodAudioDevice::EventPlayFireAndForget(const Audio2::FmodEventId &eventId, float volume)
{
    ASSERT_COREAUDIO_THREAD;
    FMOD::EventProject *eventProject = this->GetEventProject(eventId.GetEventProjectId());
    FMOD::Event *event;
    FMOD_RESULT result = eventProject->getEvent(eventId.GetEventPath().AsCharPtr(), FMOD_EVENT_DEFAULT, &event);
    if(FMOD_ERR_EVENT_FAILED == result)
    {
        FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"] failed to get event, most likely because of \"Max Playbacks behaviour == Just Fail\"",
                                eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                                eventId.GetEventPath().AsCharPtr());
        return;
    }
    FMOD_CHECK_ERROR_EXT(result, "[project: \"audio:%s\"  event: \"%s\"] failed to get event",
                         eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                         eventId.GetEventPath().AsCharPtr());
    result = event->setCallback(EventCallbackFireAndForget, NULL);
    FMOD_CHECK_ERROR(result);
    result = event->setVolume(volume);
    FMOD_CHECK_ERROR(result);
    result = event->start();
    FMOD_CHECK_ERROR(result);
    FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p] EventPlayFireAndForget()",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            event);
}

//------------------------------------------------------------------------------
/**
*/
void FmodAudioDevice::EventPlayFireAndForget3D(const Audio2::FmodEventId &eventId, const matrix44 &transform, const Math::vector &velocity, float volume)
{
    ASSERT_COREAUDIO_THREAD;
    FMOD::EventProject *eventProject = this->GetEventProject(eventId.GetEventProjectId());
    FMOD::Event *event;

    FMOD_VECTOR position, vel;
    position.x = transform.get_position().x();
    position.y = transform.get_position().y();
    position.z = transform.get_position().z();
    vel.x = velocity.x();
    vel.y = velocity.y();
    vel.z = velocity.z();

    // infoonly for setting 3D attributes, so fail quietest can work
    FMOD_RESULT result = eventProject->getEvent(eventId.GetEventPath().AsCharPtr(), FMOD_EVENT_INFOONLY, &event);
    FMOD_CHECK_ERROR(result);
    result = event->set3DAttributes(&position, &vel, NULL);
    FMOD_CHECK_ERROR(result);
    result = event->setVolume(volume);
    FMOD_CHECK_ERROR(result);

    // get the real event
    result = eventProject->getEvent(eventId.GetEventPath().AsCharPtr(), FMOD_EVENT_DEFAULT, &event);
    if(FMOD_ERR_EVENT_FAILED == result)
    {
        FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"] failed to get event, most likely because of \"Max Playbacks behaviour == Just Fail\"",
                                eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                                eventId.GetEventPath().AsCharPtr());
        return;
    }
    FMOD_CHECK_ERROR_EXT(result, "[project: \"audio:%s\"  event: \"%s\"] failed to get event",
                         eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                         eventId.GetEventPath().AsCharPtr());
    // remove set3DAttributes, its set above already?
    result = event->set3DAttributes(&position, &vel, NULL);
    FMOD_CHECK_ERROR(result);
    result = event->setCallback(EventCallbackFireAndForget, NULL);
    FMOD_CHECK_ERROR(result);
    result = event->start();
    FMOD_CHECK_ERROR(result);
    FMOD_COREAUDIO_VERBOSE("[project: \"audio:%s\"  event: \"%s\"  instance: 0x%p] EventPlayFireAndForget3D()",
                            eventId.GetEventProjectId().GetFullFilePathAndName().AsCharPtr(),
                            eventId.GetEventPath().AsCharPtr(),
                            event);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::SetAuditioningEnabled(bool enabled)
{
    ASSERT_COREAUDIO_THREAD;

    if (this->auditioningEnabled == enabled) return;
	n_assert(this->eventSystem);
	
	FMOD_RESULT result;
    if (enabled)
	{
        result = FMOD::NetEventSystem_Init(this->eventSystem);        
	}
	else
	{
        result = FMOD::NetEventSystem_Shutdown();
	}
    FMOD_CHECK_ERROR(result);

	this->auditioningEnabled = enabled;
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::MusicPlayCue(const int cueId)
{
    ASSERT_COREAUDIO_THREAD;
    Ptr<FmodMusicPromptWrapper> musicPrompt = FmodMusicPromptWrapper::Create();
    musicPrompt->Setup(cueId);
	musicPrompt->Begin();
    this->musicPrompts.Add(cueId, musicPrompt);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::MusicStopCue(const int cueId)
{
    ASSERT_COREAUDIO_THREAD
    IndexT index = this->musicPrompts.FindIndex(cueId);
    if (InvalidIndex == index) return;
	this->musicPrompts.ValueAtIndex(index)->End();
    this->musicPrompts.ValueAtIndex(index)->Discard();
    this->musicPrompts.EraseAtIndex(index);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioDevice::DiscardAllCues()
{
    ASSERT_COREAUDIO_THREAD;
    IndexT index;
    for (index = 0; index < this->musicPrompts.Size(); index++)
    {
        this->musicPrompts.ValueAtIndex(index)->Discard();
    }
    this->musicPrompts.Clear();
}



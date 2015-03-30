#pragma once
//------------------------------------------------------------------------------
/**
    @class FmodCoreAudio::FmodAudioDevice
  
    Central class of the fmod core audio subsystem. It implements the fmod Environment.
    Applications must not use the CoreAudio subsystem directly (since it's running
    in its own thread) and fmod is not thread-safe.
    Instead use the front end classes in the Audio2 subsystem, which encapsulates
    this FmodCoreAudio-subsystem
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/

#include "core/refcounted.h"
#include "core/singleton.h"
#include "core/debug.h"
#include "resources/resourceid.h"
#include "threading/threadid.h"
#include "audio2/fmodcoreaudio/fmodlistener.h"
#include "threading/thread.h"
#include <fmod_event.hpp>
#include "audio2/fmodcoreaudio/fmodmusicpromptwrapper.h"

#define ASSERT_COREAUDIO_THREAD n_assert(FmodAudioDevice::GetCoreAudioThreadId() == Threading::Thread::GetMyThreadId())

namespace FMOD { class EventSystem; class System; class EventProject; class Event; class EventCategory; }
namespace Audio2 { class FmodEventProjectId; class FmodEventId; class FmodEventCategoryId; }

//------------------------------------------------------------------------------
namespace FmodCoreAudio
{
class FmodEventWrapper;
class FmodAudioDevice : public Core::RefCounted
{
    __DeclareClass(FmodAudioDevice);
    __DeclareSingleton(FmodAudioDevice);
public:
    /// constructor
    FmodAudioDevice();
    /// destructor
    virtual ~FmodAudioDevice();

    /// open the audio device
    bool Open();
    /// close the audio device
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// do per-frame work
    void OnFrame();
    /// load a *.fev event file, path relative to "audio:", dont put the "fev"-extension
    void LoadEventProject(const Audio2::FmodEventProjectId &projectId);
    /// unload an eventproject
    void UnloadEventProject(const Audio2::FmodEventProjectId &projectId);
    // get event from an open event-project
    /// create an event, put it into a FmodEventWrapper and store it in dictionary
    Ptr<FmodEventWrapper> CreateEvent(const Audio2::FmodEventId &eventId);
    /// discard an event and remove it from dictionary
    void DiscardEvent(const Ptr<FmodEventWrapper> &event);
    /// discard all events
    void DiscardAllEvents();
    /// discard all events of a project
    void DiscardEventsOfProject(const FMOD::EventProject *project);
    /// play a fire and forget event
    void EventPlayFireAndForget(const Audio2::FmodEventId &eventId, float volume);
    /// play a fire and forget event with 3D data
    void EventPlayFireAndForget3D(const Audio2::FmodEventId &eventId, const Math::matrix44 &transform, const Math::vector &velocity, float volume);    
    /// return a loaded event project
    FMOD::EventProject* GetEventProject(const Audio2::FmodEventProjectId &projectId);
    // get category from an open event-project
    FMOD::EventCategory* GetCategory(const Audio2::FmodEventCategoryId &categoryId);
    /// set the 3d listener properties
    void SetListener(const Math::matrix44& transform, const Math::vector& velocity);
    /// get the fmod-event-system-object
    FMOD::EventSystem* GetEventSystem();
    /// dump fmod-memory-statistic
    void DumpMemoryInfo(const char *where) const;
    /// get the thread-id this coreaudiofmox-subsystem is running in
    static const Threading::ThreadId& GetCoreAudioThreadId();    
	
	/// get the fmod-system-object
    FMOD::System* GetSystem();
	/// enable/disable fmod auditioning feature
    void SetAuditioningEnabled(bool enabled);
	/// get the fmod-music-system-object
    FMOD::MusicSystem* GetMusicSystem();
	/// trigger the lifespan of a cue 
	void MusicPlayCue(const int cueId);
    /// ends the lifespan of a cue
	void MusicStopCue(const int cueId);
    /// discard all cues
    void DiscardAllCues();

	/// returns if there is a sound device present
	bool HasSoundDevice();
    
private:
    // root object of fmod-designer-api
    FMOD::EventSystem *eventSystem;
    // root object of fmodex api
    FMOD::System *system;
    // root-category master
    FMOD::EventCategory *categoryMaster;
    // the 3D-listener
    FmodListener listener;
    /// dictionary of used events, maybe a hashmap might be better, hasing the pointer
    Util::Dictionary<Ptr<FmodEventWrapper>, Ptr<FmodEventWrapper> > events;
    /// TODO, client must set this value on startup
    static const int MAX_CHANNELS;
    static Threading::ThreadId FmodCoreAudioThreadId;
	
	bool hasSoundDevice;
	bool auditioningEnabled;
	FMOD::MusicSystem* musicSystem;
    Util::Dictionary<int, Ptr<FmodMusicPromptWrapper> > musicPrompts;
};

//------------------------------------------------------------------------------
/**
*/
inline 
bool
FmodAudioDevice::IsOpen() const
{
    return (NULL != this->eventSystem);
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Threading::ThreadId& 
FmodAudioDevice::GetCoreAudioThreadId()
{
    return FmodAudioDevice::FmodCoreAudioThreadId;
}

//------------------------------------------------------------------------------
/**
*/
inline 
FMOD::EventSystem* 
FmodAudioDevice::GetEventSystem()
{
    return this->eventSystem;
}

//------------------------------------------------------------------------------
/**
*/
inline 
FMOD::System* 
FmodAudioDevice::GetSystem()
{
    return this->system;
}

//------------------------------------------------------------------------------
/**
*/
inline 
FMOD::MusicSystem*
FmodAudioDevice::GetMusicSystem()
{
    return this->musicSystem;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodCoreAudio::FmodAudioDevice::HasSoundDevice()
{
	return this->hasSoundDevice;
}


} // namespace FmodCoreAudio
//------------------------------------------------------------------------------

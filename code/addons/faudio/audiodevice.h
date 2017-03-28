#pragma once
//------------------------------------------------------------------------------
/**
    @class FAudio::AudioDevice
  
    Central class of the fmod core audio subsystem. It implements the fmod Environment.    
        
    (C) 2014-2016 Individual contributors, see AUTHORS file
*/

#include "core/refcounted.h"
#include "core/singleton.h"
#include "core/debug.h"
#include "resources/resourceid.h"
#include "threading/threadid.h"
#include "faudio/audiolistener.h"
#include "faudio/audiointerface.h"
#include "threading/thread.h"
#include <fmod_studio.hpp>


namespace FMOD { class System; namespace Studio { class System; class Bus; class Bank; class EventInstance; class EventDescription; } }
namespace FAudio {	class BusId; class EventId; class EventInstance; }

//------------------------------------------------------------------------------
namespace FAudio
{

class AudioDevice : public Core::RefCounted
{
	__DeclareClass(AudioDevice);
	__DeclareSingleton(AudioDevice);
public:
    /// constructor
	AudioDevice();
    /// destructor
	virtual ~AudioDevice();

    /// open the audio device
    bool Open();
    /// close the audio device
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// do per-frame work
    void OnFrame();
	/// load an audio bank file
	void LoadBank(const Util::String & bankFile, bool nonblocking = false);
	/// unload an audio bank file
	void UnloadBank(const Util::String & bankFile);
	/// unload all bank files
	void UnloadAll();

	/// get all available bank files
	static Util::Array<Util::String> FindBankFiles();
	/// parse a xml file with settings
	static Util::Dictionary<Util::String, bool> ParseAutoload(const IO::URI & path);    
	
	/// is bank loaded
	bool BankLoaded(const Util::String & bankFile);
        
    /// create an event
    Ptr<FAudio::EventInstance> CreateEvent(const FAudio::EventId &eventId);
    /// discard an event and remove it from dictionary
    void DiscardEvent(const Ptr<FAudio::EventInstance> &event);
    /// discard all events
    void DiscardAllEvents();
    /// play a fire and forget event
    void EventPlayFireAndForget(const FAudio::EventId &eventId, float volume);
    /// play a fire and forget event with 3D data
    void EventPlayFireAndForget3D(const FAudio::EventId &eventId, const Math::matrix44 &transform, const Math::vector &velocity, float volume);        
	/// play a fire and forget event with 3D data
	void EventPlayFireAndForget3D(const FAudio::EventId &eventId, const Math::point &position, const Math::vector & forward, const Math::vector &up, const Math::vector &velocity, float volume);
    /// set the 3d listener properties
    void SetListener(const Math::matrix44& transform, const Math::vector& velocity);    
    /// dump fmod-memory-statistic
    void DumpMemoryInfo(const char *where) const;

	/// master volume
	void SetMasterVolume(float volume);
	/// 
	float GetMasterVolume();
	/// master pitch
	void SetMasterPitch(float pitch);
	///
	float GetMasterPitch();
	/// set mute
	void SetMasterMute(bool mute);

	/// bus interface
	// get bus from an open event-project
	FMOD::Studio::Bus* GetBus(const FAudio::BusId & busId);
	/// set master volume
	void SetBusVolume(float volume, const FAudio::BusId & id);
	/// set master volume
	float GetBusVolume(const FAudio::BusId & id);
	/// set vca volume
	void SetVCAVolume(float volume, const Util::String & id);
	/// get vca volume
	float GetVCAVolume(const Util::String & id);
	/// stop/resume all sounds 
	void SetBusPaused(bool paused, const FAudio::BusId & id);


    	
	/// get bank object
	FMOD::Studio::Bank * GetBank(const Util::String & bankFile);
	/// get the fmod-system-object
    FMOD::Studio::System* GetSystem();
	
	/// returns if there is a sound device present
	bool HasSoundDevice();
    
private:
    
    /// root object of fmod studio api
    FMOD::Studio::System *system;
	/// lowlevel api object
	FMOD::System * lowlevelSystem;
    /// root-category master
    FMOD::ChannelGroup * masterGroup;
    /// the 3D-listener
    Ptr<AudioListener> listener;
	/// handler
	Ptr<AudioInterface> audiointerface;
    /// dictionary of used events, maybe a hashmap might be better, hashing the pointer
	Util::Dictionary<Ptr<EventInstance>, Ptr<EventInstance> > events;
	/// dictionary of loaded banks
	Util::Dictionary<Util::String, FMOD::Studio::Bank*> banks;
    /// TODO, client must set this value on startup
    static const int MAX_CHANNELS; 
	
	bool hasSoundDevice;
	bool auditioningEnabled;

};

//------------------------------------------------------------------------------
/**
*/
inline 
bool
AudioDevice::IsOpen() const
{
	return (NULL != this->system);
}

//------------------------------------------------------------------------------
/**
*/
inline 
FMOD::Studio::System* 
AudioDevice::GetSystem()
{
    return this->system;
}

//------------------------------------------------------------------------------
/**
*/
inline
FMOD::Studio::Bank * 
AudioDevice::GetBank(const Util::String & bankFile)
{
	if (!this->hasSoundDevice) return 0;
	
	n_assert(this->banks.Contains(bankFile));
	return this->banks[bankFile];	
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
AudioDevice::BankLoaded(const Util::String & bankFile)
{
	return this->banks.Contains(bankFile);
}


//------------------------------------------------------------------------------
/**
*/
inline
bool 
AudioDevice::HasSoundDevice()
{
	return this->hasSoundDevice;
}


} // namespace FAudio
//------------------------------------------------------------------------------

#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::FmodAudioHandler
    
    Message handler for the Audio subsystem. Runs in the Audio thread.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/

#include "interface/interfacehandlerbase.h"
#include "messaging/message.h"
#include "audio2protocol.h"
#include "io/console.h"
#include "io/ioserver.h"
#include "http/httpserverproxy.h"
#include "timing/timer.h"

namespace Audio2 { class FmodEventWrapper; }

//------------------------------------------------------------------------------
namespace FmodCoreAudio
{
class FmodAudioDevice; 
class FmodAudioHandler : public Interface::InterfaceHandlerBase
{
    __DeclareClass(FmodAudioHandler);
public:
    /// constructor
    FmodAudioHandler();
    /// destructor
    virtual ~FmodAudioHandler();

    /// open the handler
    virtual void Open();
    /// close the handler
    virtual void Close();
    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);
    /// do per-frame work (trigger the audio device)
    virtual void DoWork();

private:
    /// setup the audio runtime
    void SetupAudioRuntime();
    /// shutdown the audio runtime
    void ShutdownAudioRuntime();
    /// process SetupAudio message
    void OnSetupAudio(const Ptr<Audio2::SetupAudio>& msg);
    /// process LoadEventProject message
    void OnLoadEventProject(const Ptr<Audio2::LoadEventProject>& msg);
    /// process UnloadEventProject message
    void OnUnloadEventProject(const Ptr<Audio2::UnloadEventProject>& msg);
    /// process DiscardAllEvents message
    void OnDiscardAllEvents(const Ptr<Audio2::DiscardAllEvents>& msg);
    /// process UpdateListener message
    void OnUpdateListener(const Ptr<Audio2::UpdateListener>& msg);
    /// process EventPlayFireAndForget message
    void OnEventPlayFireAndForget(const Ptr<Audio2::EventPlayFireAndForget>& msg);
    /// process EventPlayFireAndForget3D message
    void OnEventPlayFireAndForget3D(const Ptr<Audio2::EventPlayFireAndForget3D>& msg);
    /// process EventCategorySetVolume message
    void OnEventCategorySetVolume(const Ptr<Audio2::EventCategorySetVolume> &msg);
    /// process EventCategorySetPaused message
    void OnEventCategorySetPaused(const Ptr<Audio2::EventCategorySetPaused> &msg);
    /// process EventCategorySetPitch message
    void OnEventCategorySetPitch(const Ptr<Audio2::EventCategorySetPitch> &msg);
    /// process CreateEvent message
    void OnEventCreate(const Ptr<Audio2::EventCreate>& msg);
    /// process DiscardEvent message
    void OnEventDiscard(const Ptr<Audio2::EventDiscard>& msg);
    /// process EventPlay message
    void OnEventPlay(const Ptr<Audio2::EventPlay>& msg);
	/// process EventPlayProgrammerSound message
    void OnEventPlayProgrammerSound(const Ptr<Audio2::EventPlayProgrammerSound>& msg);
    /// process EventStop message
    void OnEventStop(const Ptr<Audio2::EventStop>& msg);
    /// process EventPause message
    void OnEventPause(const Ptr<Audio2::EventPause>& msg);
    /// process EventResume message
    void OnEventResume(const Ptr<Audio2::EventResume>& msg);
    /// process EventUpdate3D message
    void OnEventUpdate3D(const Ptr<Audio2::EventUpdate3D>& msg);
    /// process EventSetParameter message
    void OnEventSetParameter(const Ptr<Audio2::EventSetParameter>& msg);
    /// process EventSetParameter message
    void OnEventSetVolume(const Ptr<Audio2::EventSetVolume>& msg);
    /// process EventUpdateState message
    void OnEventUpdateState(const Ptr<Audio2::EventUpdateState>& msg);
	/// process EventSetAuditioningEnabled message
    void OnSetAuditioningEnabled(const Ptr<Audio2::SetAuditioningEnabled>& msg);
    /// process MusicSetVolume message
    void OnMusicSetVolume(const Ptr<Audio2::MusicSetVolume>& msg);
	
	/// process PlayCue message
    void OnPlayCue(const Ptr<Audio2::PlayCue>& msg);
    /// process StopCue message
    void OnStopCue(const Ptr<Audio2::StopCue>& msg);
	
    bool isAudioRuntimeValid;
    Ptr<IO::IoServer> ioServer;
    Ptr<FmodCoreAudio::FmodAudioDevice> audioDevice;
#if __NEBULA3_HTTP__
    Ptr<Http::HttpServerProxy> httpServerProxy;
#endif
};

} // namespace FmodCoreAudio
//------------------------------------------------------------------------------


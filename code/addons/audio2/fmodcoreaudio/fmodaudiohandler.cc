//------------------------------------------------------------------------------
//  fmodaudiohandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audio2/fmodcoreaudio/fmodaudiohandler.h"
#include "audio2/fmodcoreaudio/fmodcoreaudiopagehandler.h"
#include "audio2/fmodcoreaudio/fmodaudiodevice.h"
#include "audio2/fmodcoreaudio/fmoderror.h"
#include "audio2/fmodcoreaudio/fmodeventwrapper.h"
#include "audio2/fmodeventstate.h"
#include "audio2/fmodpitchunit.h"
#if !__NEBULA_NO_ASSERT__
#include "threading/thread.h"
#endif
#if __NEBULA3_HTTP__
#include "http/httpinterface.h"
#endif

#include <fmod_event.hpp>

__ImplementClass(FmodCoreAudio::FmodAudioHandler, 'AUDH', Interface::InterfaceHandlerBase);

using namespace Audio2;
using namespace Util;
using namespace FmodCoreAudio;
using namespace Interface;
using namespace Timing;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
FmodAudioHandler::FmodAudioHandler() :
    isAudioRuntimeValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FmodAudioHandler::~FmodAudioHandler()
{
    // empty
}            

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioHandler::Open()
{
    n_assert(!this->IsOpen());
    n_assert(!this->isAudioRuntimeValid);
    InterfaceHandlerBase::Open();

    // setup core runtime
    this->ioServer = IO::IoServer::Create();

#if __NEBULA3_HTTP__
    // setup http page handlers
    if(Http::HttpInterface::HasInstance())
    {
        this->httpServerProxy = Http::HttpServerProxy::Create();
        this->httpServerProxy->Open();
        this->httpServerProxy->AttachRequestHandler(Debug::FmodCoreAudioPageHandler::Create());
    }
#endif

    // create the audio device, but don't open it yet
    this->audioDevice = FmodAudioDevice::Create();
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioHandler::Close()
{
    n_assert(this->IsOpen());

    if (this->isAudioRuntimeValid)
    {
        this->ShutdownAudioRuntime();
    }
    this->audioDevice = 0;
#if __NEBULA3_HTTP__
    if(this->httpServerProxy.isvalid())
    {
        this->httpServerProxy->Close();
        this->httpServerProxy = 0;
    }
#endif
    this->ioServer = 0;

    InterfaceHandlerBase::Close();
}

//------------------------------------------------------------------------------
/**
    Per-frame method. This is called by the when a new batch of messages
    is received, or at most after every 10th second.
*/
void
FmodAudioHandler::DoWork()
{
    if (this->isAudioRuntimeValid)
    {
        this->audioDevice->OnFrame();
    }

#if __NEBULA3_HTTP__
    // allow audio-thread HttpRequests to be processed
    if(this->httpServerProxy.isvalid())
    {
        this->httpServerProxy->HandlePendingRequests();
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Setup the audio runtime, this method is called when the
    SetupAudio message is received from the main thread. 
*/
void
FmodAudioHandler::SetupAudioRuntime()
{
    n_assert(!this->isAudioRuntimeValid);
    n_assert(!this->audioDevice->IsOpen());

    this->audioDevice->Open();
    n_assert(this->audioDevice->IsOpen());

    this->isAudioRuntimeValid = true;
}

//------------------------------------------------------------------------------
/**
    Shutdown the audio runtime.
*/
void
FmodAudioHandler::ShutdownAudioRuntime()
{
    n_assert(this->isAudioRuntimeValid);
    n_assert(this->audioDevice->IsOpen());
    this->audioDevice->Close();
    this->isAudioRuntimeValid = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
FmodAudioHandler::HandleMessage(const Ptr<Message>& msg)
{
    n_assert(msg.isvalid());
    if (msg->CheckId(SetupAudio::Id))
    {
        // the only msgm that can be handled, when FmodAudioDevice is
        // still closed
        n_assert(!FmodAudioDevice::Instance()->IsOpen());
        this->OnSetupAudio(msg.downcast<SetupAudio>());
    }
    else 
    {
        n_assert(FmodAudioDevice::Instance()->IsOpen());
        n_assert(FmodAudioDevice::GetCoreAudioThreadId() == Threading::Thread::GetMyThreadId());
        if (msg->CheckId(LoadEventProject::Id))
        {
            this->OnLoadEventProject(msg.downcast<LoadEventProject>());
        }
        else if (msg->CheckId(UnloadEventProject::Id))
        {
            this->OnUnloadEventProject(msg.downcast<UnloadEventProject>());
        }
        else if (msg->CheckId(UpdateListener::Id))
        {
            this->OnUpdateListener(msg.downcast<UpdateListener>());
        }
        else if (msg->CheckId(DiscardAllEvents::Id))
        {
            this->OnDiscardAllEvents(msg.downcast<DiscardAllEvents>());
        }
        else if (msg->CheckId(EventPlayFireAndForget::Id))
        {
            this->OnEventPlayFireAndForget(msg.downcast<EventPlayFireAndForget>());
        }
        else if (msg->CheckId(EventPlayFireAndForget3D::Id))
        {
            this->OnEventPlayFireAndForget3D(msg.downcast<EventPlayFireAndForget3D>());
        }
        else if (msg->CheckId(EventCreate::Id))
        {
            this->OnEventCreate(msg.downcast<EventCreate>());
        }
        else if (msg->CheckId(EventDiscard::Id))
        {
            this->OnEventDiscard(msg.downcast<EventDiscard>());
        }
        else if (msg->CheckId(EventUpdateState::Id))
        {
            this->OnEventUpdateState(msg.downcast<EventUpdateState>());
        }
        else if (msg->CheckId(EventPlay::Id))
        {
            this->OnEventPlay(msg.downcast<EventPlay>());
        }
		else if (msg->CheckId(EventPlayProgrammerSound::Id))
        {
            this->OnEventPlayProgrammerSound(msg.downcast<EventPlayProgrammerSound>());
        }
        else if (msg->CheckId(EventStop::Id))
        {
            this->OnEventStop(msg.downcast<EventStop>());
        }
        else if (msg->CheckId(EventPause::Id))
        {
            this->OnEventPause(msg.downcast<EventPause>());
        }
        else if (msg->CheckId(EventResume::Id))
        {
            this->OnEventResume(msg.downcast<EventResume>());
        }
        else if (msg->CheckId(EventUpdate3D::Id))
        {
            this->OnEventUpdate3D(msg.downcast<EventUpdate3D>());
        }
        else if (msg->CheckId(EventSetParameter::Id))
        {
            this->OnEventSetParameter(msg.downcast<EventSetParameter>());
        }
        else if (msg->CheckId(EventSetVolume::Id))
        {
            this->OnEventSetVolume(msg.downcast<EventSetVolume>());
        }
        else if (msg->CheckId(EventCategorySetVolume::Id))
        {
            this->OnEventCategorySetVolume(msg.downcast<EventCategorySetVolume>());
        }
        else if (msg->CheckId(EventCategorySetPaused::Id))
        {
            this->OnEventCategorySetPaused(msg.downcast<EventCategorySetPaused>());
        }
        else if (msg->CheckId(EventCategorySetPitch::Id))
        {
            this->OnEventCategorySetPitch(msg.downcast<EventCategorySetPitch>());
        }
		else if(msg->CheckId(SetAuditioningEnabled::Id))
		{
            this->OnSetAuditioningEnabled(msg.downcast<SetAuditioningEnabled>());
		}
		else if(msg->CheckId(PlayCue::Id))
		{
            this->OnPlayCue(msg.downcast<PlayCue>());
		}
		else if(msg->CheckId(StopCue::Id))
		{
            this->OnStopCue(msg.downcast<StopCue>());
		}
        else if(msg->CheckId(MusicSetVolume::Id))
        {
            this->OnMusicSetVolume(msg.downcast<MusicSetVolume>());
        }
        else
        {
            // unknown message
            return false;
        }
    }
    // fallthrough: message was handled
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioHandler::OnSetupAudio(const Ptr<SetupAudio>& msg)
{
    n_assert(!this->isAudioRuntimeValid);
    this->SetupAudioRuntime();
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioHandler::OnLoadEventProject(const Ptr<Audio2::LoadEventProject>& msg)
{
    this->audioDevice->LoadEventProject(msg->GetProjectId());
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioHandler::OnUnloadEventProject(const Ptr<Audio2::UnloadEventProject>& msg)
{
    this->audioDevice->UnloadEventProject(msg->GetProjectId());
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioHandler::OnUpdateListener(const Ptr<Audio2::UpdateListener>& msg)
{
    this->audioDevice->SetListener(msg->GetTransform(), msg->GetVelocity());
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventPlayFireAndForget(const Ptr<Audio2::EventPlayFireAndForget>& msg)
{
    this->audioDevice->EventPlayFireAndForget(msg->GetEventId(), msg->GetVolume());
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventPlayFireAndForget3D(const Ptr<Audio2::EventPlayFireAndForget3D>& msg)
{
    this->audioDevice->EventPlayFireAndForget3D(msg->GetEventId(), msg->GetTransform(), msg->GetVelocity(), msg->GetVolume());
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventCategorySetVolume(const Ptr<Audio2::EventCategorySetVolume>& msg)
{
    FMOD::EventCategory *cat = this->audioDevice->GetCategory(msg->GetCategoryId());
    FMOD_RESULT result = cat->setVolume(msg->GetVolume());
    FMOD_CHECK_ERROR(result);
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventCategorySetPaused(const Ptr<Audio2::EventCategorySetPaused>& msg)
{
    FMOD::EventCategory *cat = this->audioDevice->GetCategory(msg->GetCategoryId());
    FMOD_RESULT result = cat->setPaused(msg->GetPaused());
    FMOD_CHECK_ERROR(result);
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventCategorySetPitch(const Ptr<Audio2::EventCategorySetPitch> &msg)
{
    FMOD::EventCategory *cat = this->audioDevice->GetCategory(msg->GetCategoryId());
    Audio2::FmodPitchUnit unit = msg->GetPitchUnit();
    n_assert(unit > Audio2::PitchUnitUnknown);
    n_assert(unit < Audio2::PitchUnitCount);
    FMOD_EVENT_PITCHUNITS fmodUnit;
    switch(unit)
    {
        case PitchUnitRaw:
            fmodUnit = FMOD_EVENT_PITCHUNITS_RAW;
            break;
        case PitchUnitOctaves:
            fmodUnit = FMOD_EVENT_PITCHUNITS_OCTAVES;
            break;
        case PitchUnitSemitones:
            fmodUnit = FMOD_EVENT_PITCHUNITS_SEMITONES;
            break;
        case PitchUnitTones:
            fmodUnit = FMOD_EVENT_PITCHUNITS_TONES;
            break;
        default:
            fmodUnit = FMOD_EVENT_PITCHUNITS_RAW;
            break;
    }
    FMOD_RESULT result = cat->setPitch(msg->GetPitch(), fmodUnit);
    FMOD_CHECK_ERROR(result);
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventCreate(const Ptr<Audio2::EventCreate>& msg)
{
    Ptr<FmodEventWrapper> wrapper = this->audioDevice->CreateEvent(msg->GetEventId());
    msg->SetEventHandle(wrapper.upcast<Core::RefCounted>());
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventDiscard(const Ptr<Audio2::EventDiscard>& msg)
{
    const Audio2::FmodEventHandle handle = msg->GetEventHandle();
    n_assert(handle.isvalid());
    const Ptr<FmodEventWrapper> &wrapper = handle.downcast<FmodEventWrapper>();
    this->audioDevice->DiscardEvent(wrapper);
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnDiscardAllEvents(const Ptr<Audio2::DiscardAllEvents>& msg)
{
    this->audioDevice->DiscardAllEvents();
}
//------------------------------------------------------------------------------
/**
*/
void
FmodAudioHandler::OnEventPlay(const Ptr<Audio2::EventPlay>& msg)
{
    const Audio2::FmodEventHandle handle = msg->GetEventHandle();
    n_assert(handle.isvalid());
    const Ptr<FmodEventWrapper> &wrapper = handle.downcast<FmodEventWrapper>();
    wrapper->Play();
}

//------------------------------------------------------------------------------
/**
*/
void
FmodAudioHandler::OnEventPlayProgrammerSound(const Ptr<Audio2::EventPlayProgrammerSound>& msg)
{
    const Audio2::FmodEventHandle handle = msg->GetEventHandle();
    n_assert(handle.isvalid());
    const Ptr<FmodEventWrapper> &wrapper = handle.downcast<FmodEventWrapper>();
	wrapper->PlayProgrammerSound(msg->GetName());
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventStop(const Ptr<Audio2::EventStop>& msg)
{
    const Audio2::FmodEventHandle handle = msg->GetEventHandle();
    n_assert(handle.isvalid());
    const Ptr<FmodEventWrapper> &wrapper = handle.downcast<FmodEventWrapper>();
    wrapper->Stop();
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventPause(const Ptr<Audio2::EventPause>& msg)
{
    const Audio2::FmodEventHandle handle = msg->GetEventHandle();
    n_assert(handle.isvalid());
    const Ptr<FmodEventWrapper> &wrapper = handle.downcast<FmodEventWrapper>();
    wrapper->Pause();
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventResume(const Ptr<Audio2::EventResume>& msg)
{
    const Audio2::FmodEventHandle handle = msg->GetEventHandle();
    n_assert(handle.isvalid());
    const Ptr<FmodEventWrapper> &wrapper = handle.downcast<FmodEventWrapper>();
    wrapper->Resume();
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventSetVolume(const Ptr<Audio2::EventSetVolume>& msg)
{
    const Audio2::FmodEventHandle handle = msg->GetEventHandle();
    n_assert(handle.isvalid());
    const Ptr<FmodEventWrapper> &wrapper = handle.downcast<FmodEventWrapper>();
    wrapper->SetVolume(msg->GetVolume());
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventSetParameter(const Ptr<Audio2::EventSetParameter>& msg)
{
    const Audio2::FmodEventHandle handle = msg->GetEventHandle();
    n_assert(handle.isvalid());
    const Ptr<FmodEventWrapper> &wrapper = handle.downcast<FmodEventWrapper>();
    wrapper->SetParameter(msg->GetParameterId(), msg->GetValue());
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventUpdate3D(const Ptr<Audio2::EventUpdate3D>& msg)
{
    const Audio2::FmodEventHandle handle = msg->GetEventHandle();
    n_assert(handle.isvalid());
    const Ptr<FmodEventWrapper> &wrapper = handle.downcast<FmodEventWrapper>();
    const Math::vector *orientation;
    if(msg->GetHasOrientation())
    {
        orientation = &msg->GetOrientation();
    }
    else
    {
        orientation = NULL;
    }
    wrapper->Set3DAttributes(msg->GetPosition(), msg->GetVelocity(), orientation);
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnEventUpdateState(const Ptr<Audio2::EventUpdateState>& msg)
{
    const Audio2::FmodEventHandle handle = msg->GetEventHandle();
    n_assert(handle.isvalid());
    const Ptr<FmodEventWrapper> &wrapper = handle.downcast<FmodEventWrapper>();
    msg->SetState(wrapper->GetState());
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnSetAuditioningEnabled(const Ptr<Audio2::SetAuditioningEnabled>& msg)
{
    this->audioDevice->SetAuditioningEnabled(msg->GetEnabled());
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnPlayCue(const Ptr<Audio2::PlayCue>& msg)
{
    this->audioDevice->MusicPlayCue(msg->GetCueId());
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnStopCue(const Ptr<Audio2::StopCue>& msg)
{
    this->audioDevice->MusicStopCue(msg->GetCueId());
}


//------------------------------------------------------------------------------
/**
*/
void 
FmodAudioHandler::OnMusicSetVolume(const Ptr<Audio2::MusicSetVolume>& msg)
{
    this->audioDevice->GetMusicSystem()->setVolume(msg->GetVolume());
}

//------------------------------------------------------------------------------
//  audio2server.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audio2/audio2server.h"
#include "audio2protocol.h"
#include "audio2/audio2interface.h"
#include "audio2/audio2listener.h"
#include "audio2/audio2emitter.h"
#include "audio2/fmodeventcategoryid.h"

using namespace Audio2;

__ImplementClass(Audio2::Audio2Server, 'AUSV', Core::RefCounted);
__ImplementSingleton(Audio2::Audio2Server);

using namespace Util;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
Audio2Server::Audio2Server() :
    isOpen(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
Audio2Server::~Audio2Server()
{
    n_assert(!this->IsOpen());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Server::Open()
{
    n_assert(!this->IsOpen());

    // setup the CoreAudio device
    Ptr<Audio2::SetupAudio> msg = Audio2::SetupAudio::Create();
    Audio2Interface::Instance()->SendWait(msg.cast<Message>());
    n_assert(msg->Handled());

    // create an 3d listener singleton
    this->audioListener = Audio2Listener::Create();

    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Server::Close()
{
    n_assert(this->IsOpen());
    this->audioListener = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Server::OnFrame()
{
    n_assert(this->IsOpen());
    
    // update the audio listener
    this->audioListener->OnFrame();

    // need to update audio emitters
    // FIXME: find a way to not iterate through audio emitters, 
    // instead only update them if necessary?
    IndexT i;
    for (i = 0; i < this->audioEmitters.Size(); i++)
    {
        this->audioEmitters[i]->OnFrame();
    }

    // send off batched audio messages to the audio thread
    Audio2Interface::Instance()->FlushBatchedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Server::RegisterAudioEmitter(const Ptr<Audio2Emitter>& audioEmitter)
{
    n_assert(InvalidIndex == this->audioEmitters.BinarySearchIndex(audioEmitter));
    this->audioEmitters.InsertSorted(audioEmitter);
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Server::UnregisterAudioEmitter(const Ptr<Audio2Emitter>& audioEmitter)
{
    IndexT index = this->audioEmitters.BinarySearchIndex(audioEmitter);
    n_assert(InvalidIndex != index);
    this->audioEmitters.EraseIndex(index);
}

//------------------------------------------------------------------------------
/**
*/
void
Audio2Server::LoadEventProject(const Audio2::FmodEventProjectId &projectId)
{
    n_assert(this->IsOpen());
    n_assert(projectId.IsValid());
    Ptr<Audio2::LoadEventProject> msg = Audio2::LoadEventProject::Create();
    msg->SetProjectId(projectId);
    Audio2Interface::Instance()->Send(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
Audio2Server::UnloadEventProject(const Audio2::FmodEventProjectId &projectId)
{
    n_assert(this->IsOpen());
    n_assert(projectId.IsValid());
    Ptr<Audio2::UnloadEventProject> msg = Audio2::UnloadEventProject::Create();
    msg->SetProjectId(projectId);
    Audio2Interface::Instance()->Send(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
Audio2Server::EventCategorySetVolume(const FmodEventCategoryId &categoryId, float volume)
{
    n_assert(categoryId.IsValid());
    n_assert(volume >= 0.0f);
    n_assert(volume <= 1.0f);
    Ptr<Audio2::EventCategorySetVolume> msg = Audio2::EventCategorySetVolume::Create();
    msg->SetCategoryId(categoryId);
    msg->SetVolume(volume);
    Audio2Interface::Instance()->Send(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
Audio2Server::EventCategorySetPaused(const FmodEventCategoryId &categoryId, bool paused)
{
    n_assert(categoryId.IsValid());
    Ptr<Audio2::EventCategorySetPaused> msg = Audio2::EventCategorySetPaused::Create();
    msg->SetCategoryId(categoryId);
    msg->SetPaused(paused);
    Audio2Interface::Instance()->Send(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
Audio2Server::EventCategorySetPitch(const FmodEventCategoryId &categoryId, float pitch, Audio2::FmodPitchUnit unit)
{
    n_assert(categoryId.IsValid());
    Ptr<Audio2::EventCategorySetPitch> msg = Audio2::EventCategorySetPitch::Create();
    msg->SetCategoryId(categoryId);
    msg->SetPitch(pitch);
    msg->SetPitchUnit(unit);
    Audio2Interface::Instance()->Send(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
Audio2Server::MasterCategorySetVolume(float volume)
{
    n_assert(volume >= 0.0f);
    n_assert(volume <= 1.0f);
    static const FmodEventCategoryId categoryId("master");
    Ptr<Audio2::EventCategorySetVolume> msg = Audio2::EventCategorySetVolume::Create();
    msg->SetCategoryId(categoryId);
    msg->SetVolume(volume);
    Audio2Interface::Instance()->Send(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
Audio2Server::MasterCategorySetPaused(bool paused)
{
    static const FmodEventCategoryId categoryId("master");
    Ptr<Audio2::EventCategorySetPaused> msg = Audio2::EventCategorySetPaused::Create();
    msg->SetCategoryId(categoryId);
    msg->SetPaused(paused);
    Audio2Interface::Instance()->Send(msg);
}

//------------------------------------------------------------------------------
/**
*/
void Audio2Server::MasterCategorySetPitch(float pitch, Audio2::FmodPitchUnit unit)
{
    static const FmodEventCategoryId categoryId("master");
    Ptr<Audio2::EventCategorySetPitch> msg = Audio2::EventCategorySetPitch::Create();
    msg->SetCategoryId(categoryId);
    msg->SetPitch(pitch);
    msg->SetPitchUnit(unit);
    Audio2Interface::Instance()->Send(msg);
}

//------------------------------------------------------------------------------
/**
*/
void Audio2Server::SetAuditioningEnabled(bool enabled)
{
    Ptr<Audio2::SetAuditioningEnabled> msg = Audio2::SetAuditioningEnabled::Create();
    msg->SetEnabled(enabled);
    Audio2Interface::Instance()->Send(msg);
}


//------------------------------------------------------------------------------
/**
*/
void Audio2Server::MusicSetVolume(float volume)
{
    Ptr<Audio2::MusicSetVolume> msg = Audio2::MusicSetVolume::Create();    
    msg->SetVolume(volume);
    Audio2Interface::Instance()->Send(msg);
}
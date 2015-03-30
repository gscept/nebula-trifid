#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::Audio2Server

    Client-side front-end of the Audio subsystem. Initializes the audio 
    subsystem and manages sound banks.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "audio2/fmodpitchunit.h"

namespace Audio2 { class Audio2Emitter; 
                   class Audio2Listener; 
                   class FmodEventProjectId; 
                   class FmodEventCategoryId; }

//------------------------------------------------------------------------------
namespace Audio2
{
class Audio2Server : public Core::RefCounted
{
    __DeclareClass(Audio2Server);
    __DeclareSingleton(Audio2Server);
public:
    /// constructor
    Audio2Server();
    /// destructor
    virtual ~Audio2Server();   

    /// open the audio subsystem (waits for completion)
    void Open();
    /// close the audio subsystem (waits for completion)
    void Close();
    /// return true if the audio subsystem is open
    bool IsOpen() const;
    /// called per-frame 
    void OnFrame();

    /// load a "*.fev" eventproject-file
    void LoadEventProject(const Audio2::FmodEventProjectId &projectId);
    /// unload an eventproject
    void UnloadEventProject(const Audio2::FmodEventProjectId &projectId);

    /// event-categorie interface
    /// set category volume
    void MasterCategorySetVolume(float volume);
    /// stop/resume all events in category
    void MasterCategorySetPaused(bool paused);
    /// set pitch for master categorie
    void MasterCategorySetPitch(float pitch, Audio2::FmodPitchUnit unit);
    /// set category volume
    void EventCategorySetVolume(const FmodEventCategoryId &categoryId, float volume);
    /// stop/resume all events in category
    void EventCategorySetPaused(const FmodEventCategoryId &categoryId, bool paused);
    /// set pitch for all events in category
    void EventCategorySetPitch(const FmodEventCategoryId &categoryId, float pitch, Audio2::FmodPitchUnit unit);
    /// set volume for ambient music
    void MusicSetVolume(float volume);

    /// enable/disable fmod auditioning feature
    void SetAuditioningEnabled(bool enabled);

private:
    friend class Audio2Emitter;

    /// register an AudioEmitter (called from AudioEmitter::Setup)
    void RegisterAudioEmitter(const Ptr<Audio2Emitter>& audioEmitter);
    /// unregister an AudioEmitter (called from AudioEmitter::Discard)
    void UnregisterAudioEmitter(const Ptr<Audio2Emitter>& audioEmitter);

    bool isOpen;
    IndexT adapterIndex;
    Util::Array<Ptr<Audio2Emitter> > audioEmitters;
    Ptr<Audio2Listener> audioListener;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
Audio2Server::IsOpen() const
{
    return this->isOpen;
}

} // namespace Audio2
//------------------------------------------------------------------------------
    
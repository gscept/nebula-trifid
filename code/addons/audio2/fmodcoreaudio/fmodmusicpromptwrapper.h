#pragma once
//------------------------------------------------------------------------------
/**
    @class FmodCoreAudio::FmodMusicPromptWrapper
    
    Wrapper class for FMOD::MusicPrompt

    (C) 2010 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/

#include "core/refcounted.h"
#include <fmod_event.hpp>

//------------------------------------------------------------------------------
namespace FmodCoreAudio
{

class FmodMusicPromptWrapper : public Core::RefCounted
{
    __DeclareClass(FmodMusicPromptWrapper);
public:
    /// default constructor
    FmodMusicPromptWrapper();
    /// destructor
    virtual ~FmodMusicPromptWrapper();
	/// setup the wrapper
    void Setup(const int cueId);
    /// discard the wrapper
    void Discard();
    /// begins the music prompt
    void Begin();
	/// ends the music prompt
    void End();
	/// retrieves the active state of the music prompt 
    bool IsActive() const;

private:
    FMOD::MusicPrompt* musicPrompt;
};

} // namespace FmodCoreAudio
//------------------------------------------------------------------------------
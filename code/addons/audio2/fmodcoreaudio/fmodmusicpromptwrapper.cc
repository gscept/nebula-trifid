//------------------------------------------------------------------------------
//  fmodmusicpromptwrapper.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fmodmusicpromptwrapper.h"
#include "audio2/fmodcoreaudio/fmodaudiodevice.h"
#include "audio2/fmodcoreaudio/fmoderror.h"

namespace FmodCoreAudio
{

__ImplementClass(FmodCoreAudio::FmodMusicPromptWrapper, 'FCME', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
FmodMusicPromptWrapper::FmodMusicPromptWrapper() : musicPrompt(NULL)
{
    ASSERT_COREAUDIO_THREAD;
}

//------------------------------------------------------------------------------
/**
*/
FmodMusicPromptWrapper::~FmodMusicPromptWrapper()
{
	// musicprompt object must have been discarded
    n_assert(this->musicPrompt == NULL);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodMusicPromptWrapper::Setup(const int cueId)
{
    ASSERT_COREAUDIO_THREAD;
    FMOD_RESULT result = FmodAudioDevice::Instance()->GetMusicSystem()->prepareCue(cueId, &this->musicPrompt);
	FMOD_CHECK_ERROR(result);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodMusicPromptWrapper::Discard()
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(this->musicPrompt != NULL);
    FMOD_RESULT result = this->musicPrompt->release();
	FMOD_CHECK_ERROR(result);
    this->musicPrompt = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
FmodMusicPromptWrapper::Begin()
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(this->musicPrompt != NULL);
	FMOD_RESULT result = this->musicPrompt->begin();
	FMOD_CHECK_ERROR(result);
}

//------------------------------------------------------------------------------
/**
*/
void
FmodMusicPromptWrapper::End()
{
    ASSERT_COREAUDIO_THREAD;
    n_assert(this->musicPrompt != NULL);
	FMOD_RESULT result = this->musicPrompt->end();
	FMOD_CHECK_ERROR(result);
}

//------------------------------------------------------------------------------
/**
*/
bool
FmodMusicPromptWrapper::IsActive() const
{
    ASSERT_COREAUDIO_THREAD;
	n_assert(this->musicPrompt != NULL);
    bool active;
    FMOD_RESULT result = this->musicPrompt->isActive(&active);
    FMOD_CHECK_ERROR(result);
	return active;
}

} // namespace FmodCoreAudio
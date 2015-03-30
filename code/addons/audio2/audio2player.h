#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::Audio2Player
    
    Client-side audio player for fire-and-forget sounds. FoF-sounds don't have
    a client-side representation after they are fired, so the application
    cannot manipulate a sound once it is playing.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"

namespace Math
{
    class matrix44;
    class vector;
};

//------------------------------------------------------------------------------
namespace Audio2
{
class FmodEventId;
class Audio2Player : public Core::RefCounted
{
    __DeclareClass(Audio2::Audio2Player);
    __DeclareSingleton(Audio2::Audio2Player);
public:
    /// constructor
    Audio2Player();
    /// destructor
    virtual ~Audio2Player();
    
    /// play a fire and forget event
    void PlayEvent(const Audio2::FmodEventId& eventId, float volume = 1.0f);
    /// play a 3D fire and forget event
    void PlayEvent(const Audio2::FmodEventId& eventId, const Math::matrix44& transform, const Math::vector& velocity, float volume = 1.0f);

    /// trigger the lifespan of a cue 
	void PlayCue(const int cueId);
    /// ends the lifespan of a cue
	void StopCue(const int cueId);
};

} // namespace Audio2
//------------------------------------------------------------------------------
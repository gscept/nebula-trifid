//------------------------------------------------------------------------------
//  fmodlistener.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audio2/fmodcoreaudio/fmodlistener.h"
#include "audio2/fmodcoreaudio/fmodaudiodevice.h"
#include "audio2/fmodcoreaudio/fmoderror.h"

#include <fmod_event.hpp>

using namespace FmodCoreAudio;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
FmodListener::FmodListener() :
    isDirty(true),
    transform(matrix44::identity()),
    velocity(vector::nullvec())
{
}

//------------------------------------------------------------------------------
/**
*/
FmodListener::~FmodListener()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodListener::Set(const Math::matrix44& transform, const Math::vector& velocity)
{
    this->isDirty = true;
    this->transform = transform;
    this->velocity = velocity;
}

//------------------------------------------------------------------------------
/**
*/
void FmodListener::OnFrame()
{
    if(!this->isDirty) return;

    FMOD::EventSystem *eventSystem = FmodAudioDevice::Instance()->GetEventSystem();

    static const matrix44 rot180Y = matrix44::rotationy(N_PI);
    /// nebulas object are rotated 180 degrees around the y-axis
    const matrix44 nebulaTransform = matrix44::multiply(rot180Y, this->transform);

    FMOD_VECTOR pos, vel, forward, up;
    pos.x = nebulaTransform.get_position().x();
    pos.y = nebulaTransform.get_position().y();
    pos.z = nebulaTransform.get_position().z();
    vel.x = velocity.x();
    vel.y = velocity.y();
    vel.z = velocity.z();
    forward.x = nebulaTransform.get_zaxis().x();
    forward.y = nebulaTransform.get_zaxis().y();
    forward.z = nebulaTransform.get_zaxis().z();
    up.x = nebulaTransform.get_yaxis().x();
    up.y = nebulaTransform.get_yaxis().y();
    up.z = nebulaTransform.get_yaxis().z();

	if (FmodAudioDevice::Instance()->HasSoundDevice())
	{
		FMOD_RESULT result = eventSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
		FMOD_CHECK_ERROR(result);
	}
    

    this->isDirty = false;
}

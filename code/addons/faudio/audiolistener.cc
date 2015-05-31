//------------------------------------------------------------------------------
//  FAudiolistener.cc
//  (C) 2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudio/audiolistener.h"
#include "faudio/audiointerface.h"
#include "faudio/audiodevice.h"
#include "faudio/fmoderror.h"
#include <fmod.hpp>
#include <fmod_studio.hpp>


__ImplementClass(FAudio::AudioListener, 'AUDL', Core::RefCounted);
__ImplementSingleton(FAudio::AudioListener);

using namespace Math;
using namespace Messaging;
using namespace FAudio;

//------------------------------------------------------------------------------
/**
*/
AudioListener::AudioListener() :
    transform(matrix44::identity()),
    velocity(vector::nullvec()),
    dirty(true)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
AudioListener::~AudioListener()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioListener::OnFrame()
{
	if (!this->dirty) return;

	FMOD::Studio::System *system = AudioDevice::Instance()->GetSystem();

		
	static const matrix44 rot180Y = matrix44::rotationy(N_PI);
	/// nebulas object are rotated 180 degrees around the y-axis
	const matrix44 nebulaTransform = matrix44::multiply(rot180Y, this->transform);

	//matrix44 nebulaTransform = this->transform;

	vector forward = float4::normalize(nebulaTransform.get_zaxis());
	vector up = float4::normalize(nebulaTransform.get_yaxis());

	FMOD_3D_ATTRIBUTES newattrs;
	
	newattrs.position.x = nebulaTransform.get_position().x();
	newattrs.position.y = nebulaTransform.get_position().y();
	newattrs.position.z = nebulaTransform.get_position().z();
	newattrs.velocity.x = velocity.x();
	newattrs.velocity.y = velocity.y();
	newattrs.velocity.z = velocity.z();
	newattrs.forward.x = forward.x();
	newattrs.forward.y = forward.y();
	newattrs.forward.z = forward.z();
	newattrs.up.x = up.x();
	newattrs.up.y = up.y();
	newattrs.up.z = up.z();

	if (AudioDevice::Instance()->HasSoundDevice())
	{
#if (FMOD_VERSION >= 0x00010600)
		FMOD_RESULT result = system->setListenerAttributes(0, &newattrs);
#else
		FMOD_RESULT result = system->setListenerAttributes(&newattrs);
#endif
		FMOD_CHECK_ERROR(result);
	}

	this->dirty = false;
}

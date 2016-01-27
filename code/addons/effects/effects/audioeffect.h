#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::AudioEffect
    
    Fires and forgets an audio clip.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "effect.h"
#include "faudio/eventid.h"

namespace EffectsFeature
{
class AudioEffect : public Effect
{
	__DeclareClass(AudioEffect);
public:
	/// constructor
	AudioEffect();
	/// destructor
	virtual ~AudioEffect();

	/// set event name
	void SetEventResource(const FAudio::EventId& eventid);
	/// get effects abstract name
	const FAudio::EventId& GetEventResource() const;
	/// set event volume
	void SetVolume(float volume);
	/// get event volume
	const float GetVolume() const;
	/// set event velocity
	void SetVelocity(const Math::vector& volume);
	/// get event volume
	const Math::vector& GetVelocity() const;


	/// start the effect, called by OnFrame() after delay is over
	virtual void OnStart(Timing::Time time);

private:
	FAudio::EventId event;
	float volume;
	Math::vector velocity;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
AudioEffect::SetEventResource(const FAudio::EventId& eventid)
{
	this->event = eventid;
}

//------------------------------------------------------------------------------
/**
*/
inline const FAudio::EventId&
AudioEffect::GetEventResource() const
{
	return this->event;
}

//------------------------------------------------------------------------------
/**
*/
inline void
AudioEffect::SetVolume(float volume)
{
	this->volume = volume;
}

//------------------------------------------------------------------------------
/**
*/
inline const float
AudioEffect::GetVolume() const
{
	return this->volume;
}

//------------------------------------------------------------------------------
/**
*/
inline void
AudioEffect::SetVelocity(const Math::vector& velocity)
{
	this->velocity = velocity;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector&
AudioEffect::GetVelocity() const
{
	return this->velocity;
}

} // namespace FX
//------------------------------------------------------------------------------
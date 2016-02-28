#pragma once
//------------------------------------------------------------------------------
/**
    @class FAudio::AudioListener
    
    Singleton which defines the position, orientation and
    velocity of the audio listener.
        
    (C) 2014-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "math/matrix44.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace FAudio
{
class AudioListener : public Core::RefCounted
{
	__DeclareClass(AudioListener);
	__DeclareSingleton(AudioListener);
public:
    /// constructor
	AudioListener();
    /// destructor
	virtual ~AudioListener();
    
    /// set transform and velocity
    void Set(const Math::matrix44 &transform, const Math::vector &velocity);
    /// set world space transform
    void SetTransform(const Math::matrix44& m);
    /// get world space transform
    const Math::matrix44& GetTransform() const;
    /// set world space velocity
    void SetVelocity(const Math::vector& v);
    /// get world space velocity
    const Math::vector& GetVelocity() const;

    /// called per frame by AudioServer
    void OnFrame();

private:
    Math::matrix44 transform;
    Math::vector velocity;
    bool dirty;
};

//------------------------------------------------------------------------------
/**
*/
inline void
AudioListener::SetTransform(const Math::matrix44& m)
{
    this->transform = m;
	this->dirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
AudioListener::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline void
AudioListener::SetVelocity(const Math::vector& v)
{
    this->velocity = v;    
	this->dirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector&
AudioListener::GetVelocity() const
{
    return this->velocity;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AudioListener::Set(const Math::matrix44 &transform, const Math::vector &velocity)
{
	this->dirty = true;
    this->transform = transform;
    this->velocity = velocity;    
}

} // namespace FAudio
//------------------------------------------------------------------------------

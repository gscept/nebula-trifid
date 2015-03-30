#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::Audio2Listener
    
    Client-side singleton which defines the position, orientation and
    velocity of the audio listener.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "math/matrix44.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace Audio2
{
class Audio2Listener : public Core::RefCounted
{
    __DeclareClass(Audio2Listener);
    __DeclareSingleton(Audio2Listener);
public:
    /// constructor
    Audio2Listener();
    /// destructor
    virtual ~Audio2Listener();
    
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
    bool mustSentListenerData;
};

//------------------------------------------------------------------------------
/**
*/
inline void
Audio2Listener::SetTransform(const Math::matrix44& m)
{
    this->transform = m;
    this->mustSentListenerData = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
Audio2Listener::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Audio2Listener::SetVelocity(const Math::vector& v)
{
    this->velocity = v;
    this->mustSentListenerData = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector&
Audio2Listener::GetVelocity() const
{
    return this->velocity;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Audio2Listener::Set(const Math::matrix44 &transform, const Math::vector &velocity)
{
    this->transform = transform;
    this->velocity = velocity;
    this->mustSentListenerData = true;
}

} // namespace Audio2
//------------------------------------------------------------------------------

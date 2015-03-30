#pragma once
//------------------------------------------------------------------------------
/**
    @class FX::Effect

    Base class for effects. An effect is a small, autonomous object which
    "renders" something into the world for a limited time and which removes
    itself automatically afterwards.
   
    // TODO more specific here please

    (C) 2009 RadonLabs GmbH  
*/
#include "core/refcounted.h"
#include "timing/time.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace FX
{
class Effect : public Core::RefCounted
{
    __DeclareClass(Effect);
public:
    /// constructor
    Effect();
    /// destructor
    virtual ~Effect();

    /// set effects abstract name
    void SetName(const Util::String& n);
    /// get effects abstract name
    const Util::String& GetName() const;
    
    /// set start delay
    void SetStartDelay(Timing::Time time);
    /// get start delay
    Timing::Time GetStartDelay() const;

    /// set effect duration
    void SetDuration(Timing::Time t);
    /// get effect duration
    Timing::Time GetDuration() const;

    /// set position and orientation in world, or relativ to entity
    void SetTransform(const Math::matrix44& m);
    /// get position and orientation in world, or relative to entity
    const Math::matrix44& GetTransform() const;

    /// return true if effect hasn't started yet
    bool IsWaiting() const;
    /// return true if currently active
    bool IsPlaying() const;
    /// return true if effect has finished
    bool IsFinished() const;

    /// activate the effect
    virtual void OnActivate(Timing::Time time);
    /// start the effect, called by OnFrame() after delay is over
    virtual void OnStart(Timing::Time time);
    /// trigger the effect
    virtual void OnFrame(Timing::Time time);
    /// deactivate the effect
    virtual void OnDeactivate();

protected:
    Util::String name;
    Timing::Time activationTime;
    Timing::Time startDelay;
    Timing::Time startTime;
    Timing::Time duration;

    bool isWaiting;
    bool isPlaying;
    bool isFinished;

    Math::matrix44 transform;
};
//------------------------------------------------------------------------------
/**
*/
inline
bool
Effect::IsWaiting() const
{
    return this->isWaiting;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Effect::IsPlaying() const
{
    return this->isPlaying;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Effect::IsFinished() const
{
    return this->isFinished;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetStartDelay(Timing::Time time)
{
    this->startDelay = time;
}

//------------------------------------------------------------------------------
/**
*/
inline
Timing::Time
Effect::GetStartDelay() const
{
    return this->startDelay;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetName(const Util::String& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
Effect::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetDuration(Timing::Time t)
{
    this->duration = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
Timing::Time
Effect::GetDuration() const
{
    return this->duration;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Effect::SetTransform(const Math::matrix44& m)
{
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::matrix44&
Effect::GetTransform() const
{
    return this->transform;
}
};
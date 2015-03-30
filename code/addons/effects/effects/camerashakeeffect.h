#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::CameraShakeEffect

    A shake effect applies a shake shake to the world as long as it's alive.

    The FX::CameraShakeMixer should calculate the final shake values
    over all CameraShakeFX's and apply the modified transform to the current 
    camera

    (C) 2005 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "effect.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace EffectsFeature
{
class CameraShakeEffect : public Effect
{
	__DeclareClass(CameraShakeEffect);
public:
    /// constructor
    CameraShakeEffect();

    /// set range
    void SetRange(float r);
    /// get range
    float GetRange() const;

    /// set intensity
    void SetIntensity(const Math::vector& i);
    /// get intensity
    const Math::vector& GetIntensity() const;

    /// set additional rotation intensity
    void SetRotation(const Math::vector& r);
    /// get additional rotation intensity
    const Math::vector& GetRotation() const;

    /// set additional rotation intensity
    void SetSpeed(float s);
    /// get additional rotation intensity
    float GetSpeed() const;

    /// start the effect
    virtual void OnStart(Timing::Time time);
    /// trigger the effect
    virtual void OnFrame(Timing::Time time);

    /// get current intensity, valid after Update has been called
    const Math::vector& GetCurrentIntensity() const;
    /// get current rotation intensity, valid after Update has been called
    const Math::vector& GetCurrentRotation() const;

private:
    float range;
    Math::vector intensity;
    Math::vector rotation;
    Math::vector curIntensity;     // current intensity, updated by Trigger
    Math::vector curRotation;     // current rotation, updated by Trigger
    float speed;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
CameraShakeEffect::SetSpeed(float s)
{
    this->speed = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
CameraShakeEffect::GetSpeed() const
{
    return this->speed;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CameraShakeEffect::SetRange(float r)
{
    this->range = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
CameraShakeEffect::GetRange() const
{
    return this->range;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CameraShakeEffect::SetIntensity(const Math::vector& i)
{
    this->intensity = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
CameraShakeEffect::GetIntensity() const
{
    return this->intensity;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
CameraShakeEffect::GetCurrentIntensity() const
{
    return this->curIntensity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CameraShakeEffect::SetRotation(const Math::vector& r)
{
    this->rotation = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
CameraShakeEffect::GetRotation() const
{
    return this->rotation;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
CameraShakeEffect::GetCurrentRotation() const
{
    return this->curRotation;
}
}; // namespace FX
//------------------------------------------------------------------------------
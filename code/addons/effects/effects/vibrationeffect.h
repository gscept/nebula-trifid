#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::VibrationEffect

    A gamepad vibration effect.

    A vibrationMixer calcs the final values and controls the pads.

    (C) 2005 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "effect.h"

//------------------------------------------------------------------------------
namespace EffectsFeature
{
class VibrationEffect : public Effect
{
	__DeclareClass(VibrationEffect);
public:
    /// constructor
    VibrationEffect();

    /// set high freq duration
    void SetHighFreqDuration(Timing::Time r);
    /// get high freq duration
    Timing::Time GetHighFreqDuration() const;

    /// set low freq duration
    void SetLowFreqDuration(Timing::Time r);
    /// get low freq duration
    Timing::Time GetLowFreqDuration() const;

    /// set low freq intensity
    void SetLowFreqIntensity(float r);
    /// get intensity
    float GetLowFreqIntensity() const;

    /// set high freq intensity
    void SetHighFreqIntensity(float r);
    /// get high freq intensity
    float GetHighFreqIntensity() const;

    /// set player index
    void SetPlayerIndex(IndexT index);
    /// get player index
    IndexT GetPlayerIndex() const;

    /// start the effect
    virtual void OnStart(Timing::Time time);
    /// trigger the effect
    virtual void OnFrame(Timing::Time time);

    /// get current low freq intensity, valid after OnFrame has been called
    float GetCurrentHighFreqIntensity() const;
    /// get current low freq intensity, valid after OnFrame has been called
    float GetCurrentLowFreqIntensity() const;

private:
    IndexT playerIndex;

    float highFreqIntensity;
    float lowFreqIntensity;
    Timing::Time highFreqDuration;
    Timing::Time lowFreqDuration;

    float curHighFreqIntensity;
    float curLowFreqIntensity;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
VibrationEffect::SetPlayerIndex(IndexT index)
{
    this->playerIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
IndexT
VibrationEffect::GetPlayerIndex() const
{
    return this->playerIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
VibrationEffect::GetCurrentHighFreqIntensity() const
{
    return this->curHighFreqIntensity;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
VibrationEffect::GetCurrentLowFreqIntensity() const
{
    return this->curLowFreqIntensity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
VibrationEffect::SetHighFreqDuration(Timing::Time r)
{
    this->highFreqDuration = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
Timing::Time
VibrationEffect::GetHighFreqDuration() const
{
    return this->highFreqDuration;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
VibrationEffect::SetLowFreqDuration(Timing::Time r)
{
    this->lowFreqDuration = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
Timing::Time
VibrationEffect::GetLowFreqDuration() const
{
    return this->lowFreqDuration;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
VibrationEffect::SetHighFreqIntensity(float r)
{
    this->highFreqIntensity = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
VibrationEffect::GetHighFreqIntensity() const
{
    return this->highFreqIntensity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
VibrationEffect::SetLowFreqIntensity(float r)
{
    this->lowFreqIntensity = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
VibrationEffect::GetLowFreqIntensity() const
{
    return this->lowFreqIntensity;
}
}; // namespace FX
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::CameraShakeMixer

    This mixer calculates the resulting 'master' shake from all shake effects,
    and applies is to the current camera.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "effectmixer.h"
#include "timing/time.h"
#include "math/vector.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"

//------------------------------------------------------------------------------
namespace EffectsFeature
{
class CameraShakeMixer : public EffectsFeature::EffectMixer
{
    __DeclareClass(CameraShakeMixer);
public:
    /// constructor
    CameraShakeMixer();

    /// set desired max displacement along camera axes
    void SetMaxDisplacement(const Math::vector& d);
    /// set desired maximum tumble angles in degrees
    void SetMaxTumble(const Math::vector& a);

    /// overwritten mix method
    virtual void Mix(const Util::Array<Ptr<Effect> >& effects, Timing::Time time);    
    /// apply value to camera
    virtual void Apply() const;

private:

    /// compute/mix final shake intensity
    Math::vector ComputeShakeIntensityAtPosition(const Util::Array<Ptr<Effect> >& effects, const Math::vector& pos) const;
    /// compute/mix final rotation intensity
    Math::vector ComputeRotationIntensityAtPosition(const Util::Array<Ptr<Effect> >& effects, const Math::vector& pos) const;

    Math::vector maxDisplacement;
    Math::vector curDisplace;
    Math::vector maxTumble;
    Math::vector curTumble;
    
    Timing::Time lastTime;
    Math::matrix44 shakeCameraMatrix;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
CameraShakeMixer::SetMaxDisplacement(const Math::vector& d)
{
    this->maxDisplacement = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CameraShakeMixer::SetMaxTumble(const Math::vector& a)
{
    this->maxTumble.x() = Math::n_deg2rad(a.x());
    this->maxTumble.y() = Math::n_deg2rad(a.y());
    this->maxTumble.z() = Math::n_deg2rad(a.z());
}

}; // namespace FX
//------------------------------------------------------------------------------

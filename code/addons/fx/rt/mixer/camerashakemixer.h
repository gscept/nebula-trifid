#pragma once
//------------------------------------------------------------------------------
/**
    @class FX::CameraShakeMixer

    This mixer calculates the resulting 'master'shake from all shake effects,
    and applies is to the current camera.
    
    (C) 2009 Radon Labs GmbH
*/
#include "fx/rt/mixer/effectmixer.h"
#include "timing/time.h"
#include "math/vector.h"
#include "internalgraphics/internalgraphicsserver.h"
#include "internalgraphics/internalview.h"

//------------------------------------------------------------------------------
namespace FX
{
class CameraShakeMixer : public FX::EffectMixer
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
    /// apply calculated transform to camera
    void SetCameraTransform(const Math::matrix44& t) const;
    /// get the camera transform
    const Math::matrix44& GetCameraTransform() const;

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

//------------------------------------------------------------------------------
/**
    get current camera and get the transform
*/
inline const Math::matrix44&
CameraShakeMixer::GetCameraTransform() const
{
    return InternalGraphics::InternalGraphicsServer::Instance()->GetDefaultView()->GetCameraEntity()->GetTransform();
}

//------------------------------------------------------------------------------
/**
    set transform to current camera
*/
inline void
CameraShakeMixer::SetCameraTransform(const Math::matrix44& t) const
{
    InternalGraphics::InternalGraphicsServer::Instance()->GetDefaultView()->GetCameraEntity()->SetTransform(t);
}

}; // namespace FX
//------------------------------------------------------------------------------

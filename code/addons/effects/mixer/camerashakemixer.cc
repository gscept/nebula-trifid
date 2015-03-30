//------------------------------------------------------------------------------
//  camerashakemixer.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "camerashakemixer.h"
#include "effects/effects/camerashakeeffect.h"
#include "math/point.h"
#include "graphics/cameraentity.h"
#include "graphics/view.h"
#include "graphicsfeature/graphicsfeatureunit.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::CameraShakeMixer, 'CSHM', EffectsFeature::EffectMixer);

using namespace Math;
using namespace Graphics;
//------------------------------------------------------------------------------
/**
*/
CameraShakeMixer::CameraShakeMixer() :  
    maxDisplacement(1.0f, 1.0f, 1.0f),
    curDisplace(0, 0, 0),
    maxTumble(10.0f, 10.0f, 10.0f),
    curTumble(0, 0, 0),
    lastTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
CameraShakeMixer::Mix(const Util::Array<Ptr<Effect> >& effects, Timing::Time time)
{
	// get default view from graphics feature
	const Ptr<CameraEntity>& cam = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView()->GetCameraEntity();

    // just do if camera exists
    if (cam.isvalid())
    {
        // at first calculate accumulated shake intensity at camera position
        const Math::matrix44& camMatrix = cam->GetTransform();
        const Math::point& camPos = camMatrix.get_position();
        Math::vector shakeIntensity = this->ComputeShakeIntensityAtPosition(effects, camPos);
        Math::vector rotation = this->ComputeRotationIntensityAtPosition(effects, camPos);

        // only update displace and tumble at some relatively low frame rate
        // this prevents the shake effect from "flickering" if the 
        // frame rate is very high    
        if ((0.0 == this->lastTime) || ((time - this->lastTime) > 0.01))
        {
            this->lastTime = time;

            // compute a random displacement vector
            this->curDisplace.set(((n_rand() * 2.0f) - 1.0f) * this->maxDisplacement.x(), 
                                  ((n_rand() * 2.0f) - 1.0f) * this->maxDisplacement.y(), 
                                  ((n_rand() * 2.0f) - 1.0f) * this->maxDisplacement.z());


            // compute random tumble angles
            this->curTumble.set(((n_rand() * 2.0f) - 1.0f) * this->maxTumble.x(), 
                                ((n_rand() * 2.0f) - 1.0f) * this->maxTumble.y(), 
                                ((n_rand() * 2.0f) - 1.0f) * this->maxTumble.z());
        }   
        
        // update the current incoming camera matrix by the displace and tumble vectors
        if (shakeIntensity.x() > 0.0f || shakeIntensity.y() > 0.0f || shakeIntensity.z() > 0.0f ||
            rotation.x() > 0.0f       || rotation.y() > 0.0f       || rotation.z() > 0.0f)
        {
            matrix44 shakeMatrix;
            shakeMatrix.rotationx(this->curTumble.x() * rotation.x());
            shakeMatrix.rotationy(this->curTumble.y() * rotation.y());
            shakeMatrix.rotationz(this->curTumble.z() * rotation.z());
            vector displace(this->curDisplace.x() * shakeIntensity.x(),
                            this->curDisplace.y() * shakeIntensity.y(),
                            this->curDisplace.z() * shakeIntensity.z());
            shakeMatrix.translate(Math::matrix44::transform(displace, Math::matrix44(camMatrix.get_xaxis(),
                                                                                   camMatrix.get_yaxis(),
                                                                                   camMatrix.get_zaxis(),
                                                                                   Math::point::origin())));            
            this->shakeCameraMatrix = Math::matrix44::multiply(shakeMatrix, camMatrix);
        }
        else
        {
            this->shakeCameraMatrix = camMatrix;
        }
    }
}

//------------------------------------------------------------------------------
/**
    apply camera transform
*/
void
CameraShakeMixer::Apply() const
{
	// get default view from graphics feature
	const Ptr<CameraEntity>& cam = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView()->GetCameraEntity();

    if (cam.isvalid())
    {
		cam->SetTransform(this->shakeCameraMatrix);
    }
}

//------------------------------------------------------------------------------
/**
    Computes the accumulated shake intensity at a given position.
*/
Math::vector
CameraShakeMixer::ComputeShakeIntensityAtPosition(const Util::Array<Ptr<Effect> >& effects, const Math::vector& pos) const
{
    Math::vector shake = Math::vector::nullvec();
    int i;
    int num = effects.Size();
    for (i = 0; i < num; i++)
    {
        if (effects[i]->IsA(CameraShakeEffect::RTTI))
        {
            const Ptr<CameraShakeEffect>& curShake = effects[i].cast<CameraShakeEffect>();
            if (curShake->IsPlaying())
            {
                Math::vector distVec = pos - curShake->GetTransform().get_position();
                float absDist = distVec.length();
                if (absDist < curShake->GetRange())
                {
                    float attenuate = 1.0f - Math::n_saturate(absDist / curShake->GetRange());
                    shake += curShake->GetCurrentIntensity() * attenuate;
                }
            }
        }
    }
    return shake;
}

//------------------------------------------------------------------------------
/**
    Computes the accumulated rotation intensity at a given position.
*/
Math::vector
CameraShakeMixer::ComputeRotationIntensityAtPosition(const Util::Array<Ptr<Effect> >& effects, const Math::vector& pos) const
{
    Math::vector rotation = Math::vector::nullvec();
    int i;
    int num = effects.Size();
    for (i = 0; i < num; i++)
    {
        if (effects[i]->IsA(CameraShakeEffect::RTTI))
        {
            const Ptr<CameraShakeEffect>& curShake = effects[i].cast<CameraShakeEffect>();
            if (curShake->IsPlaying())
            {
                Math::vector distVec = pos - curShake->GetTransform().get_position();
                float absDist = distVec.length();
                if (absDist < curShake->GetRange())
                {
                    float attenuate = 1.0f - Math::n_saturate(absDist / curShake->GetRange());
                    rotation += curShake->GetCurrentRotation() * attenuate;
                }
            }
        }
    }
    return rotation;
}
};
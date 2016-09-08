//------------------------------------------------------------------------------
//  spotlightentity.h
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/spotlightentity.h"
#include "coregraphics/shaperenderer.h"
#include "threading/thread.h"
#include "math/polar.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/config.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shadersemantics.h"

namespace Graphics
{
__ImplementClass(Graphics::SpotLightEntity, 'SPLE', Graphics::AbstractLightEntity);

using namespace Math;
using namespace CoreGraphics;
using namespace Threading;
using namespace Messaging;
using namespace Lighting;

//------------------------------------------------------------------------------
/**
*/
SpotLightEntity::SpotLightEntity()
{
	// create managed resource for projection map, use default which is tex:lighting/lightcones for spotlights
	this->projectionTextureId = Util::String("tex:lighting/lightcones") + NEBULA3_TEXTURE_EXTENSION;
    this->SetLightType(LightType::Spot);
}

//------------------------------------------------------------------------------
/**
*/
SpotLightEntity::~SpotLightEntity()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Math::ClipStatus::Type
SpotLightEntity::ComputeClipStatus(const Math::bbox& box)
{
    // compute spot light clip status
    ClipStatus::Type clipStatus = box.clipstatus(this->invLightProjTransform);
    return clipStatus;
}

//------------------------------------------------------------------------------
/**
*/
void
SpotLightEntity::OnTransformChanged()
{
    AbstractLightEntity::OnTransformChanged();

    // compute the spot light's perspective projection matrix from
    // its transform matrix (spot direction is along -z, and goes
    // throught the rectangle formed by the x and y components
    // at the end of -z
    float widthAtFarZ  = this->transform.getrow0().length() * 2.0f;
    float heightAtFarZ = this->transform.getrow1().length() * 2.0f;
    float nearZ = 0.001f; // put the near plane at 0.001cm 
    float farZ = this->transform.getrow2().length();    
    n_assert(farZ > 0.0f);
    if (nearZ >= farZ)
    {
        nearZ = farZ / 2.0f;
    }
    float widthAtNearZ  = (widthAtFarZ / farZ) * nearZ;
    float heightAtNearZ = (heightAtFarZ / farZ) * nearZ;
    this->projTransform = matrix44::persprh(widthAtNearZ, heightAtNearZ, nearZ, farZ);

    // compute the invLightProj matrix which transforms from world space to light space
    this->invLightProjTransform = matrix44::multiply(this->invTransform, this->projTransform);
}

//------------------------------------------------------------------------------
/**
*/
void 
SpotLightEntity::OnRenderDebug()
{
    // render spot light frustum
    float4 color(this->GetColor() * 0.8f);
    color.w() = 0.25f;

    // remove scaling, its also considered in projection matrix
    matrix44 unscaledTransform = this->GetTransform();
    unscaledTransform.set_xaxis(float4::normalize(unscaledTransform.get_xaxis()));
    unscaledTransform.set_yaxis(float4::normalize(unscaledTransform.get_yaxis()));
    unscaledTransform.set_zaxis(float4::normalize(unscaledTransform.get_zaxis()));
    matrix44 frustum = matrix44::multiply(matrix44::inverse(this->GetProjTransform()), unscaledTransform);
    ShapeRenderer::Instance()->AddShape(RenderShape(Thread::GetMyThreadId(), RenderShape::Box, RenderShape::CheckDepth, frustum, color));

    // mark position
    color.x() = 1.0f;
    matrix44 centerTransform = matrix44::scaling(0.01, 0.01, 0.01);
    centerTransform.set_position(this->GetTransform().get_position());
    ShapeRenderer::Instance()->AddShape(RenderShape(Thread::GetMyThreadId(), RenderShape::Sphere, RenderShape::CheckDepth, centerTransform, color));

    AbstractLightEntity::OnRenderDebug();
}

//------------------------------------------------------------------------------
/**
*/
void 
SpotLightEntity::SetTransformFromPosDirRangeAndCone(const Math::point& pos, const Math::vector& dir, float range, float coneAngle)
{
    const float lightVolumeAngle = 90.0f;
    float coneScale = Math::n_sqrt(coneAngle / lightVolumeAngle);
    matrix44 lightFrustum = matrix44::scaling(vector(range, range, range / coneScale));
    polar dirAngle(dir);
    matrix44 lightTransform = matrix44::rotationyawpitchroll(dirAngle.rho, dirAngle.theta - N_PI * 0.5f, 0);
    lightTransform.set_position(pos);
    lightFrustum = matrix44::multiply(lightFrustum, lightTransform);
    this->SetTransform(lightFrustum);
}

//------------------------------------------------------------------------------
/**
    Handle a message, override this method accordingly in subclasses!
*/
void
SpotLightEntity::HandleMessage(const Ptr<Message>& msg)
{
    __Dispatch(SpotLightEntity, this, msg);
}

} // namespace Graphics

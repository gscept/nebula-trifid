//------------------------------------------------------------------------------
//  pointlightentity.h
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/pointlightentity.h"
#include "coregraphics/shaperenderer.h"
#include "threading/thread.h"
#include "coregraphics/renderdevice.h"

namespace Graphics
{
__ImplementClass(Graphics::PointLightEntity, 'IPLE', Graphics::AbstractLightEntity);

using namespace Math;
using namespace Threading;
using namespace CoreGraphics;
using namespace Messaging;
using namespace Lighting;

//------------------------------------------------------------------------------
/**
*/
PointLightEntity::PointLightEntity() :
    shadowCube(0)
{
    this->projectionTextureId = Util::String("tex:lighting/lightcube") + NEBULA3_TEXTURE_EXTENSION;
    this->SetLightType(LightType::Point);

    // patch bounding box to fully cover point light volume
    this->localBox.set(point::origin(), vector(1.0,1.0,1.0));
}

//------------------------------------------------------------------------------
/**
*/
Math::ClipStatus::Type
PointLightEntity::ComputeClipStatus(const Math::bbox& box)
{
    // compute pointlight clip status
    Math::bbox pointLightBox(this->GetTransform().get_position(), this->GetTransform().get_zaxis());
    ClipStatus::Type clipStatus = box.clipstatus(pointLightBox);
    return clipStatus;
}

//------------------------------------------------------------------------------
/**
*/
void
PointLightEntity::OnTransformChanged()
{
    AbstractLightEntity::OnTransformChanged();

    // compute the spot light's perspective projection matrix from
    // its transform matrix (spot direction is along -z, and goes
    // throught the rectangle formed by the x and y components
    // at the end of -z
    float widthAtFarZ  = this->transform.getrow0().length() * 2.0f;
    float heightAtFarZ = this->transform.getrow1().length() * 2.0f;
    widthAtFarZ *= 2.0f;
    heightAtFarZ *= 2.0f;

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

    this->shadowTransform.set_position(this->shadowTransform.get_position() + this->shadowTransform.get_zaxis() * 0.25f);
    this->shadowTransformsDirty = true;

    // compute the invLightProj matrix which transforms from world space to light space
    this->invLightProjTransform = matrix44::multiply(this->invTransform, this->projTransform);
}

//------------------------------------------------------------------------------
/**
*/
void 
PointLightEntity::OnRenderDebug()
{
    float4 color(this->GetColor() * 0.8f);
    color.w() = 0.25f;
    ShapeRenderer::Instance()->AddShape(RenderShape(Thread::GetMyThreadId(), RenderShape::Sphere, RenderShape::CheckDepth, this->GetTransform(), color));

    // mark position
	color.x() = 1.0f;
    matrix44 centerTransform = matrix44::scaling(0.01, 0.01, 0.01);
    centerTransform.set_position(this->GetTransform().get_position());
    ShapeRenderer::Instance()->AddShape(RenderShape(Thread::GetMyThreadId(), RenderShape::Sphere, RenderShape::CheckDepth, centerTransform, color));

    // render bounding box
    ShapeRenderer::Instance()->AddWireFrameBox(this->GetGlobalBoundingBox(), color, Thread::GetMyThreadId());

    AbstractLightEntity::OnRenderDebug();
}

//------------------------------------------------------------------------------
/**
*/
void
PointLightEntity::SetTransformFromPosDirAndRange(const point& pos, const vector& dir, float range)
{
    matrix44 m = matrix44::lookatrh(pos, pos + dir, vector::upvec());
    float4 scale = float4(range, range, range, 1.0f);
    m.setrow0(float4::multiply(m.getrow0(), scale));
    m.setrow1(float4::multiply(m.getrow1(), scale));
    m.setrow2(float4::multiply(m.getrow2(), scale));
    this->SetTransform(m);
}

//------------------------------------------------------------------------------
/**
    Handle a message, override this method accordingly in subclasses!
*/
void
PointLightEntity::HandleMessage(const Ptr<Message>& msg)
{
    __Dispatch(PointLightEntity, this, msg);
}
} // namespace Graphics

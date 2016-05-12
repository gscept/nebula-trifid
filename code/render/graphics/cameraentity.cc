//------------------------------------------------------------------------------
//  cameraentity.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/cameraentity.h"
#include "coregraphics/displaydevice.h"
#include "graphics/view.h"

namespace Graphics
{
__ImplementClass(Graphics::CameraEntity, 'ICME', Graphics::GraphicsEntity);

using namespace Math;
using namespace Messaging;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
CameraEntity::CameraEntity()
{
    this->SetType(GraphicsEntityType::Camera);
    float aspectRatio = DisplayDevice::Instance()->GetDisplayMode().GetAspectRatio();
    this->camSettings.SetupPerspectiveFov(n_deg2rad(60.0f), aspectRatio, 0.01f, 2500.0f);
}

//------------------------------------------------------------------------------
/**
*/
CameraEntity::~CameraEntity()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
CameraEntity::IsAttachedToView() const
{
    return this->view.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
void
CameraEntity::OnAttachToView(const Ptr<View>& v)
{
    n_assert(!this->view.isvalid());
    this->view = v;
}

//------------------------------------------------------------------------------
/**
*/
void
CameraEntity::OnRemoveFromView(const Ptr<View>& v)
{
    n_assert(this->view.isvalid() && this->view == v);
    this->view = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
CameraEntity::OnDeactivate()
{
    // if we're currently attached to a view, we need to detach first
    if (this->IsAttachedToView())
    {
        this->view->SetCameraEntity(0);
    }
    GraphicsEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    We need to keep track of modifications of the transformation matrix.
*/
void
CameraEntity::OnTransformChanged()
{
    this->camSettings.UpdateViewMatrix(matrix44::inverse(this->GetTransform()));
    GraphicsEntity::OnTransformChanged();
}

//------------------------------------------------------------------------------
/**
    Computes the clip status of a bounding box in global space against
    the view volume of this camera entity.
*/    
ClipStatus::Type
CameraEntity::ComputeClipStatus(const bbox& box)
{
    ClipStatus::Type clipStatus = box.clipstatus(this->camSettings.GetViewProjTransform());
    return clipStatus;
}

//------------------------------------------------------------------------------
/**
*/
void
CameraEntity::OnDisplayResized()
{
	float aspectRatio = DisplayDevice::Instance()->GetDisplayMode().GetAspectRatio();
	this->camSettings.SetupPerspectiveFov(this->camSettings.GetFov(), aspectRatio, this->camSettings.GetZNear(), this->camSettings.GetZFar());
}

//------------------------------------------------------------------------------
/**
*/
void
CameraEntity::SetCameraSettings(const CameraSettings& settings)
{
    this->camSettings = settings;
    this->camSettings.UpdateViewMatrix(matrix44::inverse(this->GetTransform()));
}

//------------------------------------------------------------------------------
/**
*/
Math::float2
CameraEntity::CalculateScreenSpacePosition(const Math::float4& pos)
{
	float4 screenPos = matrix44::transform(pos, this->GetViewProjTransform());
	screenPos.x() /= screenPos.w();
	screenPos.y() /= screenPos.w();
	screenPos.x() = (screenPos.x() + 1.0f) * 0.5f;
	screenPos.y() = 1.0f - ((screenPos.y() + 1.0f) * 0.5f);
	return float2(screenPos.x(), screenPos.y());
}

//------------------------------------------------------------------------------
/**
*/
Math::float4
CameraEntity::CalculateWorldSpacePosition(const Math::float2& pos)
{
	float4 worldpos = float4(pos.x(), pos.y(), this->camSettings.GetZNear(), 1);
	worldpos = matrix44::transform(worldpos, matrix44::inverse(this->GetViewProjTransform()));
	return worldpos;
}

//------------------------------------------------------------------------------
/**
    Handle a message, override this method accordingly in subclasses!
*/
void
CameraEntity::HandleMessage(const Ptr<Message>& msg)
{
    __Dispatch(CameraEntity, this, msg);
}

} // namespace Graphics

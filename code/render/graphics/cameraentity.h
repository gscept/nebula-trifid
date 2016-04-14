#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::CameraEntity
  
    Represents a camera attached to a graphics stage. Any number of
    cameras can be attached to a stage.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/    
#include "graphics/graphicsentity.h"
#include "graphics/camerasettings.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class View;

class CameraEntity : public GraphicsEntity
{
    __DeclareClass(CameraEntity);
public:
    /// constructor
    CameraEntity();
    /// destructor
    virtual ~CameraEntity();
    
    /// return true if camera is attached to a View
    bool IsAttachedToView() const;
    /// compute clip status against bounding box
    virtual Math::ClipStatus::Type ComputeClipStatus(const Math::bbox& box);

    /// set new camera settings
    void SetCameraSettings(const CameraSettings& camSettings);
    /// get camera settings
    const CameraSettings& GetCameraSettings() const;

	/// update camera if the display has been resized
	void OnDisplayResized();

    /// get projection matrix
    const Math::matrix44& GetProjTransform() const;
    /// get view transform (inverse transform)
    const Math::matrix44& GetViewTransform() const;
    /// get view projection matrix 
    const Math::matrix44& GetViewProjTransform() const;
    /// get view frustum
    const Math::frustum& GetViewFrustum() const;

	/// calculate position as a screen space coordinate
	Math::float2 CalculateScreenSpacePosition(const Math::float4& pos);
	/// calculate position in word using screen space coordinate
	Math::float4 CalculateWorldSpacePosition(const Math::float2& pos);

    /// handle a message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
    friend class View;

    /// called before entity is destroyed
    virtual void OnDeactivate();
    /// called by View when camera is attached to that view
    void OnAttachToView(const Ptr<View>& view);
    /// called by View when camera becomes detached from view
    void OnRemoveFromView(const Ptr<View>& view);
    /// called when transform matrix changed
    virtual void OnTransformChanged();

    Ptr<View> view;
    CameraSettings camSettings;
};

//------------------------------------------------------------------------------
/**
*/
inline const CameraSettings&
CameraEntity::GetCameraSettings() const
{
    return this->camSettings;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
CameraEntity::GetProjTransform() const
{
    return this->camSettings.GetProjTransform();
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
CameraEntity::GetViewTransform() const
{
    return this->camSettings.GetViewTransform();
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
CameraEntity::GetViewProjTransform() const
{
    return this->camSettings.GetViewProjTransform();
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::frustum&
CameraEntity::GetViewFrustum() const
{
    return this->camSettings.GetViewFrustum();
}

} // namespace Graphics
//------------------------------------------------------------------------------

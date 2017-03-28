#pragma once
//------------------------------------------------------------------------------
/**
    @class VR::VRCameraEntity
        
    (C) 2016 Individual contributors, see AUTHORS file
*/    
#include "graphics/cameraentity.h"

//------------------------------------------------------------------------------
namespace VR
{

class VRCameraEntity : public Graphic::CameraEntity
{
    __DeclareClass(VRCameraEntity);
public:
    /// constructor
    VRCameraEntity();
    /// destructor
    virtual ~VRCameraEntity();
    
    /// compute clip status against bounding box
    virtual Math::ClipStatus::Type ComputeClipStatus(const Math::bbox& box);

    /// get projection matrix
    const Math::matrix44& GetProjTransform() const;
    /// get view transform (inverse transform)
    const Math::matrix44& GetViewTransform() const;
    /// get view projection matrix 
    const Math::matrix44& GetViewProjTransform() const;
    /// get view frustum
    const Math::frustum& GetViewFrustum() const;

    /// handle a message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
    friend class VRView;

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

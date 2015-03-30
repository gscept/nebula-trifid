#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::PointLightEntity
    
    Implements a local point light. A point light is considered to be a sphere shape
    and projects lights in all directions. It may also cast shadows using a shadow map cube.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "graphics/abstractlightentity.h"
#include "coregraphics/rendertargetcube.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class PointLightEntity : public AbstractLightEntity
{
    __DeclareClass(PointLightEntity);
public:
    /// constructor
    PointLightEntity();
    /// compute clip status against bounding box
    virtual Math::ClipStatus::Type ComputeClipStatus(const Math::bbox& box);
    /// handle a message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

    /// setup point light entity transform from position, direction and range
    void SetTransformFromPosDirAndRange(const Math::point& pos, const Math::vector& dir, float range);

    /// set shadow casting cube
    void SetShadowCube(const Ptr<CoreGraphics::RenderTargetCube>& cube);
    /// get shadow casting cube
    const Ptr<CoreGraphics::RenderTargetCube>& GetShadowCube() const;

protected:
    /// called when transform matrix changed
    virtual void OnTransformChanged();  
    /// called to render a debug visualization of the entity
    virtual void OnRenderDebug();

private:
    Ptr<CoreGraphics::RenderTargetCube> shadowCube;
};


//------------------------------------------------------------------------------
/**
*/
inline void 
PointLightEntity::SetShadowCube( const Ptr<CoreGraphics::RenderTargetCube>& cube )
{
    this->shadowCube = cube;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTargetCube>& 
PointLightEntity::GetShadowCube() const
{
    return this->shadowCube;
}

} // namespace Graphics
//------------------------------------------------------------------------------

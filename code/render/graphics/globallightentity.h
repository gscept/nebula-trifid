#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::GlobalLightEntity
  
    Implements a global directional light intended for outdoor 
    areas or as the basis for indoor rendering. Besides the primary directional 
    light component, a global light also usually provides some sort of ambient 
    component for areas in shadow. There should only be one GlobalLight active 
    during rendering a frame. The primary light is directed along the negative
    z-axis of the light's transform matrix. Scaling and position has no
    effect on the global light.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "graphics/abstractlightentity.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class GlobalLightEntity : public AbstractLightEntity
{
    __DeclareClass(GlobalLightEntity);
public:
    /// constructor
    GlobalLightEntity();
    /// compute clip status against bounding box
    virtual Math::ClipStatus::Type ComputeClipStatus(const Math::bbox& box);
    
    /// set the backlight color
    void SetBackLightColor(const Math::float4& c);
    /// get the backlight color
    const Math::float4& GetBackLightColor() const;
    /// get the light direction (extracted from negative z-axis of transformation matrix)
    const Math::vector& GetLightDirection() const;
    /// get AmbientLightColor	
    const Math::float4& GetAmbientLightColor() const;
    /// set AmbientLightColor
    void SetAmbientLightColor(const Math::float4& val);
    /// get BackLightOffset	
    const float& GetBackLightOffset() const;
    /// set BackLightOffset
    void SetBackLightOffset(const float& val);

    /// handle a message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
    /// called when transform matrix changed
    virtual void OnTransformChanged();  
    /// called to render a debug visualization of the entity
    virtual void OnRenderDebug();

    Math::float4 backLightColor;
    Math::float4 ambientLightColor;
    Math::vector lightDir;
    float backLightOffset;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
GlobalLightEntity::SetBackLightColor(const Math::float4& c)
{
    this->backLightColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4&
GlobalLightEntity::GetBackLightColor() const
{
    return this->backLightColor;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector&
GlobalLightEntity::GetLightDirection() const
{
    return this->lightDir;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4& 
GlobalLightEntity::GetAmbientLightColor() const
{
    return this->ambientLightColor;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
GlobalLightEntity::SetAmbientLightColor(const Math::float4& color)
{
    this->ambientLightColor = color;
}

//------------------------------------------------------------------------------
/**
*/
inline const float& 
GlobalLightEntity::GetBackLightOffset() const
{
    return this->backLightOffset;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
GlobalLightEntity::SetBackLightOffset(const float& val)
{
    this->backLightOffset = val;
}

} // namespace Graphics
//------------------------------------------------------------------------------


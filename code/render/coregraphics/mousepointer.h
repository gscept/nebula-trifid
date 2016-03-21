#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::MousePointer
  
    Contains information how to render a mouse pointer through the
    CoreGraphics::MouseRenderer class. Please note not all parameters
    are supported on all platforms (like the orientation vector).
    
    The pointer works in screen space coordinates, where (0,0) is the
    middle of the screen, and -1 is top/left.    
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "util/stringatom.h"
#include "math/vector.h"
#include "math/point.h"
#include "resources/resourceid.h"

//------------------------------------------------------------------------------
namespace CoreGraphics
{
class MousePointer
{
public:
    /// constructor
    MousePointer();
    
    /// set mouse pointer texture resource id
    void SetResourceId(const Resources::ResourceId& texResId);
    /// get mouse pointer texture resource id
    const Resources::ResourceId& GetResourceId() const;
    /// set position of the mouse pointer in screen coordinates
    void SetPosition(const Math::point& pos);
    /// get position of the mouse pointer in screen coordinates
    const Math::point& GetPosition() const;
    /// set size in screen coordinates (screen size is 2.0)
    void SetSize(const Math::vector& size);
    /// get size in screen coordinates
    const Math::vector& GetSize() const;    
    /// set orienation of the mouse pointer as horizon vector (1,0) is horizontal
    void SetOrientation(const Math::vector& orient);
    /// get orientation of the mouse pointer as horizon vector
    const Math::vector& GetOrientation() const;
    /// set hot spot of the mouse vector (-1,-1 is top/left, 1,1 is bottom/right)
    void SetHotspot(const Math::point& hotSpot);
    /// get hot spot of the mouse vector
    const Math::point& GetHotspot() const;
    /// set alpha value
    void SetAlpha(Math::scalar alpha);
    /// get hot spot of the mouse vector
    Math::scalar GetAlpha() const;
    
private:
    Resources::ResourceId resId;
    Math::point pos;
    Math::vector size;
    Math::vector orient;
    Math::point hotspot;
    Math::scalar alpha;
};

//------------------------------------------------------------------------------
/**
*/
inline
MousePointer::MousePointer() :
    pos(Math::point::origin()),
    size(0.1f, 0.1f, 0.0f),
    orient(1.0f, 0.0f, 0.0f),
    hotspot(-1.0f, -1.0f, 0.0f),
    alpha(1.0f)
{ 
    // empty
}   

//------------------------------------------------------------------------------
/**
*/
inline void
MousePointer::SetResourceId(const Resources::ResourceId& id)
{
    this->resId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId&
MousePointer::GetResourceId() const
{
    return this->resId;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MousePointer::SetPosition(const Math::point& p)
{
    this->pos = p;
}
    
//------------------------------------------------------------------------------
/**
*/
inline const Math::point&
MousePointer::GetPosition() const
{
    return this->pos;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MousePointer::SetSize(const Math::vector& v)
{
    this->size = v;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector&
MousePointer::GetSize() const
{
    return this->size;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MousePointer::SetOrientation(const Math::vector& v)
{
    this->orient = v;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector&
MousePointer::GetOrientation() const
{
    return this->orient;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MousePointer::SetHotspot(const Math::point& p)
{
    this->hotspot = p;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::point&
MousePointer::GetHotspot() const
{
    return this->hotspot;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MousePointer::SetAlpha(Math::scalar a)
{
    this->alpha = a;
}

//------------------------------------------------------------------------------
/**
*/
inline Math::scalar
MousePointer::GetAlpha() const
{
    return this->alpha;
}

} // namespace CoreGraphics
//------------------------------------------------------------------------------

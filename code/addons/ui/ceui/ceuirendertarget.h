#pragma once
//------------------------------------------------------------------------------
/**
    @class CEUI::CEUIRenderTarget

    Wrapper for the cegui render target.

    (C) 2009 Radon Labs GmbH
*/
//------------------------------------------------------------------------------
#include "cegui/include/CEGUIRenderTarget.h"
#include "cegui/include/CEGUIRect.h"
#include "cegui/include/CEGUIGeometryBuffer.h"
#include "cegui/include/CEGUIRenderQueue.h"
#include "sui/ceui/ceuirenderer.h"

namespace CEUI
{

class CEUIRenderTarget : public CEGUI::RenderTarget
{
public:
    /// constructor
    CEUIRenderTarget(const CEUIRenderer& owner);
    /// destructor
    virtual ~CEUIRenderTarget();

    // implementation of the cegui RenderTarget interface

    //
    virtual void draw(const CEGUI::GeometryBuffer& buffer);
    //
    virtual void draw(const CEGUI::RenderQueue& queue);
    //
    virtual void setArea(const CEGUI::Rect& area);
    //
    virtual const CEGUI::Rect& getArea() const;
    //
    virtual bool isImageryCache() const;
    //
    virtual void activate();
    //
    virtual void deactivate();
    //
    virtual void unprojectPoint(const CEGUI::GeometryBuffer& buff, const CEGUI::Vector2& p_in, CEGUI::Vector2& p_out) const;

protected:
    CEGUI::Rect area;
    const CEUIRenderer& owner;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CEUIRenderTarget::isImageryCache() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CEUIRenderTarget::draw(const CEGUI::GeometryBuffer& buffer)
{
    buffer.draw();
}

//------------------------------------------------------------------------------
/**
*/
inline void
CEUIRenderTarget::draw(const CEGUI::RenderQueue& queue)
{
    queue.draw();
}

//------------------------------------------------------------------------------
/**
*/
inline void
CEUIRenderTarget::setArea(const CEGUI::Rect& area)
{
    this->area = area;
}

//------------------------------------------------------------------------------
/**
*/
inline const CEGUI::Rect&
CEUIRenderTarget::getArea() const
{
    return this->area;
}

}
//------------------------------------------------------------------------------
//  ceuirendertarget.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sui/ceui/ceuirendertarget.h"

namespace CEUI
{
//------------------------------------------------------------------------------
/**
*/
CEUIRenderTarget::CEUIRenderTarget(const CEUIRenderer& owner)  :
    area(CEGUI::Point(),CEGUI::Size()),
    owner(owner)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CEUIRenderTarget::~CEUIRenderTarget()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderTarget::activate()
{
    //n_error("NOT yet implemented!\n");
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderTarget::deactivate()
{
    //n_error("NOT yet implemented!\n");
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderTarget::unprojectPoint(const CEGUI::GeometryBuffer& buff, const CEGUI::Vector2& p_in, CEGUI::Vector2& p_out) const
{
    n_error("NOT yet implemented!\n");
}



} // namespace CEUI
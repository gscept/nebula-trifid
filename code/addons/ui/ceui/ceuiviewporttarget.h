#pragma once
//------------------------------------------------------------------------------
/**
    @class CEUI::CEUIViewportTarget

    Wrapper for the cegui render target.

    (C) 2009 Radon Labs GmbH
*/
//------------------------------------------------------------------------------
#include "sui/ceui/ceuirendertarget.h"

namespace CEUI
{

class CEUIViewportTarget : public CEUIRenderTarget
{
public:
    /// constructor
    CEUIViewportTarget(const CEUIRenderer& owner);
    /// destructor
    virtual ~CEUIViewportTarget();
    //
    virtual bool isImageryCache() const;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CEUIViewportTarget::isImageryCache() const
{
    return false;
}
}
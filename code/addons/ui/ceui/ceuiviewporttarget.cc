//------------------------------------------------------------------------------
//  ceuiviewporttarget.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sui/ceui/ceuiviewporttarget.h"
#include "internalgraphics/internalgraphicsserver.h"
#include "internalgraphics/internalview.h"
#include "coregraphics/rendertarget.h"

namespace CEUI
{
    using namespace InternalGraphics;

//------------------------------------------------------------------------------
/**
*/
CEUIViewportTarget::CEUIViewportTarget(const CEUIRenderer& owner)  :
    CEUIRenderTarget(owner)
{
    const Ptr<InternalView>& view = InternalGraphicsServer::Instance()->GetDefaultView();
    const Ptr<CoreGraphics::RenderTarget> rt = view->GetRenderTarget();
    CEGUI::Rect area( CEGUI::Point(0,0), owner.getDisplaySize());

    setArea(area);
}

//------------------------------------------------------------------------------
/**
*/
CEUIViewportTarget::~CEUIViewportTarget()
{
}

} // namespace CEUI
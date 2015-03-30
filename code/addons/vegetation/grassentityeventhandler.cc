//------------------------------------------------------------------------------
//  grassheightmap.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vegetation/grassentityeventhandler.h"
#include "vegetation/grassrenderer.h"
#include "internalgraphics/internalgraphicsprotocol.h"

namespace Vegetation
{
__ImplementClass(Vegetation::GrassEntityEventHandler, 'GEHA', InternalGraphics::InternalGraphicsEntityHandler);

using namespace InternalGraphics;
//------------------------------------------------------------------------------
/**
*/
GrassEntityEventHandler::GrassEntityEventHandler()
{
}

//------------------------------------------------------------------------------
/**
*/
GrassEntityEventHandler::~GrassEntityEventHandler()
{
}

//------------------------------------------------------------------------------
/**
*/
bool 
GrassEntityEventHandler::HandleEvent(const Ptr<InternalGraphicsEntityEvent>& event)
{
    n_assert(event.isvalid());
    if (event->CheckId(InternalGraphics::ModelInstanceCreated::Id))
    {   
        const Ptr<InternalGraphicsEntity>& entity = event->GetGraphicsEntity();
        n_assert(entity.isvalid());
        n_assert(GrassRenderer::HasInstance());
        GrassRenderer::Instance()->SetupGrassLayersFromGraphicsEntity(entity.cast<InternalModelEntity>()); 

        return true;
    }
    return false;
}
}
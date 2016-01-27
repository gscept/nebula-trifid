//------------------------------------------------------------------------------
//  graphicsentityhandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicsprotocol.h"
#include "messaging/staticmessagehandler.h"
#include "graphics/graphicsentity.h"
#include "graphics/stage.h"
#include "models/nodes/statenodeinstance.h"
#include "models/nodes/animatornodeinstance.h"
#include "renderutil/nodelookuputil.h"

using namespace CoreGraphics;
using namespace Models;
using namespace Graphics;
using namespace Graphics;

namespace Messaging
{

//------------------------------------------------------------------------------
/**
*/
__Handler(GraphicsEntity, DiscardGraphicsEntity)
{
    Ptr<Stage> stage = obj->GetStage();
    n_assert(stage.isvalid());
    stage->RemoveEntity(obj);    
}

//------------------------------------------------------------------------------
/**
*/
__Handler(GraphicsEntity, UpdateTransform)
{
    obj->SetTransform(msg->GetTransform());
}

//------------------------------------------------------------------------------
/**
*/
__Handler(GraphicsEntity, SetVisibility)
{
    obj->SetVisible(msg->GetVisible());
}

//------------------------------------------------------------------------------
/**
    Dispatcher method (must be positioned after the handler methods to
    prevent automatic instantiation).
*/
__Dispatcher(GraphicsEntity)
{
    __Handle(GraphicsEntity, UpdateTransform);
    __Handle(GraphicsEntity, SetVisibility);
    __Handle(GraphicsEntity, DiscardGraphicsEntity);
    __HandleUnknown(Core::RefCounted);
}

} // namespace Messaging
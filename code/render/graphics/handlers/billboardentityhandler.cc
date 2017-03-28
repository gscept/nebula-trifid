//------------------------------------------------------------------------------
//  billboardentityhandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicsprotocol.h"
#include "messaging/staticmessagehandler.h"
#include "graphics/billboardentity.h"
#include "graphics/graphicsentity.h"
#include "../graphicsserver.h"
#include "../stage.h"

using namespace Billboards;
using namespace Graphics;
using namespace Graphics;

namespace Messaging
{

//------------------------------------------------------------------------------
/**
*/
__Handler(BillboardEntity, SetOverlayColor)
{
    obj->SetColor(msg->GetColor());
}


//------------------------------------------------------------------------------
/**
*/
__StaticHandler(CreateBillboardEntity)
{
    // create a new billboard entity
    Ptr<BillboardEntity> billboardEntity = BillboardEntity::Create();
    billboardEntity->SetTransform(msg->GetTransform());
    billboardEntity->SetVisible(msg->GetVisible());
    billboardEntity->SetTexture(msg->GetResourceId());
    billboardEntity->SetViewAligned(msg->GetViewAligned());
    billboardEntity->SetPickingId(msg->GetPickingId());
    billboardEntity->SetColor(msg->GetColor());

    // lookup stage and attach entity
    const Ptr<Stage>& stage = GraphicsServer::Instance()->GetStageByName(msg->GetStageName());
    stage->AttachEntity(billboardEntity.cast<GraphicsEntity>());

    // set return value
    msg->GetObjectRef()->Validate<BillboardEntity>(billboardEntity.get());
}

//------------------------------------------------------------------------------
/**
    Dispatcher method (must be positioned after the handler methods to
    prevent automatic instantiation).
*/
__Dispatcher(BillboardEntity)
{
    __Handle(BillboardEntity, SetOverlayColor);
    __HandleUnknown(GraphicsEntity);
}

} // namespace Messaging
//------------------------------------------------------------------------------
//  cameraentityhandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/cameraentity.h"
#include "graphics/graphicsserver.h"
#include "graphics/stage.h"
#include "graphics/view.h"
#include "messaging/staticmessagehandler.h"

using namespace Util;
using namespace Graphics;
using namespace Graphics;

namespace Messaging
{
//------------------------------------------------------------------------------
/**
*/
__StaticHandler(CreateCameraEntity)
{
    Ptr<CameraEntity> cameraEntity = CameraEntity::Create();
    cameraEntity->SetTransform(msg->GetTransform());
    cameraEntity->SetVisible(msg->GetVisible());
    cameraEntity->SetCameraSettings(msg->GetCameraSettings());

    // attach camera entity to stage
    const Ptr<Stage>& stage = GraphicsServer::Instance()->GetStageByName(msg->GetStageName());
    stage->AttachEntity(cameraEntity.cast<GraphicsEntity>());

    // set return value
    msg->GetObjectRef()->Validate<CameraEntity>(cameraEntity.get());
}

//------------------------------------------------------------------------------
/**
*/
__Handler(CameraEntity, AttachCameraToView)
{
    const StringAtom& viewName = msg->GetViewName();
    n_assert(GraphicsServer::Instance()->HasView(viewName));
    GraphicsServer::Instance()->GetViewByName(viewName)->SetCameraEntity(obj);
}

//------------------------------------------------------------------------------
/**
*/
__Handler(CameraEntity, RemoveCameraFromView)
{
    const StringAtom& viewName = msg->GetViewName();
    n_assert(GraphicsServer::Instance()->HasView(viewName));
    const Ptr<View>& view = GraphicsServer::Instance()->GetViewByName(viewName);
    if (view->GetCameraEntity().isvalid())
    {
        n_assert(view->GetCameraEntity() == obj);
        view->SetCameraEntity(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(CameraEntity, UpdateCameraEntity)
{
    obj->SetCameraSettings(msg->GetCameraSettings());
}

//------------------------------------------------------------------------------
/**
    Dispatcher method (must be positioned after the handler methods to
    prevent automatic instantiation).
*/
__Dispatcher(CameraEntity)
{
    __Handle(CameraEntity, AttachCameraToView)
    __Handle(CameraEntity, RemoveCameraFromView)
    __Handle(CameraEntity, UpdateCameraEntity);
    __HandleUnknown(GraphicsEntity);
}

} // namespace Messaging
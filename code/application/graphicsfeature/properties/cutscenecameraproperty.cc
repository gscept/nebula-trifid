//------------------------------------------------------------------------------
//  cutscenecameraproperty.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/cutscenecameraproperty.h"
#include "game/entity.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "graphics/cameraentity.h"
#include "graphics/graphicsserver.h"
#include "managers/focusmanager.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::CutsceneCameraProperty, 'CUCP', GraphicsFeature::CameraProperty);

using namespace Math;
using namespace Messaging;
using namespace BaseGameFeature;

//------------------------------------------------------------------------------
/**
*/
void
CutsceneCameraProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, BeginFrame);
    CameraProperty::SetupCallbacks();
}

//------------------------------------------------------------------------------
/**
*/
void
CutsceneCameraProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(UpdateTransform::Id);
    this->RegisterMessage(CutsceneControl::Id);
    CameraProperty::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
CutsceneCameraProperty::HandleMessage(const Ptr<Message>& msg)
{
    if (msg->CheckId(CutsceneControl::Id))
    {
        switch ((msg.cast<CutsceneControl>())->GetCommand())
        {
            case Start:
                this->StartCutscene();
                break;

            case Stop:
                this->StopCutscene();
                break;

            case Rewind:
                this->RewindCutscene();
                break;
        }
    }
    else if (msg->CheckId(UpdateTransform::Id))
    {
        this->UpdateCamera(this->entity->GetMatrix44(Attr::Transform));
    }
    else
    {
        CameraProperty::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CutsceneCameraProperty::OnActivate()
{
    CameraProperty::OnActivate();

    // tell our anim path property to stop playback (otherwise it would
    // start automatically)
    Ptr<AnimStop> animStop = AnimStop::Create();
    this->entity->SendSync(animStop.cast<Message>());
}

//------------------------------------------------------------------------------
/**
    Listen for debug keys to control animated cutscene cameras.
*/
void
CutsceneCameraProperty::OnBeginFrame()
{
    #if __ENABLE_DEBUGKEYS__
    if (FocusManager::Instance()->GetInputFocusEntity() == this->entity)
    {
        InputServer* inputServer = InputServer::Instance();
        if (inputServer->Keyboard()->KeyDown(InputCameraStop))
        {
            this->StopCutscene();
        }
        if (inputServer->Keyboard()->KeyDown(InputCameraPlay))
        {
            this->StartCutscene();
        }
        if (inputServer->Keyboard()->KeyDown(InputCameraRewind))
        {
            this->RewindCutscene();
        }
    }
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
CutsceneCameraProperty::StartCutscene()
{
    //// restart camera animation
    //Ptr<AnimRewind> animRewind = AnimRewind::Create();
    //this->entity->SendSync(animRewind.cast<Message>());
    //Ptr<AnimPlay> animPlay = AnimPlay::Create();
    //this->entity->SendSync(animPlay.cast<Message>());

    // grab camera focus
    FocusManager::Instance()->SetFocusEntity(this->entity);
}

//------------------------------------------------------------------------------
/**
*/
void
CutsceneCameraProperty::StopCutscene()
{
    Ptr<AnimStop> msg = AnimStop::Create();
    this->entity->SendSync(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
CutsceneCameraProperty::RewindCutscene()
{
    // restart camera animation
    Ptr<AnimRewind> animRewind = AnimRewind::Create();
    this->entity->SendSync(animRewind.cast<Message>());
    Ptr<AnimPlay> animPlay = AnimPlay::Create();
    this->entity->SendSync(animPlay.cast<Message>());

    // grab camera focus
    FocusManager::Instance()->SetFocusEntity(this->entity);
}

//------------------------------------------------------------------------------
/**
*/
void
CutsceneCameraProperty::UpdateCamera(const matrix44& cameraTransform)
{
    if (FocusManager::Instance()->GetCameraFocusEntity() == this->entity)
    {        
        n_assert(0 != this->cameraEntity);
        this->cameraEntity->SetTransform(cameraTransform);
    }
}

} // namespace Properties
//------------------------------------------------------------------------------
//  properties/cameraproperty.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/cameraproperty.h"
#include "managers/focusmanager.h"
#include "game/entity.h"
#include "graphics/graphicsserver.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "graphics/cameraentity.h"
#include "graphics/view.h"
#include "graphics/stage.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "faudio/audiolistener.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::CameraProperty, 'CAMP', Game::Property);

using namespace Game;
using namespace Math;
using namespace BaseGameFeature;

//------------------------------------------------------------------------------
/**
*/
CameraProperty::CameraProperty()
{
    this->cameraEntity = Graphics::CameraEntity::Create();
}

//------------------------------------------------------------------------------
/**
*/
CameraProperty::~CameraProperty()
{
    this->cameraEntity = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
CameraProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, Render);
}

//------------------------------------------------------------------------------
/**
*/
void
CameraProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(CameraFocus::Id);    
    Game::Property::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
CameraProperty::OnStart()
{
    Property::OnStart();

    if (this->entity->GetBool(Attr::CameraFocus))
    {
        FocusManager::Instance()->SetCameraFocusEntity(this->entity);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CameraProperty::OnDeactivate()
{
    // clear camera focus, if we are the camera focus object
    if (this->HasFocus())
    {
        FocusManager::Instance()->SetCameraFocusEntity(0);
    }
    
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
CameraProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(msg != 0);
    
    if (msg->CheckId(CameraFocus::Id))
    {
        const Ptr<CameraFocus>& focusMsg = msg.downcast<CameraFocus>();
        if (focusMsg->GetObtainFocus())
        {
            this->OnObtainCameraFocus();
        }
        else
        {
            this->OnLoseCameraFocus();
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is called by the FocusManager when our entity gains the
    camera focus. Override this method if your subclass needs to do 
    some initialization when gaining the camera focus.
*/
void
CameraProperty::OnObtainCameraFocus()
{
    // update focus attribute
    this->entity->SetBool(Attr::CameraFocus, true);
    this->cameraEntity->SetTransform(this->GetEntity()->GetMatrix44(Attr::Transform));
    
    this->defaultStage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();
    this->defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
	this->defaultStage->AttachEntity(this->cameraEntity.cast<Graphics::GraphicsEntity>());
    this->defaultView->SetCameraEntity(this->cameraEntity);
}

//------------------------------------------------------------------------------
/**
    This method is called by the FocusManager when our entity loses
    the camera focus. Override this method if your subclass needs to do any
    cleanup work here.
*/
void
CameraProperty::OnLoseCameraFocus()
{
    // clear cam in default view
    if (this->defaultView->GetCameraEntity() == this->cameraEntity)
    {
        this->defaultView->SetCameraEntity(0);
    }
	if (this->cameraEntity->IsActive())
	{
		this->defaultStage->RemoveEntity(this->cameraEntity.cast<Graphics::GraphicsEntity>());
	}    
    this->defaultStage = 0;
    this->defaultView = 0;

    // update focus attribute
    this->entity->SetBool(Attr::CameraFocus, false);
}

//------------------------------------------------------------------------------
/**
    This method returns true if our entity has the camera focus. This 
    implementation makes sure that 2 properties cannot report that they
    have the camera focus by accident.
*/
bool
CameraProperty::HasFocus() const
{
    return this->entity->GetBool(Attr::CameraFocus);
}

//------------------------------------------------------------------------------
/**
    Called before camera is "rendered". The default camera properties
    applies shake effects to the camera.
*/
void
CameraProperty::OnRender()
{
	// do just, if we got focus
    if (FocusManager::Instance()->GetCameraFocusEntity() == this->entity)
    {                
		/* ========================================================================================= NEB 2 STUFF 
		TODO!?!?! shaker effect stuff 

		this->shakeEffectHelper.SetCameraTransform(camera->GetTransform());
        this->shakeEffectHelper.Update();
        camera->SetTransform(this->shakeEffectHelper.GetShakeCameraTransform());
        
        // if enity has transform set the current position between camera and entity as audio listener position
        // otherwise only use camera transform

        =========================================================================================== NEB 2 STUFF */

        // update audio
        this->UpdateAudioListenerPosition();       

		// get transform
		const Math::matrix44& trans = this->entity->GetMatrix44(Attr::Transform);

        // set point of interest in post effect manager
        PostEffect::PostEffectManager::Instance()->SetPointOfInterest(trans.get_position());

        // apply transform		
        this->cameraEntity->SetTransform(trans);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CameraProperty::UpdateAudioListenerPosition() const
{
    matrix44 transform = this->cameraEntity->GetTransform();
    if (this->GetEntity()->HasAttr(Attr::Transform))
    {
        const matrix44& enityTransform = this->GetEntity()->GetMatrix44(Attr::Transform);
        transform.translate((enityTransform.get_position() - transform.get_position()) * 0.5f);
    }
    FAudio::AudioListener::Instance()->SetTransform(transform);
}
}; // namespace GraphicsFeature

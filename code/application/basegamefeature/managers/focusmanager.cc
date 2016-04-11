//------------------------------------------------------------------------------
//  managers/focusmanager.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "managers/focusmanager.h"
#include "properties/inputproperty.h"
#include "properties/cameraproperty.h"
#include "managers/entitymanager.h"
#include "game/entity.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "input/keyboard.h"
#include "input/gamepad.h"
#include "input/inputserver.h"

namespace BaseGameFeature
{
__ImplementClass(FocusManager, 'MFOC', Game::Manager);
__ImplementSingleton(FocusManager);

using namespace Input;
using namespace Game;
using namespace GraphicsFeature;

//------------------------------------------------------------------------------
/**
*/
FocusManager::FocusManager()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FocusManager::~FocusManager()
{
    n_assert(!this->inputFocusEntity.isvalid());
    n_assert(!this->cameraFocusEntity.isvalid());
    n_assert(!this->newInputFocusEntity.isvalid());
    n_assert(!this->newCameraFocusEntity.isvalid());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Sets the input and camera focus to the given entity. The entity pointer
    may be 0 to clear the input and camera focus. The entity must have both
    a InputProperty and CameraProperty attached, otherwise the method
    will fail.
*/
void
FocusManager::SetFocusEntity(const Ptr<Entity>& entity, bool distribute)
{
	this->SetInputFocusEntity(entity, distribute);
	this->SetCameraFocusEntity(entity, distribute);
}

//------------------------------------------------------------------------------
/**
    Returns the current common focus entity. This method will fail if the
    current input focus entity and camera focus entity are not the same. The
    method may return 0 if there is no current focus entity.
*/
const Ptr<Entity>&
FocusManager::GetFocusEntity() const
{    
    n_assert(this->cameraFocusEntity == this->inputFocusEntity);
    return this->cameraFocusEntity;
    
}

//------------------------------------------------------------------------------
/**
    General "set focus to next entity". Can be camera and/or input focus.
*/
void
FocusManager::SetToNextEntity(bool cameraFocus, bool inputFocus)
{
    n_assert(cameraFocus || inputFocus);

    // get array of active entities
    const Util::Array<Ptr<Entity> >& entityArray = EntityManager::Instance()->GetEntities();

    // get start entity
    Util::Array<Ptr<Entity> >::Iterator iter = entityArray.Begin();
    if (cameraFocus)
    {
        if (this->cameraFocusEntity.isvalid())
        {
            iter = entityArray.Find(this->cameraFocusEntity);
        }
    }
    else
    {
        if (this->inputFocusEntity.isvalid())
        {
            iter = entityArray.Find(this->inputFocusEntity);
        }
    }
    Util::Array<Ptr<Entity> >::Iterator start = iter;
    if (iter) do
    {
        iter++;

        // wrap around
        if (iter == entityArray.End())
        {
            iter = entityArray.Begin();
        }
        Entity* entity = *iter;

        if (cameraFocus && entity->AcceptsMessage(CameraFocus::Id) &&
            inputFocus  && entity->AcceptsMessage(InputFocus::Id))
        {
            this->SetFocusEntity(entity);
            return;
        }
        else if (cameraFocus && entity->AcceptsMessage(CameraFocus::Id) && (!inputFocus))
        {
            this->SetCameraFocusEntity(entity);
            return;
        }
        else if (inputFocus && entity->AcceptsMessage(InputFocus::Id) && (!cameraFocus))
        {
            this->SetInputFocusEntity(entity);
            return;
        }
    }
    while (iter != start);
}

//------------------------------------------------------------------------------
/**
    Set focus to next entity which has both an InputProperty and
    CameraProperty attached. If no current focus entity exists, the method
    will start to iterate with the first entity. The method will wrap around. 
    The method will return false if no entities exist which have both an 
    InputProperty and CameraProperty attached.
*/
void
FocusManager::SetFocusToNextEntity()
{
    this->SetToNextEntity(true, true);
}

//------------------------------------------------------------------------------
/**
    This method is called once per frame.
*/
void
FocusManager::OnFrame()
{
#if !PUBLIC_BUILD
    // allow focus entity switch in debug mode
    const Ptr<InputServer>& inputServer = InputServer::Instance();
    const Ptr<Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
    const Ptr<GamePad>& pad = inputServer->GetDefaultGamePad(0);

    if (keyboard.isvalid() && keyboard->KeyDown(Key::Tab))
        //|| (pad.isvalid() && pad->ButtonDown(GamePad::LeftThumbButton)))
    {
        if (keyboard.isvalid() && keyboard->KeyPressed(Key::Control))
        {
            // only camera focus change
            this->SetCameraFocusToNextEntity();
        }
        else
        {
            this->SetFocusToNextEntity();
        }
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Actually switch focus entities. A focus entity switch doesn't happen
    immediately, but only once per frame. This is to prevent chain-reactions
    and circular reactions when 2 or more entities think they have the
    focus in a single frame.
*/
void
FocusManager::SwitchFocusEntities()
{
    if (this->newInputFocusEntity.isvalid())
    {
        this->inputFocusEntity = this->newInputFocusEntity;
        this->newInputFocusEntity = 0;
        Ptr<InputFocus> inputFocus = InputFocus::Create();
        inputFocus->SetObtainFocus(true);
        this->inputFocusEntity->SendSync(inputFocus.upcast<Messaging::Message>());
        n_assert(this->inputFocusEntity->GetBool(Attr::InputFocus) == true);
    }
    if (this->newCameraFocusEntity.isvalid())
    {
        this->cameraFocusEntity = this->newCameraFocusEntity;
        this->newCameraFocusEntity = 0;
        Ptr<CameraFocus> cameraFocus = CameraFocus::Create();
        cameraFocus->SetObtainFocus(true);
        this->cameraFocusEntity->SendSync(cameraFocus.upcast<Messaging::Message>());
        n_assert(this->cameraFocusEntity->GetBool(Attr::CameraFocus) == true);
    }
}

//------------------------------------------------------------------------------
/**
    Set input focus entity to the given entity. The entity pointer can be
    0, this will clear the current input focus. The entity must have
    an InputProperty attached for this to work.
*/
void
FocusManager::SetInputFocusEntity(const Ptr<Entity>& entity, bool distribute)
{
    // clear input focus on existing focus entity
    if (this->inputFocusEntity.isvalid())
    {
        Ptr<InputFocus> inputFocus = InputFocus::Create();
        inputFocus->SetObtainFocus(false);
		inputFocus->SetDistribute(distribute);
        this->inputFocusEntity->SendSync(inputFocus.upcast<Messaging::Message>());
        n_assert(this->inputFocusEntity->GetBool(Attr::InputFocus) == false);
    }

    this->inputFocusEntity = 0;
    this->newInputFocusEntity = entity;

    // immediately update focus entities
    this->SwitchFocusEntities();
}

//------------------------------------------------------------------------------
/**
    Get current input focus entity. This method may return 0 if no input
    focus entity is set.
*/
const Ptr<Entity>&
FocusManager::GetInputFocusEntity() const
{
    return this->inputFocusEntity;
}

//------------------------------------------------------------------------------
/**
    Set input focus to the next entity which has an InputProperty attached.
*/
void
FocusManager::SetInputFocusToNextEntity()
{
    this->SetToNextEntity(false, true);
}

//------------------------------------------------------------------------------
/**
    Set camera focus entity to the given entity. The entity pointer can be
    0, this will clear the current camera focus. The entity must have
    a CameraProperty attached for this to work.
*/
void
FocusManager::SetCameraFocusEntity(const Ptr<Entity>& entity, bool distribute)
{
    // clear camera focus on existing focus entity
    if (this->cameraFocusEntity.isvalid())
    {
        Ptr<CameraFocus> cameraFocus = CameraFocus::Create();
        cameraFocus->SetObtainFocus(false);
		cameraFocus->SetDistribute(distribute);
        this->cameraFocusEntity->SendSync(cameraFocus.upcast<Messaging::Message>());
        n_assert(this->cameraFocusEntity->GetBool(Attr::CameraFocus) == false);
    }

    this->cameraFocusEntity = 0;
    this->newCameraFocusEntity = entity;

    // immediately update focus entities
    this->SwitchFocusEntities();
}

//------------------------------------------------------------------------------
/**
    Get current camera focus entity. This method may return 0 if no input
    focus entity is set.
*/
const Ptr<Entity>&
FocusManager::GetCameraFocusEntity() const
{
    return this->cameraFocusEntity;
}

//------------------------------------------------------------------------------
/**
    Set camera focus to next entity which has a CameraProperty attached.
*/
void
FocusManager::SetCameraFocusToNextEntity()
{
    this->SetToNextEntity(true, false);
}

}; // namespace Managers

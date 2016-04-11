#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::FocusManager

    The FocusManager singleton object manages the global input and camera focus
    entities. There may only be one input and camera focus entity at any 
    time, the input focus entity can be different from the camera focus entity.

    The input focus entity will be the entity which receives input, the camera
    focus entity will be the entity which may manipulate the camera.

    The FocusManager requires an EntityManager to iterate through existing
    entities, and works only on game entities, which have the InputProperty
    and/or CameraProperty (or a derived class thereof) attached.

    Please note that an actual focus switch will happen only once per-frame.
    This is to avoid chain-reactions when 2 or more objects per frame 
    think they currently have the input focus.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "game/entity.h"
#include "core/ptr.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class FocusManager : public Game::Manager
{
	__DeclareClass(FocusManager);
    __DeclareSingleton(FocusManager);

public:
    /// constructor
    FocusManager();
    /// destructor
    virtual ~FocusManager();
    /// trigger the focus manager once a frame, actual focus switches will happen here
    virtual void OnFrame();

    /// set input and camera focus to entity, null ptr allowed
	virtual void SetFocusEntity(const Ptr<Game::Entity>& entity, bool distribute = true);
    /// get common focus entity, will fail if input != camera focus entity
    virtual const Ptr<Game::Entity>& GetFocusEntity() const;
    /// switch input and camera focus to next entity
    virtual void SetFocusToNextEntity();

    /// set the current input focus entity, null ptr allowed
	virtual void SetInputFocusEntity(const Ptr<Game::Entity>& entity, bool distribute = true);
    /// get the current input focus entity
    virtual const Ptr<Game::Entity>& GetInputFocusEntity() const;
    /// set input focus to next entity
    virtual void SetInputFocusToNextEntity();

    /// set the current camera focus entity, null ptr allowed
	virtual void SetCameraFocusEntity(const Ptr<Game::Entity>& entity, bool distribute = true);
    /// get the current camera focus entity
    virtual const Ptr<Game::Entity>& GetCameraFocusEntity() const;
    /// set camera focus to next entity
    virtual void SetCameraFocusToNextEntity();

protected:
    /// actually switch focus entities
    void SwitchFocusEntities();

private:
    /// generalized 'set focus to next entity' method
    void SetToNextEntity(bool cameraFocus, bool inputFocus);    

    Ptr<Game::Entity> inputFocusEntity;
    Ptr<Game::Entity> cameraFocusEntity;
    Ptr<Game::Entity> newInputFocusEntity;
    Ptr<Game::Entity> newCameraFocusEntity;
};

}; // namespace BaseGameFeature
//------------------------------------------------------------------------------



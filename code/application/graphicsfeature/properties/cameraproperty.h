#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::CameraProperty

    A camera property adds the ability to manipulate the camera to an entity.
    Please note that more advanced camera properties should always be 
    derived from the class camera property if camera focus handling is desired,
    since the FocusManager will only work on game entities which have
    a CameraProperty (or a subclass) attached.

    It is completely ok though to handle camera manipulation in a property
    not derived from CameraProperty, but please be aware that the
    FocusManager will ignore those.

    The camera property will generally 
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"


namespace Graphics
{
    class CameraEntity;
    class View;
    class Stage;
}

namespace GraphicsFeature
{
class CameraProperty : public Game::Property
{
	__DeclareClass(CameraProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    CameraProperty();
    /// destructor
    virtual ~CameraProperty();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// setup accepted messages
    virtual void SetupAcceptedMessages();
    /// setup callbacks for this property
    virtual void SetupCallbacks();    
    /// called from within Entity::OnStart() after OnLoad when the complete world exist
    virtual void OnStart();
    /// called when camera focus is obtained
    virtual void OnObtainCameraFocus();
    /// called when camera focus is lost
    virtual void OnLoseCameraFocus();
    /// called before rendering happens
    virtual void OnRender();
    /// return true if currently has camera focus
    virtual bool HasFocus() const;    
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:

    /// update audio listener position
    void UpdateAudioListenerPosition() const;

    Ptr<Graphics::CameraEntity> cameraEntity;
    Ptr<Graphics::View> defaultView;
    Ptr<Graphics::Stage> defaultStage;
};
__RegisterClass(CameraProperty);

}; // namespace GraphicsFeature
//------------------------------------------------------------------------------

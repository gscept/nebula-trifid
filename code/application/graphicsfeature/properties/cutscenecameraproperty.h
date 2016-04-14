#pragma once
//------------------------------------------------------------------------------
/**
    @class Properties::CutsceneCameraProperty
    
    A specialized camera property for ingame-cutscenes.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "properties/cameraproperty.h"
#include "graphics/cameraentity.h"

//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class CutsceneCameraProperty : public CameraProperty
{
    __DeclareClass(CutsceneCameraProperty);
public:
    /// command enum
    enum Command
    {
        Start,
        Stop,
        Rewind,
    };

    /// setup callback methods
    virtual void SetupCallbacks();
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called on begin of frame
    virtual void OnBeginFrame();

protected:
    /// start the cutscene
    virtual void StartCutscene();
    /// stop the cutscene
    virtual void StopCutscene();
    /// rewind the cutscene
    virtual void RewindCutscene();
    /// update graphics subsystem camera
    virtual void UpdateCamera(const Math::matrix44& m);
};
__RegisterClass(CutsceneCameraProperty);

} // namespace GraphicsFeature

//------------------------------------------------------------------------------

    
#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolki::N3ViewerApp
    
    Application class for the remote-controllable Nebula3 standard viewer.
    
    (C) 2009 Radon Labs GmbH
*/
#include "apprender/viewerapplication.h"
#include "remote/remoteinterface.h"
#include "remote/remotecontrolproxy.h"
#include "resources/resourceid.h"
#include "graphics/modelentity.h"
#include "graphics/globallightentity.h"
#include "graphics/spotlightentity.h"
#include "n3viewer/n3viewerremotecommandhandler.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class N3ViewerApp : public App::ViewerApplication
{
    __DeclareSingleton(N3ViewerApp);
public:
    /// constructor
    N3ViewerApp();
    /// destructor
    virtual ~N3ViewerApp();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();

    /// setup the light stage
    void SetupLights();
    /// discard lights in the scene
    void DiscardLights();
    /// load an object 
    void LoadModel(const Resources::ResourceId& resId);
    /// apply skin to character model of specified ressource ID
    void LoadSkin(const Resources::ResourceId& modelResId, const Resources::ResourceId& skinResId);
    /// remove skin from character model of specified ressource ID
    void RemoveSkin(const Resources::ResourceId& modelResId, const Resources::ResourceId& skinResId);
    /// replace animation of character model of specified ressource ID
    void LoadAnimation(const Resources::ResourceId& modelResId, const Resources::ResourceId& animResId);
	/// stops animations for the model entity
	void StopAnimation(const Resources::ResourceId& modelResId);
    /// replace variation of character model of specified ressource ID
    void LoadVariation(const Resources::ResourceId& modelResId, const Resources::ResourceId& varResId);
    /// clear models in the scene
    void DiscardModels();

protected:
    /// called to configure display (sets shd: assign to home directory, not project directory)
    virtual void OnConfigureDisplay();
    /// process input (called before rendering)
    virtual void OnProcessInput();
    /// handle per-frame stuff
    virtual void OnUpdateFrame();
	/// handle returned messages
	virtual void HandleReceivedMessages();
    /// query the project root path from the registry (if Radon Labs Toolkit is installed)
    Util::String QueryProjectPathFromRegistry() const;

    /// perform setup from command line args
    void SetupSceneFromCmdLineArgs();

	Math::float4 globalColor;
    Ptr<Remote::RemoteInterface> remoteInterface;
    Ptr<Remote::RemoteControlProxy> remoteControlProxy;
    Util::Array<Ptr<Graphics::ModelEntity> > modelEntities;
    Ptr<Graphics::GlobalLightEntity> globalLightEntity;
	Ptr<Graphics::SpotLightEntity> keyLight;
	Ptr<Graphics::SpotLightEntity> backLight;
	Ptr<Graphics::SpotLightEntity> fillLight;

	Util::Stack<Ptr<Graphics::GraphicsEntityMessage> > deferredMessages;

	int skinIterator;
	int clipIterator;
	Util::Array<Util::StringAtom> skins;
	Util::Array<Util::StringAtom> clips;
};

} // namespace Toolkit
//------------------------------------------------------------------------------

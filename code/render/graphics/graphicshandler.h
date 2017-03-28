#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::GraphicsHandler
    
    Runs in the graphics thread context, setup the graphics runtime environment
    and processes messages to the graphics thread.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file	
*/
#include "interface/interfacehandlerbase.h"
#include "messaging/message.h"
#include "graphics/graphicsprotocol.h"
#include "debugrender/debugrenderprotocol.h"
#include "io/console.h"
#include "io/ioserver.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shaperenderer.h"
#include "coregraphics/vertexlayoutserver.h"
#include "resources/resourcemanager.h"
#include "models/modelserver.h"
#include "graphics/graphicsserver.h"
#include "lighting/lightserver.h"
#include "lighting/shadowserver.h"
#include "frame2/frameserver.h"
#include "threading/safeflag.h"
#include "animation/animeventserver.h"
#include "debug/debugtimer.h"
#include "coregraphics/textrenderer.h"
#include "http/httpserverproxy.h"
#include "http/httpclientregistry.h"
#include "characters/characterserver.h"
#include "coregraphics/mouserenderdevice.h"
#include "framesync/framesynctimer.h"
#include "particles/particleserver.h"
#include "materials/materialserver.h"
#include "instancing/instanceserver.h"
#include "picking/pickingserver.h"
#include "environment/environmentserver.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class GraphicsHandler : public Interface::InterfaceHandlerBase
{
    __DeclareClass(GraphicsHandler);
public:
    /// constructor
    GraphicsHandler();
    /// destructor
    virtual ~GraphicsHandler();

    /// open the handler
    virtual void Open();
    /// close the handler
    virtual void Close();
    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);
    /// do per-frame work
    virtual void DoWork();

	/// set if the graphics handler should use multithreaded rendering
	void SetMultithreadedRendering(bool b);

    /// wait for pending resources, this method is called in the main thread context!
    void WaitForPendingResources();

private:
    /// setup the required graphics runtime
    void SetupGraphicsRuntime(const Ptr<Graphics::SetupGraphics>& msg);
    /// shutdown the graphics runtime
    void ShutdownGraphicsRuntime();
    /// handle SetupGraphics message
    void OnSetupGraphics(const Ptr<Graphics::SetupGraphics>& msg);
	/// handle SetupWindow message
	void OnSetupWindow(const Ptr<Graphics::SetupWindow>& msg);
	/// handle UpdateDisplay message
	void OnUpdateWindow(const Ptr<Graphics::UpdateWindow>& msg);
    /// handle AdapterExists message
    void OnAdapterExists(const Ptr<Graphics::AdapterExists>& msg);
    /// handle GetAvailableDisplayModes message
    void OnGetAvailableDisplayModes(const Ptr<Graphics::GetAvailableDisplayModes>& msg);
    /// handle GetCurrentAdapterDisplayMode message
    void OnGetCurrentAdapterDisplayMode(const Ptr<Graphics::GetCurrentAdapterDisplayMode>& msg);
    /// handle GetAdapterInfo message
    void OnGetAdapterInfo(const Ptr<Graphics::GetAdapterInfo>& msg);
    /// handle AttachDisplayEventHandler message
    void OnAttachDisplayEventHandler(const Ptr<Graphics::AttachDisplayEventHandler>& msg);
    /// handle RemoveDisplayEventHandler message
    void OnRemoveDisplayEventHandler(const Ptr<Graphics::RemoveDisplayEventHandler>& msg);
    /// handle AttachRenderEventHandler message
    void OnAttachRenderEventHandler(const Ptr<Graphics::AttachRenderEventHandler>& msg);
    /// handle RemoveRenderEventHandler message
    void OnRemoveRenderEventHandler(const Ptr<Graphics::RemoveRenderEventHandler>& msg);
    /// handle CreateGraphicsStage message
    void OnCreateGraphicsStage(const Ptr<Graphics::CreateGraphicsStage>& msg);
    /// handle a DiscardGraphicsStage message
    void OnDiscardGraphicsStage(const Ptr<Graphics::DiscardGraphicsStage>& msg);
    /// handle CreateGraphicsView message
    void OnCreateGraphicsView(const Ptr<Graphics::CreateGraphicsView>& msg);
    /// handle DiscardGraphicsView message
    void OnDiscardGraphicsView(const Ptr<Graphics::DiscardGraphicsView>& msg);
    /// sets the default graphics view
    void OnSetDefaultGraphicsView(const Ptr<SetDefaultGraphicsView>& msg);
    /// on update shared variable
    bool OnUpdateSharedShaderVariable(const Ptr<Graphics::UpdateSharedShaderVariable>& msg);
    /// handle PreloadMousePointerTextures
    void OnPreloadMousePointerTextures(const Ptr<Graphics::PreloadMousePointerTextures>& msg);
    /// handle UpdateMousePointers
    void OnUpdateMousePointers(const Ptr<Graphics::UpdateMousePointers>& msg);
    /// set time factor of frame sync timer
    void OnSetTimeFactor(const Ptr<Graphics::SetTimeFactor>& msg);
    /// set shadow point of interest
    void OnSetShadowPointOfInterest(const Ptr<Graphics::SetShadowPointOfInterest>& msg);

    /// handle register new animeventhandler
    void OnRegisterAnimEventHandler(const Ptr<Graphics::RegisterAnimEventHandler>& msg);
    /// handle unregister new animeventhandler
    void OnUnregisterAnimEventHandler(const Ptr<Graphics::UnregisterAnimEventHandler>& msg);

    /// handle RegisterRTPlugin
    void OnRegisterRTPlugin(const Ptr<Graphics::RegisterRTPlugin>& msg);
    /// handle UnregisterRTPlugin
    void OnUnregisterRTPlugin(const Ptr<Graphics::UnregisterRTPlugin>& msg);

    bool isGraphicsRuntimeValid;
	bool useMultithreadedRendering;
    Ptr<FrameSync::FrameSyncTimer> frameSyncTimer;

    Ptr<IO::IoServer> ioServer;
    Ptr<CoreGraphics::RenderDevice> renderDevice;
    Ptr<CoreGraphics::DisplayDevice> displayDevice;
    Ptr<CoreGraphics::TransformDevice> transformDevice;
    Ptr<CoreGraphics::ShaderServer> shaderServer;
    Ptr<CoreGraphics::ShapeRenderer> shapeRenderer;
    Ptr<CoreGraphics::VertexLayoutServer> vertexLayoutServer;
    Ptr<CoreGraphics::TextRenderer> textRenderer;
    Ptr<CoreGraphics::MouseRenderDevice> mouseRenderDevice;
    Ptr<Resources::ResourceManager> resourceManager;
    Ptr<Models::ModelServer> modelServer;
    Ptr<Graphics::GraphicsServer> graphicsServer;
    Ptr<Lighting::LightServer> lightServer;
    Ptr<Lighting::ShadowServer> shadowServer;
	Ptr<Instancing::InstanceServer> instanceServer;
	Ptr<Frame2::FrameServer> frame2Server;
	Ptr<Materials::MaterialServer> materialServer;
    Ptr<Animation::AnimEventServer> animEventServer;
    Ptr<Characters::CharacterServer> characterServer;
    Ptr<Particles::ParticleServer> particleServer;
    
    _declare_timer(GraphicsFrameTime);

#if __NEBULA3_HTTP__
    Ptr<Http::HttpServerProxy> httpServerProxy;
#endif
#if __NEBULA3_HTTP_FILESYSTEM__
    Ptr<Http::HttpClientRegistry> httpClientRegistry;
#endif 

};


//------------------------------------------------------------------------------
/**
*/
inline void 
GraphicsHandler::SetMultithreadedRendering( bool b )
{
	this->useMultithreadedRendering = b;
}

} // namespace GraphicsHandler
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// framecapturerendermodule.cc
// (C) 2010 Radon Labs GmbH
// (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framecapturerendermodule.h"
#include "framecapture/rt/framecapturertplugin.h"
#include "graphics/graphicsinterface.h"
#include "graphics/graphicsprotocol.h"

using namespace Graphics;
using namespace Messaging;

namespace FrameCapture
{
__ImplementClass(FrameCapture::FrameCaptureRenderModule,'FCRM',RenderModules::RenderModule);
__ImplementSingleton(FrameCapture::FrameCaptureRenderModule);
//------------------------------------------------------------------------------
/**
    Constructor	
*/
FrameCaptureRenderModule::FrameCaptureRenderModule()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
    Destructor	
*/
FrameCaptureRenderModule::~FrameCaptureRenderModule()
{
    __DestructSingleton;
}
//------------------------------------------------------------------------------
/**
    Setup render module	
*/
void
FrameCaptureRenderModule::Setup()
{
    n_assert(!this->IsValid());
    RenderModule::Setup();

    // Register the frame capure render-thread plugin
    Ptr<RegisterRTPlugin> msg = RegisterRTPlugin::Create();
    msg->SetType(&FrameCaptureRTPlugin::RTTI);
    GraphicsInterface::Instance()->Send(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
    Discard render module	
*/
void
FrameCaptureRenderModule::Discard()
{
    n_assert(this->IsValid());
    RenderModule::Discard();

    // Unregister the frame capture render-thread plugin
    Ptr<Graphics::UnregisterRTPlugin> msg = UnregisterRTPlugin::Create();
    msg->SetType(&FrameCaptureRTPlugin::RTTI);
    GraphicsInterface::Instance()->Send(msg.cast<Message>());
}
} // namespace FrameCapture

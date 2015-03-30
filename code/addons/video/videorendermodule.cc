//------------------------------------------------------------------------------
// videorendermodule.cc
// (C) Radon Labs GmbH
// (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "graphics/graphicsinterface.h"
#include "graphics/graphicsprotocol.h"
#include "video/rt/videortplugin.h"
#include "video/videorendermodule.h"

namespace Video
{
__ImplementClass(Video::VideoRenderModule,'VIRM',RenderModules::RenderModule);
__ImplementSingleton(Video::VideoRenderModule);

using namespace Graphics;
using namespace Messaging;
using namespace RenderModules;

//------------------------------------------------------------------------------
/**
    Constructor	
*/
VideoRenderModule::VideoRenderModule()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
    Destructor	
*/
VideoRenderModule::~VideoRenderModule()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Setup the render module	
*/
void
VideoRenderModule::Setup()
{
    n_assert(!this->IsValid());
    RenderModule::Setup();

    // Register the video render-thread plugin
    Ptr<RegisterRTPlugin> msg = RegisterRTPlugin::Create();
    msg->SetType(&VideoRTPlugin::RTTI);
    GraphicsInterface::Instance()->Send(msg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
    Discards the render module	
*/
void
VideoRenderModule::Discard()
{
    n_assert(this->IsValid());
    RenderModule::Discard();

    // Unregister the video render-thread plugin
    Ptr<UnregisterRTPlugin> msg = UnregisterRTPlugin::Create();
    msg->SetType(&VideoRTPlugin::RTTI);
    GraphicsInterface::Instance()->Send(msg.cast<Message>());
}

}
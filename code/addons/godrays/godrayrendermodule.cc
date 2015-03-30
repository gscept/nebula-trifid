//------------------------------------------------------------------------------
//  godrayrendermodule.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "godrayrendermodule.h"
#include "rt/godrayrtplugin.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "graphics/graphicsserver.h"
#include "godrayprotocol.h"

namespace Godrays
{
__ImplementClass(Godrays::GodrayRenderModule, 'GRRM', RenderModules::RenderModule);
__ImplementSingleton(GodrayRenderModule);

using namespace Graphics;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
GodrayRenderModule::GodrayRenderModule()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GodrayRenderModule::~GodrayRenderModule()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayRenderModule::Setup()
{
	n_assert(!this->IsValid());
	RenderModule::Setup();

	// register the godray render-thread plugin
	Ptr<RegisterRTPlugin> msg = RegisterRTPlugin::Create();
	msg->SetType(&GodrayRTPlugin::RTTI);
	__StaticSendWait(GraphicsInterface, msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayRenderModule::Discard()
{
	n_assert(this->IsValid());
	RenderModule::Discard();

	// unregister the godray render-thread plugin
	Ptr<UnregisterRTPlugin> msg = UnregisterRTPlugin::Create();
	msg->SetType(&GodrayRTPlugin::RTTI);
	__StaticSendWait(GraphicsInterface, msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayRenderModule::SetSunTexture( const Resources::ResourceId& res )
{
	n_assert(this->IsValid());

	Ptr<Godrays::SetSunTexture> msg = Godrays::SetSunTexture::Create();
	msg->SetTexture(res);
	__StaticSendWait(GraphicsInterface, msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayRenderModule::SetSunColor( const Math::float4& color )
{
	n_assert(this->IsValid());

	Ptr<Godrays::SetSunColor> msg = Godrays::SetSunColor::Create();
	msg->SetColor(color);
	__StaticSendWait(GraphicsInterface, msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayRenderModule::SetSunLightRelative( bool b )
{
	n_assert(this->IsValid());

	Ptr<Godrays::SetSunGlobalLightRelative> msg = Godrays::SetSunGlobalLightRelative::Create();
	msg->SetIsRelative(b);
	__StaticSendWait(GraphicsInterface, msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayRenderModule::SetSunPosition( const Math::float4& pos )
{
	n_assert(this->IsValid());

	Ptr<Godrays::SetSunPosition> msg = Godrays::SetSunPosition::Create();
	msg->SetPosition(pos);
	__StaticSendWait(GraphicsInterface, msg);
}
} // namespace Godrays
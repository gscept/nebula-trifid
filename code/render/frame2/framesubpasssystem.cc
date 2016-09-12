//------------------------------------------------------------------------------
// framesubpasssystem.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framesubpasssystem.h"
#include "lighting/lightserver.h"
#include "rendermodules/rt/rtpluginregistry.h"
#include "coregraphics/textrenderer.h"
#include "coregraphics/shaperenderer.h"
#include "lighting/shadowserver.h"

using namespace CoreGraphics;
using namespace Lighting;
namespace Frame2
{

__ImplementClass(Frame2::FrameSubpassSystem, 'FRSS', Frame2::FrameOp);
//------------------------------------------------------------------------------
/**
*/
FrameSubpassSystem::FrameSubpassSystem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameSubpassSystem::~FrameSubpassSystem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
FrameSubpassSystem::Setup()
{

}

//------------------------------------------------------------------------------
/**
	Perhaps this should also launch RT plugins, but RT plugins really need to 
	be manageable as to where they should be rendered.
*/
void
FrameSubpassSystem::Run(const IndexT frameIndex)
{
	switch (this->call)
	{
	case Lights:
		LightServer::Instance()->RenderLights();
		break;
	case LightProbes:
		LightServer::Instance()->RenderLightProbes();
		break;
	case LocalShadowsSpot:
		ShadowServer::Instance()->UpdateSpotLightShadowBuffers();
		break;
	case LocalShadowsPoint:
		ShadowServer::Instance()->UpdatePointLightShadowBuffers();
		break;
	case GlobalShadows:
		ShadowServer::Instance()->UpdateGlobalLightShadowBuffers();
		break;
	case UI:
		//RenderModules::RTPluginRegistry::Instance()->OnRender(FrameBatchType::UI);
		break;
	case Text:
		TextRenderer::Instance()->DrawTextElements();
		break;
	case Shapes:
		ShapeRenderer::Instance()->DrawShapes();
		break;
	}
}

} // namespace Frame2
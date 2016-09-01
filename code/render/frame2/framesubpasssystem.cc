//------------------------------------------------------------------------------
// framesubpasssystem.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framesubpasssystem.h"
#include "lighting/lightserver.h"

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
	case UI:
		break;
	case Text:
		break;
	}
}

} // namespace Frame2
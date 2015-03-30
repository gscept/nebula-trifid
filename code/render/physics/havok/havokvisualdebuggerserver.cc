//------------------------------------------------------------------------------
//  havokvisualdebuggerserver.h
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokvisualdebuggerserver.h"
#include "physics/scene.h"
#include "conversion.h"

// Visual Debugger includes
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>	

using namespace Math;

namespace Havok
{
__ImplementClass(Havok::HavokVisualDebuggerServer,'HKVD', Physics::BaseVisualDebuggerServer);

//------------------------------------------------------------------------------
/**
*/
HavokVisualDebuggerServer::HavokVisualDebuggerServer():
	visualDebugger(HK_NULL),
	physicsContext(HK_NULL)
{
}

//------------------------------------------------------------------------------
/**
*/
HavokVisualDebuggerServer::~HavokVisualDebuggerServer()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokVisualDebuggerServer::OnStep()
{
	BaseVisualDebuggerServer::OnStep();

	// note: this might trigger a havok error if the visual debugger application has been running for a long time (for example overnight), then just restart the vd
	this->visualDebugger->step(this->simulationFrameTime);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokVisualDebuggerServer::Initialize(Physics::Scene* w)
{
	BaseVisualDebuggerServer::Initialize(w);

	// Connect to the visual debugger
	if (HK_NULL == this->physicsContext)
	{
		this->physicsContext = n_new(hkpPhysicsContext());	
	}
	else
	{
		this->physicsContext->removeWorld(this->world);
	}

	this->world = this->scene->GetWorld();
	this->physicsContext->addWorld(world);

	hkpPhysicsContext::registerAllPhysicsProcesses();

	hkArray<hkProcessContext*> contexts;
	contexts.pushBack(this->physicsContext); 

	// need to close any previous debugger
	if (HK_NULL != this->visualDebugger)
	{
		this->visualDebugger->shutdown();
		this->visualDebugger->removeReference();
	}

	this->visualDebugger = n_new(hkVisualDebugger(contexts));
	this->visualDebugger->serve();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokVisualDebuggerServer::DrawArrow(const Math::point& p, const Math::vector& dir, const Math::float4& color /*= Math::float4(1, 1, 1, 1)*/)
{
	int c = hkColor::rgbFromFloats(color.x(), color.y(), color.z(), color.w());
	HK_DISPLAY_ARROW(Neb2HkFloat4(p), Neb2HkFloat4(dir), c);
}


//------------------------------------------------------------------------------
/**
*/
void 
HavokVisualDebuggerServer::DrawLine(const Math::point& p, const Math::vector& dir, const Math::float4& color /*= Math::float4(1, 1, 1, 1)*/)
{
	int c = hkColor::rgbFromFloats(color.x(), color.y(), color.z(), color.w());
	HK_DISPLAY_LINE(Neb2HkFloat4(p), Neb2HkFloat4(dir), c);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokVisualDebuggerServer::DrawPlane(const Math::point& p, const Math::vector& dir, float scale, const Math::float4& color /*= Math::float4(1, 1, 1, 1)*/)
{
	int c = hkColor::rgbFromFloats(color.x(), color.y(), color.z(), color.w());
	HK_DISPLAY_PLANE(Neb2HkFloat4(dir), Neb2HkFloat4(p), scale, c);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokVisualDebuggerServer::DrawText(const Math::point& p, const Util::String& text, const Math::float4& color /*= Math::float4(1, 1, 1, 1)*/)
{
	int c = hkColor::rgbFromFloats(color.x(), color.y(), color.z(), color.w());
	HK_DISPLAY_3D_TEXT(text.AsCharPtr(), Neb2HkFloat4(p), c);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokVisualDebuggerServer::DrawAABB(const Math::bbox& bbox, const Math::float4& color /*= Math::float4(1, 1, 1, 1)*/)
{
	hkAabb aabb(Neb2HkFloat4(bbox.center() - bbox.extents()), Neb2HkFloat4(bbox.center() + bbox.extents()));
	int c = hkColor::rgbFromFloats(color.x(), color.y(), color.z(), color.w());
	HK_DISPLAY_BOUNDING_BOX(aabb, c);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokVisualDebuggerServer::SetCameraView(const Math::matrix44& viewMatrix)
{
	vector position = viewMatrix.get_position();
	vector lookAt = float4::transform(float4(0, 0, -1, 1), viewMatrix);

	/// note: the user camera must be selected in the vdb
	HK_UPDATE_CAMERA(Neb2HkFloat4(position), Neb2HkFloat4(lookAt), Neb2HkFloat4(vector::upvec()), 1, 100, 90, "Nebula Application Camera");

}

}
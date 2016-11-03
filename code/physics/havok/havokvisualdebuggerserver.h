#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokVisualDebuggerServer
    
    Server for the havok visual debugger program.
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "physics/base/basevisualdebuggerserver.h"

namespace Physics
{
	class Scene;
}

class hkVisualDebugger;
class hkpPhysicsContext;
class hkpWorld;

namespace Havok
{
class HavokVisualDebuggerServer : public Physics::BaseVisualDebuggerServer
{
	__DeclareClass(HavokVisualDebuggerServer);
public:
	/// constructor
	HavokVisualDebuggerServer();
	/// destructor
	virtual ~HavokVisualDebuggerServer();

	/// do one frame
	void OnStep();
	/// initialize debugger connection
	void Initialize(Physics::Scene* w);

	/// set the camera view
	void SetCameraView(const Math::matrix44& viewMatrix);

	/// draw an arrow
	void DrawArrow(const Math::point& p, const Math::vector& dir, const Math::float4& color = Math::float4(1, 1, 1, 1));
	/// draw a line
	void DrawLine(const Math::point& p, const Math::vector& dir, const Math::float4& color = Math::float4(1, 1, 1, 1));
	/// draw a plane
	void DrawPlane(const Math::point& p, const Math::vector& dir, float scale, const Math::float4& color = Math::float4(1, 1, 1, 1));
	/// draw text
	void DrawText(const Math::point& p, const Util::String& text, const Math::float4& color = Math::float4(1, 1, 1, 1));
	/// draw an AABB
	void DrawAABB(const Math::bbox& bbox, const Math::float4& color = Math::float4(1, 1, 1, 1));

private:

	hkVisualDebugger* visualDebugger;
	hkpPhysicsContext* physicsContext;
	hkpWorld* world;
}; 
} 
// namespace Havok
//------------------------------------------------------------------------------
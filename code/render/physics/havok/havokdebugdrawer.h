#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokDebugDrawer
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "math/float4.h"
#include <Common/Base/hkBase.h>
//------------------------------------------------------------------------------

class hkpShape;
class hkpSphereShape;
class hkpBoxShape;
class hkpCylinderShape;
class hkpCapsuleShape;
class hkpMoppBvTreeShape;
class hkpMoppBvTreeShape;
class hkpPlaneShape;
class hkpConvexVerticesShape;

namespace Havok
{
class HavokDebugDrawer
{
public:
    /// used for caching geometry of complex  datastructures
    typedef struct _dbgShape
    {
        int numTriangles;
        int numVertices;
        float * vertices;
        int * indexes;
    } dbgShape;

	static const Math::float4 DEFAULT_COLOR;

	static void DrawShape(const hkpShape* shape, const hkQsTransform& transform, const Math::float4& color = DEFAULT_COLOR);
	static void DrawShape(const hkpShape* shape, const hkQTransform& transform, const Math::float4& color = DEFAULT_COLOR);
	static void DrawShape(const hkpShape* shape, const hkTransform& transform, const Math::float4& color = DEFAULT_COLOR);

	static void DrawSphere(const hkpSphereShape* sphere, const hkQsTransform& transform, const Math::float4& color = DEFAULT_COLOR);
	static void DrawCylinder(const hkpCylinderShape* cylinder, const hkQsTransform& transform, const Math::float4& color = DEFAULT_COLOR);
	static void DrawBox(const hkpBoxShape* box, const hkQsTransform& transform, const Math::float4& color = DEFAULT_COLOR);
	static void DrawCapsule(const hkpCapsuleShape* capsule, const hkQsTransform& transform, const Math::float4& color = DEFAULT_COLOR);
	static void DrawPlane(const hkpPlaneShape* plane, const hkQsTransform& transform, const Math::float4& color = DEFAULT_COLOR);
	static void DrawMopp(const hkpMoppBvTreeShape* mopp, const hkQsTransform& transform, const Math::float4& color = DEFAULT_COLOR);
    static void DrawDbgShape(const HavokDebugDrawer::dbgShape* geom, const hkQsTransform& transform, const Math::float4& color = DEFAULT_COLOR); 
}; 
} 
// namespace Havok
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  havokdebugdrawer.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokdebugdrawer.h"
#include "debugrender/debugshaperenderer.h"
#include "conversion.h"
#include "math/float4.h"

#include <Physics/Collide/Shape/hkpShape.h>
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>
#include <Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h>
#include <Physics/Collide/Shape/Convex/Cylinder/hkpCylinderShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/HeightField/Plane/hkpPlaneShape.h>

using namespace Debug;
using namespace Math;

namespace Havok
{
// default constants
const Math::float4 HavokDebugDrawer::DEFAULT_COLOR = Math::float4(0.0f, 1.0f, 0.0f, 0.25f);
#define SHAPE_RENDER_MODE CoreGraphics::RenderShape::CheckDepth

//------------------------------------------------------------------------------
/**
*/
void 
HavokDebugDrawer::DrawShape(const hkpShape* shape, const hkQTransform& transform, const Math::float4& color /*= DEFAULT_COLOR*/)
{
	HavokDebugDrawer::DrawShape(shape, hkQsTransform(transform.getTranslation(), transform.getRotation(), hkVector4(1, 1, 1, 1)), color);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokDebugDrawer::DrawShape(const hkpShape* shape, const hkTransform& transform, const Math::float4& color /*= DEFAULT_COLOR*/)
{
	hkQuaternion quaternion(transform.getRotation());

	HavokDebugDrawer::DrawShape(shape, hkQsTransform(transform.getTranslation(), quaternion, hkVector4(1, 1, 1, 1)), color);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokDebugDrawer::DrawShape(const hkpShape* shape, const hkQsTransform& transform, const Math::float4& color /*= DEFAULT_COLOR*/)
{
	n_assert(HK_NULL != shape);

	switch (shape->m_type)
	{
	case hkcdShapeType::SPHERE:
		{
			HavokDebugDrawer::DrawSphere((const hkpSphereShape*)shape, transform, color);
			break;
		}
	case hkcdShapeType::CYLINDER:
		{
			HavokDebugDrawer::DrawCylinder((const hkpCylinderShape*)shape, transform, color);
			break;
		}
	case hkcdShapeType::BOX:
		{
			HavokDebugDrawer::DrawBox((const hkpBoxShape*)shape, transform, color);
			break;
		}
	case hkcdShapeType::CAPSULE:
		{
			HavokDebugDrawer::DrawCapsule((const hkpCapsuleShape*)shape, transform, color);
			break;
		}
	case hkcdShapeType::PLANE:
		{
			HavokDebugDrawer::DrawPlane((const hkpPlaneShape*)shape, transform, color);
			break;
		}
	case hkcdShapeType::MOPP:
		{
			HavokDebugDrawer::DrawMopp((const hkpMoppBvTreeShape*)shape, transform, color);
			break;
		}
	default:
		n_error("HavokDebugDrawer::DrawShape: Unknown shape '%d'!", (int)shape->m_type);
	}
	/* remaining shapes:	
		TRIANGLE
		CONVEX_VERTICES						
		TRI_SAMPLED_HEIGHT_FIELD_COLLECTION	
		TRI_SAMPLED_HEIGHT_FIELD_BV_TREE	
		LIST								
		CONVEX_TRANSLATE					
		CONVEX_TRANSFORM					
		SAMPLED_HEIGHT_FIELD				
		EXTENDED_MESH						
		TRANSFORM							
		COMPRESSED_MESH						
		STATIC_COMPOUND						
		BV_COMPRESSED_MESH					
		COLLECTION							
	*/
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokDebugDrawer::DrawSphere(const hkpSphereShape* sphere, const hkQsTransform& transform, const Math::float4& color /*= DEFAULT_COLOR*/)
{
	// update the scale of the transform
	hkVector4 newScale = transform.getScale();
	newScale.mul(sphere->getRadius()*1.33f);	//FIXME: Why the hell is the scale*1.33 correct

	hkQsTransform tmp = transform; 
	tmp.setScale(newScale);

	DebugShapeRenderer::Instance()->DrawSphere(HkQsTransform2NebMatrix44(tmp), color, SHAPE_RENDER_MODE);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokDebugDrawer::DrawCylinder(const hkpCylinderShape* cylinder, const hkQsTransform& transform, const Math::float4& color /*= DEFAULT_COLOR*/)
{
	// update the scale of the transform
	hkVector4 newScale = transform.getScale();
	const float& radius = cylinder->getRadius();
	newScale.mul4(hkVector4(radius*2, cylinder->getVertex(0)(1)*2, radius*2, 1.0f));

	hkQsTransform tmp = transform; 
	tmp.setScale(newScale);

	DebugShapeRenderer::Instance()->DrawCylinder(HkQsTransform2NebMatrix44(tmp), color, SHAPE_RENDER_MODE);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokDebugDrawer::DrawBox(const hkpBoxShape* box, const hkQsTransform& transform, const Math::float4& color /*= DEFAULT_COLOR*/)
{
	// update the scale of the transform
	hkVector4 newScale = transform.getScale();
	hkVector4 extents;
	extents.setMul(box->getHalfExtents(), 2);
	newScale.mul4(extents);

	hkQsTransform tmp = transform; 
	tmp.setScale(newScale);

	DebugShapeRenderer::Instance()->DrawBox(HkQsTransform2NebMatrix44(tmp), color, SHAPE_RENDER_MODE);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokDebugDrawer::DrawCapsule(const hkpCapsuleShape* capsule, const hkQsTransform& transform, const Math::float4& color /*= DEFAULT_COLOR*/)
{
	// update the scale of the transform
	hkVector4 newScale = transform.getScale();
	const float& radius = capsule->getRadius();
	newScale.mul4(hkVector4(radius*2, (capsule->getVertex(0)(1) + radius)*2, radius*2, 1.0f));

	hkQsTransform tmp = transform; 
	tmp.setScale(newScale);

	// draw a cylinder since there's no DrawCapsule
	DebugShapeRenderer::Instance()->DrawCylinder(HkQsTransform2NebMatrix44(tmp), color, SHAPE_RENDER_MODE);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokDebugDrawer::DrawPlane(const hkpPlaneShape* plane, const hkQsTransform& transform, const Math::float4& color /*= DEFAULT_COLOR*/)
{
	// does nothing for now, DebugShapeRenderer::DrawBox does not seem to handle rotations
#if 0
	// draw a flat box
	hkVector4 planeEquation = plane->getPlane();
	planeEquation.normalize4();
	const hkVector4& planeCenter = plane->getAabbCenter();
	const hkVector4& extents = plane->getAabbHalfExtents();

	hkVector4 newScale = transform.getScale();
	hkVector4 e;
	e.setMul(extents, 2);
	newScale.mul4(e);

	// hkQuaternion::setShortestRotation will always complain that the vectors are not normalized even if they are, therefore calculate angle and axis manually instead
	hkVector4 up(0, 1, 0);
	float dot = up.dot3(planeEquation);
	float angle = n_acos(dot);

	hkVector4 axis;
	axis.setCross(up, planeEquation);
	axis.normalize4();	//< should be normalized already, but setAxisAngle will bark if this is not done

	hkQuaternion rotation;
	rotation.setAxisAngle(axis, angle);

	hkQsTransform tmp(planeCenter, rotation);
	hkQsTransform tmp = transform; 
	tmp.setScale(newScale);
	tmp.setTranslation(planeCenter);
	tmp.setRotation(rotation);

	DebugShapeRenderer::Instance()->DrawBox(HkQsTransform2NebMatrix44(tmp), color, SHAPE_RENDER_MODE);
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokDebugDrawer::DrawMopp(const hkpMoppBvTreeShape* mopp, const hkQsTransform& transform, const Math::float4& color /*= DEFAULT_COLOR*/)
{
	const hkpShapeCollection* collection = mopp->getShapeCollection();

	hkpExtendedMeshShape* meshShape = (hkpExtendedMeshShape*)collection;

	n_assert2(meshShape->getNumTrianglesSubparts(), "only 1 triangle subbart is currently supported");
	const hkpExtendedMeshShape::TrianglesSubpart& subPart = meshShape->getTrianglesSubpartAt(0);

	Debug::DebugShapeRenderer::Instance()->DrawIndexedPrimitives(
		HkQsTransform2NebMatrix44(transform), 
		CoreGraphics::PrimitiveTopology::TriangleList, //topology
		subPart.m_numTriangleShapes, //numPrimitives
		subPart.m_vertexBase, //vertices
		subPart.m_numVertices, //numVertices
		subPart.m_vertexStriding / sizeof(hkReal), //vertexWidth
		subPart.m_indexBase, //indices
		CoreGraphics::IndexType::Index32, //indexType
		color,
		SHAPE_RENDER_MODE);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokDebugDrawer::DrawDbgShape(const HavokDebugDrawer::dbgShape* geom, const hkQsTransform& transform, const Math::float4& color)
{
    Debug::DebugShapeRenderer::Instance()->DrawIndexedPrimitives(
        HkQsTransform2NebMatrix44(transform), 
        CoreGraphics::PrimitiveTopology::TriangleList, //topology
        geom->numTriangles, //numPrimitives
        geom->vertices, //vertices
        geom->numVertices, //numVertices
        3, //vertexWidth
        geom->indexes, //indices
        CoreGraphics::IndexType::Index32, //indexType
        color,
        SHAPE_RENDER_MODE);    
}


}
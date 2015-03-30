//------------------------------------------------------------------------------
//  havokcollider.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokcollider.h"
#include "havokdebugdrawer.h"
#include "conversion.h"
	
#include <Physics/Collide/Shape/Convex/hkpConvexShape.h>							
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>					
#include <Physics/Collide/Shape/Convex/Cylinder/hkpCylinderShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Shape/Misc/Transform/hkpTransformShape.h>	
#include <Physics/Collide/Shape/Convex/ConvexTransform/hkpConvexTransformShape.h>
#include <Physics/Collide/Shape/HeightField/Plane/hkpPlaneShape.h>
#include <Physics/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShape.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesConnectivity.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesConnectivityUtil.h>

namespace Havok
{
using namespace Physics;
using namespace Math;

__ImplementClass(Havok::HavokCollider,'HKCR', Physics::BaseCollider);

//------------------------------------------------------------------------------
/**
*/
HavokCollider::HavokCollider():
	scale(1,1,1)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
HavokCollider::~HavokCollider()
{
	this->shapes.clearAndDeallocate();
}

//------------------------------------------------------------------------------
/**
*/
HavokDebugDrawer::dbgShape * 
HavokCollider::CreateDbgShape(hkGeometry * geom)
{
    HavokDebugDrawer::dbgShape * ds = new HavokDebugDrawer::dbgShape;
    ds->vertices = (float *)Memory::Alloc(Memory::PhysicsHeap,3 * geom->m_vertices.getSize() * sizeof(float));
    ds->indexes = (int*)Memory::Alloc(Memory::PhysicsHeap,3 * geom->m_triangles.getSize()* sizeof(int));
    ds->numTriangles = geom->m_triangles.getSize();
    ds->numVertices = geom->m_vertices.getSize();

    for(int j = 0 ; j < ds->numTriangles ; j++)
    {
        const hkGeometry::Triangle & tr = geom->m_triangles[j];
        ds->indexes[j*3] = tr.m_a;
        ds->indexes[j*3+1] = tr.m_b;
        ds->indexes[j*3+2] = tr.m_c;
    }
    for(int j = 0 ; j < ds->numVertices ; j++)
    {
        const hkVector4 & v = geom->m_vertices[j];                    
        for(int k = 0 ; k < 3 ; k++)
        {
            ds->vertices[j*3 + k] = v.getComponent(k);                        
        }                    
    }
    return ds;
}
//------------------------------------------------------------------------------
/**
*/
void 
HavokCollider::RenderDebug(const Math::matrix44& t)
{
	hkQsTransform transform = NebMatrix442HkQsTransform(t);
	transform.m_scale(3) = 1.0f;
	hkQsTransform finalTransform;

	IndexT i;
	for (i = 0; i < this->shapes.getSize(); i++)
	{
		const hkpShape* transformShape = this->shapes[i];
		hkQsTransform localTransform = GetTransformFromTransformShape(transformShape);
		const hkpShape* shape = this->GetShapeFromTransformShape(transformShape);

		finalTransform.setMul(transform, localTransform);
        if(shape->m_type == hkcdShapeType::BV_COMPRESSED_MESH || shape->m_type == hkcdShapeType::CONVEX_VERTICES)
        {            
            // create a copy of the geometry data for easy rendering since the overhead is so high
            // only used for debugging
            if(!this->debugGeometry.Contains(i))
            {
                switch(shape->m_type)
                {
                case hkcdShapeType::BV_COMPRESSED_MESH:
                    {
                        hkpBvCompressedMeshShape * s = (hkpBvCompressedMeshShape*)shape;
                
                        hkGeometry geom;
                        s->convertToGeometry(geom);
                                               
                        this->debugGeometry.Add(i,this->CreateDbgShape(&geom));
                    }
                    break;
                case hkcdShapeType::CONVEX_VERTICES:
                    {
                        hkpConvexVerticesShape * s = (hkpConvexVerticesShape *)shape;
                        const hkpConvexVerticesConnectivity * conn = hkpConvexVerticesConnectivityUtil::findConnectivity(s);
                        
                        hkGeometry *geom = hkpConvexVerticesConnectivityUtil::createGeometry(s,conn);
                        conn->removeReference();
                        this->debugGeometry.Add(i,this->CreateDbgShape(geom));                        
                    }
                    break;
                }
            }
            HavokDebugDrawer::DrawDbgShape(this->debugGeometry[i],finalTransform);
        }
        else
        {
		    HavokDebugDrawer::DrawShape(shape, finalTransform);
        }        
	}
}	

//------------------------------------------------------------------------------
/**
*/
void 
HavokCollider::AddPlane(const Math::plane &plane, const Math::matrix44 &localTransform)
{
	const vector& normal = plane.get_normal();
	const vector& point = plane.get_point();

	//FIXME: halfextents params
	hkpTransformShape* shape = n_new(hkpTransformShape(
		new hkpPlaneShape(Neb2HkFloat4(normal), Neb2HkFloat4(point), hkVector4(100, 100, 100)),
		NebMatrix442HkTransform(localTransform)));

	this->AppendShape(shape);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCollider::AddBox(const Math::vector &halfWidth, const Math::matrix44 &localTransform)
{
	Math::float4 hw = halfWidth;
	hw *= this->scale;
	const Math::float4 epsilon(0.01f);
#if _DEBUG
	if(Math::float4::less3_any(hw,epsilon))
	{
		n_warning("Box with any dimension of size 0 is not allowed.");
	}
#endif
	hkpBoxShape * box = new hkpBoxShape(Neb2HkFloat4(Math::float4::maximize(hw,epsilon)));
	const hkTransform & transform = NebMatrix442HkTransform(localTransform);
	hkpConvexTransformShape* shape = n_new(hkpConvexTransformShape(box,transform));
	this->AppendShape(shape);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCollider::AddBox(const Math::bbox & box)
{
	this->AddBox(box.extents(), Math::matrix44::translation(box.center()));
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCollider::AddSphere(float radius, const Math::matrix44 &localTransform)
{
#if _DEBUG
	if(radius < 0.01f)
	{
		n_warning("Sphere collider with 0 radius is not allowed.");
	}
#endif
	hkpSphereShape * sphere = new hkpSphereShape(Math::n_max(radius,0.01f)); 
	const hkTransform & transform = NebMatrix442HkTransform(localTransform);
	hkpConvexTransformShape* shape = n_new(hkpConvexTransformShape(sphere,transform));
	this->AppendShape(shape);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCollider::AddCylinder(float radius, float height, const Math::matrix44 &localTransform)
{
	this->AddCylinder(vector(0, height*0.5f, 0), vector(0, -height*0.5f, 0), radius, localTransform);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCollider::AddCylinder(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform)
{
	hkpConvexTransformShape* shape = n_new(hkpConvexTransformShape(
		new hkpCylinderShape(Neb2HkFloat4(pointA), Neb2HkFloat4(pointB), radius),
		NebMatrix442HkTransform(localTransform)));

	this->AppendShape(shape);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCollider::AddCapsule(float radius, float height, const Math::matrix44 &localTransform)
{
	this->AddCapsule(vector(0, height*0.5f, 0), vector(0, height*-0.5f, 0), radius, localTransform);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCollider::AddCapsule(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform)
{
	hkpConvexTransformShape* shape = n_new(hkpConvexTransformShape(
		new hkpCapsuleShape(Neb2HkFloat4(pointA), Neb2HkFloat4(pointB), radius), NebMatrix442HkTransform(localTransform)));

	this->AppendShape(shape);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCollider::AddPhysicsMesh(Ptr<Physics::ManagedPhysicsMesh> colliderMesh, const Math::matrix44 & localTransform, Physics::MeshTopologyType meshType, int primGroup)
{
	hkpShape* shape;
	Ptr<Physics::PhysicsMesh> physicsMesh = colliderMesh->GetMesh();

	
	shape = n_new(hkpTransformShape(
		physicsMesh->GetShape(primGroup,meshType), 
		NebMatrix442HkTransform(localTransform)));
	this->AppendShape(shape);
}

//------------------------------------------------------------------------------
/**
*/
hkQsTransform 
HavokCollider::GetTransformFromTransformShape(const hkpShape* shape)
{
	if (!IsSupportedTransformShape(shape))
	{
		Util::String errorMessage;
		errorMessage.Format("HavokCollider::GetShapeTransform: Given shape is not a supported transformshape! Type is '%d' (might need implementing)", (int)shape->m_type);
		n_error(errorMessage.AsCharPtr());
	}		

	if (hkcdShapeType::CONVEX_TRANSFORM == shape->m_type)
	{
		hkpConvexTransformShape* convexTransformShape = (hkpConvexTransformShape*)shape;
		return convexTransformShape->getQsTransform();
	}
	else if (hkcdShapeType::TRANSFORM == shape->m_type)
	{
		hkpTransformShape* transformShape = (hkpTransformShape*)shape;
		const hkTransform& transform = transformShape->getTransform();
		return hkQsTransform(transform.getTranslation(), hkQuaternion(transform.getRotation()));
	}

	// won't happen
	return hkQsTransform::getIdentity();
}

//------------------------------------------------------------------------------
/**
*/
const hkpShape* 
HavokCollider::GetShapeFromTransformShape(const hkpShape* shape)
{
	if (!IsSupportedTransformShape(shape))
	{
		Util::String errorMessage;
		errorMessage.Format("HavokCollider::GetShapeFromTransformShape: Given shape is not a supported transformshape! Type is '%d' (might need implementing)", (int)shape->m_type);
		n_error(errorMessage.AsCharPtr());
	}		

	if (hkcdShapeType::CONVEX_TRANSFORM == shape->m_type)
	{
		hkpConvexTransformShape* convexTransformShape = (hkpConvexTransformShape*)shape;
		return convexTransformShape->getChildShape();
	}
	else if (hkcdShapeType::TRANSFORM == shape->m_type)
	{
		hkpTransformShape* transformShape = (hkpTransformShape*)shape;
		return transformShape->getChildShape();
	}

	// eurgha
	return HK_NULL;
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokCollider::IsSupportedTransformShape(const hkpShape* shape)
{
	return 
		hkcdShapeType::CONVEX_TRANSFORM == shape->m_type ||
		hkcdShapeType::TRANSFORM == shape->m_type;
}

}
//------------------------------------------------------------------------------
//  havokphysicsmesh.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokphysicsmesh.h"
#include "io/uri.h"

#include <Common/Base/hkBase.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h>
#include <Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics/Utilities/Collide/ShapeUtils/MoppCodeStreamer/hkpMoppCodeStreamer.h>
#include <Physics/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShapeCinfo.h>
#include <Common/Base/Types/Geometry/hkStridedVertices.h> 
#include <Common/Base/System/Io/OArchive/hkOArchive.h>
#include <Common/Base/System/Io/IArchive/hkIArchive.h>
#include "Common/Base/Types/Geometry/hkGeometry.h"
#include "Common/Base/Math/Vector/hkVector4.h"

namespace Havok
{
__ImplementClass(Havok::HavokPhysicsMesh, 'HKPM', Physics::PhysicsMeshBase);

//------------------------------------------------------------------------------
/**
*/
HavokPhysicsMesh::HavokPhysicsMesh()
{
}

//------------------------------------------------------------------------------
/**
*/
HavokPhysicsMesh::~HavokPhysicsMesh()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPhysicsMesh::AddMeshComponent(int id, float * vertexData, uint numVertices, uint verticeStride, uint * indexData, uint numTriangles)
{
	n_assert(!this->meshes.Contains(id));

    size_t vertexBufferSize = numVertices * verticeStride * sizeof(float);
    float *v = (float*)Memory::Alloc(Memory::PhysicsHeap, vertexBufferSize);
    Memory::Copy(vertexData,(void*)v, vertexBufferSize);

    size_t indexBufferSize = numTriangles * 3 * sizeof(uint);
    uint * idx = (uint*)Memory::Alloc(Memory::PhysicsHeap, indexBufferSize);
    Memory::Copy(indexData,(void*)idx, indexBufferSize);

    meshData mdata = {v, numVertices, verticeStride, idx, numTriangles};
    this->meshes.Add(id, mdata);
    /*

	
    */
	
}

//------------------------------------------------------------------------------
/**
*/
hkpShape* 
HavokPhysicsMesh::GetShape(int primGroup, Physics::MeshTopologyType meshType)
{
    const meshData & m = this->meshes[primGroup];

    switch(meshType)
    {
        case Physics::MeshConcave:
        case Physics::MeshConvex:
            {
                hkpExtendedMeshShape* meshShape = n_new(hkpExtendedMeshShape());
                // documentation: "it is common to have a landscape with 0 convex radius (for each triangle) and all moving objects with non zero radius."
                //FIXME: is it beneficial to make radius configurable?
                meshShape->setRadius( 0.00f );
                
                hkpExtendedMeshShape::TrianglesSubpart part;
                part.m_vertexBase = m.vertexData;
                part.m_indexBase = m.indexData;                    

                part.m_vertexStriding =  sizeof(hkReal) * m.verticeStride;
                part.m_numVertices = m.numVertices;

                part.m_indexStriding = sizeof(hkUint32) * 3;

                part.m_numTriangleShapes = m.numTriangles;
                part.m_stridingType = hkpExtendedMeshShape::INDICES_INT32;

                meshShape->addTrianglesSubpart( part );
                
                hkpMoppCompilerInput mci;
                hkpMoppCode *code = hkpMoppUtility::buildCode(meshShape, mci);                                           

                hkpMoppBvTreeShape* moppShape = n_new(hkpMoppBvTreeShape(meshShape, code));

                code->removeReference();
                meshShape->removeReference();                
                return moppShape;
            }
            break;
        case Physics::MeshConvexHull:
            // not really the same, but its a cludge anyway
        case Physics::MeshConvexDecomposition:
            {
                // This structure provides the build options
                hkpConvexVerticesShape::BuildConfig config;
                config.m_convexRadius = 0.1f;
                
                hkStridedVertices verts;
                verts.m_numVertices = m.numVertices;
                verts.m_striding = m.verticeStride*sizeof(float);
                verts.m_vertices = m.vertexData;
                
                hkpConvexVerticesShape* shape = new hkpConvexVerticesShape( verts, config );

                // hkpConvexVerticesShape maintains its own copy of the data, free this one
                Memory::Free(Memory::PhysicsHeap,m.indexData);
                Memory::Free(Memory::PhysicsHeap,m.vertexData);
                return shape;

            }
            break;
        case Physics::MeshStatic:
            {
                //FIXME: is this really worth it, so much copying, should batch to native format in content pipeline
                //       not here.
                hkGeometry geom;
                geom.m_vertices.reserveExactly(m.numVertices);
                geom.m_triangles.reserveExactly(m.numTriangles);
                for(uint i = 0 ; i < m.numVertices ; i++)
                {
                    geom.m_vertices.pushBack (hkVector4(m.vertexData[i*m.verticeStride],m.vertexData[i*m.verticeStride+1],m.vertexData[i*m.verticeStride+2]));
                }
                uint count = 0;
                for(uint i = 0 ; i < m.numTriangles ; i++)
                {
                    hkGeometry::Triangle t = {m.indexData[count++],m.indexData[count++],m.indexData[count++],0};
                    geom.m_triangles.pushBack(t);
                }
                hkpDefaultBvCompressedMeshShapeCinfo cinfo(&geom);
                hkpBvCompressedMeshShape* shape = new hkpBvCompressedMeshShape( cinfo );
                return shape;
            }
        default:
            n_assert("not implemented\n");
    }
    return NULL;
            /*


	hkpExtendedMeshShape* meshShape = this->meshes[primGroup];

	// load MOPP (Memory-Optimized Partial Polytope) code
	//TODO: these should be built upon batching the models, and then loaded here
	// note: an example in how to handle mopps is in <havokdir>\Demo\Demos\Physics\Api\Collide\Shapes\Mopp\MoppCodeStreaming\MoppCodeStreamingDemo.cpp
	Util::String path;
	{
		Util::String resourceName = this->resourceId.AsString();
		resourceName.StripFileExtension();
		IndexT index = resourceName.FindCharIndex(':');
		if (InvalidIndex != index)
		{
			Util::String tmp = resourceName.ExtractRange(index + 1, resourceName.Length() - index - 1);
			resourceName = tmp;
		}

		path = "export:havok_mopp/";
		path.Append(resourceName);
		path.Append(".mopp");

		// get the full path
		IO::URI uri = path;
		path = uri.GetHostAndLocalPath();
	}

	hkpMoppCode* code = HK_NULL;
	code = this->LoadMoppCode(path);

	if (HK_NULL == code)
	{
		// no mopp code has previously been saved, then build and save it now
		hkpMoppCompilerInput mci;
		code = hkpMoppUtility::buildCode(meshShape, mci);
		this->SaveMoppCode(code, path);
		code->removeReference();

		// test loading the file
		code = this->LoadMoppCode(path);
		n_assert(HK_NULL != code);
	}

	hkpMoppBvTreeShape* moppShape = n_new(hkpMoppBvTreeShape(meshShape, code));

	code->removeReference();
	meshShape->removeReference();

	return moppShape;
    */

}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPhysicsMesh::SaveMoppCode(hkpMoppCode* moppCode, const Util::String& moppFilename)
{
	hkOArchive outputArchive(moppFilename.AsCharPtr());
	if(outputArchive.isOk())
	{
		hkpMoppCodeStreamer::writeMoppCodeToArchive(moppCode, outputArchive);
	}
}

//------------------------------------------------------------------------------
/**
*/
hkpMoppCode* 
HavokPhysicsMesh::LoadMoppCode(const Util::String& moppFilename)
{
	hkIArchive inputArchive(moppFilename.AsCharPtr());
	if(inputArchive.isOk())
	{
		hkpMoppCode* code = hkpMoppCodeStreamer::readMoppCodeFromArchive(inputArchive);			

		n_assert(code != HK_NULL);
		return code;
	}
	else
	{
		// input archive is not valid
		return HK_NULL;
	}
}

}
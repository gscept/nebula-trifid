#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokPhysicsMesh
	    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "physics/resource/physicsmeshbase.h"
#include "physics/model/templates.h"
#include "util/dictionary.h"

class hkpExtendedMeshShape;
class hkpMoppCode;
class hkpShape;

namespace Havok
{
class HavokPhysicsMesh : public Physics::PhysicsMeshBase
{
	__DeclareClass(HavokPhysicsMesh);
public:
	/// constructor
	HavokPhysicsMesh();
	/// destructor
	virtual ~HavokPhysicsMesh();

	/// add mesh component
	void AddMeshComponent(int id, const CoreGraphics::PrimitiveGroup& group);

	/// get shape
	hkpShape* GetShape(int primGroup, Physics::MeshTopologyType meshType);	

private:
    typedef struct _meshData
    {
        float * vertexData;
        uint numVertices;
        uint verticeStride;
        uint * indexData;
        uint numTriangles;
    } meshData;
	//FIXME: move this onto some load/saver
	// saves a MOPP code
	void SaveMoppCode(hkpMoppCode* moppCode, const Util::String& moppFilename  );
	// loads a previously saved MOPP code
	hkpMoppCode* LoadMoppCode(const Util::String& moppFilename );

	Util::Dictionary<int, meshData> meshes;
};

}
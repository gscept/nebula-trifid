//------------------------------------------------------------------------------
//  physicsstreammeshloader.cc
//  (C) 2012 Johannes Hirche
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/resource/physicsstreammeshloader.h"
#include "physics/resource/physicsmesh.h"
#include "coregraphics/legacy/nvx2streamreader.h"

namespace Physics
{
__ImplementClass(Physics::PhysicsStreamMeshLoader, 'PHML', Resources::StreamResourceLoader);

using namespace IO;
using namespace CoreGraphics;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
PhysicsStreamMeshLoader::PhysicsStreamMeshLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Setup the mesh resource from legacy nvx2 file (Nebula2 binary mesh format).
*/
bool
PhysicsStreamMeshLoader::SetupResourceFromStream(const Ptr<Stream>& stream)
{
    n_assert(stream.isvalid());    
    Ptr<Legacy::Nvx2StreamReader> nvx2Reader = Legacy::Nvx2StreamReader::Create();
    nvx2Reader->SetStream(stream);
    nvx2Reader->SetUsage(Base::ResourceBase::UsageImmutable);	
    nvx2Reader->SetAccess(Base::ResourceBase::AccessNone);
	nvx2Reader->SetRawMode(true);

    if (nvx2Reader->Open())
    {
        const Ptr<PhysicsMesh>& res = this->resource.downcast<PhysicsMesh>();
        n_assert(!res->IsLoaded());
		const Util::Array<CoreGraphics::PrimitiveGroup>& groups = nvx2Reader->GetPrimitiveGroups();		

		float *vertexData = nvx2Reader->GetVertexData();
		uint *indexData = (uint*)nvx2Reader->GetIndexData(); 

		for(int i=0;i < groups.Size();i++)
		{
			n_assert2(groups[i].GetPrimitiveTopology() == PrimitiveTopology::TriangleList,"Only triangle lists are supported");
			res->AddMeshComponent(i,&(vertexData[groups[i].GetBaseVertex()]),groups[i].GetNumVertices(),nvx2Reader->GetVertexWidth(),&(indexData[groups[i].GetBaseIndex()]),groups[i].GetNumPrimitives());		
		}		
        nvx2Reader->Close();
        return true;
    }
	else
	{
		n_error("PhysicsStreamMeshLoader: Can't open file '%s'", stream->GetURI().AsString().AsCharPtr());
	}
    return false;
}

} // namespace Physics

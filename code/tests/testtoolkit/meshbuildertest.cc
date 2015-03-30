//------------------------------------------------------------------------------
//  meshbuildertest.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "meshbuildertest.h"
#include "toolkitutil/meshutil/meshbuilder.h"

namespace Test
{
__ImplementClass(Test::MeshBuilderTest, 'mblt', Test::TestCase);

using namespace Util;
using namespace ToolkitUtil;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
void
MeshBuilderTest::Run()
{
    MeshBuilder meshBuilder;

    this->Verify(meshBuilder.GetNumVertices() == 0);
    this->Verify(meshBuilder.GetNumTriangles() == 0);

    // build a simple 3-sided pyramid
    float4 coords[4];
    coords[0].set(0.0f, 1.0f, 0.0f, 1.0f);
    coords[1].set(-1.0f, 0.0f, -1.0f, 1.0f);
    coords[2].set(+1.0f, 0.0f, -1.0f, 1.0f);
    coords[3].set(0.0f, 0.0f, +1.0f, 0.0f);
    IndexT i;
    for (i = 0; i < 4; i++)
    {
        MeshBuilderVertex v;
        v.SetComponent(MeshBuilderVertex::CoordIndex, coords[i]);
        meshBuilder.AddVertex(v);
    }

    // add triangles, with 2 different group ids
    meshBuilder.AddTriangle(MeshBuilderTriangle(0, 1, 2, 0));
    meshBuilder.AddTriangle(MeshBuilderTriangle(0, 2, 3, 0));
    meshBuilder.AddTriangle(MeshBuilderTriangle(0, 3, 1, 0));
    meshBuilder.AddTriangle(MeshBuilderTriangle(1, 2, 3, 1));

    this->Verify(meshBuilder.GetNumVertices() == 4);
    this->Verify(meshBuilder.VertexAt(0).GetComponent(MeshBuilderVertex::CoordIndex) == coords[0]);
    this->Verify(meshBuilder.VertexAt(1).GetComponent(MeshBuilderVertex::CoordIndex) == coords[1]);
    this->Verify(meshBuilder.VertexAt(2).GetComponent(MeshBuilderVertex::CoordIndex) == coords[2]);
    this->Verify(meshBuilder.VertexAt(3).GetComponent(MeshBuilderVertex::CoordIndex) == coords[3]);    

    this->Verify(meshBuilder.GetNumTriangles() == 4);
    this->Verify(meshBuilder.TriangleAt(0).GetVertexIndex(0) == 0);
    this->Verify(meshBuilder.TriangleAt(0).GetVertexIndex(1) == 1);
    this->Verify(meshBuilder.TriangleAt(0).GetVertexIndex(2) == 2);
    this->Verify(meshBuilder.TriangleAt(0).GetGroupId() == 0);
    this->Verify(meshBuilder.TriangleAt(1).GetVertexIndex(0) == 0);
    this->Verify(meshBuilder.TriangleAt(1).GetVertexIndex(1) == 2);
    this->Verify(meshBuilder.TriangleAt(1).GetVertexIndex(2) == 3);
    this->Verify(meshBuilder.TriangleAt(1).GetGroupId() == 0);
    this->Verify(meshBuilder.TriangleAt(2).GetVertexIndex(0) == 0);
    this->Verify(meshBuilder.TriangleAt(2).GetVertexIndex(1) == 3);
    this->Verify(meshBuilder.TriangleAt(2).GetVertexIndex(2) == 1);
    this->Verify(meshBuilder.TriangleAt(2).GetGroupId() == 0);
    this->Verify(meshBuilder.TriangleAt(3).GetVertexIndex(0) == 1);
    this->Verify(meshBuilder.TriangleAt(3).GetVertexIndex(1) == 2);
    this->Verify(meshBuilder.TriangleAt(3).GetVertexIndex(2) == 3);
    this->Verify(meshBuilder.TriangleAt(3).GetGroupId() == 1);

    this->Verify(meshBuilder.CountGroupTriangles(0, 0) == 3);
    this->Verify(meshBuilder.CountGroupTriangles(1, 0) == 1);
    this->Verify(meshBuilder.CountGroupTriangles(2, 0) == 0);

    IndexT minVertexIndex, maxVertexIndex;
    meshBuilder.FindGroupVertexRange(0, minVertexIndex, maxVertexIndex);
    this->Verify(minVertexIndex == 0);
    this->Verify(maxVertexIndex == 3);
    meshBuilder.FindGroupVertexRange(1, minVertexIndex, maxVertexIndex);
    this->Verify(minVertexIndex == 1);
    this->Verify(maxVertexIndex == 3);

    bbox box;
    box = meshBuilder.ComputeGroupBoundingBox(0);
    this->Verify(box.pmin == point(-1.0f, 0.0f, -1.0f));
    this->Verify(box.pmax == point(1.0f, 1.0f, 1.0f));
    box = meshBuilder.ComputeGroupBoundingBox(1);
    this->Verify(box.pmin == point(-1.0f, 0.0f, -1.0f));
    this->Verify(box.pmax == point(1.0f, 0.0f, 1.0f));
    box = meshBuilder.ComputeBoundingBox();
    this->Verify(box.pmin == point(-1.0f, 0.0f, -1.0f));
    this->Verify(box.pmax == point(1.0f, 1.0f, 1.0f));

    MeshBuilder dstMeshBuilder;
    FixedArray<IndexT> indexMap(meshBuilder.GetNumVertices());
    indexMap.Fill(InvalidIndex);
    for (i = 0; i < meshBuilder.GetNumTriangles(); i++)
    {
        dstMeshBuilder.CopyTriangle(meshBuilder, i, indexMap);
    }
    this->Verify(dstMeshBuilder.GetNumVertices() == 4);
    this->Verify(dstMeshBuilder.GetNumTriangles() == 4);
}

} // namespace Test
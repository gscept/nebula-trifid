//------------------------------------------------------------------------------
//  meshbuildervertextest.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "meshbuildervertextest.h"
#include "toolkitutil/meshutil/meshbuildervertex.h"

namespace Test
{
__ImplementClass(Test::MeshBuilderVertexTest, 'mbvt', Test::TestCase);

using namespace ToolkitUtil;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
void
MeshBuilderVertexTest::Run()
{
    MeshBuilderVertex v;

    this->Verify(0 == v.GetComponentMask());
    this->Verify(!v.CheckFlag(MeshBuilderVertex::Redundant));
    this->Verify(!v.HasComponents(MeshBuilderVertex::CoordBit));
    this->Verify(!v.HasComponents(MeshBuilderVertex::Uv0Bit));

    MeshBuilderVertex::ComponentMask compMask = MeshBuilderVertex::CoordBit | MeshBuilderVertex::NormalBit | MeshBuilderVertex::Uv0Bit;
    v.InitComponents(compMask);
    this->Verify(v.HasComponents(MeshBuilderVertex::CoordBit));
    this->Verify(v.HasComponents(MeshBuilderVertex::NormalBit));
    this->Verify(v.HasComponents(MeshBuilderVertex::Uv0Bit));
    this->Verify(v.HasComponents(compMask));
    this->Verify(!v.HasComponents(compMask | MeshBuilderVertex::Uv1Bit));
    this->Verify(v.GetComponentMask() == compMask);

    v.DeleteComponents(compMask);
    this->Verify(!v.HasComponents(MeshBuilderVertex::CoordBit));
    this->Verify(!v.HasComponents(MeshBuilderVertex::NormalBit));
    this->Verify(!v.HasComponents(MeshBuilderVertex::Uv0Bit));
    this->Verify(0 == v.GetComponentMask());

    v.SetFlag(MeshBuilderVertex::Redundant);
    this->Verify(v.CheckFlag(MeshBuilderVertex::Redundant));
    v.UnsetFlag(MeshBuilderVertex::Redundant);
    this->Verify(!v.CheckFlag(MeshBuilderVertex::Redundant));
    
    float4 coord(12.0f, 23.0f, 34.0f, 1.0f);
    float4 normal(1.0f, 0.0f, 0.0f, 0.0f);
    float4 tangent(0.0f, 1.0f, 0.0f, 0.0f);
    float4 binormal(0.0f, 0.0f, 1.0f, 0.0f);
    float4 uv0(1.0f, 0.0f, 0.0f, 0.0f);
    float4 uv1(0.0f, 1.0f, 0.0f, 0.0f);
    float4 uv2(1.0f, 1.0f, 0.0f, 0.0f);
    float4 uv3(0.0f, 0.0f, 1.0f, 0.0f);
    float4 color(1.0f, 1.0f, 1.0f, 1.0f);
    float4 weights(0.1f, 0.2f, 0.3f, 0.4f);
    float4 jindices(1.0f, 2.0f, 3.0f, 4.0f);
    v.SetComponent(MeshBuilderVertex::CoordIndex, coord);
    v.SetComponent(MeshBuilderVertex::NormalIndex, normal);
    v.SetComponent(MeshBuilderVertex::TangentIndex, tangent);
    v.SetComponent(MeshBuilderVertex::BinormalIndex, binormal);
    v.SetComponent(MeshBuilderVertex::Uv0Index, uv0);
    v.SetComponent(MeshBuilderVertex::Uv1Index, uv1);
    v.SetComponent(MeshBuilderVertex::Uv2Index, uv2);
    v.SetComponent(MeshBuilderVertex::Uv3Index, uv3);
    v.SetComponent(MeshBuilderVertex::ColorIndex, color);
    v.SetComponent(MeshBuilderVertex::WeightsIndex, weights);
    v.SetComponent(MeshBuilderVertex::JIndicesIndex, jindices);
    
    MeshBuilderVertex::ComponentMask allCompMask = MeshBuilderVertex::CoordBit |
                                                   MeshBuilderVertex::NormalBit |
                                                   MeshBuilderVertex::TangentBit |
                                                   MeshBuilderVertex::BinormalBit |
                                                   MeshBuilderVertex::Uv0Bit |
                                                   MeshBuilderVertex::Uv1Bit |
                                                   MeshBuilderVertex::Uv2Bit |
                                                   MeshBuilderVertex::Uv3Bit |
                                                   MeshBuilderVertex::ColorBit |
                                                   MeshBuilderVertex::WeightsBit |
                                                   MeshBuilderVertex::JIndicesBit;
    this->Verify(v.HasComponents(compMask));
    this->Verify(v.HasComponents(allCompMask));
    this->Verify(v.GetComponent(MeshBuilderVertex::CoordIndex) == coord);
    this->Verify(v.GetComponent(MeshBuilderVertex::NormalIndex) == normal);
    this->Verify(v.GetComponent(MeshBuilderVertex::TangentIndex) == tangent);
    this->Verify(v.GetComponent(MeshBuilderVertex::BinormalIndex) == binormal);
    this->Verify(v.GetComponent(MeshBuilderVertex::Uv0Index) == uv0);
    this->Verify(v.GetComponent(MeshBuilderVertex::Uv1Index) == uv1);
    this->Verify(v.GetComponent(MeshBuilderVertex::Uv2Index) == uv2);
    this->Verify(v.GetComponent(MeshBuilderVertex::Uv3Index) == uv3);
    this->Verify(v.GetComponent(MeshBuilderVertex::ColorIndex) == color);
    this->Verify(v.GetComponent(MeshBuilderVertex::WeightsIndex) == weights);
    this->Verify(v.GetComponent(MeshBuilderVertex::JIndicesIndex) == jindices);

    matrix44 m = matrix44::translation(1.0f, 2.0f, 3.0f);
    v.Transform(m);
    float4 transformedCoord(13.0f, 25.0f, 37.0f, 1.0f);
    this->Verify(v.GetComponent(MeshBuilderVertex::CoordIndex) == transformedCoord);
    this->Verify(v.GetComponent(MeshBuilderVertex::NormalIndex) == normal);
    this->Verify(v.GetComponent(MeshBuilderVertex::TangentIndex) == tangent);
    this->Verify(v.GetComponent(MeshBuilderVertex::BinormalIndex) == binormal);
    this->Verify(v.GetComponent(MeshBuilderVertex::Uv0Index) == uv0);
    this->Verify(v.GetComponent(MeshBuilderVertex::Uv1Index) == uv1);
    this->Verify(v.GetComponent(MeshBuilderVertex::Uv2Index) == uv2);
    this->Verify(v.GetComponent(MeshBuilderVertex::Uv3Index) == uv3);
    this->Verify(v.GetComponent(MeshBuilderVertex::ColorIndex) == color);
    this->Verify(v.GetComponent(MeshBuilderVertex::WeightsIndex) == weights);
    this->Verify(v.GetComponent(MeshBuilderVertex::JIndicesIndex) == jindices);
}

} // namespace Test

//------------------------------------------------------------------------------
//  d3d9shaprenderer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/win360/d3d9shaperenderer.h"
#include "coregraphics/win360/d3d9types.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/renderdevice.h"
#include "threading/thread.h"
#include "coregraphics/shaderserver.h"
#include "resources/resourceid.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9ShapeRenderer, 'D9SR', Base::ShapeRendererBase);

using namespace Threading;
using namespace Math;
using namespace CoreGraphics;
using namespace Threading;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
D3D9ShapeRenderer::D3D9ShapeRenderer()
{
    Memory::Clear(this->shapeMeshes, sizeof(this->shapeMeshes));
}

//------------------------------------------------------------------------------
/**
*/
D3D9ShapeRenderer::~D3D9ShapeRenderer()
{
    n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShapeRenderer::Open()
{
    n_assert(!this->IsOpen());
    n_assert(!this->shapeShader.isvalid());

    // call parent class
    ShapeRendererBase::Open();

    // create shape shader instance
    this->shapeShader = ShaderServer::Instance()->CreateShaderInstance(ResourceId("shd:shape"));

    // create D3DX shapes
    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = RenderDevice::Instance()->GetDirect3DDevice();
    n_assert(0 != d3d9Dev);
    // extents (half width of these objects must be 1, like in the unit-circle the radius is 1)
    hr = D3DXCreateBox(d3d9Dev, 2.0f, 2.0f, 2.0f, &this->shapeMeshes[RenderShape::Box], NULL);
    n_assert(SUCCEEDED(hr));
    hr = D3DXCreateSphere(d3d9Dev, 1.0f, 10, 5, &this->shapeMeshes[RenderShape::Sphere], NULL);
    n_assert(SUCCEEDED(hr));
    hr = D3DXCreateCylinder(d3d9Dev, 1.0f, 1.0f, 1.0f, 10, 1, &this->shapeMeshes[RenderShape::Cylinder], NULL);
    n_assert(SUCCEEDED(hr));
    hr = D3DXCreateTorus(d3d9Dev, 0.5f, 0.5f, 10, 10, &this->shapeMeshes[RenderShape::Torus], NULL);
    n_assert(SUCCEEDED(hr));
    
    // lookup ModelViewProjection shader variable
    this->modelViewProj = this->shapeShader->GetVariableBySemantic(ShaderVariable::Semantic("ModelViewProjection"));
    this->diffuseColor  = this->shapeShader->GetVariableBySemantic(ShaderVariable::Semantic("MatDiffuse"));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShapeRenderer::Close()
{
    n_assert(this->IsOpen());
    n_assert(this->shapeShader.isvalid());

    this->diffuseColor = 0;
    this->modelViewProj = 0;

    // release D3DX shapes
    IndexT i;
    for (i = 0; i < RenderShape::NumShapeTypes; i++)
    {
        if (0 != this->shapeMeshes[i])
        {
            this->shapeMeshes[i]->Release();
            this->shapeMeshes[i] = 0;
        }
    }

    // discard shape shader
    this->shapeShader->Discard();
    this->shapeShader = 0;

    // call parent class
    ShapeRendererBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShapeRenderer::DrawShapes()
{
    n_assert(this->IsOpen());

	for (int depthType = 0;depthType<RenderShape::NumDepthFlags;depthType++)
    {
	    if (this->shapes[depthType].Size() > 0)
	    {
	        this->shapeShader->Begin();
	        this->shapeShader->BeginPass(0);
	
	        // render indiviual shapes
	        IndexT i;
	        for (i = 0; i < this->shapes[depthType].Size(); i++)
	        {
	            const RenderShape& curShape = this->shapes[depthType][i];
	            n_assert(InvalidThreadId != curShape.GetThreadId());
	            switch (curShape.GetShapeType())
	            {
	                case RenderShape::Primitives:
	                    this->DrawPrimitives(curShape.GetModelTransform(),
	                                         curShape.GetTopology(),
	                                         curShape.GetNumPrimitives(),
	                                         curShape.GetVertexData(),
	                                         curShape.GetVertexWidth(),
	                                         curShape.GetColor());
	                    break;
	
	                case RenderShape::IndexedPrimitives:
	                    this->DrawIndexedPrimitives(curShape.GetModelTransform(),
	                                                curShape.GetTopology(),
	                                                curShape.GetNumPrimitives(),
	                                                curShape.GetVertexData(),
	                                                curShape.GetNumVertices(),
	                                                curShape.GetVertexWidth(),
	                                                curShape.GetIndexData(),
	                                                curShape.GetIndexType(),
	                                                curShape.GetColor());
	                    break;
	
	                default:
	                    this->DrawSimpleShape(curShape.GetModelTransform(), curShape.GetShapeType(), curShape.GetColor());
	                    break;
	            }
	        }
    }
        this->shapeShader->EndPass();
        this->shapeShader->End();

        // delete the shapes of my own thread id, all other shapes
        // are from other threads and will be deleted through DeleteShapesByThreadId()
        this->DeleteShapesByThreadId(Thread::GetMyThreadId());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShapeRenderer::DrawSimpleShape(const matrix44& modelTransform, RenderShape::Type shapeType, const float4& color)
{
    n_assert(0 != this->shapeMeshes[shapeType]);
    n_assert(shapeType < RenderShape::NumShapeTypes);
    HRESULT hr;

    // resolve model-view-projection matrix and update shader
    TransformDevice* transDev = TransformDevice::Instance();
    transDev->SetModelTransform(modelTransform);
    this->modelViewProj->SetMatrix(transDev->GetModelViewProjTransform());
    this->diffuseColor->SetFloat4(color);
    this->shapeShader->Commit();

    // draw shape
    n_assert(RenderDevice::Instance()->IsInBeginFrame());
    hr = this->shapeMeshes[shapeType]->DrawSubset(0);
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShapeRenderer::DrawPrimitives(const matrix44& modelTransform, 
                                  PrimitiveTopology::Code topology,
                                  SizeT numPrimitives,
                                  const void* vertices,
                                  SizeT vertexWidth,
                                  const Math::float4& color)
{
    n_assert(0 != vertices);
    HRESULT hr;

    // resolve model-view-projection matrix and update shader
    TransformDevice* transDev = TransformDevice::Instance();
    transDev->SetModelTransform(modelTransform);
    this->modelViewProj->SetMatrix(transDev->GetModelViewProjTransform());
    this->diffuseColor->SetFloat4(color);
    this->shapeShader->Commit();

    // draw primitives
    D3DPRIMITIVETYPE d3d9PrimType = D3D9Types::AsD3D9PrimitiveType(topology);
    IDirect3DDevice9* d3d9Dev = RenderDevice::Instance()->GetDirect3DDevice();
    hr = d3d9Dev->DrawPrimitiveUP(d3d9PrimType,                     // PrimitiveType
                                  numPrimitives,                    // PrimitiveCount
                                  vertices,                         // pVertexStreamZeroData
                                  vertexWidth * sizeof(float));     // VertexStreamZeroStride
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShapeRenderer::DrawIndexedPrimitives(const matrix44& modelTransform,
                                         PrimitiveTopology::Code topology,
                                         SizeT numPrimitives,
                                         const void* vertices,
                                         SizeT numVertices,
                                         SizeT vertexWidth,
                                         const void* indices,
                                         IndexType::Code indexType,
                                         const float4& color)
{
    n_assert(0 != vertices);
    n_assert(0 != indices);
    HRESULT hr;

    // resolve model-view-projection matrix and update shader
    TransformDevice* transDev = TransformDevice::Instance();
    transDev->SetModelTransform(modelTransform);
    this->modelViewProj->SetMatrix(transDev->GetModelViewProjTransform());
    this->diffuseColor->SetFloat4(color);
    this->shapeShader->Commit();

    // draw indexed primitives
    D3DPRIMITIVETYPE d3d9PrimType = D3D9Types::AsD3D9PrimitiveType(topology);
    D3DFORMAT d3d9IndexType = (IndexType::Index16 == indexType) ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
    IDirect3DDevice9* d3d9Dev = RenderDevice::Instance()->GetDirect3DDevice();
    hr = d3d9Dev->DrawIndexedPrimitiveUP(d3d9PrimType,      // PrimitiveType
                                         0,                 // MinVertexIndex
                                         numVertices,       // NumVertices
                                         numPrimitives,     // NumPrimitives
                                         indices,           // pIndexData
                                         d3d9IndexType,     // IndexDataFormat
                                         vertices,          // pVertexStreamZeroData
                                         vertexWidth * sizeof(float));  // VertexStreamZeroStride
    n_assert(SUCCEEDED(hr));
}

} // namespace Direct3D9
